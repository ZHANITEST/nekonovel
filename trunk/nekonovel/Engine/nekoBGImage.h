// ===============================================================
//  nekoBGImage - 2009.7.7
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoBGImage_h__
#define __nekoBGImage_h__

#include "nekoIBlock.h"
#include "nekoIBlockInstance.h"
#include "nekoHWTexture.h"

namespace neko
{
	enum E_BGIMAGE_EFFECT_TYPE
	{
		EBET_NONE = 0,
		EBET_FADE,
		EBET_BLOCK,
		EBET_LEFTCURTAIN,
		EBET_RIGHTCURTAIN,
		EBET_LEFTSLIDE,
		EBET_RIGHTSLIDE,
		EBET_UPSLIDE,
		EBET_DOWNSLIDE,
		EBET_4SLICE,
		EBET_ROTATINGAWAY,
		EBET_ZOOMIN,
		EBET_LEFTSWEEP,
		EBET_RIGHTSWEEP,
		EBET_UPSWEEP,
		EBET_DOWNSWEEP,
		EBET_EMBEDDED_COUNT,

		EBET_FROMRULE,
		EBET_COUNT,
	};

	class NEKONOVEL_API nekoBGImageInstance : public nekoIBlockInstance
	{
	protected:

		uint32 hue;
		nekoAutoPtr<nekoHWTexture> mTexture;

		float32 mEffectSpeed;
		float32 mTempFloat;
		int32 mTempInteger;
		bool mEffectFadeIn;
		bool mZeroAlphaDeleting;
		E_BGIMAGE_EFFECT_TYPE mEffectType;
		nekoAutoPtr<nekoHWTexture> mRuleTexture;

	public:

		nekoBGImageInstance(nekoIBlock *block);
		virtual ~nekoBGImageInstance();

		inline void SetHue(uint32 hue) { this->hue = hue | 0xff000000; }
		inline uint32 GetHue() { return this->hue; }
		void SetImage(const char8 *fileName);

		virtual void Update(float32 frameTime);
		virtual void Draw();

		void SetEffect(E_BGIMAGE_EFFECT_TYPE effectType, float32 speed, bool fadeIn, nekoHWTexture *ruleTex = NULL);
		inline E_BGIMAGE_EFFECT_TYPE GetEffectType() { return mEffectType; }
		inline void ClearEffect() { mEffectType = EBET_NONE; mRuleTexture = NULL; }

		void OnEffectFinish(bool fadeIn);

		/// 데이터 덤프하기
		result Dump(int32 version, nekoByteBuffer &outBuf);
		result Load(int32 version, nekoByteBuffer &inBuf);

		inline void SetZeroAlphaDeleting(bool value) { mZeroAlphaDeleting = value; }
		bool IsZeroAlphaDeleting() const { return mZeroAlphaDeleting; }

		bool OnEvent(const nekoEvent &event);
	};

	class nekoBGImage : public nekoIBlock
	{
	protected:

	public:

		nekoBGImage();
		virtual ~nekoBGImage();

		/// 인스턴스를 만든다
		virtual nekoIBlockInstance *CreateInstance();

	};
}


#endif // __nekoBGImage_h__