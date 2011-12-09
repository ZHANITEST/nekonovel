// ===============================================================
//  nekoFlash - 2009.7.7
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoFlash_h__
#define __nekoFlash_h__

#include "nekoIBlock.h"
#include "nekoIBlockInstance.h"
#include "nekoHWTexture.h"
#import "PROGID:ShockwaveFlash.ShockwaveFlash" named_guids
namespace ShockwaveFlashObjects { struct IShockwaveFlash; }
struct IOleObject;
class IOleInPlaceObjectWindowless;

namespace neko
{
	enum E_FLASH_TYPE
	{
		FLASH_MOVIE = 0,
		FLASH_GUI,
	};

	class nekoFlashInstance;

	class nekoFlashSite : public IOleInPlaceSiteWindowless, public IOleClientSite
	{
	public:
		int	refCount;
		nekoFlashInstance* owner;

	public:
		nekoFlashSite() : refCount(1), owner(0)
		{		
		}	

		virtual ~nekoFlashSite()
		{
			//if(owner)
			//	owner->comCount--;
		}

		void Init(nekoFlashInstance *owner)
		{
			this->owner = owner;
			//owner->comCount++;
		}

		HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID* ppv)
		{
			*ppv = 0;

			if(riid == IID_IUnknown)
			{
				*ppv = (IUnknown*)(IOleWindow*)this;
				AddRef();
				return S_OK;
			}
			else if (riid == IID_IOleWindow)
			{
				*ppv = (IOleWindow*)this;
				AddRef();
				return S_OK;
			}
			else if (riid == IID_IOleInPlaceSite)
			{
				*ppv = (IOleInPlaceSite*)this;
				AddRef();
				return S_OK;
			}
			else if (riid == IID_IOleInPlaceSiteEx)
			{
				*ppv = (IOleInPlaceSiteEx*)this;
				AddRef();
				return S_OK;
			}
			else if (riid == IID_IOleInPlaceSiteWindowless)
			{
				*ppv = (IOleInPlaceSiteWindowless*)this;
				AddRef();
				return S_OK;
			}
			else if (riid == IID_IOleClientSite)
			{
				*ppv = (IOleClientSite*)this;
				AddRef();
				return S_OK;
			}
			else
			{   
				return E_NOTIMPL;
			}
		}

		ULONG STDMETHODCALLTYPE AddRef()
		{  
			return ++refCount;
		}

		ULONG STDMETHODCALLTYPE Release()
		{  
			--refCount;

			if(!refCount)
				delete this;

			return refCount;
		}

		virtual HRESULT STDMETHODCALLTYPE SaveObject(void)
		{
			return S_OK;
		}

		virtual HRESULT STDMETHODCALLTYPE GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker** ppmk)
		{
			*ppmk = 0;
			return E_NOTIMPL;
		}

		virtual HRESULT STDMETHODCALLTYPE GetContainer(IOleContainer ** theContainerP)
		{			
			return E_NOINTERFACE;
		}

		virtual HRESULT  STDMETHODCALLTYPE ShowObject(void)
		{
			return E_NOTIMPL;
		}

		virtual HRESULT  STDMETHODCALLTYPE OnShowWindow(BOOL)
		{
			return E_NOTIMPL;
		}

		virtual HRESULT  STDMETHODCALLTYPE RequestNewObjectLayout(void)
		{
			return E_NOTIMPL;
		}

		HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL fEnterMode)
		{
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE GetWindow(HWND __RPC_FAR* theWnndow)
		{
			return E_FAIL;
		}

		HRESULT STDMETHODCALLTYPE CanInPlaceActivate(void)
		{
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE OnInPlaceActivate(void)
		{		
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE OnUIActivate(void)
		{		
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE GetWindowContext(IOleInPlaceFrame __RPC_FAR *__RPC_FAR *ppFrame, IOleInPlaceUIWindow __RPC_FAR *__RPC_FAR *ppDoc, 
			LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo);

		HRESULT STDMETHODCALLTYPE Scroll(SIZE scrollExtant)
		{
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE OnUIDeactivate(BOOL fUndoable)
		{		
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate(void)
		{	
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE DiscardUndoState(void)
		{
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE DeactivateAndUndo(void)
		{
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE OnPosRectChange(LPCRECT lprcPosRect)
		{
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE OnInPlaceActivateEx(BOOL __RPC_FAR *pfNoRedraw, DWORD dwFlags)
		{		
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE OnInPlaceDeactivateEx(BOOL fNoRedraw)
		{		
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE RequestUIActivate(void)
		{
			return S_FALSE;
		}

		HRESULT STDMETHODCALLTYPE CanWindowlessActivate(void)
		{
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE GetCapture(void)
		{
			return S_FALSE;
		}

		HRESULT STDMETHODCALLTYPE SetCapture(BOOL fCapture)
		{
			return S_FALSE;
		}

		HRESULT STDMETHODCALLTYPE GetFocus(void)
		{
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE SetFocus(BOOL fFocus)
		{
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE GetDC(LPCRECT pRect, DWORD grfFlags, HDC __RPC_FAR *phDC)
		{		
			return E_INVALIDARG;		
		}

		HRESULT STDMETHODCALLTYPE ReleaseDC(HDC hDC)
		{
			return E_INVALIDARG;
		}

		HRESULT STDMETHODCALLTYPE InvalidateRect(LPCRECT pRect, BOOL fErase);

		HRESULT STDMETHODCALLTYPE InvalidateRgn(HRGN hRGN, BOOL fErase);

		HRESULT STDMETHODCALLTYPE ScrollRect(INT dx, INT dy, LPCRECT pRectScroll, LPCRECT pRectClip)
		{
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE AdjustRect(LPRECT prc)
		{
			if(!prc)
				return E_INVALIDARG;

			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE OnDefWindowMessage(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT __RPC_FAR *plResult)
		{
			return S_FALSE;
		}
	};

	class nekoFlashInstance : public nekoIBlockInstance
	{
	protected:

		E_FLASH_TYPE mFlashType;

		float32 mSkipAbleTime;
		nekoAutoPtr<nekoHWTexture> mTexture;

		nekoFlashSite *mFlashSite;
		ShockwaveFlashObjects::IShockwaveFlash *flashInterface;
		IOleObject *mOleObject;

		IViewObject *mCurView;

		static HDC mMainDC;
		static HBITMAP mMainBitmap;
		static DWORD *mMainBuffer;
		static HDC mAlphaDC;
		static HBITMAP mAlphaBitmap;
		static DWORD *mAlphaBuffer;
		static int32 mBufferUsingCount;

	public:

		RECT mDirtyBounds;
		bool mInvalidated;

		nekoFlashInstance(nekoIBlock *block);
		virtual ~nekoFlashInstance();

		void SetFlash(const char8 *fileName, float32 skipAbleTime);

		void Update(float32 frameTime);
		void Draw();

		/// 재생하기
		void Play();

		/// 정지하기
		void Stop(bool pause = false);

		bool IsPlaying();

		void OnSizeUpdated(int32 width, int32 height);

		void InvalidateTotally();

		/// 데이터 덤프하기
		result Dump(int32 version, nekoByteBuffer &outBuf);
		result Load(int32 version, nekoByteBuffer &inBuf);

		bool IsZeroAlphaDeleting() const { return true; }
		inline bool IsSkipAble() const
		{
			if(IsFloatZero(mSkipAbleTime))
				return true;

			if(mSkipAbleTime < 0.0f)
				return false;

			return false;
		}
	};

	class nekoFlash : public nekoIBlock
	{
	protected:

	public:

		nekoFlash();
		virtual ~nekoFlash();

		/// 인스턴스를 만든다
		virtual nekoIBlockInstance *CreateInstance();

	};
}


#endif // __nekoFlash_h__