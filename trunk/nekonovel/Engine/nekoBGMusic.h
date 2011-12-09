// ===============================================================
//  nekoBGMusic - 2009.7.6
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoBGMusic_h__
#define __nekoBGMusic_h__

#include "nekoIBlock.h"
#include "nekoHWMusic.h"
#include "nekoIBlockInstance.h"

namespace neko
{
	class nekoBGMusicInstance : public nekoIBlockInstance
	{
	protected:

		nekoAutoPtr<nekoHWMusic> mMusic;

	public:

		nekoBGMusicInstance(nekoIBlock *block);
		virtual ~nekoBGMusicInstance();

		void SetMusic(const char8 *fileName, uint32 repeat);

		virtual void Update(float32 frameTime);
		virtual void Draw();

		void Play();
		void Stop(bool pause = false);

		void SetAlpha(int32 alpha);

		/// 데이터 덤프하기
		result Dump(int32 version, nekoByteBuffer &outBuf);
		result Load(int32 version, nekoByteBuffer &inBuf);
	};

	class nekoBGMusic : public nekoIBlock
	{
	protected:

	public:

		nekoBGMusic();
		virtual ~nekoBGMusic();

		/// 인스턴스를 만든다
		virtual nekoIBlockInstance *CreateInstance();
	};
}


#endif // __nekoBGMusic_h__