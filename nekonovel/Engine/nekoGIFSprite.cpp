#include "nekoPrecompiledHeader.h"
#include "nekoGIFSprite.h"
#include "NekoNovel.h"
#include "nekoHWResourceManager.h"

namespace neko
{
	nekoGIFSpriteInstance::nekoGIFSpriteInstance(nekoIBlock *block)
		: nekoIBlockInstance(block)
		, mDelayTimer(0)
		, mImageList(0)
	{

	}

	nekoGIFSpriteInstance::~nekoGIFSpriteInstance()
	{

	}

	// pre-declaration:
	int LZWDecoder (char*, char*, short, int, int, int, const int);

	result nekoGIFSpriteInstance::LoadGIF(const char8 *fileName, uint32 repeatCount)
	{
		mHWGIFImage = GetNekoNovel()->GetHWResourceManager()->LoadGIF((string("이미지\\") + fileName).c_str());
		mRepeatCount = repeatCount;
		return S_OK;
	}

	void nekoGIFSpriteInstance::Update(float32 frameTime)
	{
		if(!mHWGIFImage)
			return;

		mHWGIFImage->Refresh();
		if(mHWGIFImage->GetState() != EHRS_LOADED)
		{
			if(FAILED(mHWGIFImage->GetLastReturnCode()))
				mRepeatCount = 0;
			return;
		}

		if(!mImageList)
		{
			mImageList = &mHWGIFImage->GetImageList();
			mPlayingPos = mImageList->begin();
			if(!mImageList->empty())
				mDelayTimer = (*mPlayingPos)->mDelay;
			mBoundingBox.SetSize(mHWGIFImage->GetSize().x, mHWGIFImage->GetSize().y);
		}

		if(mImageList->empty()
			|| mPlayingPos == mImageList->end()
			|| !mRepeatCount)
			return;

		mDelayTimer -= frameTime;
		if(mDelayTimer <= 0.0f)
		{
			mPlayingPos++;
			if(mPlayingPos == mImageList->end())
			{
				if(mRepeatCount > 0)
				{
					--mRepeatCount;

					if(!mRepeatCount)
					{
						--mPlayingPos;
						return;
					}

					mPlayingPos = mImageList->begin();
				}
			}
			mDelayTimer = (*mPlayingPos)->mDelay;
		}
	}

	void nekoGIFSpriteInstance::Draw()
	{
		if(mImageList && !mImageList->empty())
		{
			list< nekoAutoPtr<nekoGIFImage> >::iterator iter = (mHWGIFImage->IsTransparentEnabled()) ? mImageList->begin() : mPlayingPos;
			nekoVideoDriver *videoDrv = GetNekoNovel()->GetVideoDriver();

			for(;iter != mImageList->end();++iter)
			{
				videoDrv->DrawImageZ((*iter)->GetTexture(), mBoundingBox.left + (*iter)->mX, mBoundingBox.top + (*iter)->mY, 
					(*iter)->mWidth, (*iter)->mHeight, (mAlpha << 24) | 0xffffff, 0, 0, (*iter)->mWidth, (*iter)->mHeight, mRotation, mScale.x, mScale.y, _GetRealZ());
				
				if(iter == mPlayingPos)
					break;
			}
		}
	}

	/// 데이터 덤프하기
	result nekoGIFSpriteInstance::Dump(int32 version, nekoByteBuffer &outBuf)
	{
		nekoIBlockInstance::Dump(version, outBuf);

		outBuf << mDelayTimer;
		outBuf << mRepeatCount;
		outBuf << ((mHWGIFImage) ? mHWGIFImage->GetName() : "");

		return S_OK;
	}

	result nekoGIFSpriteInstance::Load(int32 version, nekoByteBuffer &inBuf)
	{
		nekoIBlockInstance::Load(version, inBuf);

		string name;
		inBuf >> mDelayTimer;
		inBuf >> mRepeatCount;
		inBuf >> name;

		if(!name.empty())
		{
			mHWGIFImage = GetNekoNovel()->GetHWResourceManager()->LoadGIF(name.c_str());
		}

		return S_OK;
	}

	/// Constructor
	nekoGIFSprite::nekoGIFSprite()
		: nekoIBlock(BLOCK_GIFSPRITE)
	{

	}

	/// Destructor
	nekoGIFSprite::~nekoGIFSprite()
	{

	}

	/// 인스턴스를 만든다
	nekoIBlockInstance *nekoGIFSprite::CreateInstance()
	{
		return nekoNew nekoGIFSpriteInstance(this);
	}
}