#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace NekoMimi
{
	public ref class ExtCanvas : public System::Windows::Forms::PictureBox
	{
	public:

		ExtCanvas()
		{

		}

		protected: virtual void WndProc(Message %m) override
		{
		   if(m.Msg == WM_LBUTTONDOWN || m.Msg == WM_RBUTTONDOWN)
		   {
				this->Focus();	   
		   }

		   nekoEvent e(EVENT_WINDOW_MESSAGE);
		   e.uMsg = m.Msg;
		   e.wParam = m.WParam.ToInt32();
		   e.lParam = m.LParam.ToInt32();
		   GetNekoNovel()->OnEvent(e);

		   switch(m.Msg)
		   {
		   case WM_CHAR:
		   case WM_IME_STARTCOMPOSITION:
		   case WM_IME_ENDCOMPOSITION:
		   case WM_IME_COMPOSITION:
		   case WM_IME_NOTIFY:
		   case WM_IME_CHAR:
		   case WM_IME_SETCONTEXT:
			   return;
		   }

		   PictureBox::WndProc(m);
		}

	protected:
	};
}