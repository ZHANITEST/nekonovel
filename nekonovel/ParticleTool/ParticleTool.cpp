// ParticleTool.cpp : 기본 프로젝트 파일입니다.

#include "stdafx.h"
#include "MainForm.h"

using namespace ParticleTool;

class MainEventListener : public nekoIEventListener
{
public:


	/// 이벤트 도착시
	bool OnEvent(const nekoEvent &event)
	{
		if(event.type == EVENT_LOG)
		{
			// 경고에만!
			if(event.value[1] == (int32)LOG_LEVEL_WARN)
			{
				MessageBox::Show(MainForm::self, ConvertToString(event.GetText(0)), "네코노벨 알림!", MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
				return true;
			}
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
	//MainEventListener mainListener;
	//nekoNovel->AddEventListener(&mainListener);

	// 주 창을 만들어 실행합니다.
	//Application::Run();
	MainForm ^frm = gcnew MainForm();
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
