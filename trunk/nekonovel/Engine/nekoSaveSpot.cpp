#include "nekoPrecompiledHeader.h"
#include "nekoSaveSpot.h"
#include "nekoCommands.h"
#include "nekoLogger.h"
#include "NekoNovel.h"

namespace neko
{
	nekoSaveSpot::nekoSaveSpot(uint32 flags)
		: mFlags(flags)
	{

	}

	nekoSaveSpot::~nekoSaveSpot()
	{

	}

	void nekoSaveSpot::Clear()
	{

	}

	/*
	/// Refresh the spot
	void nekoSaveSpot::Refresh(nekoCommands *commands)
	{
		mBlockInstanceList.clear();
		mVariableList.clear();
		mStackList.clear();

		if(mFlags & ESSF_BLOCK_STATE)
		{
			list<nekoIBlockInstance *> &lst = GetNekoNovel()->GetCurrentGame()->GetBlockInstanceList();
			list<nekoIBlockInstance *>::iterator iter = lst.begin();

			for(;iter != lst.end();++iter)
			{
				mBlockInstanceList.push_back(nekoSaveSpotBlockInstance());
				nekoSaveSpotBlockInstance &inst = (*(--mBlockInstanceList.end()));

				(*iter)->Dump(inst.buf);
			}
		}
	}

	/// 메모리에 불러오기.
	void nekoSaveSpot::Load()
	{
		if(mFlags & ESSF_BLOCK_STATE)
		{
			list<nekoSaveSpotBlockInstance>::iterator iter = mBlockInstanceList.begin();

			E_BLOCK_TYPE blockType;
			nekoIBlock *block;
			nekoIBlockInstance *inst;

			for(;iter != mBlockInstanceList.end();++iter)
			{
				nekoByteBuffer &buf = (*iter).buf;

				buf >> blockType;

				block = GetNekoNovel()->GetBlock(blockType);
				if(block == NULL)
				{
					LogPrint(LOG_LEVEL_WARN, "'%d' 블럭을 찾을 수 없습니다.", blockType);
					continue;
				}

				inst = GetNekoNovel()->GetCurrentGame()->AddInstance(block->CreateInstance());
				inst->Load(version, buf);
			}
		}
	}
	*/
}