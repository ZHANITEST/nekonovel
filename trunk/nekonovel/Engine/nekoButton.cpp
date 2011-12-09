#include "nekoPrecompiledHeader.h"
#include "nekoButton.h"
#include "nekoNovelReal.h"

namespace neko
{
	nekoButtonInstance::nekoButtonInstance(nekoIBlock *block)
		: nekoIBlockInstance(block)
		, mBtState(EBS_NORMAL)
	{
		mZOrder = -120;
	}

	nekoButtonInstance::~nekoButtonInstance()
	{

	}

	void nekoButtonInstance::SetButtonType(E_BUTTON_INSTANCE_TYPE btType)
	{
		mBtType = btType;
	}

	void nekoButtonInstance::SetJump(const string &fileName, const string &bookMarkName)
	{
		mBtType = EBIT_JUMP;
		mArguments1 = (fileName == "여기") ? "" : fileName;
		mArguments2 = (bookMarkName == "처음") ? "" : bookMarkName;
	}

	void nekoButtonInstance::SetLink(const string &address)
	{
		mBtType = EBIT_LINK;
		mArguments1 = address;
	}

	void nekoButtonInstance::SetMusic(const string &fileName)
	{
		mBtType = EBIT_BGM;
		mArguments1 = fileName;
	}

	void nekoButtonInstance::SetFXSound(const string &fileName)
	{
		mBtType = EBIT_FX;
		mArguments1 = fileName;
	}

	void nekoButtonInstance::SetScriptType(const string &fileName)
	{
		mBtType = EBIT_SCRIPT;
		mArguments1 = fileName;
	}

	void nekoButtonInstance::SetNormalImage(const string &fileName)
	{
		if(fileName == "없음")
			return;

		mTextures[EBS_NORMAL] = GetNekoNovel()->GetHWResourceManager()->LoadTexture((string("이미지\\") + fileName).c_str());
	}

	void nekoButtonInstance::SetRolloverImage(const string &fileName)
	{
		if(fileName == "없음")
			return;

		mTextures[EBS_ROLLOVER] = GetNekoNovel()->GetHWResourceManager()->LoadTexture((string("이미지\\") + fileName).c_str());
	}

	void nekoButtonInstance::SetPressedImage(const string &fileName)
	{
		if(fileName == "없음")
			return;

		mTextures[EBS_PRESSED] = GetNekoNovel()->GetHWResourceManager()->LoadTexture((string("이미지\\") + fileName).c_str());
	}

	void nekoButtonInstance::SetRolloverSound(const string &fileName)
	{
		if(fileName == "없음")
			return;

		mRolloverSound = GetNekoNovel()->GetHWResourceManager()->LoadSound((string("효과음\\") + fileName).c_str());
		mRolloverSound->Stop();
	}

	void nekoButtonInstance::SetPressedSound(const string &fileName)
	{
		if(fileName == "없음")
			return;

		mPressedSound = GetNekoNovel()->GetHWResourceManager()->LoadSound((string("효과음\\") + fileName).c_str());
		mPressedSound->Stop();
	}

	void nekoButtonInstance::Run()
	{
		nekoGame *currentGame = GetNekoNovel()->GetCurrentGame();

		if(currentGame->GetGameMode() != GAME_RUN_PLAYING)
			return;

		switch(mBtType)
		{
		case EBIT_BGM:
			{
				nekoBGMusicInstance *music = (nekoBGMusicInstance *)currentGame->CreateInstance(BLOCK_BGMUSIC, "");
				music->SetMusic(mArguments1.c_str(), false);
			}
			break;
		case EBIT_FX:
			{
				if(mTempSound == NULL)
				{
					mTempSound = GetNekoNovel()->GetHWResourceManager()->LoadSound((string("효과음\\") + mArguments1).c_str());
				}
				else
				{
					mTempSound->Play();
				}
			}
			break;
		case EBIT_JUMP:
			{
				currentGame->JumpTo(mArguments1.c_str(), mArguments2.c_str());
			}
			break;
		case EBIT_LINK:
			{
				ShellExecute(NULL, "open", mArguments1.c_str(), "", "", SW_NORMAL);
			}
			break;
		case EBIT_END:
			{
				if(GetNekoNovel()->IsOwnerOfHWND())
					SendMessage(GetNekoNovel()->GetHWND(), WM_CLOSE, 0, 0);
			};
			break;
		case EBIT_SCRIPT:
			currentGame->PlayCommands(mArguments1.c_str());
			break;
		case EBIT_MSGBOX_OK:
			currentGame->OnButtonPressed(true);
			currentGame->OnMsgBoxCompleted(true);
			return;
		case EBIT_MSGBOX_CANCEL:
			currentGame->OnButtonPressed(true);
			currentGame->OnMsgBoxCompleted(false);
			return;
		case EBIT_PHOTOMODE:
			currentGame->GetEnvironment().mPhotoMode = !currentGame->GetEnvironment().mPhotoMode;
			break;
		};

		currentGame->OnButtonPressed();
	}

	void nekoButtonInstance::Update(float32 frameTime)
	{
		if(mBtState >= EBS_ROLLOVER)
		{
			if(GetNekoNovel()->GetCurrentGame()->GetPickedInstance() != this)
				mBtState = EBS_NORMAL;
		}

		if(!(GetFlags() & EBIF_SIZE_LOADED) && mTextures[EBS_NORMAL] && mTextures[EBS_NORMAL]->GetState() == EHRS_LOADED)
		{
			if(!mBoundingBox.GetWidth())
				mBoundingBox.SetWidth(mTextures[EBS_NORMAL]->GetSize().x);

			if(!mBoundingBox.GetHeight())
				mBoundingBox.SetHeight(mTextures[EBS_NORMAL]->GetSize().y);

			AddFlags(EBIF_SIZE_LOADED);
		}

		// Autosize Property.
		if((GetFlags() & EBIF_AUTO_SIZE_X) && mTextures[mBtState] && mTextures[mBtState]->GetState() == EHRS_LOADED)
		{
			mBoundingBox.SetWidth(mTextures[mBtState]->GetSize().x);
		}

		if((GetFlags() & EBIF_AUTO_SIZE_Y) && mTextures[mBtState] && mTextures[mBtState]->GetState() == EHRS_LOADED)
		{
			mBoundingBox.SetHeight(mTextures[mBtState]->GetSize().y);
		}
	}

	void nekoButtonInstance::Draw()
	{
		if(mTextures[mBtState])
		{
			if(mFlags & EBIF_IGNORE_CAMERA)
				GetNekoNovel()->GetVideoDriver()->EnableCamera(false);

			GetNekoNovel()->GetVideoDriver()->DrawImage(mTextures[mBtState], mBoundingBox.left, mBoundingBox.top, mBoundingBox.GetWidth(), mBoundingBox.GetHeight(), 
			D3DCOLOR_ARGB(mAlpha, 255, 255, 255), 0, 0, -1, -1, mRotation, mScale.x, mScale.y, _GetRealZ());

			if(mFlags & EBIF_IGNORE_CAMERA)
				GetNekoNovel()->GetVideoDriver()->EnableCamera(true);
		}
	}

	// 인스턴스로 오는 메세지는 선별된 것임.
	bool nekoButtonInstance::OnEvent(const nekoEvent &event)
	{
		if(event.type == EVENT_WINDOW_MESSAGE)
		{
			if(event.uMsg == WM_MOUSEMOVE)
			{
				if(mBtState == EBS_NORMAL)
				{
					if(mRolloverSound)
					{
						mRolloverSound->SetVolume(
							GetNekoNovel()->GetCurrentGame()->GetEnvironment().mDefFXVolume / 100.0f
						);
						mRolloverSound->Play();
					}
					mBtState = EBS_ROLLOVER;
				}
				return true;
			} else if(event.uMsg == WM_LBUTTONDOWN)
			{
				if(mPressedSound)
				{
					mPressedSound->SetVolume(
						GetNekoNovel()->GetCurrentGame()->GetEnvironment().mDefFXVolume / 100.0f
					);
					mPressedSound->Play();
				}
				mBtState = EBS_PRESSED;
				return true;
			} else if(event.uMsg == WM_LBUTTONUP)
			{
				mBtState = EBS_NORMAL;
				Run();
				return true;
			}
		}

		return false;
	}

	/// 데이터 덤프하기
	result nekoButtonInstance::Dump(int32 version, nekoByteBuffer &outBuf)
	{
		nekoIBlockInstance::Dump(version, outBuf);

		outBuf << mBtState << mBtType
			<< mArguments1 << mArguments1
			<< ((mRolloverSound) ? mRolloverSound->GetName() : "")
			<< ((mTextures[0]) ? mTextures[0]->GetName() : "")
			<< ((mTextures[1]) ? mTextures[1]->GetName() : "")
			<< ((mTextures[2]) ? mTextures[2]->GetName() : "");

		return S_OK;
	}

	result nekoButtonInstance::Load(int32 version, nekoByteBuffer &inBuf)
	{
		nekoIBlockInstance::Load(version, inBuf);

		string path;
		inBuf >> mBtState >> mBtType
			>> mArguments1 >> mArguments2
			>> path;

		if(!path.empty())
		{
			mRolloverSound = GetNekoNovel()->GetHWResourceManager()->LoadSound(path.c_str());
			mRolloverSound->Stop();
		}

		inBuf >> path;
		if(!path.empty())
			mTextures[0] = GetNekoNovel()->GetHWResourceManager()->LoadTexture(path.c_str());

		inBuf >> path;
		if(!path.empty())
			mTextures[1] = GetNekoNovel()->GetHWResourceManager()->LoadTexture(path.c_str());

		inBuf >> path;
		if(!path.empty())
			mTextures[2] = GetNekoNovel()->GetHWResourceManager()->LoadTexture(path.c_str());

		return S_OK;
	}

	/// Constructor
	nekoButton::nekoButton()
		: nekoIBlock(BLOCK_BUTTON)
	{

	}

	/// Destructor
	nekoButton::~nekoButton()
	{

	}

	/// 인스턴스를 만든다
	nekoIBlockInstance *nekoButton::CreateInstance()
	{
		return nekoNew nekoButtonInstance(this);
	}
}