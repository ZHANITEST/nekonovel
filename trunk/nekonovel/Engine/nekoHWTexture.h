// ===============================================================
//  nekoHWTexture - 2009.7.6
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoHWTexture_h__
#define __nekoHWTexture_h__

#include <d3d9.h>

#include "nekoPoint.h"
#include "nekoAutoPtr.h"
#include "nekoIHWResource.h"

namespace neko
{
	class nekoHWTexture : public nekoIHWResource
	{
	protected:

		char8 *mData;
		int32 mDataLength;
		IDirect3DTexture9 *mHandle;
		nekoPoint mVideoSize;
		nekoPoint mImageSize;
		bool mDynamicBuffer;
		uint32 mColorKey;
		bool mPreserveSize;

	public:
		
		/// Constructor
		nekoHWTexture(nekoHWResourceManager *resMgr, const char8 *fileName);
		nekoHWTexture(nekoHWResourceManager *resMgr, int32 width, int32 height, bool dynamicBuffer = true);

		/// 생성자 - 포인터는 내가 가진다!
		nekoHWTexture(nekoHWResourceManager *resMgr, char8 *data, int32 length);

		inline void SetColorKey(uint32 colorKey) { mColorKey = colorKey; }
		inline void SetPreserveSize(bool value) { mPreserveSize = value; }
		
		/// Destructor
		virtual ~nekoHWTexture();

		inline IDirect3DTexture9 *GetHandle() { return mHandle; }

		inline const nekoPoint &GetSize() { return mImageSize; }
		inline const nekoPoint &GetVideoSize() { return mVideoSize; }
		inline const nekoPointF GetSizeRatio() { return nekoPointF((float32)mImageSize.x / mVideoSize.x, (float32)mImageSize.y / mVideoSize.y); }

		/// On Device Lost
		virtual void OnDeviceLost();

		/// On Device Reset
		virtual void OnDeviceReset();

		/// 실제로 로드/언로드를 시도하는 메소드
		virtual result OnCatching();
		virtual result OnDropping();
	};
}


#endif // __nekoHWTexture_h__