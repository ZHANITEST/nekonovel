// ===============================================================
//  nekoIHWResource - 2009.7.6
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoIHWResource_h__
#define __nekoIHWResource_h__

#include <list>
using namespace std;
#include "nekoIUnknown.h"
#include "nekoAutoPtr.h"
#include "nekoLockSection.h"
#include "nekoTimer.h"
#include "nekoIEventListener.h"
#include "nekoLogger.h"

namespace neko
{
	enum E_HW_RESOURCE_STATE
	{
		EHRS_NONE = 0,
		EHRS_LOADING,
		EHRS_LOADED,
		EHRS_UNLOADING,
	};

	enum E_HW_RESOURCE_TYPE
	{
		EHRT_SOUND = 0,
		EHRT_TEXTURE,
		EHRT_MUSIC,
		EHRT_FONT,
		EHRT_PARTICLE,
		EHRT_GIFIMAGE,
	};

	class nekoHWResourceManager;
	
	class nekoIHWResource : public nekoIUnknown
	{
	protected:

		nekoLockSection mLockSeciton;
		nekoHWResourceManager *mResManager;

		E_HW_RESOURCE_TYPE mType;

		/// State
		E_HW_RESOURCE_STATE mState;

		/// Name
		string mName;

		/// Last Return Code
		result mLastReturnCode;

		/// Auto-Dropping enabled ?
		bool mAutoDropping;

		/// Timeout timer
		nekoTimer mTimeoutTimer;

		/// Event listeners
		list<nekoIEventListener *> mEventListeners;

		///
		bool mSharable;
	
	private:

		nekoTimer mReleaseTimer;

	public:
		
		/// Constructor
		nekoIHWResource(nekoHWResourceManager *resManager, E_HW_RESOURCE_TYPE type);
		
		/// Destructor
		virtual ~nekoIHWResource();

		inline bool IsAutoDropEnabled() { return mAutoDropping; }
		inline E_HW_RESOURCE_STATE GetState() const { return mState; }
		inline result GetLastReturnCode() { return mLastReturnCode; }

		/// 리소스 잡기를 요청.
		virtual void Catch() { mLockSeciton.Begin(); mState = EHRS_LOADING; mLockSeciton.End();}
		virtual void Drop() { mLockSeciton.Begin(); mState = EHRS_UNLOADING; mLockSeciton.End(); }

		virtual void UpdateInThread(float32 frameTime)
		{
			mLockSeciton.Begin();

			if(mAutoDropping && mState == EHRS_LOADED)
			{			
				mTimeoutTimer.Update(frameTime);

				if(mTimeoutTimer.IsCompleted())
				{
					Drop();
				}
			}

			if(mReleaseTimer.GetTimerTime() >= 0)
			{
				/// will be released soon..
				if(this->GetRefCount() == 1)
				{
					mReleaseTimer.Update(frameTime);
					if(mReleaseTimer.IsCompleted())
					{
						this->Release();
						return;
					}
				}
				else
				{
					mReleaseTimer.Clear();
				}
			}
				
			mLockSeciton.End();	
		}

		inline const char8 *GetName() { return mName.c_str(); }
		inline E_HW_RESOURCE_TYPE GetType() { return mType; }
		inline void SetTimeout(float32 timeoutTime) { mTimeoutTimer.SetTime(timeoutTime); }
		inline void SetReleaseDelay(float32 delayTime) { mReleaseTimer.SetTime(delayTime); }

		inline void SetSharable(bool v) { mSharable = v; }
		inline bool IsSharable() { return mSharable; }

		/// On Device Lost
		virtual void OnDeviceLost() {}

		/// On Device Reset
		virtual void OnDeviceReset() {}

		/// 실제로 로드/언로드를 시도하는 메소드
		virtual result OnCatching() { mState = EHRS_LOADED; return S_OK; }
		virtual result OnDropping() { mState = EHRS_NONE; mTimeoutTimer.Clear(); return S_OK; }

		/// 갱신됨.
		virtual void Refresh(bool waitingToLoad = false);
		
	};
}


#endif // __nekoIHWResource_h__