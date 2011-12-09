// ===============================================================
//  nekoGUISelectionButtons - 2009.8.26
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoGUISelectionButtons_h__
#define __nekoGUISelectionButtons_h__

#include <d3dx9.h>
#include <string>
using namespace std;
#include "nekoIBlock.h"
#include "nekoIBlockInstance.h"
#include "nekoHWTexture.h"
#include "nekoHWSound.h"

namespace neko
{
	class nekoGUISelectionButtons : public nekoIBlockInstance
	{
	protected:

		struct nekoOneButton
		{
			nekoRectangle rect;
			nekoPoint mTextSize;

			string mCaption;
			nekoAutoPtr<nekoHWTexture> mImage[EBS_COUNT];

			/// idx -- 0 : 마우스 올렸을때, 1 : 마우스 눌렀을 때
			nekoAutoPtr<nekoHWSound> mSound[2];

			/// 사용하는 폰트
			ID3DXFont *mFont;

			nekoOneButton() : mFont(0) {}
			~nekoOneButton() { SafeRelease(mFont); }
		};
		int32 mSelectedIndex;
		E_BUTTON_STATE mButtonState;

		list<nekoOneButton *> mButtons;

		int32 _PickButton(int32 x, int32 y, nekoOneButton **out = NULL);

	public:

		nekoGUISelectionButtons();
		~nekoGUISelectionButtons();

		void Update(float32 frameTime);
		void Draw();

		bool OnEvent(const nekoEvent &event);

		void Clear();

		void AddButton(const string &caption, const string **imageNames, const string **soundNames);
		void ClearButtons();

		/// 데이터 덤프하기
		result Dump(int32 version, nekoByteBuffer &outBuf);
		result Load(int32 version, nekoByteBuffer &inBuf);
	};
}


#endif // __nekoButton_h__