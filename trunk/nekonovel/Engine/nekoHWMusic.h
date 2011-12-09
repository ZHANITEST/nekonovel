// ===============================================================
//  nekoHWTexture - 2009.7.6
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoHWMusic_h__
#define __nekoHWMusic_h__

#include "nekoPoint.h"
#include "nekoAutoPtr.h"
#include "nekoIHWResource.h"

namespace audiere
{
	class OutputStream;
}

using namespace audiere;

namespace neko
{
	class nekoAudiereListener;
	class nekoHWMusic : public nekoIHWResource
	{
	protected:

		OutputStream *mHandle;
		float32 mVolume;
		uint32 mRepeatCount;
		bool mPlayWhenLoad;

		nekoAudiereListener *mListener;

	public:

		/// Constructor
		nekoHWMusic(nekoHWResourceManager *resMgr, const char8 *fileName, uint32 repeat, float32 volume);

		/// Destructor
		~nekoHWMusic();

		inline OutputStream *GetHandle() { return mHandle; }

		/// On Device Lost
		virtual void OnDeviceLost();

		/// On Device Reset
		virtual void OnDeviceReset();

		/// 실제로 로드/언로드를 시도하는 메소드
		result OnCatching();
		result OnDropping();

		/// 반복 설정
		void SetRepeat(uint32 repeat);
		inline bool IsRepeat() { return (mRepeatCount > 1); }
		inline uint32 GetRepeatCount() { return mRepeatCount; }

		/// 볼륨 설정
		void SetVolume(float32 volume);
		inline float32 GetVolume() { return mVolume; }

		/// 재생중인가 ? 혹은, 재생할 예정인가 ?
		bool IsPlaying();

		/// 정지 부탁!
		void Stop(bool pause = false);

		/// 재생 부탁!
		void Play();

		void UpdateInThread(float32 frameTime);

		/// 플레이 종료시!
		void OnEnded();
	};
}


#endif // __nekoHWTexture_h__