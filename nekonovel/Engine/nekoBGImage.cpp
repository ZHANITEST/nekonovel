#include "nekoPrecompiledHeader.h"
#include "nekoBGImage.h"
#include "nekoNovelReal.h"
#include "nekoUtility.h"

namespace neko
{
	nekoBGImageInstance::nekoBGImageInstance(nekoIBlock *block)
		: nekoIBlockInstance(block)
		, mEffectType(EBET_NONE)
		, mEffectSpeed(1.0f)
		, mZeroAlphaDeleting(true)
		, hue(0xffffffff)
	{
		mBoundingBox = nekoRectangle(0, 0, GetNekoNovel()->GetWidth(), GetNekoNovel()->GetHeight());
	}

	nekoBGImageInstance::~nekoBGImageInstance()
	{

	}

	void nekoBGImageInstance::SetImage(const char8 *fileName)
	{
		if(fileName == NULL)
		{
			mTexture = NULL;
			return;
		}

		mTexture = GetNekoNovel()->GetHWResourceManager()->LoadTexture((string("이미지\\") + fileName).c_str());

		mAlpha = 255;
		mRotation = 0;
		mScale = nekoPointF(1, 1);
	}
	
	void nekoBGImageInstance::Update(float32 frameTime)
	{
		switch(mEffectType)
		{
		case EBET_BLOCK:

			if(mEffectFadeIn)
			{
				mTempFloat = mTempFloat + frameTime * 1000.0f * mEffectSpeed;
				if(mTempFloat >= 255.0f)
				{
					++mTempInteger;
					mTempFloat = 0.0f;
				}
			}
			else
			{
				mTempFloat = max(mTempFloat - frameTime * 1000.0f * mEffectSpeed, 0);
				if(mTempFloat <= 0.0f)
				{
					++mTempInteger;
					mTempFloat = 255.0f;
				}
			}

			if(mTempInteger >= 25)
			{
				mEffectType = EBET_NONE;
				mAlpha = (mEffectFadeIn) ? 255 : 0;

				OnEffectFinish(mEffectFadeIn);
			}
			break;
		case EBET_FADE:
			if(mEffectFadeIn)
			{
				mTempFloat = mTempFloat + frameTime * 100.0f * mEffectSpeed;
				if(mTempFloat >= 255.0f)
				{
					mEffectType = EBET_NONE;
					mAlpha = 255;

					OnEffectFinish(true);
				}
				else
				{
					mAlpha = (int32)mTempFloat;
				}
			}
			else
			{
				mTempFloat = max(mTempFloat - frameTime * 100.0f * mEffectSpeed, 0);
				if(mTempFloat <= 0.0f)
				{
					mEffectType = EBET_NONE;
					mAlpha = 0;

					OnEffectFinish(false);
				}
				else
				{
					mAlpha = (int32)mTempFloat;
				}
			}
			break;
		case EBET_LEFTCURTAIN:
		case EBET_RIGHTCURTAIN:

			if(mEffectFadeIn)
			{
				mTempFloat = mTempFloat + frameTime * 100.0f * mEffectSpeed;
			}
			else
			{
				mTempFloat = mTempFloat - frameTime * 100.0f * mEffectSpeed;
			}

			break;
		case EBET_LEFTSWEEP:
		case EBET_RIGHTSWEEP:
		case EBET_UPSWEEP:
		case EBET_DOWNSWEEP:

			mTempFloat = mTempFloat + frameTime * 200.0f * mEffectSpeed;
			if(mTempFloat >= 100.0f)
			{
				mEffectType = EBET_NONE;
				if(mEffectFadeIn)
					mAlpha = 255.0f;
				else
					mAlpha = 0.0f;

				OnEffectFinish(mEffectFadeIn);
			}

			break;
		case EBET_LEFTSLIDE:
		case EBET_UPSLIDE:
		case EBET_RIGHTSLIDE:
		case EBET_DOWNSLIDE:
		case EBET_4SLICE:
		case EBET_ROTATINGAWAY:

			if(mEffectFadeIn)
			{
				mTempFloat = mTempFloat + frameTime * 200.0f * mEffectSpeed;
				if(mTempFloat > 100.0f)
				{
					mEffectType = EBET_NONE;
					OnEffectFinish(true);
					mAlpha = 255;
				}
			}
			else
			{
				mTempFloat = mTempFloat - frameTime * 200.0f * mEffectSpeed;
				if(mTempFloat < -100.0f)
				{
					mEffectType = EBET_NONE;
					mAlpha = 0;

					OnEffectFinish(false);
				}
			}
			break;
		case EBET_FROMRULE:

			if(mEffectFadeIn)
			{
				mTempFloat = mTempFloat + frameTime * 200.0f * mEffectSpeed;
				if(mTempFloat > 100.0f)
				{
					mEffectType = EBET_NONE;
					OnEffectFinish(true);
					mAlpha = 255;
				}
			}
			else
			{
				mTempFloat = mTempFloat - frameTime * 200.0f * mEffectSpeed;
				if(mTempFloat < -100.0f)
				{
					mEffectType = EBET_NONE;
					mAlpha = 0;

					OnEffectFinish(false);
				}
			}
			break;
		case EBET_ZOOMIN:
			mTempFloat = mTempFloat + frameTime * mEffectSpeed * 10.0F;
			if(mTempFloat >= 15.0f)
			{
				mEffectType = EBET_NONE;

				if(mEffectFadeIn)
					mAlpha = 255.0f;
				else
					mAlpha = 0.0f;

				OnEffectFinish(mEffectFadeIn);
			}
			break;
		}
	}

	void nekoBGImageInstance::Draw()
	{
		nekoVideoDriver *videoDrv = GetNekoNovel()->GetVideoDriver();

		float32 ZOrder = 0.6f + (GetZOrder() - 200) * 0.0001f - REAL_Z_DIFF;

		if(mFlags & EBIF_IGNORE_CAMERA)
			videoDrv->EnableCamera(false);

		switch(mEffectType)
		{
		case EBET_BLOCK:
			{
				int32 width = mBoundingBox.GetWidth() / 5;
				int32 height = mBoundingBox.GetHeight() / 5;
				float32 srcWidth = mTexture->GetSize().x / 5.0f;
				float32 srcHeight = mTexture->GetSize().y / 5.0f;

				if(mEffectFadeIn)
				{
					int32 i;
					int32 k;
					for(int32 b = 0;b <= mTempInteger;++b)
					{
						if((int32)(b / 5) % 2 == 1)
						{
							i = (int32)(b / 5) * 5 + 5 - (b % 5 + 1);
							k = (int32)(mTempInteger / 5) * 5 + 5 - (mTempInteger % 5 + 1);
						}
						else
						{
							i = b;
							k = mTempInteger;
						}

						// 마지막일 경우에만 적용
						if(i == k)
						{
							videoDrv->DrawImage(mTexture, mBoundingBox.left + (i % 5) * width, mBoundingBox.top + (int32)(i / 5) * height, width, height,
								MixColorsAlpha(hue, (int32)mTempFloat), (i % 5) * srcWidth, (int32)(i / 5) * srcHeight, srcWidth, srcHeight,
								mRotation, mScale.x, mScale.y, ZOrder);
							break;
						}
						else
						{
							videoDrv->DrawImage(mTexture, mBoundingBox.left + (i % 5) * width, mBoundingBox.top + (int32)(i / 5) * height, width, height,
								hue, (i % 5) * srcWidth, (int32)(i / 5) * srcHeight, srcWidth, srcHeight,
								mRotation, mScale.x, mScale.y, ZOrder);
						}
					}
				}
				else
				{
					int32 i;
					int32 k;
					for(int32 b = mTempInteger;b < 25;++b)
					{
						if((int32)(b / 5) % 2 == 1)
						{
							i = (int32)(b / 5) * 5 + 5 - (b % 5 + 1);
							k = (int32)(mTempInteger / 5) * 5 + 5 - (mTempInteger % 5 + 1);
						}
						else
						{
							i = b;
							k = mTempInteger;
						}

						/// 처음에만 적용.
						if(i == k)
						{
							videoDrv->DrawImage(mTexture, mBoundingBox.left + (i % 5) * width, mBoundingBox.top + (int32)(i / 5) * height, width, height,
								MixColorsAlpha(hue, (int32)mTempFloat), (i % 5) * srcWidth, (int32)(i / 5) * srcHeight, srcWidth, srcHeight,
								mRotation, mScale.x, mScale.y, ZOrder);
						}
						else
						{
							videoDrv->DrawImage(mTexture, mBoundingBox.left + (i % 5) * width, mBoundingBox.top + (int32)(i / 5) * height, width, height,
								hue, (i % 5) * srcWidth, (int32)(i / 5) * srcHeight, srcWidth, srcHeight,
								mRotation, mScale.x, mScale.y, ZOrder);
						}
					}
				}
			}
			break;
		case EBET_LEFTCURTAIN:
			{
				float32 width = mBoundingBox.GetWidth();
				int32 height = mBoundingBox.GetHeight();
				float32 srcWidth = mTexture->GetSize().x;
				float32 w;

				for(int32 i = 0;i < 20;++i)
				{
					if(mEffectFadeIn)
						w = max(min((width / 20 - i) * mTempFloat / 100.0f, width / 20), 0);
					else
						w = max(min((width / 20 - (i - 19)) * mTempFloat / 100.0f, width / 20), 0);

					videoDrv->DrawImage(mTexture, mBoundingBox.left + width / 20 * i, mBoundingBox.top, w, height,
						hue, srcWidth / 20 * i, 0, srcWidth / 20, -1,
						mRotation, mScale.x, mScale.y, ZOrder);

					if(i == 19)
					{
						if((mEffectFadeIn && w >= width / 20) || (!mEffectFadeIn && w <= 0))
						{
							mEffectType = EBET_NONE;
							mAlpha = (mEffectFadeIn) ? 255 : 0;

							OnEffectFinish(mAlpha == 255);
							break;
						}
					}
				}
			}
			break;
		case EBET_RIGHTCURTAIN:
			{
				float32 width = mBoundingBox.GetWidth();
				int32 height = mBoundingBox.GetHeight();
				float32 srcWidth = mTexture->GetSize().x;
				float32 w;

				for(int32 i = 0;i < 20;++i)
				{
					if(mEffectFadeIn)
						w = max(min(width / 20 * mTempFloat / 100.0f - (19 - i), width / 20), 0);
					else
						w = max(min(width / 20 * mTempFloat / 100.0f - (-i), width / 20), 0);

					videoDrv->DrawImage(mTexture, mBoundingBox.left + width / 20.0f * i + (width / 20.0f) - w, mBoundingBox.top, w, height,
						hue, srcWidth / 20 * i, 0, srcWidth / 20, -1,
						mRotation, mScale.x, mScale.y, ZOrder);

					if(i == 0)
					{
						if((mEffectFadeIn && w >= width / 20) || (!mEffectFadeIn && w <= 0))
						{
							mEffectType = EBET_NONE;
							mAlpha = (mEffectFadeIn) ? 255 : 0;

							OnEffectFinish(mAlpha == 255);
						}
					}
				}
			}
			break;
		case EBET_LEFTSLIDE:
			{
				videoDrv->DrawImage(mTexture, mBoundingBox.left, mBoundingBox.top,
					mBoundingBox.GetWidth() * mTempFloat / 100.0f, mBoundingBox.GetHeight(),
						MixColorsAlpha(hue, mAlpha), 0, 0, -1, -1, mRotation, mScale.x, mScale.y, ZOrder);
			}
			break;
		case EBET_RIGHTSLIDE:
			{
				videoDrv->DrawImage(mTexture, mBoundingBox.left + mBoundingBox.GetWidth() * (100.0f - mTempFloat) / 100.0f, mBoundingBox.top,
					mBoundingBox.GetWidth() * (mTempFloat) / 100.0f, mBoundingBox.GetHeight(),
					MixColorsAlpha(hue, mAlpha), 0, 0, -1, -1, mRotation, mScale.x, mScale.y, ZOrder);
			}
			break;
		case EBET_UPSLIDE:
			{
				videoDrv->DrawImage(mTexture, mBoundingBox.left, mBoundingBox.top,
					mBoundingBox.GetWidth(), mBoundingBox.GetHeight() * mTempFloat / 100.0f,
					MixColorsAlpha(hue, mAlpha), 0, 0, -1, -1, mRotation, mScale.x, mScale.y, ZOrder);
			}
			break;
		case EBET_DOWNSLIDE:
			{
				videoDrv->DrawImage(mTexture, mBoundingBox.left, mBoundingBox.top + mBoundingBox.GetWidth() * (100.0f - mTempFloat) / 100.0f,
					mBoundingBox.GetWidth(), mBoundingBox.GetHeight() * (mTempFloat) / 100.0f,
					MixColorsAlpha(hue, mAlpha), 0, 0, -1, -1, mRotation, mScale.x, mScale.y, ZOrder);
			}
			break;
		case EBET_4SLICE:
			{
				float32 width = mBoundingBox.GetWidth();
				float32 height = mBoundingBox.GetHeight();
				float32 srcWidth = mTexture->GetSize().x;

				for(int32 i = 0;i < 4;++i)
				{
					videoDrv->DrawImage(mTexture, mBoundingBox.left + width / 4 * i, mBoundingBox.top + ((i % 2 == 1) ? +1 : -1) * height * (100.0f - mTempFloat) / 100.0f, width / 4, height,
						hue, srcWidth / 4 * i, 0, srcWidth / 4, -1,
						mRotation, mScale.x, mScale.y, ZOrder);
				}
			}
			break;
		case EBET_ROTATINGAWAY:
			{
				videoDrv->DrawImage(mTexture, mBoundingBox.left, mBoundingBox.top, mBoundingBox.GetWidth(), mBoundingBox.GetHeight(),
					MixColorsAlpha(hue, mAlpha), 0, 0, -1, -1, mTempFloat / 10.0f, mScale.x * mTempFloat / 100.0f, mScale.y * mTempFloat / 100.0f, ZOrder);
			}
			break;
		case EBET_ZOOMIN:
			{
				float32 tt = (mEffectFadeIn) ? max(16.0f - mTempFloat, 1.0f) : mTempFloat;
				videoDrv->DrawImage(mTexture, mBoundingBox.left , mBoundingBox.top, mBoundingBox.GetWidth(), mBoundingBox.GetHeight(),
					MixColorsAlpha(hue, mAlpha), 0, 0, -1, -1, mRotation, mScale.x * tt, mScale.y * tt, ZOrder);
			}
			break;

		case EBET_LEFTSWEEP:
		case EBET_UPSWEEP:
			{
				float32 width = mBoundingBox.GetWidth();
				float32 height = mBoundingBox.GetHeight();

				if(mEffectFadeIn)
				{
					videoDrv->DrawImage(mTexture, mBoundingBox.left, mBoundingBox.top,
						(mEffectType != EBET_LEFTSWEEP) ? width : width * mTempFloat / 100.0f,
						(mEffectType != EBET_UPSWEEP) ? height : height * mTempFloat / 100.0f,
						MixColorsAlpha(hue, mAlpha), 0, 0,
						(mEffectType != EBET_LEFTSWEEP) ? -1 : width * mTempFloat / 100.0f,
						(mEffectType != EBET_UPSWEEP) ? -1 : height * mTempFloat / 100.0f, mRotation, mScale.x, mScale.y , ZOrder);
				}
				else
				{
					videoDrv->DrawImage(mTexture,
						mBoundingBox.left + (mEffectType != EBET_LEFTSWEEP) ? 0 : (width * mTempFloat / 100.0f),
						mBoundingBox.top + (mEffectType != EBET_UPSWEEP) ? 0 : (height * mTempFloat / 100.0f),
						(mEffectType != EBET_LEFTSWEEP) ? width : (width * (100 - mTempFloat) / 100.0f),
						(mEffectType != EBET_UPSWEEP) ? height : (height * (100 - mTempFloat) / 100.0f),
						MixColorsAlpha(hue, mAlpha),
						(mEffectType != EBET_LEFTSWEEP) ? 0 : (width * mTempFloat / 100.0f),
						(mEffectType != EBET_UPSWEEP) ? 0 : (height * mTempFloat / 100.0f),
						(mEffectType != EBET_LEFTSWEEP) ? -1 : (width * (100.0f - mTempFloat) / 100.0f),
						(mEffectType != EBET_UPSWEEP) ? -1 : (height * (100.0f - mTempFloat) / 100.0f),
						mRotation, mScale.x, mScale.y, ZOrder);
				}
			}
			break;
		case EBET_RIGHTSWEEP:
		case EBET_DOWNSWEEP:
			{
				float32 width = mBoundingBox.GetWidth();
				float32 height = mBoundingBox.GetHeight();

				if(mEffectFadeIn)
				{
					videoDrv->DrawImage(mTexture,
						mBoundingBox.left + (mEffectType != EBET_RIGHTSWEEP) ? 0 : (width * (100 - mTempFloat) / 100.0f),
						mBoundingBox.top + (mEffectType != EBET_DOWNSWEEP) ? 0 : (height * (100 - mTempFloat) / 100.0f),
						(mEffectType != EBET_RIGHTSWEEP) ? width : (width * mTempFloat / 100.0f),
						(mEffectType != EBET_DOWNSWEEP) ? height : (height * mTempFloat / 100.0f),
						MixColorsAlpha(hue, mAlpha),
						(mEffectType != EBET_RIGHTSWEEP) ? 0 : (width * (100 - mTempFloat) / 100.0f),
						(mEffectType != EBET_DOWNSWEEP) ? 0 : (height * (100 - mTempFloat) / 100.0f),
						(mEffectType != EBET_RIGHTSWEEP) ? -1 : (width * mTempFloat / 100.0f),
						(mEffectType != EBET_DOWNSWEEP) ? -1 : (height * mTempFloat / 100.0f), mRotation, mScale.x, mScale.y , ZOrder);
				}
				else
				{
					videoDrv->DrawImage(mTexture,
						mBoundingBox.left,
						mBoundingBox.top,
						(mEffectType != EBET_RIGHTSWEEP) ? width : (width * (100 - mTempFloat) / 100.0f),
						(mEffectType != EBET_DOWNSWEEP) ? height : (height * (100 - mTempFloat) / 100.0f),
						MixColorsAlpha(hue, mAlpha),
						0, 0,
						(mEffectType != EBET_RIGHTSWEEP) ? -1 : (width * (100.0f - mTempFloat) / 100.0f),
						(mEffectType != EBET_DOWNSWEEP) ? -1 : (height * (100.0f - mTempFloat) / 100.0f),
						mRotation, mScale.x, mScale.y, ZOrder);
				}
			}
			break;
		case EBET_FROMRULE:
			{
				GetNekoNovel()->GetVideoDriver()->ConvertGrayscaleToAlphaMap(mRuleTexture);

				mTexture->Refresh();
				if(!mTexture->GetHandle())
					return;

				videoDrv->EndSprite();
				IDirect3DDevice9 *device = videoDrv->GetHandle();
				nekoPointF ratio = mTexture->GetSizeRatio();

				int32 alpha = ((int32)(min(max(abs(mTempFloat), 0), 255) / 100.0f * 255.0f) & 0xff);
				alpha = 0x000000 | (alpha << 24);
				device->SetRenderState(D3DRS_TEXTUREFACTOR, alpha);

				device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
				device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

				device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
				device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);
				device->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);

				device->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
				device->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
				device->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
				device->SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
				device->SetTextureStageState(1, D3DTSS_ALPHAOP,  (mTempFloat > 0) ? D3DTOP_ADD : D3DTOP_SUBTRACT);

				device->SetTexture(0, mTexture->GetHandle());
				device->SetTexture(1, mRuleTexture->GetHandle());

				float32 x = mBoundingBox.left - 0.5f, y = mBoundingBox.top - 0.5f;
				int32 width = mBoundingBox.GetWidth(), height = mBoundingBox.GetHeight();

				float32 u = mTexture->GetSize().x / (float32)mTexture->GetVideoSize().x;
				float32 v = mTexture->GetSize().y / (float32)mTexture->GetVideoSize().y;
				float32 u2 = mRuleTexture->GetSize().x / (float32)mRuleTexture->GetVideoSize().x;
				float32 v2 = mRuleTexture->GetSize().y / (float32)mRuleTexture->GetVideoSize().y;

				nekoVertex2T verts[4] = 
				{
					{x, y, ZOrder, 0, 0, 0, 0},
					{x + width, y, ZOrder, u / ratio.x, 0, u2 / ratio.x, 0},
					{x, y + height, ZOrder, 0, v / ratio.y, 0, v2 / ratio.y},
					{x + width, y + height, ZOrder, u / ratio.x, v / ratio.y, u2 / ratio.x, v2 / ratio.y}
				};

				D3DXMATRIX matWorld = videoDrv->GetCamera().matCamera;
				matWorld *= videoDrv->GetBaseMatrix();
				device->SetTransform(D3DTS_WORLD, &matWorld);

				device->SetFVF(nekoVertex2T::FVF);
				device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, verts, sizeof(nekoVertex2T));

				/// 원래대로 돌려놓기
				device->SetTexture(1, NULL);
				device->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
				device->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
				device->SetRenderState(D3DRS_TEXTUREFACTOR, 0xffffffff);

				videoDrv->BeginSprite();
			}
			break;
		default:
			videoDrv->DrawImage(mTexture, mBoundingBox.left, mBoundingBox.top, mBoundingBox.GetWidth(), mBoundingBox.GetHeight(),
				MixColorsAlpha(hue, mAlpha), 0, 0, -1, -1, mRotation, mScale.x, mScale.y, ZOrder + REAL_Z_DIFF);
			break;
		}

		if(mFlags & EBIF_IGNORE_CAMERA)
			videoDrv->EnableCamera(true);
	}

	void nekoBGImageInstance::SetEffect(E_BGIMAGE_EFFECT_TYPE effectType, float32 speed, bool fadeIn, nekoHWTexture *ruleTex)
	{
		if(mRuleTexture = ruleTex)
			ruleTex->AddRef();

		mEffectType = effectType;
		mEffectSpeed = speed;
		mEffectFadeIn = fadeIn;

		switch(mEffectType)
		{
		case EBET_BLOCK:
			mTempInteger = 0;
			mTempFloat = (fadeIn) ? 0.0f : 255.0f;
			break;
		case EBET_FADE:
			mTempFloat = (fadeIn) ? 0.0f : 255.0f;
			break;
		case EBET_LEFTCURTAIN:
			mTempFloat = (fadeIn) ? 0.0f : 100.0f;
			if(!mEffectFadeIn)
				mEffectType = EBET_RIGHTCURTAIN;
			break;
		case EBET_RIGHTCURTAIN:
			mTempFloat = (fadeIn) ? 0.0f : 100.0f;
			if(!mEffectFadeIn)
				mEffectType = EBET_LEFTCURTAIN;
			break;
		case EBET_LEFTSWEEP:
		case EBET_RIGHTSWEEP:
		case EBET_UPSWEEP:
		case EBET_DOWNSWEEP:
			mTempFloat = 0.0f;
			break;

		case EBET_RIGHTSLIDE:
		case EBET_DOWNSLIDE:
		case EBET_LEFTSLIDE:
		case EBET_UPSLIDE:
		case EBET_ROTATINGAWAY:
		case EBET_4SLICE:
			mTempFloat = 0.0f;//(mEffectFadeIn) ? -100.0f : 100.0f;
			break;
		case EBET_FROMRULE:
			mTempFloat = (mEffectFadeIn) ? -100.0f : 100.0f;
			break;
		case EBET_ZOOMIN:
			mTempFloat = 1.0f;
			break;
		}
	}

	void nekoBGImageInstance::OnEffectFinish(bool fadeIn)
	{
		if(fadeIn)
		{
			GetNekoNovel()->GetCurrentGame()->ClearInstances(BLOCK_BGIMAGE, this, true);
		}
		else
		{
			if(IsZeroAlphaDeleting())
				SetValid(false);
		}

		mRuleTexture = NULL;
	}

	/// 데이터 덤프하기
	result nekoBGImageInstance::Dump(int32 version, nekoByteBuffer &outBuf)
	{
		nekoIBlockInstance::Dump(version, outBuf);

		outBuf << mEffectSpeed << mEffectType << mEffectFadeIn << mTempFloat << mTempInteger
			<< ((mTexture) ? mTexture->GetName() : "")
			<< hue;

		return S_OK;
	}

	result nekoBGImageInstance::Load(int32 version, nekoByteBuffer &inBuf)
	{
		nekoIBlockInstance::Load(version, inBuf);

		string path;
		inBuf >> mEffectSpeed
			>> mEffectType
			>> mEffectFadeIn
			>> mTempFloat
			>> mTempInteger
			>> path;

		if(!path.empty())
		{
			mTexture = GetNekoNovel()->GetHWResourceManager()->LoadTexture(path.c_str());
		}

		if(mEffectType == EBET_FROMRULE)
		{
			if(mRuleTexture = GetNekoNovel()->GetCurrentGame()->GetBGEffectRule())
				mRuleTexture->AddRef();
		}

		if(version >= 1110)
			inBuf >> hue;

		return S_OK;
	}

	bool nekoBGImageInstance::OnEvent(const nekoEvent &event)
	{
		if(event.type == EVENT_WINDOW_SIZE_CHANGED)
		{
			mBoundingBox.SetSize(GetNekoNovel()->GetWidth(), GetNekoNovel()->GetHeight());
		}

		return false;
	}

	/// Constructor
	nekoBGImage::nekoBGImage()
		: nekoIBlock(BLOCK_BGIMAGE)
	{

	}

	/// Destructor
	nekoBGImage::~nekoBGImage()
	{

	}

	/// 인스턴스를 만든다
	nekoIBlockInstance *nekoBGImage::CreateInstance()
	{
		return nekoNew nekoBGImageInstance(this);
	}
}