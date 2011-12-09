// ===============================================================
//  nekoThread - 2009.7.6
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoThread_h__
#define __nekoThread_h__

#include "nekoIBlock.h"
#include "nekoCommands.h"

namespace neko
{
	class nekoThread : public nekoIBlock
	{
	protected:

		nekoCommands *commands;

	public:
		
		/// Constructor
		nekoThread();
		
		/// Destructor
		virtual ~nekoThread();
		
	};
}


#endif // __nekoThread_h__