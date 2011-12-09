#include "nekoPrecompiledHeader.h"
#include "nekoGUIScreenSaveLoad.h"
#include "NekoNovel.h"
#include "nekoVideoDriver.h"
#include "nekoUtility.h"
#include "nekoHWResourceManager.h"

namespace neko
{
	nekoGUIScreenSaveLoad::nekoGUIScreenSaveLoad(E_SCREEN_SAVELOAD_TYPE mode)
		: mType(0)
		, mSelectedOne(-1)
		, mMode(mode)
		, mPage(1)
	{
		mNoSaveFile = GetNekoNovel()->GetHWResourceManager()->LoadTexture("이미지\\시스템_세이브파일없음.png");
	}

	nekoGUIScreenSaveLoad::~nekoGUIScreenSaveLoad()
	{

	}


	/// Update
	void nekoGUIScreenSaveLoad::Update(float32 frameTime)
	{
		
	}

	/// Draw
	void nekoGUIScreenSaveLoad::Draw()
	{	
		nekoVideoDriver *videoDrv = GetNekoNovel()->GetVideoDriver();

		videoDrv->EnableCamera(false);

		switch(mType)
		{
		case 0:
			{
			nekoPoint pt = nekoPoint(66, 50);
			nekoSaveData *sdata;
			for(int32 y = 0; y < 3;++y)
			{
				for(int32 x = 0;x < 4;++x)	
				{
					sdata = GetNekoNovel()->GetCurrentGame()->GetSaveData(mPage * 12 - 12 + y * 4 + x);
					if(sdata && sdata->GetPreview())
						videoDrv->DrawImage(sdata->GetPreview(), pt.x, pt.y, 150, 113, -1, 0, 0, -1, -1, 0, 1, 1, 0.465f);
					else
						videoDrv->DrawImage(mNoSaveFile, pt.x, pt.y, 150, 113, D3DCOLOR_ARGB((mSelectedOne == y * 4 + x) ? 125 : 255, 0, 0, 0), 0, 0, -1, -1, 0, 1, 1, 0.465f);

					uint32 color = (mSelectedOne == y * 4 + x) ? D3DCOLOR_XRGB(255, 255, 255) : D3DCOLOR_ARGB(200, 255, 255, 255);
					if(sdata)
					{
						videoDrv->DrawText(NULL, pt.x + 1, pt.y + 1, 150, 113, sdata->GetDateString(), D3DCOLOR_XRGB(0, 0, 0), DT_VCENTER|DT_CENTER|DT_WORDBREAK);
						videoDrv->DrawText(NULL, pt.x, pt.y, 150, 113, sdata->GetDateString(), color, DT_VCENTER|DT_CENTER|DT_WORDBREAK);
					}
					//else
					//	videoDrv->DrawText(NULL, pt.x, pt.y, 150, 113, "비어있는 슬롯", color, DT_VCENTER|DT_CENTER|DT_WORDBREAK);
					pt.x += 177;
				}
				
				pt.x = 66;
				pt.y += 136;
			}
			}
			break;
		}

		videoDrv->DrawText(NULL, 680, 550, 800, 600, nekoUtility::FormString("%d/%d", mPage, MaxPage), D3DCOLOR_XRGB(255, 255, 255));

		videoDrv->EnableCamera(true);
	}

	bool nekoGUIScreenSaveLoad::OnEvent(const nekoEvent &event)
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

				if(mMode == ESST_SAVE)
					GetNekoNovel()->GetCurrentGame()->SaveToSlot(mSelectedOne + mPage * 12 - 12);
				else
					GetNekoNovel()->GetCurrentGame()->LoadFromSlot(mSelectedOne + mPage * 12 - 12);

			}
		}
		return false;
	}

	result nekoGUIScreenSaveLoad::GoNextPage()
	{
		mPage++;
		mPage = min(mPage, MaxPage);
		return S_OK;
	}

	result nekoGUIScreenSaveLoad::GoPrevPage()
	{
		mPage--;
		mPage = max(mPage, 1);
		return S_OK;
	}
}