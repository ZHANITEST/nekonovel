// ===============================================================
//  nekoMessageLogger - 2010.01.14
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoMessageLogger_h__
#define __nekoMessageLogger_h__

#include "nekoIEventListener.h"
#include "nekoAutoPtr.h"
#include "nekoByteBuffer.h"
#include "nekoHWFont.h"

namespace neko
{
	struct nekoMessageLogBlock
	{
		string mVoiceName;
		string mName;
		string mText;
		nekoRectangle mBoundingBox;
		int32 mLeftMargin;
		int32 mPageMargin;
		uint32 mColor;

		nekoMessageLogBlock(const char8 *nameLabel, const char8 *voiceName, const char8 *text
			, int32 leftMargin, int32 pageMargin, uint32 color)
			: mName(nameLabel), mVoiceName(voiceName), mText(text)
			, mLeftMargin(leftMargin), mPageMargin(pageMargin), mColor(color)
			, mBoundingBox(0, 0, 0, 0)
			{}
		~nekoMessageLogBlock() {}
	};

	enum 
	{
		ScrollBarButtonWidth = 22,
	};

	class nekoMessageLogger : public nekoIEventListener
	{
	protected:

		list<nekoMessageLogBlock> mBlocks;
		nekoAutoPtr<nekoHWFont> mFont;
		nekoAutoPtr<nekoHWFont> mBoldFont;
		int32 mCharHeight;
		int32 mLeftMargin;
		int32 mNormalLeftMarign;
		int32 mRightMargin;
		int32 mPageMargin;
		int32 mEmptyPage;
		int32 mTotalHeight;
		uint32 mDefaultColor;
		uint32 mColor;

		nekoPoint mSize;

		bool mUpdateNeeded;
		void _UpdateBoundingBoxes();
		inline int32 _GetRealViewportWidth(int32 leftMargin)
		{
			return (mSize.x - mRightMargin - ScrollBarButtonWidth - 60 - leftMargin);
		}

	public:

		nekoMessageLogger();
		~nekoMessageLogger();

		void Clear();
		void ClearBuffer();
		void WriteText(const char8 *nameLabel, const char8 *voiceName, const char8 *text, bool append = false);

		void Initialize(int32 width, int32 height, int32 rightMargin, const char8 *fontName, int32 fontSize);

		void Update(float32 frameTime);

		inline void SetLeftMargin(int32 leftMargin) { mLeftMargin = leftMargin; }
		inline void SetNormalLeftMargin(int32 value) { mNormalLeftMarign = value; }
		inline void SetPageMargin(int32 pageMargin) { mPageMargin = pageMargin; }
		inline void SetEmptyPage(int32 emptyPage) { mEmptyPage = emptyPage; }
		inline void SetDefaultColor(uint32 color) { mDefaultColor = color; }
		inline void SetColor(uint32 color) { mColor = color; }

		inline bool IsUpdateNeeded() { return mUpdateNeeded; }
		inline nekoHWFont *GetFont() { return mFont; }
		inline nekoHWFont *GetNameFont() { return mBoldFont; }
		inline const nekoPoint &GetSize() { return mSize; }
		inline int32 GetRightMargin() { return mRightMargin; }
		inline int32 GetCharHeight() { return mCharHeight; }
		inline int32 GetTotalHeight() { return mTotalHeight; }
		inline list<nekoMessageLogBlock> &GetBlocks() { return mBlocks; }

		void Load(int32 version, nekoByteBuffer &buf);
		void Dump(int32 version, nekoByteBuffer &buf);
	};
}


#endif // __nekoGUIScreenRecollection_h__