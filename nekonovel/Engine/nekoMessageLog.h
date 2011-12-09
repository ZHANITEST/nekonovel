// ===============================================================
//  nekoMessageLog - 2010.01.14
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoMessageLog_h__
#define __nekoMessageLog_h__

#include <string>
using namespace std;
#include "nekoIBlock.h"
#include "nekoIBlockInstance.h"
#include "nekoHWTexture.h"
#include "nekoHWFont.h"
#include "nekoMessageLogger.h"

namespace neko
{
	enum E_MESSAGELOG_PICKED_TYPE
	{
		EMPT_UPBUTTON = 0,
		EMPT_SCROLLBAR,
		EMPT_SCROLLSPACE,
		EMPT_DOWNBUTTON,
		EMPT_VOICEBUTTON,
		EMPT_NOTHING,
	};

	class nekoMessageLogInstance : public nekoIBlockInstance
	{
	protected:

		nekoMessageLogger *mMessageLogger;
		E_MESSAGELOG_PICKED_TYPE mPickedType;
		nekoAutoPtr<nekoHWTexture> mSkinImage;
		int32 mCharHeight;
		bool mMousePressed;

		bool mUpdateNeeded;

		int32 mValue;
		int32 mMaxValue;
		int32 mScrollBarSize;
		int32 mScrollSpaceSize;
		int32 mScrollBarHeight;
		int32 mScrollBarLimitedSize;

		int32 mScrollTemp;
		list<nekoMessageLogBlock>::iterator mVoicedPicked;

		inline int32 _GetOnePageSize() { return (mBoundingBox.GetHeight() - 20); }
		inline int32 _GetScrollBarPos() { return (mMaxValue > 0) ? (ScrollBarButtonWidth + (float32)mValue / mMaxValue * (mScrollSpaceSize - mScrollBarSize)) : 0; }

		bool _OnMouseMove(WPARAM wParam, int32 x, int32 y);
		E_MESSAGELOG_PICKED_TYPE _CheckPickedType(int32 x, int32 y);

		// 자동 스크롤 타이머
		nekoTimer mAutoScrollTimer;

		// 값 업데이트 알림.
		bool mValueUpdated;
		inline void _OnValueUpdated() { mValueUpdated = true;}

		list<nekoMessageLogBlock>::iterator mIterFirstBlock;

		bool mInfoUpdated;

	public:

		nekoMessageLogInstance(nekoIBlock *block);
		virtual ~nekoMessageLogInstance();

		void SetScrollBar(int32 scrollBarLimitedSize, int32 scrollBarHeight);
		void Initialize(const char8 *skinFileName);

		void Update(float32 frameTime);
		void Draw();

		bool OnEvent(const nekoEvent &event);

		/// 데이터 덤프하기
		result Dump(int32 version, nekoByteBuffer &outBuf);
		result Load(int32 version, nekoByteBuffer &inBuf);

		bool IsZeroAlphaDeleting() const { return true; }
	};

	class nekoMessageLog : public nekoIBlock
	{
	protected:

	public:

		nekoMessageLog();
		virtual ~nekoMessageLog();

		/// 인스턴스를 만든다
		virtual nekoIBlockInstance *CreateInstance();

	};
}


#endif // __nekoMessageLog_h__