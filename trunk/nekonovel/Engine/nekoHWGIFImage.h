// ===============================================================
//  nekoHWGIFImage - 2009.7.6
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoHWGIFImage_h__
#define __nekoHWGIFImage_h__

#include <d3d9.h>

#include "nekoPoint.h"
#include "nekoAutoPtr.h"
#include "nekoIHWResource.h"

namespace neko
{

	struct nekoGIFColor;
	class nekoGIFImage : public nekoIUnknown
	{
	protected:

		nekoAutoPtr<IDirect3DTexture9> mTexture;

	public:
		// standard members:
		int mWidth, mHeight;		// Dimensions in pixels
		int BPP;					// Bits Per Pixel	
		char *Raster;				// Bits of Raster Data (Byte Aligned)
		nekoGIFColor *Palette;		// Color Map
		int BytesPerRow;			// Width (in bytes) including alignment!
		int Transparent;			// Index of Transparent color (-1 for none)
		// Extra members for animations:
		int32 mX, mY;				// Relative Position
		float32 mDelay;				// Delay after image in 1/1000 seconds.
		int Transparency;			// Animation Transparency.

		// constructor and destructor:
		nekoGIFImage() : Raster(0), Palette(0) {}
		~nekoGIFImage() { }

		void Init (int iWidth, int iHeight, int iBPP);

		result OnReady();
		inline IDirect3DTexture9 *GetTexture() { return mTexture; }
		inline void ClearRaster() { SafeDeleteArray(Raster); }
	};

	class nekoHWGIFImage : public nekoIHWResource
	{
	protected:

		nekoPoint mSize;
		bool mTransparentEnabled;

		list< nekoAutoPtr<nekoGIFImage> > mImageList;
		inline void _AddImage(nekoGIFImage *image)
		{
			mImageList.push_back(image);
		}

		result _LoadGIF(const char8 *fileName);

		void _ApplyTransparency(nekoGIFImage *pImage, short nColorIndex);

	public:

		/// Constructor
		nekoHWGIFImage(nekoHWResourceManager *resMgr, const char8 *fileName);

		/// Destructor
		~nekoHWGIFImage();

		inline const nekoPoint &GetSize() { return mSize; }
		inline bool IsTransparentEnabled() { return mTransparentEnabled; }
		inline list< nekoAutoPtr<nekoGIFImage> > &GetImageList() { return mImageList; }

		/// 실제로 로드/언로드를 시도하는 메소드
		virtual result OnCatching();
		virtual result OnDropping();
	};
}


#endif // __nekoHWGIFImage_h__