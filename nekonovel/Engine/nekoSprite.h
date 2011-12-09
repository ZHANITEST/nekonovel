// ===============================================================
//  nekoSprite - 2009.7.6
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoSprite_h__
#define __nekoSprite_h__

#include "nekoIBlock.h"
#include "nekoIBlockInstance.h"
#include "nekoHWTexture.h"
#include "nekoAutoPtr.h"

namespace neko
{
	class nekoSpriteInstance : public nekoIBlockInstance
	{
	protected:

		list< nekoAutoPtr<nekoHWTexture> > mTextureList;
		list< nekoAutoPtr<nekoHWTexture> >::iterator mPlayingPos;
		nekoTimer mDelayTimer;

	public:
		
		nekoSpriteInstance(nekoIBlock *block);

		virtual ~nekoSpriteInstance();

		inline void SetDelayTimer(float32 delayTime) { mDelayTimer.SetTime(delayTime); }

		result AddTexture(const char8 *name);

		void Update(float32 frameTime);
		void Draw();

		/// 데이터 덤프하기
		result Dump(int32 version, nekoByteBuffer &outBuf);
		result Load(int32 version, nekoByteBuffer &inBuf);
	};

	class nekoSprite : public nekoIBlock
	{
	protected:


	public:
		
		/// Constructor
		nekoSprite();
		
		/// Destructor
		virtual ~nekoSprite();

		/// 인스턴스를 만든다
		nekoIBlockInstance *CreateInstance();
		
	};
}


#endif // __nekoSprite_h__