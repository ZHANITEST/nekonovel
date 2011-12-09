#include "nekoPrecompiledHeader.h"
#include "nekoVoice.h"
#include "NekoNovel.h"
#include "nekoHWResourceManager.h"
#include "nekoLogger.h"

namespace neko
{
	nekoVoiceInstance::nekoVoiceInstance(nekoIBlock *block)
		: nekoIBlockInstance(block)
	{

	}

	nekoVoiceInstance::~nekoVoiceInstance()
	{
		if(mMusic)
			mMusic->Stop();

	}

	void nekoVoiceInstance::SetMusic(const char8 *fileName, bool repeat)
	{
		if(mMusic)
		{
			mMusic->Stop();
		}

		mMusic = GetNekoNovel()->GetHWResourceManager()->LoadMusic((string("보이스\\") + fileName).c_str(),
			(repeat) ? -1 : 1, 
			GetNekoNovel()->GetCurrentGame()->GetEnvironment().mDefVoiceVolume / 100.0f
			);

		if(mMusic)
		{
			mAlpha = (mMusic->GetVolume()) * 255;
		}
	}

	void nekoVoiceInstance::Update(float32 frameTime)
	{

	}

	void nekoVoiceInstance::Draw()
	{

		//GetNekoNovel()->GetVideoDriver()->DrawImage(mTexture, mBoundingBox.left, mBoundingBox.top, mTexture->GetSize().x, mTexture->GetSize().y);
	}

	void nekoVoiceInstance::Play()
	{
		if(mMusic)
			mMusic->Play();
	}

	void nekoVoiceInstance::Stop(bool pause)
	{
		if(mMusic)
		{
			mMusic->Stop(pause);
		}
	}

	void nekoVoiceInstance::SetAlpha(int32 alpha)
	{
		nekoIBlockInstance::SetAlpha(alpha);

		if(mMusic)
			mMusic->SetVolume((float32)alpha / 255.0f);
	}

	/// 데이터 덤프하기
	result nekoVoiceInstance::Dump(int32 version, nekoByteBuffer &outBuf)
	{
		nekoIBlockInstance::Dump(version, outBuf);

		outBuf << ((mMusic) ? mMusic->GetName() : "");
		if(mMusic)
		{
			outBuf << mMusic->IsRepeat();
		}

		return S_OK;
	}

	result nekoVoiceInstance::Load(int32 version, nekoByteBuffer &inBuf)
	{
		nekoIBlockInstance::Load(version, inBuf);
		string path;
		inBuf >> path;

		if(!path.empty())
		{
			bool repeat;
			inBuf >> repeat;

			mMusic = GetNekoNovel()->GetHWResourceManager()->LoadMusic(path.c_str(), repeat, (float32)mAlpha / 255.0f);

		}

		return S_OK;
	}

	/// Constructor
	nekoVoice::nekoVoice()
		: nekoIBlock(BLOCK_VOICE)
	{

	}

	/// Destructor
	nekoVoice::~nekoVoice()
	{

	}

	/// 인스턴스를 만든다
	nekoIBlockInstance *nekoVoice::CreateInstance()
	{
		return nekoNew nekoVoiceInstance(this);
	}
}