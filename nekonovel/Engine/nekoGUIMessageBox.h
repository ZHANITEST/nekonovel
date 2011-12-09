// ===============================================================
//  nekoGUIMessageBox - 2009.7.16
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoGUIMessageBox_h__
#define __nekoGUIMessageBox_h__

#include "nekoIGUIScreen.h"
#include "nekoHWTexture.h"
#include "nekoAutoPtr.h"
#include "nekoIBlockInstance.h"

namespace neko
{
	class nekoGUIMessageBox : public nekoIGUIScreen
	{
	protected:

		int32 mPickedOne; // -1, [0, 1]
		E_BUTTON_STATE mButtonState;

		nekoAutoPtr<nekoHWTexture> mBackground;
		nekoAutoPtr<nekoHWTexture> mButtonImage;

		string mMessage;

	public:

		nekoGUIMessageBox();
		~nekoGUIMessageBox();

		/// Update
		void Update(float32 frameTime);

		/// Draw
		void Draw();

		bool OnEvent(const nekoEvent &event);
	};
}


#endif // __nekoGUIMessageBox_h__