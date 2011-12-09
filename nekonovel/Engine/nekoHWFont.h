// ===============================================================
//  nekoHWFont - 2009.7.6
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoHWFont_h__
#define __nekoHWFont_h__

#include <d3dx9.h>
#include "nekoIHWResource.h"

namespace neko
{
	class nekoHWFont : public nekoIHWResource
	{
	protected:

		ID3DXFont *mHandle;
		int32 mWidth;
		int32 mHeight;
		bool mBold;

	public:
		
		nekoHWFont(nekoHWResourceManager *resMgr, const char8 *faceName, int32 width, int32 height, bool bold);
		virtual ~nekoHWFont();

		inline ID3DXFont *GetHandle() { return mHandle; }

		inline HDC GetDC()
		{
			if(!mHandle)
				return NULL;

			return mHandle->GetDC();
		}

		inline int32 GetWidth() { return mWidth; }
		inline int32 GetHeight() { return mHeight; }
		inline bool IsBold() { return mBold; }

		/// On Device Lost
		virtual void OnDeviceLost();

		/// On Device Reset
		virtual void OnDeviceReset();

		/// 실제로 로드/언로드를 시도하는 메소드
		virtual result OnCatching();
		virtual result OnDropping();
		
	};
}


#endif // __nekoHWFont_h__