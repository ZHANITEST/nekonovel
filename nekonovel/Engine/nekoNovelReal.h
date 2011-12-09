// ===============================================================
//  nekoNovelReal - 2009.7.6
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoNovelReal_h__
#define __nekoNovelReal_h__

#include "NekoNovel.h"
#include "nekoVideoDriver.h"
#include "nekoSoundDriver.h"
#include "nekoHWResourceManager.h"
#include "nekoFileSystem.h"

namespace neko
{

	class nekoHWResourceManager;

	class nekoNovelReal : public NekoNovel
	{
	protected:

		bool mInitialized;
		bool mReleased;
		bool mOwnerOfHwnd;
		HWND mHwnd;
		int32 mWidth;
		int32 mHeight;
		int32 mRealWidth;
		int32 mRealHeight;
		nekoAutoPtr<nekoGame> mPlayingGame;
		nekoAutoPtr<nekoVideoDriver> mVideoDriver;
		nekoAutoPtr<nekoSoundDriver> mSoundDriver;
		nekoAutoPtr<nekoFileSystem> mFileSystem;
		nekoAutoPtr<nekoHWResourceManager> mResourceMgr;
		uint32 mLastTime;

		string mAppFolderPath;
		nekoLockSection mLockSection;
		
		list<nekoIBlock *> mBlockList;
		list<string> mLogList;
		list<int32> mLogLevelList;

		float32 mZoomLevel;

		HMODULE mFlashLib;
		int32 mFlashLibCount;
		nekoTimer mFlashTimeoutTimer;

		HIMC mIMEHandle;
		bool mAlwaysTop;

		friend NekoNovel *GetNekoNovel();

	public:
		
		nekoNovelReal();
		virtual ~nekoNovelReal();

		bool IsActive() { return GetActiveWindow() == mHwnd; }
		HWND GetHWND() { return mHwnd; }
		bool IsOwnerOfHWND() { return mOwnerOfHwnd; }
		nekoGame *LoadGame(); // 미미에서 불러올 때
		nekoGame *RunGame(); // 런타임으로 실행할 때

		result Initialize(HWND canvas, int32 width, int32 height, uint32 flags, HINSTANCE hInst = NULL);
		bool Update();
		nekoIBlock *GetBlock(E_BLOCK_TYPE blockType);
		inline nekoGame *GetCurrentGame() { return mPlayingGame; }
		inline nekoVideoDriver *GetVideoDriver() { return mVideoDriver; }
		inline nekoSoundDriver *GetSoundDriver() { return mSoundDriver; }
		inline nekoFileSystem *GetFileSystem() { return mFileSystem; }
		inline nekoHWResourceManager *GetHWResourceManager() { return mResourceMgr; }
		void ResetWindowStyle(bool fullscreen);

		inline void SetWindowTitle(const char8 *title)
		{
			if(mOwnerOfHwnd && mHwnd)
				SetWindowText(mHwnd, title);
		}
		void SetAppFolderPath(const char8 *str) { mAppFolderPath = str; }
		const char8 *GetAppFolderPath() const
		{
			return mAppFolderPath.c_str();
		}
		bool OnEvent(const nekoEvent &event);
		void OnWindowChanged(int32 width, int32 height, bool fullscreen);

		void SetScreenZoom(float32 zoomLevel = 1.0f);

		bool IsReleased() { return mReleased; }

		void Exit();

		HMODULE GetFlashLib();
		void ReleaseFlashLib();

		inline HIMC GetIMEHandle() { return mIMEHandle; }
		
		int32 GetWidth() { return mWidth; }
		int32 GetHeight() { return mHeight; }
		int32 GetRealWidth() { return mRealWidth; }
		int32 GetRealHeight() { return mRealHeight; }
		inline void SetRealSize(int32 width, int32 height)
		{
			mRealWidth = width;
			mRealHeight = height;
		}
		inline bool IsUsingVirtualFullScreen()
		{
			return (mRealWidth != mWidth || mRealHeight != mHeight);
		}

		inline bool IsAlwaysTop() { return mAlwaysTop; }
		void SetAlwaysTop(bool enabled);

		/// Refresh last time
		inline void RefreshLastTime() { mLastTime = timeGetTime(); }
		inline void RefreshFlashTimeout() { mFlashTimeoutTimer.Clear(); }

		void LoadExternalFonts(const string &fontList, bool unload = false);
	};
}


#endif // __nekoNovelReal_h__