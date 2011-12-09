// ===============================================================
//  nekoParticle - 2009.8.13
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoParticle_h__
#define __nekoParticle_h__

#include "nekoIBlock.h"
#include "nekoIBlockInstance.h"
#include "nekoHWTexture.h"

namespace neko
{
	struct NEKONOVEL_API nekoParticleEmitter;

	struct NEKONOVEL_API nekoParticleNode
	{
		nekoPointF pos;
		nekoPointF gravity;
		float32 scale;

		float32 r, g, b, a;
		float32 vr, vg, vb, va;
		float32 er, eg, eb, ea;

		bool alive;
		bool enabled;
		nekoPointF dir;
		float32 delta;
		float32 lifetime;
		float32 rotation;
		float32 rotSpeed;
		float32 scaleSpeed;

		float32 deltaColor;

		// 이벤트 실행 여부를 담고 있는 플래그
		uint32 eventFlags;

		void Revive(const nekoParticleEmitter &e);
		inline uint32 Color()
		{
			return ((((int32)(r)) & 0xff) << 16) | ((((int32)(g)) & 0xff) << 8) | ((((int32)(b)) & 0xff)) | ((((int32)(a)) & 0xff) << 24);
		}
	};

	enum E_PARTICLE_EVENT_FLAGS
	{
		EPEF_LOOP			= 0x00000001,
		EPEF_DIR			= 0x00000002,
		EPEF_GRAVITY		= 0x00000004,
		EPEF_ROTATION		= 0x00000008,
		EPEF_SCALE			= 0x00000010,
		EPEF_COLOR			= 0x00000020,
	};

	struct NEKONOVEL_API nekoParticleEvent
	{
		float32 mTime;
		uint32 mFlags;

		nekoPointF mMinDir;
		nekoPointF mMaxDir;
		nekoPointF mGravity;
		float32 mRotationSpeed;
		float32 mScaleSpeed;
		uint32 mColor;

		nekoParticleEvent()
			: mFlags(0), mTime(1.0f)
			, mMinDir(-1, -1), mMaxDir(1, 1), mGravity(0, 0)
			, mRotationSpeed(0), mColor(0xffffffff)
			, mScaleSpeed(0)
		{

		}
	};

	struct NEKONOVEL_API nekoParticleEmitter
	{
		string mName;

		float32 mEmitDelay;
		nekoPointF mMinDir;
		nekoPointF mMaxDir;
		float32 mMinScale;
		float32 mMaxScale;
		uint32 mMaxCount;
		bool mLoop;
		nekoPointF mSpawnPos;
		nekoPoint mSpawnRadius;

		float32 mMinLifeTime;
		float32 mMaxLifeTime;

		float32 mMinFirstAppearTime;
		float32 mMaxFirstAppearTime;

		uint32 mMinStartColor;
		uint32 mMaxStartColor;
		uint32 mMinEndColor;
		uint32 mMaxEndColor;

		float32 mRotation;

		float32 mRotationSpeed;
		float32 mScaleSpeed;

		nekoPointF mGravity;

		uint32 mSrcBlend;
		uint32 mDestBlend;

		nekoParticleNode *mNodeList;
		list<nekoParticleEvent> mEventList;
		nekoAutoPtr<nekoHWTexture> mTexture;

		nekoParticleEmitter() : mNodeList(NULL) {}
		~nekoParticleEmitter() { SafeDeleteArray(mNodeList); }

		void Initialize();
		void FillNodes();
		void SetTexture(const char8 *name);
		inline uint32 AddEvent()
		{
			mEventList.push_back(nekoParticleEvent());
			return mEventList.size() - 1;
		}
		nekoParticleEvent *GetEvent(int32 index);
		bool RemoveEvent(int32 index);
		inline uint32 ImportEvent(const nekoParticleEvent &evt)
		{
			mEventList.push_back(evt);
			return mEventList.size() - 1;
		}
	};

	class NEKONOVEL_API nekoParticleInstance : public nekoIBlockInstance
	{
	protected:

		list<nekoParticleEmitter> mEmitters;
		bool mForUseOnly;
		string mFileName;

	public:

		nekoParticleInstance(nekoIBlock *block);
		virtual ~nekoParticleInstance();

		nekoParticleEmitter *AddEmitter(const char8 *name);
		nekoParticleEmitter *GetEmitter(int32 index);
		bool RemoveEmitter(int32 index);
		inline uint32 GetEmitterCount() { return mEmitters.size(); }
		void AtFirst();

		result LoadParticle(const char8 *fileName, bool forUseOnly = false);
		result SaveParticle(const char8 *fileName);
		void Clear() { mEmitters.clear(); }

		// 파티클 생성 위치 지정
		void SetAllSpwanPosX(float32 pos);
		void SetAllSpwanPosY(float32 pos);

		void Update(float32 frameTime);
		void Draw();

		/// 데이터 덤프하기
		result Dump(int32 version, nekoByteBuffer &outBuf);
		result Load(int32 version, nekoByteBuffer &inBuf);

		bool IsZeroAlphaDeleting() const { return true; }
	};

	class nekoParticle : public nekoIBlock
	{
	protected:

	public:

		nekoParticle();
		virtual ~nekoParticle();

		/// 인스턴스를 만든다
		virtual nekoIBlockInstance *CreateInstance();

	};
}


#endif // __nekoParticle_h__