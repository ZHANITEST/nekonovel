#ifndef NekoIBlock_h__
#define NekoIBlock_h__

#include "nekoIUnknown.h"
#include "nekoDefined.h"
#include "nekoByteBuffer.h"

/********************************************************************
	filename: 	NekoIBlock.h
	author:		kernys

	purpose:	Neko 블럭의 베이스
*********************************************************************/



namespace neko
{
	enum E_BLOCK_TYPE
	{
		BLOCK_IMAGE = 0,
		BLOCK_BGIMAGE,
		BLOCK_SOUND,
		BLOCK_MOVIE,
		BLOCK_BGMUSIC,
		BLOCK_SPRITE,
		BLOCK_BUTTON,
		BLOCK_SYSTEMBUTTON,
		BLOCK_TRACKBAR,
		BLOCK_VOICE,
		BLOCK_GIFSPRITE,
		BLOCK_LABEL,
		BLOCK_PARTICLE,
		BLOCK_FLASH,
		BLOCK_TEXTBOX,
		BLOCK_MESSAGELOG,
		BLOCK_UNKNOWN = -1,
	};

	class nekoIBlockInstance;
	class nekoIBlock : public nekoIUnknown
	{
	public:

		nekoIBlock(E_BLOCK_TYPE blockType)
			: mType(blockType)
		{

		}

		virtual ~nekoIBlock()
		{

		}

		virtual nekoIBlockInstance *CreateInstance() { return NULL; } /// 인스턴스를 만든다
		virtual E_BLOCK_TYPE GetType() { return mType; }

	protected:

    	E_BLOCK_TYPE mType;
		string mName;

	};
}


#endif
