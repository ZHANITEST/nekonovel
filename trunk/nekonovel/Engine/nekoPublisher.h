// ===============================================================
//  nekoPublisher - 2009.7.16
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoPublisher_h__
#define __nekoPublisher_h__

#include "nekoIRunnable.h"
#include "nekoFileSystem.h"

namespace neko
{
	class nekoPublisher : public nekoIRunnable
	{
	protected:


	public:
		
		nekoPublisher();
		virtual ~nekoPublisher();

		/// Do something
		uint32 Do();

		
	};
}


#endif // __nekoPublisher_h__