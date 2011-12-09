#include "nekoPrecompiledHeader.h"
#include "nekoIHWResource.h"
#include "nekoVideoDriver.h"
#include "nekoHWResourceManager.h"
#include "NekoNovel.h"

namespace neko
{
	/// Constructor
	nekoIHWResource::nekoIHWResource(nekoHWResourceManager *resManager, E_HW_RESOURCE_TYPE type)
		: mLastReturnCode(0)
		, mState(EHRS_NONE)
		, mAutoDropping(true)
		, mResManager(resManager)
		, mTimeoutTimer(10)
		, mReleaseTimer(5)
		, mType(type)
		, mSharable(true)
	{
		mResManager->_AddHWResource(this);
	}

	/// Destructor
	nekoIHWResource::~nekoIHWResource()
	{
		mResManager->_RemoveHWResource(this);
	}

	/// °»½ÅµÊ.
	void nekoIHWResource::Refresh(bool waitingToLoad)
	{
		mLockSeciton.Begin();

		mTimeoutTimer.Clear();
		if(SUCCEEDED(mLastReturnCode) && mState != EHRS_LOADED)
		{
			Catch();

			if(mResManager->IsWaitingLoad() || waitingToLoad)
			{
				OnCatching();
				GetNekoNovel()->RefreshLastTime();
			}

			mLockSeciton.End();

			return;
		}

		mLockSeciton.End();
	}
}