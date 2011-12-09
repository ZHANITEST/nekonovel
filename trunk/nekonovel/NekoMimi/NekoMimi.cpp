// NekoMimi.cpp : 기본 프로젝트 파일입니다.

#include "stdafx.h"
#include "MainForm.h"
#include "GamePublishForm.h"

using namespace System::Threading;
using namespace NekoMimi;

class MainEventListener : public nekoIEventListener
{
public:


	/// 이벤트 도착시
	bool OnEvent(const nekoEvent &event)
	{
		if(event.type == EVENT_LOG)
		{
			frmMain::OnLogReceived(event);
			return true;	
		}
		else if(event.type == EVENT_PUBLISHING_STATUS)
		{
			GamePublishForm::OnStatusUpdated(event);
			return true;
		}
		// 엔진이 얼었다.
		else if(event.type == EVENT_ENGINE_FREEZED)
		{
			frmMain::self->SaveEmergencyFiles();
			return true;
		}

		return false;
	}
};

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// 컨트롤이 만들어지기 전에 Windows XP 시각 효과를 활성화합니다.
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);

	NekoNovel *nekoNovel = GetNekoNovel();
	MainEventListener mainListener;
	nekoNovel->AddEventListener(&mainListener);

	// 주 창을 만들어 실행합니다.
	//Application::Run();
	frmMain ^frm = gcnew frmMain();
	frm->Show();

	while (frm->Created)
	{
		Application::DoEvents();
		if(!nekoNovel->Update())
			break;
	}

	ReleaseNekoNovel();

	return 0;
}
