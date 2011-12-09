// ===============================================================
//  nekoGUIManager - 2009.7.6
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoGUIManager_h__
#define __nekoGUIManager_h__

#include "nekoIEventListener.h"
#include "nekoIGUIScreen.h"

namespace neko
{
	class nekoGUIManager : public nekoIEventListener
	{
	protected:

		nekoAutoPtr<nekoIGUIScreen> mCurrentScreen;

	public:
		
		nekoGUIManager();
		virtual ~nekoGUIManager();

		inline void SetScreen(nekoIGUIScreen *scene)
		{
			if(mCurrentScreen)
				mCurrentScreen->OnClosed();

			mCurrentScreen = scene;

			if(scene)
			{
				scene->AddRef();
				scene->OnOpened();
			}
		}

		inline void Update(float32 frameTime)
		{
			if(mCurrentScreen)
				mCurrentScreen->Update(frameTime);
		}

		inline void Draw()
		{
			if(mCurrentScreen)
				mCurrentScreen->Draw();
		}

		bool OnEvent(const nekoEvent &event)
		{
			if(mCurrentScreen)
				return mCurrentScreen->OnEvent(event);
			return false;
		}

		inline nekoIGUIScreen *GetCurrentScreen() { return mCurrentScreen; }
		
	};
}


#endif // __nekoGUIManager_h__