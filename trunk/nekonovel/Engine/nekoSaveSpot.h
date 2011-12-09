// ===============================================================
//  nekoSaveSpot - 2009.7.7
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoSaveSpot_h__
#define __nekoSaveSpot_h__

#include <list>
using namespace std;
#include "nekoIBlock.h"
#include "nekoVariable.h"
#include "nekoByteBuffer.h"
#include "nekoVariable.h"

namespace neko
{
	enum E_SAVE_SPOT_FLAG
	{
		ESSF_BLOCK_STATE		= 0x00000001,
		ESSF_GLOBAL_VARIABLE	= 0x00000002,
		ESSF_LOCAL_VARIABLE		= 0x00000004,
		ESSF_STACK				= 0x00000008,
	};

	struct nekoSaveSpotBlockInstance
	{
		nekoByteBuffer buf;

		nekoSaveSpotBlockInstance() {}
		~nekoSaveSpotBlockInstance() {}
	};

	struct nekoSaveSpotStack
	{

	};

	class nekoGame;
	class nekoCommands;
	class nekoSaveSpot
	{
	protected:

		uint32 mFlags;

		/// Block Instance list
		list<nekoSaveSpotBlockInstance> mBlockInstanceList;

		/// Variable list
		list<nekoVariable> mVariableList;

		/// Stack list
		list<nekoSaveSpotStack> mStackList;

		friend class nekoGame;

	public:
		
		nekoSaveSpot(uint32 flags);
		virtual ~nekoSaveSpot();

		void Clear();

		/*
		void Refresh(nekoCommands *commands);

		/// 메모리에 불러오기.
		void Load();
		*/
		
	};
}


#endif // __nekoSaveSpot_h__