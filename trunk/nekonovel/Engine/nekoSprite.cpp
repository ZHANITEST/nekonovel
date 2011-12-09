#include "nekoPrecompiledHeader.h"
#include "nekoSprite.h"
#include "NekoNovel.h"
#include "nekoHWResourceManager.h"

namespace neko
{
	nekoSpriteInstance::nekoSpriteInstance(nekoIBlock *block)
		: nekoIBlockInstance(block)
	{

	}

	nekoSpriteInstance::~nekoSpriteInstance()
	{

	}

	result nekoSpriteInstance::AddTexture(const char8 *name)
	{
		nekoHWTexture *tex = GetNekoNovel()->GetHWResourceManager()->LoadTexture((string("이미지\\") + name).c_str());
		if(tex == NULL)
			return E_FAIL;

		mTextureList.push_back(tex);

		if(mTextureList.size() == 1)
			mPlayingPos = mTextureList.begin();
		return S_OK;
	}

	void nekoSpriteInstance::Update(float32 frameTime)
	{
		if(mTextureList.size() == 0)
			return;

		if(GetNekoNovel()->GetCurrentGame()->GetGameMode() == GAME_RUN_PREVIEW)
			return;

		mDelayTimer.Update(frameTime);
		if(mDelayTimer.IsCompleted())
		{
			mPlayingPos++;
			if(mPlayingPos == mTextureList.end())
				mPlayingPos = mTextureList.begin();
		}
	}

	void nekoSpriteInstance::Draw()
	{
		if(mTextureList.size())
		{
			if(mFlags & EBIF_IGNORE_CAMERA)
				GetNekoNovel()->GetVideoDriver()->EnableCamera(false);

			nekoHWTexture *tex = (*mPlayingPos);
			GetNekoNovel()->GetVideoDriver()->DrawImage(tex, mBoundingBox.left, mBoundingBox.top,
			tex->GetSize().x, tex->GetSize().y, 
			D3DCOLOR_ARGB(mAlpha, 255, 255, 255), 0, 0, -1, -1, mRotation, mScale.x, mScale.y, _GetRealZ());

			if(mFlags & EBIF_IGNORE_CAMERA)
				GetNekoNovel()->GetVideoDriver()->EnableCamera(true);
		}
	}

	/// 데이터 덤프하기
	result nekoSpriteInstance::Dump(int32 version, nekoByteBuffer &outBuf)
	{
		nekoIBlockInstance::Dump(version, outBuf);

		outBuf << mDelayTimer.GetTimerTime();
		outBuf << (uint32)mTextureList.size();
		list< nekoAutoPtr<nekoHWTexture> >::iterator iter = mTextureList.begin();
		for(;iter != mTextureList.end();++iter)
			outBuf << ((*iter) ? (*iter)->GetName() : "");

		return S_OK;
	}

	result nekoSpriteInstance::Load(int32 version, nekoByteBuffer &inBuf)
	{
		nekoIBlockInstance::Load(version, inBuf);

		float32 time;
		inBuf >> time; mDelayTimer.SetTime(time);

		uint32 count;
		string path;

		inBuf >> count;
		for(uint32 i = 0;i < count;++i)
		{
			inBuf >> path;
			if(!path.empty())
			{
				mTextureList.push_back(GetNekoNovel()->GetHWResourceManager()->LoadTexture(path.c_str()));
			}
		}

		if(count > 0)
		{
			mPlayingPos = mTextureList.begin();
		}

		return S_OK;
	}

	/// Constructor
	nekoSprite::nekoSprite()
		: nekoIBlock(BLOCK_SPRITE)
	{

	}

	/// Destructor
	nekoSprite::~nekoSprite()
	{

	}

	/// 인스턴스를 만든다
	nekoIBlockInstance *nekoSprite::CreateInstance()
	{
		return nekoNew nekoSpriteInstance(this);
	}
}