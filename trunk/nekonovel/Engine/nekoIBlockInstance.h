#ifndef NekoIBlockInstance_h__
#define NekoIBlockInstance_h__

#include <list>
using namespace std;
#include "nekoIEventListener.h"
#include "nekoIBlock.h"
#include "nekoDefined.h"
#include "nekoIBlockEvent.h"

/********************************************************************
	filename: 	NekoIBlockInstance_h__.h
	author:		kernys

	purpose:	Neko 블럭의 인스턴스
*********************************************************************/

#include "nekoDefined.h"
#include "nekoRectangle.h"

namespace neko
{
	enum E_BUTTON_STATE
	{
		EBS_NORMAL = 0,
		EBS_ROLLOVER,
		EBS_PRESSED,
		EBS_COUNT,
	};

	enum E_BLOCK_INSTANCE_FLAGS
	{
		EBIF_NOTHING		= 0x00000000,
		EBIF_SIZE_LOADED	= 0x00000001,
		EBIF_UPDATE_NEEDED	= 0x00000002,
		EBIF_IGNORE_CAMERA	= 0x00000004,
		EBIF_AUTO_SIZE_X	= 0x00000008, // 상황에 따라 자동으로 사이즈를 결정한다. (가로)
		EBIF_AUTO_SIZE_Y	= 0x00000010, // 상황에 따라 자동으로 사이즈를 결정한다. (세로)

	};

	class nekoIBlockInstance : public nekoIEventListener
	{
	protected:

		/// 베이스
		nekoIBlock *mBaseBlock;
		list<nekoIBlockEvent *> mEventList;

		/// 이름
		string mName;

		/// 알파 혹은 볼륨
		int32 mAlpha;

		/// 스케일링
		nekoPointF mScale;

		/// 보이는가 ?
		bool mVisible;

		/// 회전
		float32 mRotation;

		/// 바운딩 박스
		nekoRectangle mBoundingBox;

		/// Z 오더
		int32 mZOrder;

		/// 유효한가 ?(mValid == false 시 삭제 예정..)
		bool mValid;

		/// 플래그
		uint32 mFlags;

		inline float32 _GetRealZ()
		{
			return (0.5f + (mZOrder - 200) * 0.0001f);
		}

	public:

		/// 생성자
		nekoIBlockInstance(nekoIBlock *block)
			: mBaseBlock(block)
			, mAlpha(255)
			, mVisible(true)
			, mRotation(0)
			, mScale(1, 1)
			, mBoundingBox(0, 0, 0, 0)
			, mZOrder(100)
			, mValid(true)
			, mFlags(0)
		{
			//nekoAssert(block, "기반이 되는 블럭이 존재하지 않습니다.");
		}

		/// 소멸자
		virtual ~nekoIBlockInstance()
		{

		}

		/// 데이터 덤프하기
		virtual result Dump(int32 version, nekoByteBuffer &outBuf)
		{
			if(mBaseBlock)
				outBuf << mBaseBlock->GetType();
			outBuf << mName;
			outBuf << mBoundingBox;
			outBuf << mAlpha;
			outBuf << mScale;
			outBuf << mRotation;
			outBuf << mVisible
				<< mZOrder
				<< mFlags;

			return S_OK;
		}

		/// 데이터 읽어오기
		virtual result Load(int32 version, nekoByteBuffer &fromBuf)
		{
			fromBuf >> mName;
			fromBuf >> mBoundingBox;
			fromBuf >> mAlpha;
			fromBuf >> mScale;
			fromBuf >> mRotation;
			fromBuf >> mVisible;

			if(version >= 1100)
				fromBuf >> mZOrder;

			if(version >= 1106)
				fromBuf >> mFlags;

			return S_OK;
		}

		/// 클리어
		virtual void Clear() {}

		/// 재생하기
		virtual void Play() {}

		/// 정지하기
		virtual void Stop(bool pause = false) {}

		/// 바운딩 박스 얻기
		inline nekoRectangle &GetBoundingBox() { return mBoundingBox; }

		inline void SetZOrder(int32 value) { mZOrder = max(min(value, 200), -200); }
		inline int32 GetZOrder() { return mZOrder; }

		inline void SetVisible(bool value) { mVisible = value; }
		inline bool IsVisible() const { return mVisible && (mAlpha > 0); }
		virtual void SetName(const char8 *name) { mName = name; }
		inline const char8 *GetName() const { return mName.c_str(); }
		inline int32 GetAlpha() { return mAlpha; }
		virtual void SetAlpha(int32 alpha) { mAlpha = alpha; }
		inline float32 GetRotation() { return mRotation; }
		inline void SetRotation(float32 rotAngle) { mRotation = rotAngle; }
		inline nekoPointF &GetScale() { return mScale; }
		inline void SetScale(nekoPointF &scale) { mScale = scale; }
		inline E_BLOCK_TYPE GetType() const { return mBaseBlock->GetType(); }
		virtual bool IsWaiting() { return false; } /// 대기중인가 ?
		virtual void Update(float32 frameTime) {}
		virtual void Draw() {}
		virtual bool IsIncludedInSnapShot() { return true; }

		inline bool IsValid() { return mValid; }
		inline void SetValid(bool value) { mValid = value; }
		virtual bool IsZeroAlphaDeleting() const { return false; }

		inline void AddFlags(uint32 flags) { mFlags |= flags; }
		inline void RemoveFlags(uint32 flags) { mFlags &= ~flags; }
		inline uint32 GetFlags() { return mFlags; }

	};
}


#endif
