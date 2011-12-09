// ===============================================================
//  nekoGUIScreenSaveLoad - 2009.7.16
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoGUIScreenSaveLoad_h__
#define __nekoGUIScreenSaveLoad_h__

#include "nekoIGUIScreen.h"
#include "nekoHWTexture.h"
#include "nekoAutoPtr.h"

namespace neko
{
	enum E_SCREEN_SAVELOAD_TYPE
	{
		ESST_SAVE = 0,
		ESST_LOAD,
	};

	class nekoGUIScreenSaveLoad : public nekoIGUIScreen
	{
	protected:

		enum
		{
			MaxPage = 5
		};

		E_SCREEN_SAVELOAD_TYPE mMode;
		int32 mType;
		int32 mSelectedOne;
		int32 mPage;

		nekoAutoPtr<nekoHWTexture> mNoSaveFile;

	public:
		
		nekoGUIScreenSaveLoad(E_SCREEN_SAVELOAD_TYPE mode);
		virtual ~nekoGUIScreenSaveLoad();

		/// Update
		void Update(float32 frameTime);

		/// Draw
		void Draw();

		bool OnEvent(const nekoEvent &event);

		result GoNextPage();
		result GoPrevPage();
	};
}


#endif // __nekoGUIScreenSaveLoad_h__