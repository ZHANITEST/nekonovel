#ifndef nekoIUnknown_h__
#define nekoIUnknown_h__

#include "nekoDefined.h"
#include "nekoDataTypes.h"

/********************************************************************
	created:	2009/07/05
	created:	5:7:2009   18:48
	filename: 	nekoIUnknown.h
	author:		kernys
	
	purpose:	
*********************************************************************/

namespace neko
{
	class nekoIUnknown
	{
	private:

		/// Reference count
		int32 ref;

	public:

		/// Constructor
		nekoIUnknown()
		{
			ref = 1;
		}

		/// Destructor
		virtual ~nekoIUnknown()
		{

		}

		/// Refer
		inline void AddRef()
		{
			++ref;
		}

		/// Release
		bool Release()
		{
			--ref;

			if(!ref)
			{
				if(this)
					delete this;
				return true;
			}

			return false;
		}

		/// Get Ref Count
		inline int32 GetRefCount()
		{
			return ref;
		}
	};
};


#endif