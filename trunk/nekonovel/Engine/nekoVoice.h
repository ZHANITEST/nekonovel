// ===============================================================
//  nekoVoice - 2009.7.26
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoVoice_h__
#define __nekoVoice_h__

#include "nekoIBlock.h"
#include "nekoHWMusic.h"
#include "nekoIBlockInstance.h"
#include "nekoUtility.h"

namespace neko
{
	class nekoVoiceInstance : public nekoIBlockInstance
	{
	protected:

		nekoAutoPtr<nekoHWMusic> mMusic;

	public:

		nekoVoiceInstance(nekoIBlock *block);
		virtual ~nekoVoiceInstance();

		void SetMusic(const char8 *fileName, bool repeat);

		virtual void Update(float32 frameTime);
		virtual void Draw();

		void Play();
		void Stop(bool pause = false);

		void SetAlpha(int32 alpha);

		/// 데이터 덤프하기
		result Dump(int32 version, nekoByteBuffer &outBuf);
		result Load(int32 version, nekoByteBuffer &inBuf);

		inline bool IsPlaying()
		{
			return (mMusic && mMusic->IsPlaying());
		}

		inline const char8 *GetFileName()
		{
			return ((mMusic) ? nekoUtility::GetFileName(mMusic->GetName()) : 0);
		}
	};

	class nekoVoice : public nekoIBlock
	{
	protected:

	public:

		nekoVoice();
		virtual ~nekoVoice();

		/// 인스턴스를 만든다
		virtual nekoIBlockInstance *CreateInstance();
	};
}


#endif // __nekoVoice_h__