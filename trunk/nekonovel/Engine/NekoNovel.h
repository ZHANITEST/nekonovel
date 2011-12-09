#ifndef NekoNovel_h__
#define NekoNovel_h__

#include <windows.h>
#include "nekoIEventListener.h"
#include "nekoDefined.h"
#include "nekoDataTypes.h"
#include "nekoAutoPtr.h"
#include "nekoGame.h"

/********************************************************************
	filename: 	NekoNovel.h
	author:		kernys
	
	purpose:	Neko Novel의 시작지점이다.
*********************************************************************/


namespace neko
{
#	define ENGINE_CODENAME "NEKONOVEL -_-v"
	const int32 ENGINE_VERSION = 122;

	enum E_NEKONOVEL_FLAGS
	{
		ENF_FULLSCREEN = 0x00000001,
		ENF_FORCE32BIT = 0x00000002,
	};

	class NEKONOVEL_API nekoVideoDriver;
	class NEKONOVEL_API nekoSoundDriver;
	class NEKONOVEL_API nekoHWResourceManager;
	class NEKONOVEL_API nekoFileSystem;

	class NEKONOVEL_API NekoNovel;
	NEKONOVEL_API NekoNovel *GetNekoNovel();
	NEKONOVEL_API void ReleaseNekoNovel();

	class NEKONOVEL_API NekoNovel : public nekoIEventListener
	{
	public:

		NekoNovel() {}
		~NekoNovel() {}

		int32 GetVersion() { return ENGINE_VERSION; }

		virtual bool IsActive() = 0;
		virtual HWND GetHWND() = 0;
		virtual bool IsOwnerOfHWND() = 0;
		virtual nekoGame *LoadGame() = 0;
		virtual nekoGame *RunGame() = 0;
		virtual result Initialize(HWND canvas, int32 width, int32 height, uint32 flags, HINSTANCE hInst = NULL) = 0;
		virtual bool Update() = 0;
		virtual nekoIBlock *GetBlock(E_BLOCK_TYPE blockType) = 0;
		virtual nekoGame *GetCurrentGame() = 0;
		virtual nekoVideoDriver *GetVideoDriver() = 0;
		virtual nekoSoundDriver *GetSoundDriver() = 0;
		virtual nekoFileSystem *GetFileSystem() = 0;
		virtual nekoHWResourceManager *GetHWResourceManager() = 0;
		virtual void ResetWindowStyle(bool fullscreen) = 0;
		virtual void Exit() = 0;

		virtual bool IsReleased() = 0;
		virtual void OnWindowChanged(int32 width, int32 height, bool fullscreen) = 0;
		virtual void SetWindowTitle(const char8 *title) = 0;
		virtual void SetAppFolderPath(const char8 *str) = 0;
		virtual const char8 *GetAppFolderPath() const = 0;

		virtual void SetScreenZoom(float32 zoomLevel = 1.0f) = 0;

		virtual HMODULE GetFlashLib() = 0;
		virtual void ReleaseFlashLib() = 0;

		virtual HIMC GetIMEHandle() = 0;

		virtual int32 GetWidth() = 0;
		virtual int32 GetHeight() = 0;
		virtual int32 GetRealWidth() = 0;
		virtual int32 GetRealHeight() = 0;
		virtual void SetRealSize(int32 width, int32 height) = 0;
		virtual bool IsUsingVirtualFullScreen() = 0;

		virtual bool IsAlwaysTop() = 0;
		virtual void SetAlwaysTop(bool enabled) = 0;

		/// Refresh last time
		virtual void RefreshLastTime() = 0;

		virtual void LoadExternalFonts(const string &fontList, bool unload = false) = 0;
	};
}


#endif