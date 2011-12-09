// ===============================================================
//  nekoVideoDriver - 2009.7.6
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoVideoDriver_h__
#define __nekoVideoDriver_h__

#include <list>
using namespace std;
#include <d3d9.h>
#include <d3dx9.h>
#include "nekoHWFont.h"
#include "nekoAutoPtr.h"
#include "nekoHWTexture.h"

namespace neko
{
	struct nekoVertexColored
	{
		const static uint32 FVF = (D3DFVF_XYZ|D3DFVF_DIFFUSE);
		float32 x, y, z;
		uint32 diffuse;
	};

	struct nekoVertexT
	{
		const static uint32 FVF = (D3DFVF_XYZ|D3DFVF_TEX1);
		float32 x, y, z;
		float32 u, v;
	};

	struct nekoVertex2T
	{
		const static uint32 FVF = (D3DFVF_XYZ|D3DFVF_TEX2);
		float32 x, y, z;
		float32 u1, v1;
		float32 u2, v2;
	};

	struct nekoCamera
	{
		float32 mScaleX;
		float32 mScaleY;
		float32 mRotation;
		nekoPointF mCenterPos;
		D3DXMATRIX matCamera;

		nekoCamera() { Clear(); }
		void Clear();
		void Update();
	};

	class nekoVideoDriver : public nekoIUnknown
	{
	protected:

		D3DPRESENT_PARAMETERS mParams;
		IDirect3D9 *mD3D9;
		IDirect3DDevice9 *mD3DDevice;
		ID3DXSprite *mSprite;
		ID3DXFont *mDefaultFont;

		/// 임시로..
		IDirect3DTexture9 *mTempTarget;
		IDirect3DSurface9 *mTempTargetSurface;
		IDirect3DSurface9 *mBackupSurface;

		nekoLockSection mLockSection;

		uint32 mClearColor;

		nekoCamera mCamera;
		bool mCameraEnabled;

		bool mTRRendering;

		// Base World Matrix
		D3DXMATRIX mBaseMatrix;

	public:
		inline IDirect3DDevice9 *GetDevice() { return mD3DDevice; }
		
		/// Constructor
		nekoVideoDriver(int32 width, int32 height);
		
		/// Destructor
		virtual ~nekoVideoDriver();

		/// Initialize
		result Initialize(HWND hWnd, int32 width, int32 height, bool fullscreen);

		inline IDirect3DDevice9 *GetHandle() { return mD3DDevice; }
		inline ID3DXFont *GetDefaultFont() { return mDefaultFont; }
		inline ID3DXSprite *GetSprite() { return mSprite; }

		inline void Lock() { mLockSection.Begin(); }
		inline void Unlock() { mLockSection.End(); }

		/// On Device Lost
		void OnDeviceLost();

		/// On Device Reset
		void OnDeviceReset();

		/// On Window changed.
		/// (width or height == -1) don't affect.
		result OnWindowChanged(int32 width, int32 height, bool fullscreen);

		void DrawRectangle(int32 x, int32 y, int32 width, int32 height, uint32 color, float32 ZOrder = 0.5f);
		void DrawText(nekoHWFont *font, int32 x, int32 y, int32 width, int32 height, const char8 *txt, uint32 color = 0xffffffff, uint32 flags = 0, float32 z = 0.0f);
		void CalculateText(nekoHWFont *font, const char8 *text, nekoRectangle &resultRect, uint32 flags = 0);
		void CalculateTextZ(ID3DXFont *fontHandle, const char8 *text, nekoRectangle &resultRect, uint32 flags = 0);
		void DrawImage(nekoHWTexture *texture, int32 x, int32 y, int32 width, int32 height, uint32 color = 0xffffffff, 
			int32 srcX = 0, int32 srcY = 0, int32 srcWidth = -1, int32 srcHeight = -1, float32 rotAngle = 0.0f, float32 scaleX = 1, float32 scaleY = 1, float32 z = 0.5f);

		// * 구버전 그래픽카드에서는 매우 크기가 큰 텍스쳐에 대해서 출력하는데 문제가 있다.
		void DrawImageZ(IDirect3DTexture9 *texture, int32 x, int32 y, int32 width, int32 height, uint32 color = 0xffffffff, 
			int32 srcX = 0, int32 srcY = 0, int32 srcWidth = -1, int32 srcHeight = -1, float32 rotAngle = 0.0f, float32 scaleX = 1, float32 scaleY = 1, float32 z = 0.5f);

		result BeginRender();
		result EndRender();
		inline void BeginSprite(uint32 flags = 0) { GetSprite()->Begin(D3DXSPRITE_ALPHABLEND|D3DXSPRITE_SORT_DEPTH_FRONTTOBACK|flags);}
		inline void EndSprite() { GetSprite()->End(); }
		inline bool IsTargetRendering() { return mTRRendering; }
		inline float32 GetTRRatio() { return (320/800.0f); }

		void SetBitmapCache(HDC hdc, int32 width, int32 height);
		result FlushBitmapCache(HDC hdc, nekoHWTexture *outTexture);

		void SetEnvironment();

		inline uint32 GetClearColor() { return mClearColor; }
		inline void SetClearColor(uint32 clearColor) { mClearColor = clearColor; }
		inline bool IsFullScreen() { return !mParams.Windowed; }

		result BeginTargetRender();
		void EndTargetRender();
		inline IDirect3DSurface9 *GetTempTargetSurface() { return mTempTargetSurface;}

		inline nekoCamera &GetCamera() { return mCamera; }
		void EnableCamera(bool value) { mCameraEnabled = value; }
		inline bool IsCameraEnabled() const { return mCameraEnabled; }

		result ConvertGrayscaleToAlphaMap(nekoHWTexture *tex);

		inline const D3DPRESENT_PARAMETERS &GetParams() { return mParams; }

		inline const D3DXMATRIX &GetBaseMatrix() { return mBaseMatrix; }
	};
}


#endif // __nekoVideoDriver_h__