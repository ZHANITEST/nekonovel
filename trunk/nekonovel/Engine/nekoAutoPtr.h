// ===============================================================
//  nekoAutoPtr - 2009.7.6
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoAutoPtr_h__
#define __nekoAutoPtr_h__

#include "nekoIUnknown.h"

namespace neko
{
	template <class T>
	class nekoAutoPtr
	{
	protected:

		/// Object
		T *object;

	public:

		/// Constructor
		nekoAutoPtr(T *ptr = 0)
			: object(ptr)
		{
			
		}

		/// Destructor
		virtual ~nekoAutoPtr()
		{
			if(object)
				object->Release();
		}

		/// Copy Constructor
		nekoAutoPtr(const nekoAutoPtr &other)
		{
			if(this->object = other.object)
				this->object->AddRef();
		}

		/// Substitute self
		nekoAutoPtr &operator =(nekoAutoPtr &other)
		{
			/*if(other.object)
				other.object->AddRef();*/

			if(this->object)
				this->object->Release();

			this->object = other.object;

			return (*this);
		}

		/// Operator '='
		nekoAutoPtr &operator =(T *ptr)
		{
			// Release old
			if(object)
				object->Release();

			object = ptr;
			return (*this);
		}

		/// Operator '*'
		inline operator T *()
		{
			return (T *)object;
		}

		/// Operator '->'
		inline T *operator ->()
		{
			return (T *)object;
		}

		inline T **Pptr()
		{
			(*this) = NULL;
			return &object;
		}
		
	};
}


#endif // __nekoAutoPtr_h__