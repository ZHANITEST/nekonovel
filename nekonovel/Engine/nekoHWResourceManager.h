// ===============================================================
//  nekoHWResourceManager - 2009.7.6
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoHWResourceManager_h__
#define __nekoHWResourceManager_h__

#include <list>
using namespace std;

#include "nekoIRunnable.h"
#include "nekoIHWResource.h"
#include "nekoHWSound.h"
#include "nekoHWMusic.h"
#include "nekoHWFont.h"
#include "nekoHWTexture.h"
#include "nekoHWGIFImage.h"
#include "nekoVideoDriver.h"
#include "nekoSoundDriver.h"

namespace neko
{
	class nekoHWResourceManager : public nekoIRunnable
	{
	protected:

		nekoLockSection mLockSection;
		nekoVideoDriver *mVideoDriver;
		nekoSoundDriver *mSoundDriver;
		list<nekoIHWResource *> mManagedResourceList;

		friend class nekoIHWResource;

		void _AddHWResource(nekoIHWResource *resource);
		bool _RemoveHWResource(nekoIHWResource *resource);

		/// Wait until load
		bool mWaitingLoad;

	public:
		
		/// Constructor
		nekoHWResourceManager(nekoVideoDriver *videoDriver, nekoSoundDriver *soundDriver);
		
		/// Destructor
		~nekoHWResourceManager();

		nekoHWFont *LoadFont(const char8 *faceName, int32 width, int32 height, bool bold);
		nekoHWSound *LoadSound(const char8 *fileName);
		nekoHWTexture *LoadTexture(const char8 *fileName, uint32 colorKey = 0, bool preserveSize = false);
		nekoHWGIFImage *LoadGIF(const char8 *fileName);
		nekoHWMusic *LoadMusic(const char8 *fileName, uint32 repeat, float32 volume);

		/// Get Resource
		nekoIHWResource *GetResource(E_HW_RESOURCE_TYPE type, const char8 *name);

		/// Do something
		uint32 Do();

		void OnDeviceLost();
		void OnDeviceReset();

		inline nekoVideoDriver *GetVideoDriver() { return mVideoDriver; }
		inline nekoSoundDriver *GetSoundDriver() { return mSoundDriver; }

		inline void SetWaitingLoad(bool value) { mWaitingLoad = value; }
		inline bool IsWaitingLoad() { return mWaitingLoad; }
	};
}


#endif // __nekoHWResourceManager_h__