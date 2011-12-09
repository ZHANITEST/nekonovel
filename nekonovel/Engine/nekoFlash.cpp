#include "nekoPrecompiledHeader.h"
#include "nekoFlash.h"
#include "nekoNovel.h"
#include "nekoVideoDriver.h"

namespace neko
{
	HDC nekoFlashInstance::mMainDC = NULL;
	HBITMAP nekoFlashInstance::mMainBitmap = NULL;
	DWORD *nekoFlashInstance::mMainBuffer = NULL;

	HDC nekoFlashInstance::mAlphaDC = NULL;
	HBITMAP nekoFlashInstance::mAlphaBitmap = NULL;
	DWORD *nekoFlashInstance::mAlphaBuffer = NULL;

	int32 nekoFlashInstance::mBufferUsingCount = 0;

	HRESULT STDMETHODCALLTYPE nekoFlashSite::GetWindowContext(IOleInPlaceFrame __RPC_FAR *__RPC_FAR *ppFrame, IOleInPlaceUIWindow __RPC_FAR *__RPC_FAR *ppDoc, 
		LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo)
	{
		RECT rect;
		rect.left = 0;
		rect.top = 0;
		rect.right = owner->GetBoundingBox().GetWidth();
		rect.bottom = owner->GetBoundingBox().GetHeight();

		*lprcPosRect = rect;
		*lprcClipRect = rect;

		*ppFrame = 0;
		QueryInterface(__uuidof(IOleInPlaceFrame), (void**)ppFrame);		
		*ppDoc = 0;

		lpFrameInfo->fMDIApp = FALSE;
		lpFrameInfo->hwndFrame = 0;
		lpFrameInfo->haccel = 0;
		lpFrameInfo->cAccelEntries = 0;

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE nekoFlashSite::InvalidateRect(LPCRECT pRect, BOOL fErase)
	{	
		//if(owner->isTotallyDirty)
		//	return S_OK;

		if(pRect)
		{
			/*if(owner->isClean)
			{
			owner->dirtyBounds.left = pRect->left;
			owner->dirtyBounds.right = pRect->right;
			owner->dirtyBounds.top = pRect->top;
			owner->dirtyBounds.bottom = pRect->bottom;
			owner->isClean = false;
			}
			else
			{
			*/
			owner->mDirtyBounds.left = min(owner->mDirtyBounds.left, pRect->left);
			owner->mDirtyBounds.right = max(owner->mDirtyBounds.right, pRect->right);
			owner->mDirtyBounds.top = min(owner->mDirtyBounds.top, pRect->top);
			owner->mDirtyBounds.bottom = max(owner->mDirtyBounds.bottom, pRect->bottom);
			//}
		}

		if(owner->mDirtyBounds.left <= 0 || owner->mDirtyBounds.top <= 0
			|| owner->mDirtyBounds.right >= owner->GetBoundingBox().GetWidth()
			|| owner->mDirtyBounds.bottom >= owner->GetBoundingBox().GetHeight()
			|| !pRect
			|| fErase)
		{
			owner->InvalidateTotally();
		}

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE nekoFlashSite::InvalidateRgn(HRGN hRGN, BOOL fErase)
	{	
		owner->InvalidateTotally();

		return S_OK;
	}

	typedef HRESULT (__stdcall *GetClassObject)(REFCLSID rclsid, REFIID riid, LPVOID * ppv);

	nekoFlashInstance::nekoFlashInstance(nekoIBlock *block)
		: nekoIBlockInstance(block)
		, mFlashType(FLASH_MOVIE)
		, mCurView(NULL)
		, mSkipAbleTime(0)
		, mOleObject(0)
		, mFlashSite(0)
		, flashInterface(0)
	{
		HMODULE flashLib = GetNekoNovel()->GetFlashLib();

		mFlashSite = nekoNew nekoFlashSite;
		mFlashSite->Init(this);

		if(flashLib)
		{
			IClassFactory* factory = 0;
			GetClassObject getClassFunc = (GetClassObject)GetProcAddress(flashLib, "DllGetClassObject");
			HRESULT result = getClassFunc(ShockwaveFlashObjects::CLSID_ShockwaveFlash, IID_IClassFactory, (void**)&factory);
			if(SUCCEEDED(result))
			{
				factory->CreateInstance(NULL, IID_IOleObject, (void**)&mOleObject);
				factory->Release();	
			}
		}

		// If we still don't have the object, try loading from registry
		if(!mOleObject)
		{
			HRESULT result = CoCreateInstance(ShockwaveFlashObjects::CLSID_ShockwaveFlash, 0, CLSCTX_INPROC_SERVER, IID_IOleObject, (void**)&mOleObject);
			if(FAILED(result))
			{
				LogPrint(LOG_LEVEL_WARN, "플래시 컨트롤을 불러올 수 없습니다.\nFlash.ocx가 설치되어 있지 않을 수 있습니다.");
				return;
			}
		}


		IOleClientSite* clientSite = 0;
		mFlashSite->QueryInterface(__uuidof(IOleClientSite), (void**)&clientSite);
		mOleObject->SetClientSite(clientSite);

		clientSite->Release();

		mOleObject->QueryInterface(__uuidof(ShockwaveFlashObjects::IShockwaveFlash), (LPVOID*)&flashInterface);
		flashInterface->PutWMode("opaque");
		
		mOleObject->DoVerb(OLEIVERB_SHOW, 0, clientSite, 0, 0, 0);
		clientSite->Release();

		//mOleObject->QueryInterface(__uuidof(IOleInPlaceObjectWindowless), (LPVOID*)windowlessObject.Pptr());

		flashInterface->QueryInterface(IID_IViewObject, (void**)&mCurView);

		mBufferUsingCount++;
	}

	nekoFlashInstance::~nekoFlashInstance()
	{
		if(mCurView)
		{
			mCurView->Release();
			mCurView = NULL;
		}

		if(flashInterface)
		{
			flashInterface->Stop();
			flashInterface->Release();
			flashInterface = NULL;
		}

		if(mOleObject)
		{
			mOleObject->Close(OLECLOSE_NOSAVE);
			mOleObject->Release();
			mOleObject = NULL;
		}

		if(mFlashSite)
		{
			mFlashSite->Release();
			mFlashSite = NULL;
		}

		--mBufferUsingCount;

		if(mBufferUsingCount == 0)
		{
			if(mMainDC)
			{
				DeleteDC(mMainDC);
				mMainDC = NULL;
			}

			if(mMainBitmap)
			{
				DeleteObject(mMainBitmap);
				mMainBitmap = NULL;
			}

			if(mAlphaDC)
			{
				DeleteDC(mAlphaDC);
				mAlphaDC = NULL;
			}

			if(mAlphaBitmap)
			{
				DeleteObject(mAlphaBitmap);
				mAlphaBitmap = NULL;
			}
		}

		GetNekoNovel()->ReleaseFlashLib();
	}

	void nekoFlashInstance::SetFlash(const char8 *fileName, float32 skipAbleTime)
	{
		if(flashInterface == NULL)
			return;

		char8 *data;
		int32 length;

		string moviePath = GetNekoNovel()->GetAppFolderPath() + string("동영상\\") + fileName;
		/*if(FAILED(GetNekoNovel()->GetFileSystem()->LoadData(moviePath.c_str(), &data, &length)))
		{
			LogPrint(LOG_LEVEL_WARN, "'%s' 플래시 파일을 로드할 수 없습니다.", mName.c_str());
			return;
		}*/

		flashInterface->PutMovie(moviePath.c_str());
		flashInterface->PutLoop(false);
		//delete []data;

		mCurView = NULL;
		mSkipAbleTime = skipAbleTime;
	}

	void nekoFlashInstance::Update(float32 frameTime)
	{
		if(!flashInterface)
		{
			SetValid(false);
			return;
		}

		if(!mTexture)
			return;

		if(mSkipAbleTime > 0.0f)
		{
			mSkipAbleTime = max(mSkipAbleTime - frameTime, 0);
		}

		int32 dirtyWidth = mDirtyBounds.right - mDirtyBounds.left;
		int32 dirtyHeight = mDirtyBounds.bottom - mDirtyBounds.top;
		int32 dirtyBufSize = dirtyWidth * dirtyHeight;

		flashInterface->QueryInterface(IID_IViewObject, (void**)&mCurView);
		if(!mCurView || !dirtyBufSize)
			return;

		IDirect3DSurface9 *pSurface;
		D3DLOCKED_RECT lockedRect;

		if(mTexture->GetHandle() == NULL)
			return;

		if(SUCCEEDED(mTexture->GetHandle()->LockRect(0, &lockedRect, 0, D3DLOCK_DISCARD|D3DLOCK_NOOVERWRITE)))
		{
			lockedRect.Pitch /= 4;

			if(mMainBitmap == NULL)
			{
				HDC hdc = GetDC(0);
				BITMAPINFOHEADER bih = {0};
				bih.biSize = sizeof(BITMAPINFOHEADER);
				bih.biBitCount = 32;
				bih.biCompression = BI_RGB;
				bih.biPlanes = 1;
				bih.biWidth = 800;
				bih.biHeight = -600;
				mMainDC = CreateCompatibleDC(hdc);
				mMainBitmap = CreateDIBSection(hdc, (BITMAPINFO*)&bih, DIB_RGB_COLORS, (void**)&mMainBuffer, 0, 0);
				SelectObject(mMainDC, mMainBitmap);

				mAlphaDC = CreateCompatibleDC(hdc);
				mAlphaBitmap = CreateDIBSection(hdc, (BITMAPINFO*)&bih, DIB_RGB_COLORS, (void**)&mAlphaBuffer, 0, 0);
				SelectObject(mAlphaDC, mAlphaBitmap);
			}

			RECT local = {-mDirtyBounds.left, -mDirtyBounds.top,
				-mDirtyBounds.left + mDirtyBounds.right, -mDirtyBounds.top + mDirtyBounds.bottom};

			//memset(mMainBuffer, 0, dirtyBufSize * 4);
			//memset(mAlphaBuffer, 255, dirtyBufSize * 4);

			OleDraw(mCurView, DVASPECT_TRANSPARENT, mMainDC, &local);
			OleDraw(mCurView, DVASPECT_TRANSPARENT, mAlphaDC, &local);

			DWORD *blackBuffer, *whiteBuffer;
			DWORD *ptr;

			int32 ay = mDirtyBounds.top;
			int32 x, y;

			for(y = 0;y < dirtyHeight;++y, ++ay)
			{
				blackBuffer = (mMainBuffer + y * 800);
				whiteBuffer = (mAlphaBuffer + y * 800);
				ptr = (DWORD *)lockedRect.pBits + ay * lockedRect.Pitch + mDirtyBounds.left;

				for(x = 0;x < dirtyWidth;++x)
				{
					//blackBuffer[3] = 255 - (*blackBuffer - *whiteBuffer);
					*ptr++ = ((*blackBuffer) & 0xffffff) | ((255 - (((*blackBuffer++) & 0xff) - ((*whiteBuffer++) & 0xff))) << 24);
				}
			}

			mTexture->GetHandle()->UnlockRect(0);

			mDirtyBounds.left = mDirtyBounds.right = 400;
			mDirtyBounds.top = mDirtyBounds.bottom = 400;
		}

		mCurView->Release();
		mCurView = 0;
	}

	void nekoFlashInstance::Draw()
	{
		if(mTexture)
		{
			GetNekoNovel()->GetVideoDriver()->DrawImage(mTexture, mBoundingBox.left, mBoundingBox.top, mBoundingBox.GetWidth(), mBoundingBox.GetHeight(), D3DCOLOR_ARGB(mAlpha, 255, 255, 255), 0, 0, -1, -1, mRotation, mScale.x, mScale.y, _GetRealZ());
		}
	}

	/// 재생하기
	void nekoFlashInstance::Play()
	{
		if(flashInterface)
			flashInterface->Play();
	}

	/// 정지하기
	void nekoFlashInstance::Stop(bool pause)
	{
		if(!flashInterface)
			return;

		flashInterface->Stop();
		if(pause == false)
		{
			flashInterface->Rewind();
		}
	}

	bool nekoFlashInstance::IsPlaying()
	{
		return (flashInterface && flashInterface->IsPlaying());
	}

	void nekoFlashInstance::OnSizeUpdated(int32 width, int32 height)
	{
		if(!mOleObject)
			return;

		mTexture = NULL;
		mTexture = nekoNew nekoHWTexture(GetNekoNovel()->GetHWResourceManager(), width, height);
		mTexture->Refresh();

		IOleInPlaceObject* inPlaceObject = 0;	
		mOleObject->QueryInterface(__uuidof(IOleInPlaceObject), (LPVOID*)&inPlaceObject);

		if(inPlaceObject)
		{
			InvalidateTotally();

			inPlaceObject->SetObjectRects(&mDirtyBounds, &mDirtyBounds);
			inPlaceObject->Release();
		}
	}

	void nekoFlashInstance::InvalidateTotally()
	{
		mDirtyBounds.left = 0;
		mDirtyBounds.top = 0;
		mDirtyBounds.right = mBoundingBox.GetWidth();
		mDirtyBounds.bottom = mBoundingBox.GetHeight();
	}

	/// 데이터 덤프하기
	result nekoFlashInstance::Dump(int32 version, nekoByteBuffer &outBuf)
	{
		nekoIBlockInstance::Dump(version, outBuf);

		outBuf << mFlashType
			<< ((mTexture) ? mTexture->GetName() : "")
			<< mSkipAbleTime;

		return S_OK;
	}

	result nekoFlashInstance::Load(int32 version, nekoByteBuffer &inBuf)
	{
		nekoIBlockInstance::Load(version, inBuf);

		string path;
		float32 skipAbleTime = 0; 
		inBuf >> mFlashType;
		inBuf >> path;

		if(version >= 1125)
			inBuf >> skipAbleTime;

		if(!path.empty())
		{
			SetFlash(path.c_str(), skipAbleTime);
		}

		return S_OK;
	}

	/// Constructor
	nekoFlash::nekoFlash()
		: nekoIBlock(BLOCK_FLASH)
	{

	}

	/// Destructor
	nekoFlash::~nekoFlash()
	{

	}

	/// 인스턴스를 만든다
	nekoIBlockInstance *nekoFlash::CreateInstance()
	{
		return nekoNew nekoFlashInstance(this);
	}
}