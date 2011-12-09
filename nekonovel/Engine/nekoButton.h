// ===============================================================
//  nekoButton - 2009.7.13
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoButton_h__
#define __nekoButton_h__

#include <string>
using namespace std;
#include "nekoIBlock.h"
#include "nekoIBlockInstance.h"
#include "nekoHWTexture.h"
#include "nekoHWSound.h"

namespace neko
{
	enum E_BUTTON_INSTANCE_TYPE
	{
		EBIT_JUMP = 0,
		EBIT_LOAD, /* 비사용 */
		EBIT_OPTION, /* 비사용 */
		EBIT_END,
		EBIT_LINK,
		EBIT_BGM,
		EBIT_FX,
		EBIT_SCRIPT,
		EBIT_PHOTOMODE,
		EBIT_MSGBOX_OK,
		EBIT_MSGBOX_CANCEL,
		EBIT_NOTHING,
	};

	class nekoButtonInstance : public nekoIBlockInstance
	{
	protected:

		string mArguments1;
		string mArguments2;

		E_BUTTON_STATE mBtState;
		E_BUTTON_INSTANCE_TYPE mBtType;
		nekoAutoPtr<nekoHWTexture> mTextures[3];
		nekoAutoPtr<nekoHWSound> mRolloverSound;
		nekoAutoPtr<nekoHWSound> mPressedSound;
		nekoAutoPtr<nekoHWSound> mTempSound;

	public:

		nekoButtonInstance(nekoIBlock *block);
		virtual ~nekoButtonInstance();

		void SetButtonType(E_BUTTON_INSTANCE_TYPE btType);
		void SetJump(const string &fileName, const string &bookMarkName);
		void SetLink(const string &address);
		void SetMusic(const string &fileName);
		void SetFXSound(const string &fileName);
		void SetScriptType(const string &fileName);

		void SetNormalImage(const string &fileName);
		void SetRolloverImage(const string &fileName);
		void SetPressedImage(const string &fileName);
		void SetRolloverSound(const string &fileName);
		void SetPressedSound(const string &fileName);

		inline E_BUTTON_INSTANCE_TYPE GetButtonType() const { return mBtType; }

		void Run();

		void Update(float32 frameTime);
		void Draw();

		bool OnEvent(const nekoEvent &event);

		/// 데이터 덤프하기
		result Dump(int32 version, nekoByteBuffer &outBuf);
		result Load(int32 version, nekoByteBuffer &inBuf);

		bool IsZeroAlphaDeleting() const { return true; }
	};

	class nekoButton : public nekoIBlock
	{
	protected:

	public:

		nekoButton();
		virtual ~nekoButton();

		/// 인스턴스를 만든다
		virtual nekoIBlockInstance *CreateInstance();

	};
}


#endif // __nekoButton_h__