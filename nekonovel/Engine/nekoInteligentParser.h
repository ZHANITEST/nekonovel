// ===============================================================
//  nekoInteligentParser - 2009.7.6
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoInteligentParser_h__
#define __nekoInteligentParser_h__

#include "nekoCommands.h"
#include "nekoIRunnable.h"
#include "nekoIEventListener.h"

namespace neko
{


	class NEKONOVEL_API nekoInteligentParser : public nekoIEventListener
	{
	protected:
		
		nekoCommands *mCurrentTarget;
		nekoAutoPtr<nekoThreadParser> mThreadParser;

	public:
		
		/// Constructor
		nekoInteligentParser();
		
		/// Destructor
		~nekoInteligentParser();

		
	};
}


#endif // __nekoInteligentParser_h__