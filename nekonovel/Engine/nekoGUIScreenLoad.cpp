#include "nekoPrecompiledHeader.h"
#include "nekoGUIScreenLoad.h"
#include "NekoNovel.h"
#include "nekoVideoDriver.h"

namespace neko
{
	nekoGUIScreenLoad::nekoGUIScreenLoad()
		: mType(0)
		, mSelectedOne(-1)
	{

	}

	nekoGUIScreenLoad::~nekoGUIScreenLoad()
	{

	}


	/// Update
	void nekoGUIScreenLoad::Update(float32 frameTime)
	{

	}

	/// Draw
	void nekoGUIScreenLoad::Draw()
	{	
		nekoVideoDriver *videoDrv = GetNekoNovel()->GetVideoDriver();

		switch(mType)
		{
		case 0:
			{
				nekoPoint pt = nekoPoint(66, 50);
				nekoLoadData *sdata;
				for(int32 y = 0; y < 3;++y)
				{
					for(int32 x = 0;x < 4;++x)	
					{
						sdata = GetNekoNovel()->GetCurrentGame()->GetLoadData(y * 4 + x);
						if(sdata && sdata->GetPreview())
							videoDrv->DrawImage(sdata->GetPreview(), pt.x, pt.y, 150, 113, -1, 0, 0, -1, -1, 0, 1, 1, 0.5f);
						else
							videoDrv->DrawRectangle(pt.x, pt.y, 150, 113, D3DCOLOR_ARGB((mSelectedOne == y * 4 + x) ? 125 : 255, 0, 0, 0));

						uint32 color = (mSelectedOne == y * 4 + x) ? D3DCOLOR_XRGB(255, 255, 255) : D3DCOLOR_ARGB(100, 255, 255, 255);
						if(sdata)
						{
							videoDrv->DrawText(NULL, pt.x + 1, pt.y + 1, 150, 113, sdata->GetDateString(), D3DCOLOR_XRGB(0, 0, 0), DT_VCENTER|DT_CENTER|DT_WORDBREAK);
							videoDrv->DrawText(NULL, pt.x, pt.y, 150, 113, sdata->GetDateString(), color, DT_VCENTER|DT_CENTER|DT_WORDBREAK);
						}
						else
							videoDrv->DrawText(NULL, pt.x, pt.y, 150, 113, "비어있는 슬롯", color, DT_VCENTER|DT_CENTER|DT_WORDBREAK);
						pt.x += 177;
					}

					pt.x = 66;
					pt.y += 136;
				}
			}
			break;
		}
	}

	bool nekoGUIScreenLoad::OnEvent(const nekoEvent &event)
	{
		if(event.type == EVENT_WINDOW_MESSAGE)
		{
			if(event.uMsg == WM_MOUSEMOVE)
			{
				mSelectedOne = -1;

				switch(mType)
				{
				case 0:
					{
						nekoPoint pt = nekoPoint(66, 50);
						for(int32 y = 0; y < 3;++y)
						{
							for(int32 x = 0;x < 4;++x)	
							{
								nekoRectangle rect(pt.x, pt.y, pt.x + 150, pt.y + 113);
								if(rect.IsPointIn(LOWORD(event.lParam), HIWORD(event.lParam)))
								{
									mSelectedOne = y * 4 + x;

									return true;
								}
								pt.x += 177;
							}
							pt.x = 66;
							pt.y += 136;
						}
					}
					break;
				}
			} 
			else if(event.uMsg == WM_LBUTTONDOWN)
			{
				if(mSelectedOne == -1)
					return false;

				GetNekoNovel()->GetCurrentGame()->LoadToSlot(mSelectedOne);
			}
		}
		return false;
	}
}