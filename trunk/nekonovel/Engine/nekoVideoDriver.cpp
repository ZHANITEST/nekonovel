#include "nekoPrecompiledHeader.h"
#include "nekoVideoDriver.h"
#include "nekoLogger.h"
#include "nekoHWResourceManager.h"
#include "NekoNovel.h"

namespace neko
{
	void nekoCamera::Clear()
	{
		mScaleX = mScaleY = 1.0f;
		mRotation = 0.0f;
		mCenterPos.x = 400;
		mCenterPos.y = 300;

		Update();
	}

	void nekoCamera::Update()
	{
		D3DXMatrixIdentity(&matCamera);
		D3DXMatrixTransformation2D(&matCamera, &D3DXVECTOR2(mCenterPos.x, mCenterPos.y), 0, &D3DXVECTOR2(mScaleX, mScaleY),
			&D3DXVECTOR2(mCenterPos.x, mCenterPos.y), mRotation, &D3DXVECTOR2(-(mCenterPos.x - 400), -(mCenterPos.y - 300)));
	}

	/// Constructor
	nekoVideoDriver::nekoVideoDriver(int32 width, int32 height)
		: mClearColor(0xff000000)
		, mTempTargetSurface(NULL)
		, mCameraEnabled(true)
		, mD3DDevice(NULL)
		, mSprite(NULL)
		, mDefaultFont(NULL)
		, mTempTarget(NULL)
		, mBackupSurface(NULL)
		, mTRRendering(false)
		, mD3D9(0)
	{
		D3DXMatrixIdentity(&mBaseMatrix);
	}

	/// Destructor
	nekoVideoDriver::~nekoVideoDriver()
	{
		SafeRelease(mSprite);
		SafeRelease(mTempTargetSurface);
		SafeRelease(mTempTarget);
		SafeRelease(mDefaultFont);
		SafeRelease(mD3DDevice);
		SafeRelease(mD3D9);
	}

	/// Initialize
	result nekoVideoDriver::Initialize(HWND hWnd, int32 width, int32 height, bool fullscreen)
	{
		nekoAssert(hWnd && width > 0 && height > 0, "");

		HRESULT hr;
		
		mD3D9 = Direct3DCreate9(D3D_SDK_VERSION);
		if(mD3D9 == NULL)
		{
			LogPrint(LOG_LEVEL_ERRO, "Direct3D9를 초기화 할 수 없습니다.");
			return E_FAIL;
		}


		D3DDISPLAYMODE d3ddm;
		if(FAILED(mD3D9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
		{
			LogPrint(LOG_LEVEL_ERRO, "모니터 정보를 얻을 수 없습니다.");
			return E_FAIL;
		}

		memset(&mParams, 0, sizeof(D3DPRESENT_PARAMETERS));
		mParams.Windowed = !fullscreen;
		mParams.hDeviceWindow = hWnd;
		mParams.BackBufferWidth = width;
		mParams.BackBufferHeight = height;
		mParams.BackBufferFormat = d3ddm.Format;
		mParams.EnableAutoDepthStencil = false;
		mParams.AutoDepthStencilFormat = D3DFMT_D16;
		mParams.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
		mParams.SwapEffect = (fullscreen) ? D3DSWAPEFFECT_FLIP : D3DSWAPEFFECT_DISCARD;

		if(FAILED(hr = mD3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING|D3DCREATE_MULTITHREADED, &mParams, &mD3DDevice)))
		{
			LogPrint(LOG_LEVEL_ERRO, "Direct3D9 장치를 초기화 할 수 없습니다.(오류 번호 : %d)", hr);
			return E_FAIL;
		}

		if(FAILED(D3DXCreateSprite(mD3DDevice, &mSprite)))
		{
			LogPrint(LOG_LEVEL_ERRO, "Direct3D9 스프라이트를 초기화 할 수 없습니다.(오류 번호 : %d)", hr);
			return E_FAIL;
		}

		if(FAILED(D3DXCreateFont(mD3DDevice, 15, 0, 0, 1, false, DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_DONTCARE, "돋움체", &mDefaultFont)))
		{
			LogPrint(LOG_LEVEL_WARN, "돋움체' 폰트 로드 실패");
		}
		
		if(FAILED(D3DXCreateTexture(mD3DDevice, 320, 240, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &mTempTarget)))
		{
			LogPrint(LOG_LEVEL_WARN, "렌더 타겟 생성 실패, 세이브 화면이 표시되지 않을 수 있습니다.");
		}
		else
		{
			mTempTarget->GetSurfaceLevel(0, &mTempTargetSurface);
		}

		SetEnvironment();

		LogPrint(LOG_LEVEL_INFO, "그래픽 시스템이 정상적으로 시작되었어요!");
		return S_OK;
	}

	/// On Device Lost
	void nekoVideoDriver::OnDeviceLost()
	{
		GetNekoNovel()->GetHWResourceManager()->OnDeviceLost();
	}

	/// On Device Reset
	void nekoVideoDriver::OnDeviceReset()
	{
		GetNekoNovel()->GetHWResourceManager()->OnDeviceReset();
	}

	/// On Window changed.
	/// (width or height == -1) don't affect.
	result nekoVideoDriver::OnWindowChanged(int32 width, int32 height, bool fullscreen)
	{
		result hr;

		if(GetNekoNovel()->IsOwnerOfHWND() == false)
		{
			if(fullscreen)
				return E_FAIL;
		}
		
		mLockSection.Begin();

		OnDeviceLost();

		if(fullscreen == false)
		{
			if(width < 0)
				mParams.BackBufferWidth = GetNekoNovel()->GetWidth();
			if(height < 0)
				mParams.BackBufferHeight = GetNekoNovel()->GetHeight();
		}

		if(width > 0)
			mParams.BackBufferWidth = width;
		if(height > 0)
			mParams.BackBufferHeight = height;

		mParams.Windowed = !fullscreen;
		mParams.SwapEffect = (mParams.Windowed) ? D3DSWAPEFFECT_DISCARD : D3DSWAPEFFECT_FLIP;

		if(mDefaultFont) mDefaultFont->OnLostDevice();
		SafeRelease(mTempTargetSurface);
		SafeRelease(mTempTarget);

		mSprite->OnLostDevice();

		if(FAILED(hr = mD3DDevice->Reset(&mParams)))
		{
			mLockSection.End();

			// 만약 디스플레이 모드가 지원되지 않는 것 이라면?
			D3DDISPLAYMODE d3ddm;
			uint32 adapterCount = mD3D9->GetAdapterModeCount(D3DADAPTER_DEFAULT, mParams.BackBufferFormat);

			for(uint32 i = 0;i < adapterCount;++i)
			{
				mD3D9->EnumAdapterModes(D3DADAPTER_DEFAULT, mParams.BackBufferFormat, i, &d3ddm);

				if(d3ddm.Width == width && d3ddm.Height == height)
				{
					// 존재한다면 진짜 문제가 있는 것.
					return hr;
				}
			}

			RECT desktopRect;
			GetWindowRect(GetDesktopWindow(), &desktopRect);

			// 이렇다면, 지원되지 않는 해상도이다.
			GetNekoNovel()->SetRealSize(desktopRect.right - desktopRect.left, desktopRect.bottom - desktopRect.top);

			return OnWindowChanged(desktopRect.right - desktopRect.left, desktopRect.bottom - desktopRect.top, true);
		}

		mSprite->OnResetDevice();

		if(mDefaultFont) mDefaultFont->OnResetDevice();
		if(SUCCEEDED(D3DXCreateTexture(mD3DDevice, 320, 240, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &mTempTarget)))
		{
			mTempTarget->GetSurfaceLevel(0, &mTempTargetSurface);
		}

		OnDeviceReset();

		SetEnvironment();

		if(fullscreen == false)
		{
			GetNekoNovel()->SetRealSize(mParams.BackBufferWidth, mParams.BackBufferHeight);
		}

		D3DXMatrixIdentity(&mBaseMatrix);

		// 가상 해상도를 써야할 경우....
		if(fullscreen && GetNekoNovel()->IsUsingVirtualFullScreen())
		{
			// 원래 비율
			float32 hratio = (float32)GetNekoNovel()->GetRealWidth() / GetNekoNovel()->GetWidth();
			float32 vratio = (float32)GetNekoNovel()->GetRealHeight() / GetNekoNovel()->GetHeight();
			float32 rratio = (float32)GetNekoNovel()->GetRealWidth() / GetNekoNovel()->GetRealHeight();

			if(rratio - 4.f / 3.f < -REAL_Z_DIFF)
			{

				D3DXMatrixScaling(&mBaseMatrix, 
					vratio, 
					vratio, 1.0f);

				mBaseMatrix._41 = (GetNekoNovel()->GetRealWidth() - (vratio * GetNekoNovel()->GetWidth())) / 2.0f;
			}
			else
			{
				D3DXMatrixScaling(&mBaseMatrix, 
					hratio, 
					hratio, 1.0f);

				mBaseMatrix._42 = (GetNekoNovel()->GetRealHeight() - (hratio * GetNekoNovel()->GetHeight())) / 2.0f;
			}

		}
		GetNekoNovel()->ResetWindowStyle(fullscreen);

		mLockSection.End();
		return S_OK;
	}

	void nekoVideoDriver::DrawRectangle(int32 x, int32 y, int32 width, int32 height, uint32 color, float32 ZOrder)
	{
		nekoVertexColored verts[] = 
		{
			{x, y, ZOrder, color},
			{x + width, y, ZOrder, color},
			{x, y + height, ZOrder, color},
			{x + width, y + height, ZOrder, color}
		};

		EndSprite();

		mD3DDevice->SetFVF(nekoVertexColored::FVF);
		mD3DDevice->SetTexture(0, NULL);
		mD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, verts, sizeof(nekoVertexColored));

		BeginSprite();
	}

	void nekoVideoDriver::DrawText(nekoHWFont *font, int32 x, int32 y, int32 width, int32 height, const char8 *txt, uint32 color, uint32 flags, float32 z)
	{
		ID3DXFont *fontHandle;

		if((color >> 24) == 0)
			return;

		if(font == NULL)
			fontHandle = mDefaultFont;
		else
		{
			font->Refresh();

			fontHandle = font->GetHandle();
		}

		if(fontHandle == NULL)
			return;

		D3DXMATRIX matWorld;
		D3DXMatrixIdentity(&matWorld);
		matWorld._43 = z;
		matWorld *= mBaseMatrix;

		RECT rect = {x, y, x + width, y + height};
		mSprite->SetTransform(&matWorld);
		fontHandle->DrawTextA(mSprite, txt, -1, &rect, flags, color);
	}

	void nekoVideoDriver::CalculateText(nekoHWFont *font, const char8 *text, nekoRectangle &resultRect, uint32 flags)
	{
		ID3DXFont *fontHandle;

		if(font == NULL)
			fontHandle = mDefaultFont;
		else
		{
			font->Refresh();

			fontHandle = font->GetHandle();
		}

		if(fontHandle == NULL)
			return;

		resultRect.SetEmpty();
		fontHandle->DrawTextA(mSprite, text, -1, (RECT *)&resultRect, flags|DT_CALCRECT, 0xffffffff);
	}

	void nekoVideoDriver::CalculateTextZ(ID3DXFont *fontHandle, const char8 *text, nekoRectangle &resultRect, uint32 flags)
	{
		if(fontHandle == NULL)
		{
			fontHandle = mDefaultFont;

			if(fontHandle == NULL)
				return;
		}

		resultRect.SetEmpty();
		fontHandle->DrawTextA(mSprite, text, -1, (RECT *)&resultRect, flags|DT_CALCRECT, 0xffffffff);
	}

	void nekoVideoDriver::DrawImage(nekoHWTexture *texture, int32 x, int32 y, int32 width, int32 height, uint32 color,
		int32 srcX, int32 srcY, int32 srcWidth, int32 srcHeight, float32 rotAngle, float32 scaleX, float32 scaleY, float32 z)
	{
		if(texture == NULL || (color >> 24) == 0 || !width || !height)
			return;

		texture->Refresh();
		if(!texture->GetHandle())
			return;

		nekoPointF ratio = texture->GetSizeRatio();

		if(srcWidth < 0)
			srcWidth = texture->GetSize().x;
		if(srcHeight < 0)
			srcHeight = texture->GetSize().y;

		if(width < 0)
			width = srcWidth;
		if(height < 0)
			height = srcHeight;

		/*
		x -= (mCamera.mCenterPos.x - 400);
		y -= (mCamera.mCenterPos.y - 400);
		rotAngle += mCamera.mRotation;
		scaleX *= mCamera.mScaleX;
		scaleY *= mCamera.mScaleY;
		*/

		D3DXMATRIX matWorld;
		D3DXMatrixIdentity(&matWorld);
		D3DXMatrixTransformation2D(&matWorld, 0, 0, &D3DXVECTOR2((float32)width / srcWidth * scaleX * ratio.x,
			(float32)height / srcHeight * scaleY * ratio.y), &D3DXVECTOR2(width * scaleX / 2, height * scaleY / 2), rotAngle, &D3DXVECTOR2(x, y));
		if(mCameraEnabled)
			matWorld *= mCamera.matCamera;
		matWorld *= mBaseMatrix;

		RECT srcRect = {(srcX) / ratio.x, (srcY) / ratio.y, (srcX + srcWidth) / ratio.x, (srcY + srcHeight) / ratio.y};

		mSprite->SetTransform(&matWorld);
		mSprite->Draw(texture->GetHandle(), &srcRect, NULL, &D3DXVECTOR3(0, 0, z), color);
	}

	void nekoVideoDriver::DrawImageZ(IDirect3DTexture9 *texture, int32 x, int32 y, int32 width, int32 height, uint32 color,
		int32 srcX, int32 srcY, int32 srcWidth, int32 srcHeight, float32 rotAngle, float32 scaleX, float32 scaleY, float32 z)
	{
		if(texture == NULL || (color >> 24) == 0 || !width || !height)
			return;

		if(width < 0)
			width = srcWidth;
		if(height < 0)
			height = srcHeight;

		D3DSURFACE_DESC desc;

		/*
		x -= (mCamera.mCenterPos.x - 400);
		y -= (mCamera.mCenterPos.y - 400);
		rotAngle += mCamera.mRotation;
		scaleX *= mCamera.mScaleX;
		scaleY *= mCamera.mScaleY;
		*/

		D3DXMATRIX matWorld;
		D3DXMatrixIdentity(&matWorld);
		D3DXMatrixTransformation2D(&matWorld, 0, 0, &D3DXVECTOR2((float32)width / srcWidth * scaleX,
			(float32)height / srcHeight * scaleY), &D3DXVECTOR2(width * scaleX / 2, height * scaleY / 2), rotAngle, &D3DXVECTOR2(x, y));
		if(mCameraEnabled)
			matWorld *= mCamera.matCamera;
		matWorld *= mBaseMatrix;

		RECT srcRect = {srcX, srcY, srcX + srcWidth, srcY + srcHeight};

		mSprite->SetTransform(&matWorld);
		mSprite->Draw(texture, &srcRect, NULL, &D3DXVECTOR3(0, 0, z), color);
	}

	result nekoVideoDriver::BeginRender()
	{
		result hr;

		nekoAssert(mD3DDevice, "");

		if(FAILED(hr = mD3DDevice->BeginScene()))
			return hr;

		mD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, mClearColor, 1.0f, 0);

		BeginSprite();

		mD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		mD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		mD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		return S_OK;
	}

	result nekoVideoDriver::EndRender()
	{
		result hr;

		EndSprite();
 
		// 가상 해상도를 써야할 경우.... 나머지 여백을 칠해준다
		if(GetNekoNovel()->IsUsingVirtualFullScreen())
		{
			// 원래 비율
			float32 hratio = (float32)GetNekoNovel()->GetRealWidth() / GetNekoNovel()->GetWidth();
			float32 vratio = (float32)GetNekoNovel()->GetRealHeight() / GetNekoNovel()->GetHeight();
			float32 rratio = (float32)GetNekoNovel()->GetRealWidth() / GetNekoNovel()->GetRealHeight();

			if(rratio - 4.f / 3.f < -REAL_Z_DIFF)
			{
				int32 blank = (GetNekoNovel()->GetRealWidth() - (vratio * GetNekoNovel()->GetWidth())) / 2.0f;

				DrawRectangle(0, 0, blank, GetNekoNovel()->GetRealHeight(), mClearColor);
				DrawRectangle(GetNekoNovel()->GetRealWidth() - blank, 0, blank, GetNekoNovel()->GetRealHeight(), mClearColor);
			}
			else
			{
				int32 blank = (GetNekoNovel()->GetRealHeight() - (hratio * GetNekoNovel()->GetHeight())) / 2.0f;

				DrawRectangle(0, 0, GetNekoNovel()->GetRealWidth(), blank, mClearColor);
				DrawRectangle(0, GetNekoNovel()->GetRealHeight() - blank, GetNekoNovel()->GetRealWidth(), blank, mClearColor);
			}

		}

		mD3DDevice->EndScene();
		hr = mD3DDevice->Present(NULL, NULL, NULL, NULL);
		if(FAILED(hr))
		{
			if(hr == D3DERR_DEVICELOST)
			{
				while( D3DERR_DEVICENOTRESET != hr )
				{
					hr = mD3DDevice->TestCooperativeLevel();

					MSG msg;
					BOOL windowMessage;
					windowMessage = PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE );
					if( windowMessage )
					{
						TranslateMessage( &msg );
						DispatchMessage( &msg );
					}
				}

				return OnWindowChanged(-1, -1, !mParams.Windowed);

			}

			// 복구 가능!
			else if(hr == D3DERR_DEVICENOTRESET)
			{
				return OnWindowChanged(-1, -1, !mParams.Windowed);
			}
		}

		return S_OK;
	}

	/*
	void nekoVideoDriver::SetBitmapCache(HDC hdc, int32 width, int32 height)
	{
		RECT rect = {0, 0, width, height};

		bmpCache.oldBitmap = (HBITMAP)SelectObject(hdc, bmpCache.handle);
		FillRect(hdc, &rect, bmpCache.brush);
		SetBkMode(hdc, TRANSPARENT);
	}

	result nekoVideoDriver::FlushBitmapCache(HDC hdc, nekoHWTexture *outTexture)
	{
		if(!bmpCache.handle)
			return E_FAIL;

		D3DLOCKED_RECT rect;
		if(FAILED(outTexture->GetHandle()->LockRect(0, &rect, 0, 0)))
			return E_FAIL;

		rect.Pitch /= 4;

		int32 x, y;
		uint32 *src = bmpCache.cachePtr;
		uint32 *dest = (uint32 *)rect.pBits;
		nekoPoint size = outTexture->GetSize();
		for(y = 0;y < size.y;++y)
		{
			for(x = 0;x < size.x;++x)
			{
				if(!src[x] || src[x] == 0x00010101)
					dest[x] = 0x00;
				else
					dest[x] = src[x] | 0xff000000;
			}

			src += bmpCache.width;
			dest += rect.Pitch;
		}

		outTexture->GetHandle()->UnlockRect(0);

		/// Clean up
		SelectObject(hdc, bmpCache.oldBitmap);
		::SetTextColor(hdc, 0x00ffffff);
		SetBkMode(hdc, OPAQUE);
		return S_OK;
	}
	*/

	void nekoVideoDriver::SetEnvironment()
	{
		// 투영 행렬 설정
		D3DXMATRIX matProj;
		D3DXMatrixOrthoOffCenterLH(&matProj, 0, mParams.BackBufferWidth, mParams.BackBufferHeight, 0, 0.1f, 1.0f);
		//D3DXMatrixPerspectiveOffCenterLH(&matProj, 0, pSurface->w, pSurface->h, 0, 0.1f, 1);
		mD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);

		// 렌더링 상태 설정
		mD3DDevice->SetRenderState(D3DRS_ZENABLE, true);
		mD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);
		mD3DDevice->SetRenderState(D3DRS_LIGHTING, false);
		mD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		mD3DDevice->SetRenderState( D3DRS_DITHERENABLE, false);
		mD3DDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
		mD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		mD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		mD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
		mD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		mD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
		mD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
		mD3DDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);
		mD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		mD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		mD3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		// 알파블렌딩 켜기
		mD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		mD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		mD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		// 알파테스트 셋팅
		mD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, true);
		mD3DDevice->SetRenderState(D3DRS_ALPHAREF, 0x01);
	}

	result nekoVideoDriver::BeginTargetRender()
	{
		result hr;

		if(mTempTarget == NULL)
			return E_FAIL;

		if(FAILED(hr = mD3DDevice->GetRenderTarget(0, &mBackupSurface)))
			return hr;

		mBackupSurface->Release();

		if(FAILED(hr = mD3DDevice->SetRenderTarget(0, mTempTargetSurface)))
			return hr;

		if(FAILED(hr = mD3DDevice->BeginScene()))
			return hr;

		mD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, mClearColor, 1.0f, 0);

		BeginSprite();

		// 투영 행렬 설정
		D3DXMATRIX matProj;
		D3DXMatrixOrthoOffCenterLH(&matProj, 0, mParams.BackBufferWidth, mParams.BackBufferHeight, 0, 0.1f, 1.0f);
		mD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);

		mTRRendering = true;

		return S_OK;
	}

	void nekoVideoDriver::EndTargetRender()
	{
		EndSprite();

		mTRRendering = false;

		mD3DDevice->EndScene();
		mD3DDevice->Present(NULL, NULL, NULL, NULL);

		mD3DDevice->SetRenderTarget(0, mBackupSurface);

		// 투영 행렬 원상 복귀
		D3DXMATRIX matProj;
		D3DXMatrixOrthoOffCenterLH(&matProj, 0, mParams.BackBufferWidth, mParams.BackBufferHeight, 0, 0.1f, 1.0f);
		mD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);
	}

	result nekoVideoDriver::ConvertGrayscaleToAlphaMap(nekoHWTexture *tex)
	{
		if(tex == NULL)
			return E_FAIL;

		tex->Refresh();
		if(tex->GetHandle() == NULL)
			return E_FAIL;

		D3DLOCKED_RECT lockedRect;
		if(FAILED(tex->GetHandle()->LockRect(0, &lockedRect, NULL, 0)))
			return E_FAIL;

		DWORD *ptr = (DWORD *)lockedRect.pBits;
		uint32 bytes = tex->GetVideoSize().x * tex->GetVideoSize().y;
		for(uint32 i = 0;i < bytes;++i, ++ptr)
		{
			(*ptr) = (((255 - (*ptr)) & 0xff) << 24) | ((*ptr) & 0xffffff);
		}

		tex->GetHandle()->UnlockRect(0);
		return S_OK;
	}
}