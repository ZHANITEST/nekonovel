#include "nekoPrecompiledHeader.h"
#include "nekoGUIMessageBox.h"
#include "NekoNovel.h"
#include "nekoHWResourceManager.h"

namespace neko
{

	nekoGUIMessageBox::nekoGUIMessageBox()
		: mButtonState(EBS_NORMAL)
		, mPickedOne(-1)
	{
		mBackground = GetNekoNovel()->GetHWResourceManager()->LoadTexture("이미지\\시스템_메세지박스.png");
		mButtonImage = GetNekoNovel()->GetHWResourceManager()->LoadTexture("이미지\\시스템_메세지박스_버튼.png");
	}

	nekoGUIMessageBox::~nekoGUIMessageBox()
	{

	}

	/// Update
	void nekoGUIMessageBox::Update(float32 frameTime)
	{

	}

	/// Draw
	void nekoGUIMessageBox::Draw()
	{
		nekoVideoDriver *videoDrv = GetNekoNovel()->GetVideoDriver();
		videoDrv->DrawImage(mBackground
			, (GetNekoNovel()->GetWidth() - mBackground->GetSize().x) / 2
			, (GetNekoNovel()->GetHeight() - mBackground->GetSize().y) / 2
			, mBackground->GetSize().x, mBackground->GetSize().y, -1, 0, 0, -1, -1, 0, 1, 1, 0.5f);

		videoDrv->DrawImage(mBackground
			, (GetNekoNovel()->GetWidth() - mBackground->GetSize().x) / 2
			, (GetNekoNovel()->GetHeight() - mBackground->GetSize().y) / 2
			, mBackground->GetSize().x, mBackground->GetSize().y, -1, 0, 0, -1, -1, 0, 1, 1, 0.5f);

		videoDrv->DrawText(0, 0, 0, GetNekoNovel()->GetWidth(), GetNekoNovel()->GetHeight(), mMessage.c_str(), -1, DT_CENTER|DT_VCENTER);
	}

	bool nekoGUIMessageBox::OnEvent(const nekoEvent &event)
	{
		return false;
	}
}