#include "nekoPrecompiledHeader.h"
#include "nekoImage.h"
#include "nekoNovelReal.h"

namespace neko
{
	nekoImageInstance::nekoImageInstance(nekoIBlock *block)
		: nekoIBlockInstance(block)
		, mImageType(IMAGE_NORMAL)
		, mIncludeInSnapShot(true)
		, hue(0xffffffff)
		, mFirstFadeIn(false)
		, mLastFadeOut(false)
	{
			
	}

	nekoImageInstance::~nekoImageInstance()
	{

	}

	void nekoImageInstance::SetImage(const char8 *fileName)
	{
		// 같은 이미지일 경우 배제!
		if(mTexture && stricmp(nekoUtility::GetFileName(mTexture->GetName()), fileName) == 0)
			return;

		if(!fileName)
		{
			mTexture = NULL;
			return;
		}

		mTransAlpha = 0.0f;

		// 이미지 전환을 요구한다.
		if(GetNekoNovel()->GetCurrentGame()->GetGameMode() != GAME_RUN_PREVIEW &&
			mTexture)
		{
			mNextTexture = GetNekoNovel()->GetHWResourceManager()->LoadTexture((string("이미지\\") + fileName).c_str());
			mFirstFadeIn = false;
		}
		else
		{
			mTexture = GetNekoNovel()->GetHWResourceManager()->LoadTexture((string("이미지\\") + fileName).c_str());

			// 초기 CG 페이드인을 실행할 것인가 ?
			if(GetNekoNovel()->GetCurrentGame()->GetGameMode() != GAME_RUN_PREVIEW &&
				!IsFloatZero(GetNekoNovel()->GetCurrentGame()->GetEnvironment().mCGFadeInSpeed))
				mFirstFadeIn = true;
			else
				mFirstFadeIn = false;
		}
	}

	void nekoImageInstance::SetName(const char8 *name)
	{
		nekoIBlockInstance::SetName(name);

		if(stricmp(name, "페이스CG") == 0)
		{
			mZOrder = 49;
			mImageType = IMAGE_FACECG;
		}
	}

	void nekoImageInstance::Update(float32 frameTime)
	{
		if(mTexture && (!mBoundingBox.GetWidth() || !mBoundingBox.GetHeight()))
		{
			mTexture->Refresh();
			mBoundingBox.SetSize(mTexture->GetSize().x, mTexture->GetSize().y);
		}

		// 전환할 녀석이 있는 경우..
		if(mNextTexture)
		{
			mTransAlpha += frameTime * 500.0f;
			if(mTransAlpha > 335)
			{
				(mTexture = mNextTexture)->AddRef();
				mNextTexture = NULL;

				mTransAlpha = 0.0F;
			}
		}

		if(mFirstFadeIn)
		{
			mTransAlpha += frameTime * GetNekoNovel()->GetCurrentGame()->GetEnvironment().mCGFadeInSpeed;
			if(mTransAlpha > 255.0f)
			{
				mFirstFadeIn = false;

				mTransAlpha = 0.0f;
			}
		}

		else if(mLastFadeOut)
		{
			mTransAlpha -= frameTime * GetNekoNovel()->GetCurrentGame()->GetEnvironment().mCGFadeOutSpeed;
			if(mTransAlpha < 0.0f)
			{
				mLastFadeOut = false;
				mTransAlpha = 0.0f;
				mAlpha = 0.0f;
			}
		}
	}

	void nekoImageInstance::Draw()
	{
		if(mTexture)
		{
			nekoVideoDriver *videoDrv = GetNekoNovel()->GetVideoDriver();

			if(mImageType == IMAGE_FACECG)
			{
				nekoGUIChatBox *chatBox = GetNekoNovel()->GetCurrentGame()->GetChatBox();
				videoDrv->EnableCamera(false);

				int32 alpha = chatBox->GetAlpha() * mAlpha / 255.0f;

				// 전환할 텍스쳐가 있는 경우..
				if(mNextTexture)
				{
					int32 alpha2;

					alpha2 = alpha * (255 - max(mTransAlpha - 80, 0)) / 255.0f;
					videoDrv->DrawImage(mTexture,
						mBoundingBox.left,
						mBoundingBox.top,
						-1, -1,
						MixColorsAlpha(hue, alpha2), 0, 0, -1, -1, mRotation, mScale.x, mScale.y, _GetRealZ());

					

					alpha2 = alpha * min(mTransAlpha, 255) / 255.0f;
					videoDrv->DrawImage(mNextTexture,
						mBoundingBox.left,
						mBoundingBox.top,
						-1, -1,
						MixColorsAlpha(hue, alpha2), 0, 0, -1, -1, mRotation, mScale.x, mScale.y, _GetRealZ() - REAL_Z_DIFF);
				}
				else
				{
					videoDrv->DrawImage(mTexture,
						mBoundingBox.left,
						mBoundingBox.top,
						-1, -1,
						(mFirstFadeIn || mLastFadeOut) ? MixColorsAlpha(hue, (int32)mTransAlpha) : MixColorsAlpha(hue, alpha),
						0, 0, -1, -1, mRotation, mScale.x, mScale.y, _GetRealZ());
				}

				videoDrv->EnableCamera(true);
			}
			else
			{
				if(mFlags & EBIF_IGNORE_CAMERA)
					videoDrv->EnableCamera(false);

				// 전환할 텍스쳐가 있는 경우..
				if(mNextTexture)
				{
					int32 alpha;

					alpha = mAlpha * (255 - max(mTransAlpha - 80, 0)) / 255.0f;
					videoDrv->DrawImage(mTexture, mBoundingBox.left, mBoundingBox.top, -1, -1,
						MixColorsAlpha(hue, alpha), 0, 0, -1, -1, mRotation, mScale.x, mScale.y, _GetRealZ());

					alpha = mAlpha * min(mTransAlpha, 255) / 255.0f;
					videoDrv->DrawImage(mNextTexture, mBoundingBox.left, mBoundingBox.top, -1, -1,
						MixColorsAlpha(hue, alpha), 0, 0, -1, -1, mRotation, mScale.x, mScale.y, _GetRealZ() - REAL_Z_DIFF);
				}
				else
				{
					videoDrv->DrawImage(mTexture,
						mBoundingBox.left,
						mBoundingBox.top,
						-1, -1,
						(mFirstFadeIn || mLastFadeOut) ? MixColorsAlpha(hue, (int32)mTransAlpha) : MixColorsAlpha(hue, mAlpha),
						0, 0, -1, -1, mRotation, mScale.x, mScale.y, _GetRealZ());
				}

				if(mFlags & EBIF_IGNORE_CAMERA)
					videoDrv->EnableCamera(true);
			}
		}
	}

	/// 데이터 덤프하기
	result nekoImageInstance::Dump(int32 version, nekoByteBuffer &outBuf)
	{
		nekoIBlockInstance::Dump(version, outBuf);

		outBuf << mImageType
		 << ((mTexture) ? mTexture->GetName() : "")
		 << ((mNextTexture) ? mNextTexture->GetName() : "")
		 << mIncludeInSnapShot
		 << hue;

		return S_OK;
	}

	result nekoImageInstance::Load(int32 version, nekoByteBuffer &inBuf)
	{
		nekoIBlockInstance::Load(version, inBuf);

		string path;
		inBuf >> mImageType;
		inBuf >> path;

		if(!path.empty())
		{
			mTexture = GetNekoNovel()->GetHWResourceManager()->LoadTexture(path.c_str());
		}

		if(version >= 1107)
		{
			inBuf >> path;

			if(!path.empty())
			{
				mNextTexture = GetNekoNovel()->GetHWResourceManager()->LoadTexture(path.c_str());
			}
		}

		if(version >= 1105)
			inBuf >> mIncludeInSnapShot;

		if(version >= 1110)
			inBuf >> hue;

		return S_OK;
	}

	/// 제거 요청
	bool nekoImageInstance::RequestToRemove()
	{
		if(GetNekoNovel()->GetCurrentGame()->GetGameMode() != GAME_RUN_PREVIEW &&
			!IsFloatZero(GetNekoNovel()->GetCurrentGame()->GetEnvironment().mCGFadeOutSpeed))
		{
			mTransAlpha = mAlpha;
			mLastFadeOut = true;
		}
		else
			return false;

		return true;
	}

	/// Constructor
	nekoImage::nekoImage()
		: nekoIBlock(BLOCK_IMAGE)
	{

	}

	/// Destructor
	nekoImage::~nekoImage()
	{

	}

	/// 인스턴스를 만든다
	nekoIBlockInstance *nekoImage::CreateInstance()
	{
		return nekoNew nekoImageInstance(this);
	}
}