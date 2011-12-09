#include "nekoPrecompiledHeader.h"
#include "nekoNovelReal.h"
#include "nekoLogger.h"
#include "nekoVideoDriver.h"
#include "nekoSoundDriver.h"
#include "nekoHWResourceManager.h"
#include "nekoImage.h"
#include "nekoBGImage.h"
#include "nekoMovie.h"
#include "nekoSound.h"
#include "nekoBGMusic.h"
#include "nekoSprite.h"
#include "nekoButton.h"
#include "nekoSystemButton.h"
#include "nekoTrackBar.h"
#include "nekoVoice.h"
#include "nekoLabel.h"
#include "nekoFlash.h"
#include "nekoParticle.h"
#include "nekoTextBox.h"
#include "nekoMessageLog.h"
#include "nekoGIFSprite.h"

#include <time.h>

#include "nekoUtility.h"
#include <dbghelp.h>
#pragma comment(lib, "dbghelp")
#include <BugTrap.h>
#pragma comment(lib, "bugtrap")

namespace neko
{
	const int32 defWindowStyle = WS_OVERLAPPED|WS_CAPTION|WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX;
	LRESULT CALLBACK WndProc(HWND hWnd, UINT32 uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch(uMsg)
		{
		case WM_IME_SETCONTEXT:
			return 0;
		case WM_CLOSE:
			{
				if(GetNekoNovel()->GetCurrentGame())
				{
					bool fullscreen = GetNekoNovel()->GetVideoDriver()->IsFullScreen();
					if(fullscreen)
						GetNekoNovel()->GetVideoDriver()->OnWindowChanged(-1, -1, false);

					if(MessageBox(hWnd, nekoUtility::FormString("'%s'를 종료하시겠습니까 ?", GetNekoNovel()->GetCurrentGame()->GetName()), 
						GetNekoNovel()->GetCurrentGame()->GetName(), MB_YESNO|MB_ICONEXCLAMATION|MB_TOPMOST) == IDNO)
					{
						if(fullscreen)
							GetNekoNovel()->GetVideoDriver()->OnWindowChanged(-1, -1, true);

						return 0;
					}

				}
				PostQuitMessage(0);
				break;
			}
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}
		nekoEvent e(EVENT_WINDOW_MESSAGE);
		e.uMsg = uMsg;
		e.wParam = wParam;
		e.lParam = lParam;

		GetNekoNovel()->OnEvent(e);

		switch(uMsg)
		{
		case WM_CHAR:
		case WM_IME_STARTCOMPOSITION:
		case WM_IME_ENDCOMPOSITION:
		case WM_IME_COMPOSITION:
		case WM_IME_NOTIFY:
		case WM_IME_CHAR:
			return 0;
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	// 엔진이 얼떄 호출되는 함수
	void CALLBACK OnEngineFreezed(INT_PTR param)
	{
		nekoEvent e(EVENT_ENGINE_FREEZED);
		e.value[0] = param;
		GetNekoNovel()->OnEvent(e);
	}

	nekoNovelReal::nekoNovelReal()
		: mHwnd(NULL)
		, mPlayingGame(NULL)
		, mReleased(true)
		, mFlashLib(NULL)
		, mFlashLibCount(0)
		, mInitialized(false)
		, mZoomLevel(1.0f)
		, mIMEHandle(0)
		, mFlashTimeoutTimer(5)
		, mAlwaysTop(false)
	{
		BT_InstallSehFilter();

		BT_SetAppName("네코노벨에 오류가 났어요 ㅠ_ㅠ..");
		//BT_SetSupportEMail("lesiabugs@gmail.com");
		BT_SetFlags(BTF_DETAILEDMODE);
		//BT_SetSupportServer(address, 9999);
		BT_SetSupportURL("http://cafe.naver.com/kernyslab");
		BT_SetDumpType( MiniDumpWithDataSegs | MiniDumpWithIndirectlyReferencedMemory );
		//BT_SetPreErrHandler(&OnEngineFreezed, 0);


#ifdef _DEBUG
		_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG | _CRTDBG_LEAK_CHECK_DF);
#endif
		//_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG | _CRTDBG_LEAK_CHECK_DF);
		//_CrtSetBreakAlloc(59060);

		// 폴더 위치.
		char8 buf[MAX_PATH];
		GetCurrentDirectory(512, buf);
		mAppFolderPath = buf;
		if(mAppFolderPath[mAppFolderPath.length() - 1] != '\\')
			mAppFolderPath += '\\';

		// 블럭 추가하기
		mBlockList.push_back(nekoNew nekoImage);
		mBlockList.push_back(nekoNew nekoBGImage);
		mBlockList.push_back(nekoNew nekoMovie);
		mBlockList.push_back(nekoNew nekoSound);
		mBlockList.push_back(nekoNew nekoBGMusic);
		mBlockList.push_back(nekoNew nekoSprite);
		mBlockList.push_back(nekoNew nekoButton);
		mBlockList.push_back(nekoNew nekoSystemButton);
		mBlockList.push_back(nekoNew nekoTrackBar);
		mBlockList.push_back(nekoNew nekoVoice);
		mBlockList.push_back(nekoNew nekoLabel);
		mBlockList.push_back(nekoNew nekoFlash);
		mBlockList.push_back(nekoNew nekoParticle);
		mBlockList.push_back(nekoNew nekoTextBox);
		mBlockList.push_back(nekoNew nekoMessageLog);
		mBlockList.push_back(nekoNew nekoGIFSprite);

		CoInitialize(0);

		srand(time(NULL));
	}

	nekoNovelReal::~nekoNovelReal()
	{
		mPlayingGame = NULL;

		if(mResourceMgr)
		{
			mResourceMgr->Stop();
			mResourceMgr = NULL;
		}

		list<nekoIBlock *>::iterator iter = mBlockList.begin();
		for(uint32 i = 0;i < mBlockList.size();++i, ++iter)
			(*iter)->Release();

		mVideoDriver = NULL;
		mSoundDriver = NULL;
		mFileSystem = NULL;

		if(mIMEHandle)
		{
			ImmReleaseContext(mHwnd, mIMEHandle);
		}

		if(mOwnerOfHwnd && mHwnd)
		{
			DestroyWindow(mHwnd);
		}

		if(mFlashLib)
		{
			FreeLibrary(mFlashLib);
		}

		CoUninitialize();
		UnregisterClass(NEKO_CLASSNAME, GetModuleHandle(NULL));
	}

	nekoGame *nekoNovelReal::LoadGame()
	{
		if(mPlayingGame != NULL)
		{
			LogPrint(LOG_LEVEL_WARN, "열려 있는 게임을 닫아주세요!");
			return 0;
		}

		mFileSystem = nekoNew nekoFileSystem(EFM_LOCALFILE);
		mPlayingGame = nekoNew nekoGame(this);
		if(FAILED(mPlayingGame->Initialize()))
			return 0;
		if(FAILED(mPlayingGame->Load()))
			return 0;

		mPlayingGame->ChangeGameMode(GAME_RUN_PREVIEW);

		mInitialized = true;
		return mPlayingGame;
	}

	nekoGame *nekoNovelReal::RunGame()
	{
		if(mPlayingGame != NULL)
		{
			LogPrint(LOG_LEVEL_WARN, "열려 있는 게임을 닫아주세요!");
			return 0;
		}

		mResourceMgr->SetWaitingLoad(true);

		// 프로젝트 파일이 있는지 확인
		string fullPath = GetNekoNovel()->GetAppFolderPath();
		FILE *fp = fopen((fullPath + "Game.nknovel").c_str(), "rb");
		if(fp != NULL)
		{
			mReleased = false;

			mFileSystem = nekoNew nekoFileSystem(EFM_LOCALFILE);
			char8 *xmlData;
			int32 length;

			// 파일 로드.
			fseek(fp, 0, SEEK_END);
			length = ftell(fp);
			fseek(fp, 0, SEEK_SET);
			xmlData = nekoNew char8[length + 1];
			xmlData[length] = '\0';
			fread(xmlData, length, 1, fp);
			fclose(fp);

			mPlayingGame = nekoNew nekoGame(this);
			if(FAILED(mPlayingGame->Initialize()))
			{
				delete []xmlData;
				return 0;
			}
			if(FAILED(mPlayingGame->Load(xmlData)))
			{
				delete []xmlData;
				return 0;
			}
			delete []xmlData;

			nekoCommands::LoadCommandReplacements();
			mPlayingGame->LoadSaveDatas();

			// 스크립트 불러오기
			nekoCommands *cmds = mPlayingGame->CreateCommands(mPlayingGame->GetEnvironment().mFirstScriptName.c_str());
			if(FAILED(cmds->LoadFromFile(mPlayingGame->GetEnvironment().mFirstScriptName.c_str())))
				return 0;

			mPlayingGame->ChangeGameMode(GAME_RUN_PLAYING);
			mPlayingGame->GotoLine(cmds, 1, true);
			mPlayingGame->LoadEnvironment();
			
			mPlayingGame->OnRunCompleted();

			mInitialized = true;
			return mPlayingGame;
		}

		// 여기서부턴 진짜 런타임이다!
		mReleased = true;

		mFileSystem = nekoNew nekoFileSystem(EFM_PACKAGE);

		// 파일 시스템 셋팅
		mFileSystem->LoadPackage("Core.nkpack");
		mFileSystem->LoadPackage("FX.nkpack");
		mFileSystem->LoadPackage("Images.nkpack");
		mFileSystem->LoadPackage("BGM.nkpack");
		mFileSystem->LoadPackage("Scripts.nkpack");
		mFileSystem->LoadPackage("Voice.nkpack");
		mFileSystem->LoadPackage("Particles.nkpack");

		mPlayingGame = nekoNew nekoGame(this);
		if(FAILED(mPlayingGame->Initialize()))
			return 0;
		if(FAILED(mPlayingGame->Run()))
			return 0;

		nekoCommands::LoadCommandReplacements();
		mPlayingGame->LoadSaveDatas();

		nekoCommands *cmds = mPlayingGame->CreateCommands(mPlayingGame->GetEnvironment().mFirstScriptName.c_str());
		if(FAILED(cmds->LoadFromFile(mPlayingGame->GetEnvironment().mFirstScriptName.c_str())))
			return 0;

		mPlayingGame->ChangeGameMode(GAME_RUN_PLAYING);
		mPlayingGame->GotoLine(cmds, 1, true);
		mPlayingGame->LoadEnvironment();

		mPlayingGame->OnRunCompleted();

		mInitialized = true;
		return mPlayingGame;
	}

	result nekoNovelReal::Initialize(HWND canvas, int32 width, int32 height, uint32 flags, HINSTANCE hInst)
	{
		result hr;

		mWidth = width;
		mHeight = height;
		mRealWidth = width;
		mRealHeight = height;

		/// 윈도우 생성
		if(canvas == NULL)
		{
			WNDCLASSEX wndClass = {0, };
			wndClass.cbSize = sizeof(WNDCLASSEX);
			wndClass.hInstance = (hInst) ? hInst : GetModuleHandle(NULL);
			wndClass.lpfnWndProc = WndProc;
			wndClass.style = CS_CLASSDC;
			wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
			wndClass.lpszClassName = NEKO_CLASSNAME;
			wndClass.hIcon = (HICON)LoadImage(hInst, MAKEINTRESOURCE(101), IMAGE_ICON, 0, 0, 0);
			wndClass.hCursor = LoadCursor(hInst, MAKEINTRESOURCE(103));
			RegisterClassEx(&wndClass);

			RECT rect = {0, 0, width, height};
			AdjustWindowRect(&rect, defWindowStyle, false);

			HWND hDesktop = GetDesktopWindow();
			RECT rtDesktop;
			GetClientRect(hDesktop, &rtDesktop);

			mHwnd = CreateWindow(NEKO_CLASSNAME, NEKO_WINDOWNAME, defWindowStyle, (rtDesktop.right - (rect.right - rect.left )) / 2,
				(rtDesktop.bottom - (rect.bottom - rect.top )) / 2, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL,
				wndClass.hInstance, NULL);
			if(mHwnd == NULL)
			{
				LogPrint(LOG_LEVEL_ERRO, "새로운 윈도우를 생성할 수 없습니다");
				return E_FAIL;
			}

			mOwnerOfHwnd = true;
		}
		else
		{
			mHwnd = canvas;
			mOwnerOfHwnd = false;
		}

		mVideoDriver = nekoNew nekoVideoDriver(width, height);
		if(FAILED(hr = mVideoDriver->Initialize(mHwnd, width, height, (flags & ENF_FULLSCREEN))))
			return hr;

		mSoundDriver = nekoNew nekoSoundDriver;
		if(FAILED(hr = mSoundDriver->Initialize()))
			return hr;

		mResourceMgr = nekoNew nekoHWResourceManager(mVideoDriver, mSoundDriver);
		mResourceMgr->Start();

		// 현재 IME 컨텍스트를 얻는다.
		mIMEHandle = ImmGetContext(mHwnd);

		ImmSetConversionStatus(mIMEHandle, IME_CMODE_HANGEUL, IME_CMODE_HANGEUL);

		/*
		DWORD dwTemp, dwConv, dwSent;
		ImmGetConversionStatus(mIMEHandle, &dwConv, &dwSent);

		// 한글 IME 설정을 위한 작업
		if(!(dwConv & IME_CMODE_NATIVE)) 
		{
			dwTemp = dwConv & ~IME_CMODE_LANGUAGE;
			dwTemp |= IME_CMODE_NATIVE;
			dwConv = dwTemp;
		}
		ImmSetConversionStatus(mIMEHandle, dwConv, dwSent);
		*/


		mLastTime = timeGetTime();
		return S_OK;	
	}

	bool nekoNovelReal::Update()
	{
		uint32 l = timeGetTime();
		float32 frameTime = (l - mLastTime) * 0.001f;
		mLastTime = l;

		if(mOwnerOfHwnd)
		{
			MSG msg;
			while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if(msg.message == WM_QUIT || msg.message == WM_QUIT)
				{
					return false;
				}

				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		if(mPlayingGame) mPlayingGame->Update(frameTime);
		if(mSoundDriver) mSoundDriver->Update(frameTime);

		if(mVideoDriver)
		{
			if(SUCCEEDED(mVideoDriver->BeginRender()))
			{
				if(mPlayingGame)
					mPlayingGame->Draw();
			}


#if (0)
			static int32 frameCount = 0;
			static float32 time = 0.0f;
			static float32 fps = 0.0f;

			time += frameTime;
			frameCount++;

			if(time >= 1.0f)
			{
				fps = frameCount / time;
				time = 0;
				frameCount = 0;


			}

			mVideoDriver->DrawText(NULL, 20, 20, 100, 100, nekoUtility::FormString("FPS : %.1f", fps));
#endif

			mVideoDriver->EndRender();
		}

		if(mFlashLib && !mFlashLibCount)
		{
			mFlashTimeoutTimer.Update(frameTime);
			if(mFlashTimeoutTimer.IsCompleted())
			{
				FreeLibrary(mFlashLib);
				mFlashLib = 0;
			}
		}

		mLockSection.Begin();
		list<string>::iterator iter = mLogList.begin();
		list<int32>::iterator iter2 = mLogLevelList.begin();
		for(;iter != mLogList.end();++iter, ++iter2)
		{
			nekoEvent e(EVENT_LOG);
			e.SetText(0, (*iter).c_str());
			e.value[1] = (*iter2);
			
			nekoIEventListener::OnEvent(e);
		}
		mLogList.clear();
		mLogLevelList.clear();
		mLockSection.End();

		Sleep(1);
		return true;
	}

	nekoIBlock *nekoNovelReal::GetBlock(E_BLOCK_TYPE blockType)
	{
		list<nekoIBlock *>::iterator iter = mBlockList.begin();
		for(uint32 i = 0;i < mBlockList.size();++i, ++iter)
		{
			if((*iter)->GetType() == blockType)
				return (*iter);
		}

		return NULL;
	}

	void nekoNovelReal::ResetWindowStyle(bool fullscreen)
	{
		if(!mHwnd || !mOwnerOfHwnd)
			return;

		HWND hDesktop = GetDesktopWindow();
		RECT rtDesktop;
		GetClientRect(hDesktop, &rtDesktop);

		SetWindowLong(mHwnd, GWL_STYLE, (fullscreen) ? WS_POPUP|WS_CLIPCHILDREN : defWindowStyle);
		RECT rect = {0, 0, mRealWidth, mRealHeight};
		AdjustWindowRect(&rect, defWindowStyle, false);
		SetWindowPos(mHwnd, HWND_TOP, (rtDesktop.right - (rect.right - rect.left )) / 2,
			(rtDesktop.bottom - (rect.bottom - rect.top )) / 2, rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW);

		// 항상위 플래그 재적용.(풀스크린 일 경우 무조건 항상위를 유지하도록.)
		if(fullscreen == false)
			SetAlwaysTop(mAlwaysTop);
	}

	bool nekoNovelReal::OnEvent(const nekoEvent &event)
	{
		if(event.type == EVENT_LOG)
		{
			if(mInitialized == false)
			{
				nekoIEventListener::OnEvent(event);
				return true;
			}

			mLockSection.Begin();
			mLogList.push_back(event.GetText(0));
			mLogLevelList.push_back(event.value[1]);
			mLockSection.End();
			return true;
		}

		if(mPlayingGame)
		{
			if(event.type == EVENT_WINDOW_MESSAGE)
			{
				if(mZoomLevel != 1.0f)
				{
					switch(event.uMsg)
					{
					case WM_MOUSEMOVE:
					case WM_LBUTTONDOWN:
					case WM_LBUTTONUP:
					case WM_RBUTTONDOWN:
					case WM_RBUTTONUP:
					case WM_MBUTTONDOWN:
					case WM_MBUTTONUP:
						*(LPARAM *)(&event.lParam) = MAKELONG((int32)(LOWORD(event.lParam) / mZoomLevel), (int32)(HIWORD(event.lParam) / mZoomLevel));
						break;
					}
				}

				if(IsUsingVirtualFullScreen())
				{
					switch(event.uMsg)
					{
					case WM_MOUSEMOVE:
					case WM_LBUTTONDOWN:
					case WM_LBUTTONUP:
					case WM_RBUTTONDOWN:
					case WM_RBUTTONUP:
					case WM_MBUTTONDOWN:
					case WM_MBUTTONUP:
						{
							int32 x = LOWORD(event.lParam);
							int32 y = HIWORD(event.lParam);

							// 원래 비율
							float32 hratio = (float32)mRealWidth / mWidth;
							float32 vratio = (float32)mRealHeight / mHeight;
							float32 rratio = (float32)mRealWidth / mRealHeight;

							if(rratio - 4.f / 3.f < -REAL_Z_DIFF)
							{
								x -= (mRealWidth - (vratio * mWidth)) / 2.0f;
								x /= vratio;
								y /= vratio;
							}
							else
							{
								y -= (mRealHeight - (hratio * mHeight)) / 2.0f;
								x /= hratio;
								y /= hratio;
							}
							*(LPARAM *)(&event.lParam) = MAKELONG(x, y);
						}
						break;
					}
				}
			}

			if(mPlayingGame->OnEvent(event))
				return true;
		}

		return nekoIEventListener::OnEvent(event);
	}

	void nekoNovelReal::OnWindowChanged(int32 width, int32 height, bool fullscreen)
	{
		if(mVideoDriver)
		{
			if(width > 0) mWidth = width;
			if(height > 0) mHeight = height;

			if(FAILED(mVideoDriver->OnWindowChanged(width, height, fullscreen)))
			{
				// 복구
				if(FAILED(mVideoDriver->OnWindowChanged(width, height, false)))
					return;
			}

			nekoEvent e(EVENT_WINDOW_SIZE_CHANGED);
			e.value[0] = width;
			e.value[1] = height;
			OnEvent(e);
		}
	}

	void nekoNovelReal::SetScreenZoom(float32 zoomLevel)
	{
		mZoomLevel = zoomLevel;
	}

	void nekoNovelReal::Exit()
	{
		if(mOwnerOfHwnd == true)
		{
			PostQuitMessage(0);
		}
	}

	HMODULE nekoNovelReal::GetFlashLib()
	{
		if(mFlashLib)
		{
			mFlashLibCount++;
			RefreshFlashTimeout();

			return mFlashLib;
		}

		mFlashLib = LoadLibrary((mAppFolderPath + "Flash.ocx").c_str());
		if(mFlashLib == NULL)
		{
			return NULL;
		}

		mFlashLibCount++;
		RefreshFlashTimeout();

		return mFlashLib;
	}

	void nekoNovelReal::ReleaseFlashLib()
	{
		if(mFlashLibCount > 0)
		{
			--mFlashLibCount;
		}
	}

	void nekoNovelReal::SetAlwaysTop(bool enabled)
	{
		mAlwaysTop = enabled;

		SetWindowPos(mHwnd, (enabled) ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
	}

	void nekoNovelReal::LoadExternalFonts(const string &fontList, bool unload)
	{
		const char8 *p = fontList.c_str();
		char8 buf[128];
		uint32 bufLen = 0;

		do
		{
			if(*p == '\r' && *(p + 1) == '\n')
				++p;

			if(*p == '\n' || *p == '\0')
			{
				if(bufLen)
				{
					buf[bufLen] = 0;

					if(!unload)
					{
						if(!AddFontResourceEx(buf, FR_PRIVATE, 0))
						{
							nekoLogger::Log(LOG_LEVEL_WARN, "외부 폰트 '%s' 를 불러올 수 없습니다.", buf);
						}
					}
					else
						RemoveFontResourceEx(buf, FR_PRIVATE, 0);

					bufLen = 0;
				}

				if(*p == '\0')
					break;

				++p;
				continue;
			}

			buf[bufLen++] = *p++;
		}
		while(true);
	}
}