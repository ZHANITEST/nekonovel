#include "nkrtStdafx.h"
#include "nkrtMain.h"
#include <nekoVideoDriver.h>

using namespace nekoRt;

NekoNovel *nekoNovel;

namespace nekoRt
{
	bool nkrtEventListener::OnEvent(const nekoEvent &event)
	{
		if(event.type == EVENT_LOG)
		{
			// 경고에만!
			if(event.value[1] == (int32)LOG_LEVEL_WARN)
			{
				::MessageBox(0, event.GetText(0), "네코노벨 알림!", MB_ICONEXCLAMATION);
				return true;
			}
		}
		else if(event.type == EVENT_WINDOW_MESSAGE)
		{
			if(event.uMsg == WM_SYSKEYUP)
			{
				if(event.wParam == VK_F10)
				{
					nekoNovel->OnWindowChanged(-1, -1, !nekoNovel->GetVideoDriver()->IsFullScreen());
					return true;
				}
			}
			else if(event.uMsg == WM_KEYUP)
			{
				if(event.wParam == VK_ESCAPE)
				{

				}
			}
		}
		else if(event.type == EVENT_GAME_SAVED)
		{
			// 게임 저장 성공시
			if(SUCCEEDED(event.value[0]))
			{
				// 환경도 같이 저장해주자!
				nekoNovel->GetCurrentGame()->SaveEnvironment();
			}

		}

		return false;
	}
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	nekoNovel = GetNekoNovel();

	nekoAutoPtr<nkrtEventListener> mainListener = nekoNew nkrtEventListener;
	nekoNovel->AddEventListener(mainListener);

#ifndef _DEBUG
	{
		char8 buf[MAX_PATH];
		GetModuleFileName(NULL, buf, MAX_PATH);

		char8 *p = buf + strlen(buf);
		while(*p-- != '\\') {}; *(p + 2) = '\0';

		nekoNovel->SetAppFolderPath(buf);
	}
#endif

	if(FAILED(nekoNovel->Initialize(NULL, 800, 600, 0, hInstance)))
	{
		ReleaseNekoNovel();
		return -1;
	}

	if(nekoNovel->RunGame() == 0)
	{
		ReleaseNekoNovel();
		return -1;
	}

	// 릴리즈 모드라면..
	if(nekoNovel->IsReleased())
	{
		
	}

	while(true)
	{
		if(!nekoNovel->Update())
			break;

	}

	// 환경설정 저장하기
	nekoNovel->GetCurrentGame()->SaveEnvironment();

	nekoNovel->RemoveEventListener(mainListener);
	ReleaseNekoNovel();
	return 0;
}