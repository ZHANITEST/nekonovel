// ===============================================================
//  nekoHWTexture - 2009.7.6
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoHWSound_h__
#define __nekoHWSound_h__



#include "nekoPoint.h"
#include "nekoAutoPtr.h"
#include "nekoIHWResource.h"

namespace audiere
{
	class SoundEffect;
}

using namespace audiere;

namespace neko
{
	class nekoHWSound : public nekoIHWResource
	{
	protected:

		SoundEffect *mHandle;
		float32 mVolume;
		bool mPlayWhenLoad;

	public:

		/// Constructor
		nekoHWSound(nekoHWResourceManager *resMgr, const char8 *fileName);

		/// Destructor
		~nekoHWSound();

		inline SoundEffect *GetHandle() { return mHandle; }

		/// 실제로 로드/언로드를 시도하는 메소드
		result OnCatching();
		result OnDropping();

		/// 정지 부탁!
		void Stop();

		/// 재생 부탁!
		void Play();

		void SetVolume(float32 volume);
		inline float32 GetVolume() { return mVolume; }
	};
}


#endif // __nekoHWTexture_h__