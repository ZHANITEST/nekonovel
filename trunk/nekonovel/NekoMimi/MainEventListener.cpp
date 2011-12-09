#include "StdAfx.h"
#include "MainEventListener.h"

namespace NekoMimi
{
	/// 이벤트 도착시
	bool MainEventListener::OnEvent(const nekoEvent &event)
	{
		if(event.type == EVENT_LOG)
		{
			frmMain::OnLogReceived(event);
			return true;	
		}

		return false;
	}
}