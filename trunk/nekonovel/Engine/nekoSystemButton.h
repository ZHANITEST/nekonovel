// ===============================================================
//  nekoSystemButton - 2009.7.16
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoSystemButton_h__
#define __nekoSystemButton_h__

#include <string>
using namespace std;
#include "nekoIBlock.h"
#include "nekoIBlockInstance.h"
#include "nekoHWTexture.h"
#include "nekoHWSound.h"

namespace neko
{
	// * 시스템 버튼은 대기중에만 사용할 수 있다.
	enum E_SYSTEM_BUTTON_INSTANCE_TYPE
	{
		ESBIT_JUMP = 0,
		ESBIT_SCRIPT,
		ESBIT_RECOLLECTION,
		ESBIT_QUICKSKIP,
		ESBIT_AUTOTURN,
		ESBIT_WINDOWMODE,
		ESBIT_PRINT_DELAY,
		ESBIT_NEXT_PAGE,
		ESBIT_PREV_PAGE,
		ESBIT_EXIT,
		ESBIT_LOAD_SLOT,
		ESBIT_SAVE_SLOT,
		ESBIT_NOTHING,
		ESBIT_GO_SLOTPAGE,
		ESBIT_DELETE_SLOT,
		ESBIT_ALWAYS_TOP,
		ESBIT_SET_VARIABLE,
	};

	enum E_SYSTEM_BUTTON_STATE
	{
		ESBS_NORMAL = 0,
		ESBS_ROLLOVER,
		ESBS_PRESSED,
		ESBS_COUNT,
	};

	class nekoSystemButtonInstance : public nekoIBlockInstance
	{
	protected:

		bool mValue;
		bool mCheckButton;

		string mArguments1;
		string mArguments2;
		int32 mArguments3;

		E_SYSTEM_BUTTON_STATE mBtState;
		E_SYSTEM_BUTTON_INSTANCE_TYPE mBtType;
		nekoAutoPtr<nekoHWTexture> mTextures[3];
		nekoAutoPtr<nekoHWSound> mRolloverSound;
		nekoAutoPtr<nekoHWSound> mPressedSound;
		nekoAutoPtr<nekoHWSound> mTempSound;

	public:

		nekoSystemButtonInstance(nekoIBlock *block);
		virtual ~nekoSystemButtonInstance();

		inline void SetButtonType(E_SYSTEM_BUTTON_INSTANCE_TYPE btType) { mBtType = btType; }
		void SetJump(const string &fileName, const string &bookMarkName);
		void SetQuickSkipMode(const string &enable);
		void SetAutoTurnMode(const string &enable);
		void SetScriptMode(const string &scriptName);

		void SetWindowMode(const string &value);
		void SetPrintDelay(const string &value);
		void SetLoadSaveSlotMode(E_SYSTEM_BUTTON_INSTANCE_TYPE btType, int32 slotNo);
		inline void SetSlotPage(int32 pageNo)
		{
			mBtType = ESBIT_GO_SLOTPAGE;
			mArguments3 = pageNo;
		}
		inline void SetAlwaysTopMode(bool enabled)
		{
			mBtType = ESBIT_ALWAYS_TOP;
			mArguments3 = (enabled) ? 1 : 0;
			mCheckButton = true;
		}
		inline void SetVariableSetMode(const string &arg1, const string &arg2)
		{
			mBtType = ESBIT_SET_VARIABLE;
			mArguments1 = arg1;
			mArguments2 = arg2;
			mCheckButton = true;
		}

		void SetNormalImage(const string &fileName);
		void SetRolloverImage(const string &fileName);
		void SetPressedImage(const string &fileName);
		void SetRolloverSound(const string &fileName);
		void SetPressedSound(const string &fileName);

		// 작동
		void Run();

		void Update(float32 frameTime);
		void Draw();

		bool OnEvent(const nekoEvent &event);

		/// 데이터 덤프하기
		result Dump(int32 version, nekoByteBuffer &outBuf);
		result Load(int32 version, nekoByteBuffer &inBuf);

		bool IsZeroAlphaDeleting() const { return true; }
	};

	class nekoSystemButton : public nekoIBlock
	{
	protected:

	public:

		nekoSystemButton();
		virtual ~nekoSystemButton();

		/// 인스턴스를 만든다
		virtual nekoIBlockInstance *CreateInstance();

	};
}


#endif // __nekoSystemButton_h__