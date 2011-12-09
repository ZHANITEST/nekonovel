#include "nekoPrecompiledHeader.h"
#include "nekoSystemButton.h"
#include "nekoNovelReal.h"

namespace neko
{
	nekoSystemButtonInstance::nekoSystemButtonInstance(nekoIBlock *block)
		: nekoIBlockInstance(block)
		, mBtState(ESBS_NORMAL)
		, mCheckButton(false)
	{
		mZOrder = -120;
	}

	nekoSystemButtonInstance::~nekoSystemButtonInstance()
	{

	}

	void nekoSystemButtonInstance::SetJump(const string &fileName, const string &bookMarkName)
	{
		mBtType = ESBIT_JUMP;
		mArguments1 = fileName;
		mArguments2 = (bookMarkName == "처음") ? "" : bookMarkName;
	}

	void nekoSystemButtonInstance::SetQuickSkipMode(const string &enable)
	{
		mBtType = ESBIT_QUICKSKIP;
		mArguments1 = enable;
	}

	void nekoSystemButtonInstance::SetAutoTurnMode(const string &enable)
	{
		mBtType = ESBIT_AUTOTURN;
		mArguments1 = enable;
	}

	void nekoSystemButtonInstance::SetScriptMode(const string &scriptName)
	{
		mBtType = ESBIT_SCRIPT;
		mArguments1 = scriptName;
	}

	void nekoSystemButtonInstance::SetWindowMode(const string &value)
	{
		mBtType = ESBIT_WINDOWMODE;
		mCheckButton = true;

		mValue = (value == "윈도우");
	}

	void nekoSystemButtonInstance::SetPrintDelay(const string &value)
	{
		mBtType = ESBIT_PRINT_DELAY;
		mCheckButton = true;

		mValue = (value == "응");
	}

	void nekoSystemButtonInstance::SetLoadSaveSlotMode(E_SYSTEM_BUTTON_INSTANCE_TYPE btType, int32 slotNo)
	{
		nekoAssert((btType >= ESBIT_LOAD_SLOT && btType <= ESBIT_SAVE_SLOT) || (btType == ESBIT_DELETE_SLOT)
			, "SetLoadSaveSlotMode 함수에 잘못된 버튼 타입이 정해졌습니다.");
		
		mBtType = btType;
		mArguments3 = slotNo;
	}

	void nekoSystemButtonInstance::SetNormalImage(const string &fileName)
	{
		if(fileName == "없음")
			return;

		mTextures[ESBS_NORMAL] = GetNekoNovel()->GetHWResourceManager()->LoadTexture((string("이미지\\") + fileName).c_str());
	}

	void nekoSystemButtonInstance::SetRolloverImage(const string &fileName)
	{
		if(fileName == "없음")
			return;

		mTextures[ESBS_ROLLOVER] = GetNekoNovel()->GetHWResourceManager()->LoadTexture((string("이미지\\") + fileName).c_str());
	}

	void nekoSystemButtonInstance::SetPressedImage(const string &fileName)
	{
		if(fileName == "없음")
			return;

		mTextures[ESBS_PRESSED] = GetNekoNovel()->GetHWResourceManager()->LoadTexture((string("이미지\\") + fileName).c_str());
	}

	void nekoSystemButtonInstance::SetRolloverSound(const string &fileName)
	{
		if(fileName == "없음")
			return;

		mRolloverSound = GetNekoNovel()->GetHWResourceManager()->LoadSound((string("효과음\\") + fileName).c_str());
		mRolloverSound->Stop();
	}

	void nekoSystemButtonInstance::SetPressedSound(const string &fileName)
	{
		if(fileName == "없음")
			return;

		mPressedSound = GetNekoNovel()->GetHWResourceManager()->LoadSound((string("효과음\\") + fileName).c_str());
		mPressedSound->Stop();
	}

	void nekoSystemButtonInstance::Run()
	{
		nekoGame *currentGame = GetNekoNovel()->GetCurrentGame();

		if(currentGame->GetGameMode() != GAME_RUN_PLAYING)
			/*
			// 대기중에만 누를 수 있다.
			|| !(currentGame->GetSystemState() >= EGSS_WAIT && currentGame->GetSystemState() <= EGSS_BUTTON_PRESS_WAIT))
			*/
			return;

		if(currentGame->IsSelecting())
			return;

		// 배경 이펙트가 진행중이라면.. 안돼.
		if(GetNekoNovel()->GetCurrentGame()->HasBGEffect())
			return;

		switch(mBtType)
		{
		case ESBIT_JUMP:
			{
				currentGame->JumpTo(mArguments1.c_str(), mArguments2.c_str());
			}
			break;
		case ESBIT_SCRIPT:
			{
				currentGame->PlayCommands(mArguments1.c_str());
			}
			break;
		case ESBIT_QUICKSKIP:
			{
				currentGame->SetGameFlowState((mArguments1 == "응") ? EGFS_QUICKSKIPPING : EGFS_NORMAL);
			}
			break;
		case ESBIT_AUTOTURN:
			{
				currentGame->SetGameFlowState((mArguments1 == "응") ? EGFS_AUTOTURNING : EGFS_NORMAL);
			}
			break;
		case ESBIT_WINDOWMODE:
			{
				GetNekoNovel()->GetVideoDriver()->OnWindowChanged(-1, -1, !mValue);
			}
			break;
		case ESBIT_PRINT_DELAY:
			{
				currentGame->GetEnvironment().mPrintDelay = mValue;
			}
			break;
		case ESBIT_NEXT_PAGE:
			{
				currentGame->GoNextSlotPage();
			}
			break;
		case ESBIT_PREV_PAGE:
			{
				currentGame->GoPrevSlotPage();
			}
			break;
		case ESBIT_GO_SLOTPAGE:
			{
				currentGame->GoSlotPage(mArguments3);
			}
			break;
		case ESBIT_EXIT:
			{
				if(GetNekoNovel()->IsOwnerOfHWND())
					SendMessage(GetNekoNovel()->GetHWND(), WM_CLOSE, 0, 0);
			}
			break;
		case ESBIT_SAVE_SLOT:
			{
				int32 startIdx = (GetNekoNovel()->GetCurrentGame()->GetEnvironment().mSlotPage - 1) * 20;
				nekoSaveData *saveData = currentGame->GetSaveData(mArguments3 + startIdx);
				if(saveData
					&& (currentGame->ShowMsgBox(EMT_OVERSAVE
					, nekoUtility::FormString("%s\n%s\n게임에 정말 덮어쓰시겠습니까 ?", saveData->GetCaption(), saveData->GetLiteDateString())
					, mArguments3 + startIdx)))
				{
				}
				else
					currentGame->SaveToSlot(mArguments3 + startIdx);
			}
			break;
		case ESBIT_LOAD_SLOT:
			{
				int32 startIdx = (GetNekoNovel()->GetCurrentGame()->GetEnvironment().mSlotPage - 1) * 20;
				nekoSaveData *saveData = currentGame->GetSaveData(mArguments3 + startIdx);
				if(saveData
					&& (currentGame->ShowMsgBox(EMT_LOAD
					, nekoUtility::FormString("%s\n%s\n게임을 로드하시겠습니까 ?", saveData->GetCaption(), saveData->GetLiteDateString())
					, mArguments3 + startIdx)))
				{
					
				}
				else
					currentGame->LoadFromSlot(mArguments3 + startIdx);
			}
			break;
		case ESBIT_DELETE_SLOT:
			{
				int32 startIdx = (currentGame->GetEnvironment().mSlotPage - 1) * 20;
				nekoSaveData *saveData = currentGame->GetSaveData(mArguments3 + startIdx);
				if(saveData
					&& (currentGame->ShowMsgBox(EMT_REMOVE
					, nekoUtility::FormString("%s\n%s\n게임을 정말 삭제하시겠습니까 ?", saveData->GetCaption(), saveData->GetLiteDateString())
					, mArguments3 + startIdx)))
				{

				}
				else
					currentGame->RemoveSlot(mArguments3 + startIdx);
			}
			break;
		case ESBIT_ALWAYS_TOP:
			GetNekoNovel()->SetAlwaysTop(mArguments3 > 0);
			break;
		case ESBIT_SET_VARIABLE:
			nekoVariable *var = currentGame->AddVariable(mArguments1.c_str());
			if(var)
				var->SetValue(mArguments2.c_str());
			break;
		};
	}

	void nekoSystemButtonInstance::Update(float32 frameTime)
	{
		if(mBtState >= ESBS_ROLLOVER)
		{
			if(GetNekoNovel()->GetCurrentGame()->GetPickedInstance() != this)
				mBtState = ESBS_NORMAL;
		}


		// 체크 버튼 처리..
		if(mCheckButton && mBtState != ESBS_ROLLOVER)
		{
			switch(mBtType)
			{
			case ESBIT_WINDOWMODE:

				if(!GetNekoNovel()->GetVideoDriver()->IsFullScreen() == mValue)
					mBtState = ESBS_PRESSED;
				else if(mBtState == ESBS_PRESSED)
					mBtState = ESBS_NORMAL;
				break;
			case ESBIT_PRINT_DELAY:
				if(GetNekoNovel()->GetCurrentGame()->GetEnvironment().mPrintDelay == mValue)
					mBtState = ESBS_PRESSED;
				else
					mBtState = ESBS_NORMAL;
				break;
			case ESBIT_ALWAYS_TOP:

				mBtState = ESBS_NORMAL;
				if(GetNekoNovel()->IsAlwaysTop())
				{
					if(mArguments3 > 0)
						mBtState = ESBS_PRESSED;
						
				}
				else
				{
					if(!mArguments3)
						mBtState = ESBS_PRESSED;
				}
				break;
			case ESBIT_SET_VARIABLE:
				if(nekoVariable::If(EOT_EQUAL, mArguments1, mArguments2))
					mBtState = ESBS_PRESSED;
				else
					mBtState = ESBS_NORMAL;
				break;
			}
		}

		if(!(GetFlags() & EBIF_SIZE_LOADED) && mTextures[ESBS_NORMAL] && mTextures[EBS_NORMAL]->GetState() == EHRS_LOADED)
		{
			if(!mBoundingBox.GetWidth())
				mBoundingBox.SetWidth(mTextures[ESBS_NORMAL]->GetSize().x);

			if(!mBoundingBox.GetHeight())
				mBoundingBox.SetHeight(mTextures[ESBS_NORMAL]->GetSize().y);

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

	void nekoSystemButtonInstance::Draw()
	{
		nekoVideoDriver *videoDrv = GetNekoNovel()->GetVideoDriver();

		if(mFlags & EBIF_IGNORE_CAMERA)
			videoDrv->EnableCamera(false);

		switch(mBtType)
		{
		case ESBIT_DELETE_SLOT:
			{
				int32 startIdx = (GetNekoNovel()->GetCurrentGame()->GetEnvironment().mSlotPage - 1) * 20;
				nekoSaveData *saveData = GetNekoNovel()->GetCurrentGame()->GetSaveData(mArguments3 + startIdx);

				if(saveData || GetNekoNovel()->GetCurrentGame()->GetGameMode() == GAME_RUN_PREVIEW)
				{
					if(mTextures[mBtState])
						videoDrv->DrawImage(mTextures[mBtState], mBoundingBox.left, mBoundingBox.top, mBoundingBox.GetWidth(), mBoundingBox.GetHeight(), 
						D3DCOLOR_ARGB(mAlpha, 255, 255, 255), 0, 0, -1, -1, mRotation, mScale.x, mScale.y, _GetRealZ());
				}
			}
			break;
		case ESBIT_LOAD_SLOT:
		case ESBIT_SAVE_SLOT:
			{
				int32 startIdx = (GetNekoNovel()->GetCurrentGame()->GetEnvironment().mSlotPage - 1) * 20;
				nekoSaveData *saveData = GetNekoNovel()->GetCurrentGame()->GetSaveData(mArguments3 + startIdx);
				if(saveData && saveData->GetPreview())
				{
					uint32 color = (mBtState < ESBS_ROLLOVER) ? D3DCOLOR_XRGB(255, 255, 255) : D3DCOLOR_ARGB(150, 255, 255, 255);

					/*if(mBtState >= ESBS_ROLLOVER)
					{
						videoDrv->DrawText(NULL, mBoundingBox.left, mBoundingBox.top, mBoundingBox.GetWidth(), mBoundingBox.GetHeight(),
							saveData->GetDateString(), D3DCOLOR_XRGB(255, 255, 255), DT_VCENTER|DT_CENTER|DT_WORDBREAK);
						videoDrv->DrawText(NULL, mBoundingBox.left, mBoundingBox.top, mBoundingBox.GetWidth(), mBoundingBox.GetHeight(),
							saveData->GetDateString(), color, DT_VCENTER|DT_CENTER|DT_WORDBREAK);
					}*/

					videoDrv->DrawImage(saveData->GetPreview(), mBoundingBox.left, mBoundingBox.top, mBoundingBox.GetWidth(), mBoundingBox.GetHeight(), 
						MixColorsAlpha(color, mAlpha), 0, 0, -1, -1, mRotation, mScale.x, mScale.y, _GetRealZ());
					break;
				}
			}
		default:
			{
				// 현재 페이지라면.. 선택된 것처럼 표시한다.
				if(mBtType == ESBIT_GO_SLOTPAGE
					&& mBtState == ESBS_NORMAL
					&& mArguments3 == GetNekoNovel()->GetCurrentGame()->GetEnvironment().mSlotPage)
				{
					if(mTextures[ESBS_ROLLOVER])
						videoDrv->DrawImage(mTextures[ESBS_ROLLOVER], mBoundingBox.left, mBoundingBox.top, mBoundingBox.GetWidth(), mBoundingBox.GetHeight(), 
						D3DCOLOR_ARGB(mAlpha, 255, 255, 255), 0, 0, -1, -1, mRotation, mScale.x, mScale.y, _GetRealZ());
				}
				else
				{
					if(mTextures[mBtState])
						videoDrv->DrawImage(mTextures[mBtState], mBoundingBox.left, mBoundingBox.top, mBoundingBox.GetWidth(), mBoundingBox.GetHeight(), 
						D3DCOLOR_ARGB(mAlpha, 255, 255, 255), 0, 0, -1, -1, mRotation, mScale.x, mScale.y, _GetRealZ());
				}
			}
		}

		if(mFlags & EBIF_IGNORE_CAMERA)
			videoDrv->EnableCamera(true);
	}

	// 인스턴스로 오는 메세지는 선별된 것임.
	bool nekoSystemButtonInstance::OnEvent(const nekoEvent &event)
	{
		if(event.type == EVENT_WINDOW_MESSAGE)
		{
			if(event.uMsg == WM_MOUSEMOVE)
			{
				if(mBtState == ESBS_NORMAL)
				{
					if(mRolloverSound)
					{
						mRolloverSound->SetVolume(
						GetNekoNovel()->GetCurrentGame()->GetEnvironment().mDefFXVolume / 100.0f
							);
						mRolloverSound->Play();
					}
					mBtState = ESBS_ROLLOVER;
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
				mBtState = ESBS_PRESSED;
				return true;
			} else if(event.uMsg == WM_LBUTTONUP)
			{
				mBtState = ESBS_NORMAL;
				Run();

				if(mBtType == ESBIT_NOTHING)
					return false;

				return true;
			}
		}

		return false;
	}

	/// 데이터 덤프하기
	result nekoSystemButtonInstance::Dump(int32 version, nekoByteBuffer &outBuf)
	{
		nekoIBlockInstance::Dump(version, outBuf);

		outBuf << mCheckButton << mBtState << mBtType
			<< mArguments1 << mArguments2
			<< mArguments3 // 1117
		<< ((mRolloverSound) ? mRolloverSound->GetName() : "")
		<< ((mTextures[0]) ? mTextures[0]->GetName() : "")
		<< ((mTextures[1]) ? mTextures[1]->GetName() : "")
		<< ((mTextures[2]) ? mTextures[2]->GetName() : "");

		return S_OK;
	}

	result nekoSystemButtonInstance::Load(int32 version, nekoByteBuffer &inBuf)
	{
		nekoIBlockInstance::Load(version, inBuf);

		string path;
		inBuf >> mCheckButton >> mBtState >> mBtType
			>> mArguments1 >> mArguments2;

		if(version >= 1117)
		{
			inBuf >> mArguments3;
		}

		inBuf >> path;

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
	nekoSystemButton::nekoSystemButton()
		: nekoIBlock(BLOCK_SYSTEMBUTTON)
	{

	}

	/// Destructor
	nekoSystemButton::~nekoSystemButton()
	{

	}

	/// 인스턴스를 만든다
	nekoIBlockInstance *nekoSystemButton::CreateInstance()
	{
		return nekoNew nekoSystemButtonInstance(this);
	}
}