// ===============================================================
//  nekoGUIChatBox - 2009.7.11
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoGUIChatBox_h__
#define __nekoGUIChatBox_h__

#include <vector>
#include "nekoHWTexture.h"
#include "nekoIBlockInstance.h"
#include "nekoHWFont.h"

namespace neko
{
	enum E_TEXTBLOCK_TYPE
	{
		ETT_NORMAL = 0,
		ETT_SELECTION
	};

	enum E_CHATBOX_CURSOR_TYPE
	{
		ECCT_TWINKLE = 0,
		ECCT_ANIMATION,
	};

	enum E_TEXT_ALIGN_TYPE
	{
		ETAT_LEFT  = 0,
		ETAT_CENTER,
		ETAT_CENTER2,
		ETAT_RIGHT,
	};

	struct nekoChatBoxFont
	{
		ID3DXFont *mFont;
		string mFaceName;
		int32 mHeight;
		bool mBold;

		nekoChatBoxFont() {}
		~nekoChatBoxFont() {}
	};

	struct nekoChatBoxTextBlock
	{
		E_TEXTBLOCK_TYPE mType;
		nekoChatBoxFont *mFont;
		string mCaption;
		uint32 mColor;
		uint32 mShadowColor;
		char8 mShadowType;
		nekoRectangle mRect; // 선택지에서만 쓰임.
		E_TEXT_ALIGN_TYPE mAlignType;

		/// 출력 예정중인 텍스트
		string mWaitingText;

		/// 출력 예정중인 텍스트가 쓰여진 길이
		uint32 mWaitingTextWrittenLen;

		/// 라인별 크기
		vector<uint32> mWidthPerLine;

		/// 라인별 크기가 초기화 되었는가 ?
		bool mWidthesInitialized;

		nekoChatBoxTextBlock()
			: mWaitingTextWrittenLen(0), mAlignType(ETAT_LEFT)
			, mWidthesInitialized(false)
		{
			mWidthPerLine.push_back(0);
		}
		~nekoChatBoxTextBlock()
		{
			
		}

		inline const char8 *GetCurrentText() { return mWaitingText.c_str() + mWaitingTextWrittenLen; }
	};

	class NEKONOVEL_API nekoGUIChatBox : public nekoIBlockInstance
	{
	protected:

		nekoAutoPtr<nekoHWTexture> mBackground;
		nekoAutoPtr<nekoHWTexture> mCursor;

		nekoRectangle mPrintingBoundary;

		list<nekoChatBoxFont *> mFontList;
		list<nekoChatBoxTextBlock *> mTextBlockList;

		int32 mSelectedIndex;
		int32 mMaxHeight;
		nekoPoint mCursorPos;

		/// 현재 선택된 폰트
		nekoChatBoxFont *mCurrentFont;

		/// 커서 보임 ?
		bool mCursorVisible;

		/// 커서 깜빡임 체크
		int32 mCursorTickIndex;
		nekoTimer mCursorTick;
		nekoRectangle mCursorSourceRect;

		/// 대사만 보임 설정
		bool mChatVisible;

		/// 줄 간격
		int32 mLineInterval;

		/// 대사 부드럽게 흘림용 알파값
		float32 mSmoothAlpha;

		/// 대사 부드럽게 흘림용이 마지막으로 끝난 대사
		int32 mSmoothTextEnd;

		/// 총 글씨 수
		int32 mTextCount;

		/// 대사 출력 타이머
		float32 mPrintTimer;

		/// 다음 페이지에 넘어가기를 기다리고 있는가 ?
		bool mWaitingForNextPage;
		nekoTimer mAutoTurningTimer;

		/// 라인별 문자열 폭
		vector<int32> mWidthPerLine;

		/// 출력 가능한 공간이 있는지 체크
		inline bool _IsPrintAbleNow()
		{
			return (_PreCalculateText(false) > 0);
		}

		/// 다음 페이지로 넘기기
		void _GoNextPage();

		// 보통을 위한 계산
		uint32 _PreCalculateText(bool skipping = false, nekoChatBoxTextBlock **outBlock = 0);

		// Center2를 위한 계산
		void _PreCalculateForCenter2(nekoChatBoxTextBlock *block, const char8 *text);

		nekoChatBoxTextBlock *_GetFirstTextBlock();

	public:

		nekoGUIChatBox();
		virtual ~nekoGUIChatBox();

		/// Set Font
		void SetFont(const char8 *faceName, int32 height, int32 bold);

		/// Set Skin
		void SetSkin(const char8 *fileName);

		/// Update
		void Update(float32 frameTime);

		/// Draw
		void Draw();

		/// Refresh the texture
		void RefreshTexture();

		/// Clear all
		void Clear();

		/// Clear the buffer
		void ClearBuffer();

		inline nekoChatBoxFont *GetLastFont() { return (*(--mFontList.end())); }
		nekoChatBoxTextBlock *PushNewBlock();

		/// Write text
		void WriteText(const char8 *sentence, E_TEXTBLOCK_TYPE blockType = ETT_NORMAL);

		/// 부드러운 텍스트를 끝낼 때.
		inline void OnSmoothTextEnd() { mSmoothTextEnd = mTextCount; mSmoothAlpha = 0.0f; }

		/// On Event
		bool OnEvent(const nekoEvent &event);

		inline void SetCursorVisible(bool value) { mCursorVisible = value; mCursorTickIndex = 1; mCursorTick.Clear(); }

		inline nekoRectangle &GetPrintingBox() { return mPrintingBoundary; }
		inline void SetChatVisible(bool value) { mChatVisible = value; }
		inline void SetLineInterval(int32 pixels) { mLineInterval = pixels; }

		bool IsWaitingTextLeft();
		bool IsSmoothTexting();

		/// 스킵하기
		bool Skip();

		result Dump(int32 version, nekoByteBuffer &outBuf);
		result Load(int32 version, nekoByteBuffer &inBuf);

	};
}


#endif // __nekoGUIChatBox_h__