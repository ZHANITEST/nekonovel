#include "nekoPrecompiledHeader.h"
#include "nekoGUISelectionButtons.h"
#include "nekoNovel.h"
#include "nekoHWResourceManager.h"

namespace neko
{
	nekoGUISelectionButtons::nekoGUISelectionButtons()
		: mSelectedIndex(-1)
		, nekoIBlockInstance(NULL)
		, mButtonState(EBS_NORMAL)
	{
		mBoundingBox = nekoRectangle(0, 100, GetNekoNovel()->GetWidth(), 400);
	}

	nekoGUISelectionButtons::~nekoGUISelectionButtons()
	{
		ClearButtons();
	}

	int32 nekoGUISelectionButtons::_PickButton(int32 x, int32 y, nekoOneButton **out)
	{
		list<nekoOneButton *>::iterator iter = mButtons.begin();
		int32 i = 0;
		for(;iter != mButtons.end();++iter, ++i)
		{
			if((*iter)->rect.IsPointIn(x, y))
			{
				if(out)
					*out = (*iter);

				return i;
			}
		}

		if(out) *out = NULL;
		return -1;
	}

	void nekoGUISelectionButtons::Update(float32 frameTime)
	{
		if(mButtons.empty())
			return;

		// 사이즈를 재계산해주자!
		if(mFlags & EBIF_UPDATE_NEEDED)
		{
			int32 blockGap = 0;

			list<nekoOneButton *>::iterator iter = mButtons.begin();
			for(;iter != mButtons.end();++iter)
			{
				if((*iter)->mImage[0] && (*iter)->mImage[0]->GetState() == EHRS_LOADED)
				{
					blockGap += (*iter)->mImage[0]->GetSize().y;

				}
				else
				{
					return;
				}
			}

			blockGap = (mBoundingBox.GetHeight() - blockGap) / (mButtons.size() + 1);

			int32 y = blockGap;
			for(iter = mButtons.begin();iter != mButtons.end();++iter, y += blockGap)
			{
				(*iter)->rect = nekoRectangle(
					(mBoundingBox.GetWidth() - (*iter)->mImage[0]->GetSize().x) / 2,
					mBoundingBox.top + y);
				(*iter)->rect.SetSize((*iter)->mImage[0]->GetSize().x, (*iter)->mImage[0]->GetSize().y);

				y += (*iter)->mImage[0]->GetSize().y;
			}

			RemoveFlags(EBIF_UPDATE_NEEDED);
		}
	}

	void nekoGUISelectionButtons::Draw()
	{
		if(mButtons.empty())
			return;

		nekoVideoDriver *videoDrv = GetNekoNovel()->GetVideoDriver();
		ID3DXSprite *sprite = videoDrv->GetSprite();

		uint32 color = GetNekoNovel()->GetCurrentGame()->GetEnvironment().mSelectionColor;
		list<nekoOneButton *>::iterator iter = mButtons.begin();
		int32 a;
		int32 i = 0;
		D3DXMATRIX matIdentity;

		D3DXMatrixIdentity(&matIdentity);
		matIdentity *= videoDrv->GetBaseMatrix();

		for(;iter != mButtons.end();++iter, ++i)
		{
			if(mSelectedIndex == i)
				a = mButtonState;
			else
				a = 0;

			if(!(*iter)->mImage[a])
				continue;

			videoDrv->DrawImage((*iter)->mImage[a], 
				(*iter)->rect.left, (*iter)->rect.top,
				-1, -1, 0xffffffff, 0, 0, -1, -1, 0, 1, 1, 0.46f);

			sprite->SetTransform(&matIdentity);

			nekoPoint center = (*iter)->rect.GetCenter();
			RECT rect = {center.x - (*iter)->mTextSize.x / 2, center.y - (*iter)->mTextSize.y / 2, 0, 0};
			(*iter)->mFont->DrawText(sprite, (*iter)->mCaption.c_str(), -1, &rect, DT_NOCLIP, color);
		}
	}

	bool nekoGUISelectionButtons::OnEvent(const nekoEvent &event)
	{
		if(mButtons.empty())
			return false;

		// 윈도우 이벤트만 필요해!
		if(event.type != EVENT_WINDOW_MESSAGE)
			return false;

		// 위치나 크기 등 계산이 덜 되었다.
		if(mFlags & EBIF_UPDATE_NEEDED)
			return false;

		switch(event.uMsg)
		{
		case WM_MOUSEMOVE:
			{
				if(!mBoundingBox.IsPointIn(LOWORD(event.lParam), HIWORD(event.lParam)))
				{
					mSelectedIndex = -1;
					mButtonState = EBS_NORMAL;

					return false;
				}

				nekoOneButton *bt;
				int32 sel = _PickButton(LOWORD(event.lParam), HIWORD(event.lParam), &bt);
				if(sel == -1)
				{
					mSelectedIndex = -1;
					mButtonState = EBS_NORMAL;
					return false;
				}

				if(mSelectedIndex != sel)
				{
					if(bt->mSound[0])
						bt->mSound[0]->Play();

					mSelectedIndex = sel;
					mButtonState = EBS_ROLLOVER;
				}

				return true;
			}
		case WM_LBUTTONDOWN:
			{
				if(!mBoundingBox.IsPointIn(LOWORD(event.lParam), HIWORD(event.lParam)))
					return false;

				nekoOneButton *bt;
				mSelectedIndex = _PickButton(LOWORD(event.lParam), HIWORD(event.lParam), &bt);
				if(mSelectedIndex == -1)
				{
					mButtonState = EBS_NORMAL;
					return false;
				}

				mButtonState = EBS_PRESSED;
				if(bt->mSound[1])
					bt->mSound[1]->Play();

				return true;
			}
		case WM_LBUTTONUP:
			if(!mBoundingBox.IsPointIn(LOWORD(event.lParam), HIWORD(event.lParam)))
				return false;

			mSelectedIndex = _PickButton(LOWORD(event.lParam), HIWORD(event.lParam));
			if(mSelectedIndex == -1)
			{
				mButtonState = EBS_NORMAL;
				return false;
			}

			if(mSelectedIndex != -1)
			{
				mButtonState = EBS_NORMAL;

				// 실제 명령 처리
				ClearButtons();
				GetNekoNovel()->GetCurrentGame()->OnSelectionCompleted(mSelectedIndex);
			}

			return true;
		}

		return false;
	}

	void nekoGUISelectionButtons::Clear()
	{
		mBoundingBox = nekoRectangle(0, 100, GetNekoNovel()->GetWidth(), 400);
		ClearButtons();
	}

	void nekoGUISelectionButtons::AddButton(const string &caption, const string **imageNames, const string **soundNames)
	{
		nekoAssert(imageNames, "");
		nekoAssert(soundNames, "");

		nekoOneButton *bt = nekoNew nekoOneButton;
		bt->mCaption = caption;

		for(int32 i = 0;i < EBS_COUNT;++i)
		{
			if(*imageNames[i] != "없음")
			{
				bt->mImage[i] = GetNekoNovel()->GetHWResourceManager()->LoadTexture((string("이미지\\") + *imageNames[i]).c_str());
			}
		}

		if(*soundNames[0] != "없음")
		{
			if(bt->mSound[0] = GetNekoNovel()->GetHWResourceManager()->LoadSound((string("효과음\\") + *soundNames[0]).c_str()))
				bt->mSound[0]->Stop();
		}
		if(*soundNames[1] != "없음")
		{
			if(bt->mSound[1] = GetNekoNovel()->GetHWResourceManager()->LoadSound((string("효과음\\") + *soundNames[1]).c_str()))
				bt->mSound[1]->Stop();
		}

		bt->mFont = GetNekoNovel()->GetCurrentGame()->GetChatBox()->GetLastFont()->mFont;
		if(bt->mFont)
			bt->mFont->AddRef();


		nekoRectangle resultRect;
		GetNekoNovel()->GetVideoDriver()->CalculateTextZ(bt->mFont, caption.c_str(), resultRect);
		bt->mTextSize = nekoPoint(resultRect.GetWidth(), resultRect.GetHeight());

		mButtons.push_back(bt);
		AddFlags(EBIF_UPDATE_NEEDED);
	}

	void nekoGUISelectionButtons::ClearButtons()
	{
		list<nekoOneButton *>::iterator iter = mButtons.begin();
		for(;iter != mButtons.end();++iter)
			delete (*iter);
		mButtons.clear();
	}

	/// 데이터 덤프하기
	result nekoGUISelectionButtons::Dump(int32 version, nekoByteBuffer &outBuf)
	{
		nekoIBlockInstance::Dump(version, outBuf);

		// 이 상태에서 저장 지원 안함

		outBuf << (int32)mButtons.size();

		list<nekoOneButton *>::iterator iter = mButtons.begin();
		for(;iter != mButtons.end();++iter)
		{
			outBuf << (*iter)->rect
				<< (*iter)->mCaption;

			for(int32 i = 0;i < EBS_COUNT;++i)
			{
				outBuf << (((*iter)->mImage[i]) ? (*iter)->mImage[i]->GetName() : "");
			}

			outBuf << (((*iter)->mSound[0]) ? (*iter)->mSound[0]->GetName() : "");
			outBuf << (((*iter)->mSound[1]) ? (*iter)->mSound[1]->GetName() : "");
		}

		return S_OK;
	}

	result nekoGUISelectionButtons::Load(int32 version, nekoByteBuffer &inBuf)
	{
		nekoIBlockInstance::Load(version, inBuf);

		int32 count;
		string path;

		inBuf >> count;

		while(count--)
		{
			nekoOneButton *bt = nekoNew nekoOneButton;

			inBuf >> bt->rect
				>> bt->mCaption;

			for(int32 i = 0;i < EBS_COUNT;++i)
			{
				inBuf >> path;

				if(!path.empty())
					bt->mImage[i] = GetNekoNovel()->GetHWResourceManager()->LoadTexture(path.c_str());
			}

			inBuf >> path;
			if(!path.empty())
				bt->mSound[0] = GetNekoNovel()->GetHWResourceManager()->LoadSound(path.c_str());

			inBuf >> path;
			if(!path.empty())
				bt->mSound[1] = GetNekoNovel()->GetHWResourceManager()->LoadSound(path.c_str());

			nekoRectangle resultRect;
			GetNekoNovel()->GetVideoDriver()->CalculateTextZ(bt->mFont, bt->mCaption.c_str(), resultRect);
			bt->mTextSize = nekoPoint(resultRect.GetWidth(), resultRect.GetHeight());

			mButtons.push_back(bt);
		}

		AddFlags(EBIF_UPDATE_NEEDED);

		return S_OK;
	}
}