// ===============================================================
//  nekoTextBox - 2010.01.09
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoTextBox_h__
#define __nekoTextBox_h__

#include "nekoIBlock.h"
#include "nekoIBlockInstance.h"
#include "nekoHWTexture.h"
#include "nekoHWSound.h"
#include "nekoHWFont.h"

namespace neko
{
	class nekoTextBoxInstance : public nekoIBlockInstance
	{
	protected:

		nekoAutoPtr<nekoHWTexture> mButtonTexture;
		nekoAutoPtr<nekoHWSound> mFxUp;
		nekoAutoPtr<nekoHWSound> mFxDown;
		nekoAutoPtr<nekoHWFont> mFont;
		E_BUTTON_STATE mButtonState;

		string mCaption;
		string mStringToDraw;
		string mVarName;
		bool mNumeric;
		bool mComp;
		char mCompBuf[4];
		bool mPassword;
		HIMC mIMEHandle;
		int32 caret;
		uint32 mMaxLength;
		uint32 mColor;
		int32 mCaretPos;

		inline bool _IsInButton(int32 x, int32 y)
		{
			nekoRectangle btRect(mBoundingBox.right, mBoundingBox.top);
			btRect.left -= mButtonTexture->GetSize().x / 3;
			btRect.SetSize(mButtonTexture->GetSize().x / 3, mBoundingBox.GetHeight());
			return btRect.IsPointIn(x, y);
		}

		void _PressOK();

	public:

		nekoTextBoxInstance(nekoIBlock *baseBlock);
		~nekoTextBoxInstance();

		result Initialize(const string &varName, nekoRectangle &rect, uint32 maxLength, const char8 *imageName, const char8 *fx1, const char8 *fx2, 
			uint32 color, int32 fontSize, int32 fontBold, const char8 *fontName);

		void UpdateCaption();

		void Update(float32 frameTime);
		void Draw();

		bool OnEvent(const nekoEvent &event);

		inline bool IsNumeric() { return mNumeric; }
		inline bool IsPassword() { return mPassword; }
		inline void ClearCompBuffer() { mCompBuf[0] = '\0'; }

		/// 데이터 덤프하기
		result Dump(int32 version, nekoByteBuffer &outBuf);
		result Load(int32 version, nekoByteBuffer &inBuf);
	};


	class nekoTextBox : public nekoIBlock
	{
	protected:

	public:

		nekoTextBox();
		virtual ~nekoTextBox();

		/// 인스턴스를 만든다
		virtual nekoIBlockInstance *CreateInstance();

	};
}


#endif // __nekoTextBox_h__