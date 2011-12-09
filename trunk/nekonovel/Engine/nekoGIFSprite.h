// ===============================================================
//  nekoGIFSprite - 2009.7.26
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoGIFSprite_h__
#define __nekoGIFSprite_h__

#include "nekoIBlock.h"
#include "nekoIBlockInstance.h"
#include "nekoHWTexture.h"
#include "nekoAutoPtr.h"
#include "nekoHWGIFImage.h"

namespace neko
{

	class nekoGIFSpriteInstance : public nekoIBlockInstance
	{
	protected:

		nekoAutoPtr<nekoHWGIFImage> mHWGIFImage;
		list< nekoAutoPtr<nekoGIFImage> > *mImageList;
		list< nekoAutoPtr<nekoGIFImage> >::iterator mPlayingPos;
		float32 mDelayTimer;
		uint32 mRepeatCount;

	public:

		nekoGIFSpriteInstance(nekoIBlock *block);
		~nekoGIFSpriteInstance();

		result LoadGIF(const char8 *fileName, uint32 repeatCount);

		void Update(float32 frameTime);
		void Draw();

		inline bool IsPlaying() { return (mRepeatCount != 0); }

		/// 데이터 덤프하기
		result Dump(int32 version, nekoByteBuffer &outBuf);
		result Load(int32 version, nekoByteBuffer &inBuf);
	};

	class nekoGIFSprite : public nekoIBlock
	{
	protected:


	public:

		/// Constructor
		nekoGIFSprite();

		/// Destructor
		virtual ~nekoGIFSprite();

		/// 인스턴스를 만든다
		nekoIBlockInstance *CreateInstance();

	};
}


#endif // __nekoGIFSprite_h__