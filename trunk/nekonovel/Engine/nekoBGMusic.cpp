#include "nekoPrecompiledHeader.h"
#include "nekoBGMusic.h"
#include "NekoNovel.h"
#include "nekoHWResourceManager.h"
#include "nekoLogger.h"

namespace neko
{
	nekoBGMusicInstance::nekoBGMusicInstance(nekoIBlock *block)
		: nekoIBlockInstance(block)
	{
		
	}

	nekoBGMusicInstance::~nekoBGMusicInstance()
	{
		if(mMusic)
			mMusic->Stop();

	}

	void nekoBGMusicInstance::SetMusic(const char8 *fileName, uint32 repeat)
	{
		if(mMusic)
		{
			mMusic->Stop();
		}

		mAlpha = 255;

		mMusic = GetNekoNovel()->GetHWResourceManager()->LoadMusic(
			(string("배경음악\\") + fileName).c_str(),
			repeat,
			(GetNekoNovel()->GetCurrentGame()->GetEnvironment().mDefBGMVolume / 100.0f)
			);
	}

	void nekoBGMusicInstance::Update(float32 frameTime)
	{
		
	}

	void nekoBGMusicInstance::Draw()
	{

	}

	void nekoBGMusicInstance::Play()
	{
		if(mMusic)
			mMusic->Play();
	}

	void nekoBGMusicInstance::Stop(bool pause)
	{
		if(mMusic)
		{
			mMusic->Stop(pause);
		}
	}

	void nekoBGMusicInstance::SetAlpha(int32 alpha)
	{
		nekoIBlockInstance::SetAlpha(alpha);

		if(mMusic)
			mMusic->SetVolume((float32)alpha / 255.0f *
			(GetNekoNovel()->GetCurrentGame()->GetEnvironment().mDefBGMVolume / 100.0f));
	}

	/// 데이터 덤프하기
	result nekoBGMusicInstance::Dump(int32 version, nekoByteBuffer &outBuf)
	{
		nekoIBlockInstance::Dump(version, outBuf);

		outBuf << ((mMusic) ? mMusic->GetName() : "");
		if(mMusic)
		{
			outBuf << mMusic->GetRepeatCount();
		}

		return S_OK;
	}

	result nekoBGMusicInstance::Load(int32 version, nekoByteBuffer &inBuf)
	{
		nekoIBlockInstance::Load(version, inBuf);
		string path;
		inBuf >> path;

		if(!path.empty())
		{
			uint32 repeatCount;

			if(version >= 1127)
			{
				inBuf >> repeatCount;
			}
			else
			{
				bool repeat;
				inBuf >> repeat;
				repeatCount = (repeat) ? -1 : 1;
			}

			mMusic = GetNekoNovel()->GetHWResourceManager()->LoadMusic(path.c_str(),
				repeatCount, 
				(float32)mAlpha / 255.0f *
				(GetNekoNovel()->GetCurrentGame()->GetEnvironment().mDefBGMVolume / 100.0f)
				);
		}

		return S_OK;
	}

	/// Constructor
	nekoBGMusic::nekoBGMusic()
		: nekoIBlock(BLOCK_BGMUSIC)
	{

	}

	/// Destructor
	nekoBGMusic::~nekoBGMusic()
	{
		
	}

	/// 인스턴스를 만든다
	nekoIBlockInstance *nekoBGMusic::CreateInstance()
	{
		return nekoNew nekoBGMusicInstance(this);
	}
}