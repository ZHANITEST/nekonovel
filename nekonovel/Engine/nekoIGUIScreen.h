// ===============================================================
//  nekoIGUIScreen - 2009.7.6
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoIGUIScreen_h__
#define __nekoIGUIScreen_h__

#include "nekoIGUIWindow.h"

namespace neko
{
	class nekoIGUIScreen : public nekoIGUIWindow
	{
	protected:


	public:
		
		/// Constructor
		nekoIGUIScreen()
		{
			mBoundingBox = nekoRectangle(0, 0, 800, 600);
		}
		
		/// Destructor
		virtual ~nekoIGUIScreen()
		{

		}


		virtual result GoNextPage() { return E_FAIL; }
		virtual result GoPrevPage() { return E_FAIL; }

		virtual void OnOpened() {}
		virtual void OnClosed() {}
	};
}


#endif // __nekoIGUIScreen_h__