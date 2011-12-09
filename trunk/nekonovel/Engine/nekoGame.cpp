#include "nekoPrecompiledHeader.h"
#include "nekoGame.h"
#include "nekoLogger.h"
#include "nekoUtility.h"
#include "NekoNovel.h"
#include <tinyxml.h>
#include "nekoVideoDriver.h"
#include <time.h>
#include "nekoHWResourceManager.h"

#define MAX_STACK_SIZE 30

namespace neko
{
	void nekoGameEnvironment::Clear()
	{
		mPrintX = 0;
		mPrintY = 0;
		mSpeechSpeed = 0.04f;
		mSelectionColor = D3DCOLOR_ARGB(255, 255, 255, 0);
		mSpeechColor = D3DCOLOR_ARGB(255, 255, 255, 255);
		mSpeechShadowType = 1;
		mSpeechShadowColor = D3DCOLOR_ARGB(255, 0, 0, 0);
		mSystemMenuEnabled = true;
		mProgressingType = EGPT_NORMAL;
		mPrintDelay = true;
		mPhotoMode = false;
		mCameraIgnored = false;
		mCGFadeInSpeed = 0.0f;
		mCGFadeOutSpeed = 0.0f;
		mSmoothPrint = true;
		mGameRuntimes = 0;
		mTextAlign = ETAT_LEFT;
		mTextSubSpeed = 0;
		mSlotPage = 1;
		mIW2Use = true;
		mAutoWaitTime = 3.0f;
		mGamePlayTime = 0.0f;
	}

	nekoSaveData::nekoSaveData()
		: mVersion(0)
	{

	}

	nekoSaveData::~nekoSaveData()
	{
		
	}

	result nekoSaveData::Initialize(const char8 *fileName)
	{
		mFileName = fileName;

		// 파일 체크
		string fullPath = GetNekoNovel()->GetAppFolderPath();
		FILE *fp = fopen((fullPath + fileName).c_str(), "rb");
		if(fp == NULL)
			return E_NOT_EXIST;

		fseek(fp, 0, SEEK_END);
		int32 totalLength = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		// 파일 체크
		const string &str = nekoUtility::ReadString(fp, strlen(SAVEDATA_CODENAME));
		if(str != SAVEDATA_CODENAME)
		{
			fclose(fp);
			LogPrint(LOG_LEVEL_WARN, "세이브 파일 '%s'는 손상되었습니다.", fileName);
			return E_FAIL;
		}

		int32 temp;
		fread(&(temp = ENGINE_VERSION), 1, sizeof(int32), fp);

		if(temp < 102)
		{
			fclose(fp);
			LogPrint(LOG_LEVEL_WARN, "세이브 파일 '%s'는 버전이 낮아 지원되지 않습니다.", fileName);
			return E_FAIL;
		}

		uint32 length;

		// 사진 열기
		fread(&length, 1, sizeof(uint32), fp);
		if(length > 0)
		{
			char8 *data = nekoNew char8[length];
			fread(data, 1, length, fp);
			// 프리뷰만 있으면 되!
			mPreview = nekoNew nekoHWTexture(GetNekoNovel()->GetHWResourceManager(), data, length);
		}

		mDateString = nekoUtility::ReadString(fp);

		// 버전만 살짝 얻어오기
		if(temp >= 119)
		{
			mCaption = nekoUtility::ReadString(fp);
			fread(&mTime, 1, sizeof(time_t), fp);
		}
		else
		{
			mLiteDateString = "2010.01.01 12:00:00";
		}

		fread(&mVersion, 1, sizeof(int32), fp);

		length = (totalLength - ftell(fp));
		char8 *data = nekoNew char8[length];
		fread(data, length, 1, fp);
		mBuffer.AdoptBuffer(data, length);

		fclose(fp);
		return S_OK;
	}

	result nekoSaveData::LoadToGame()
	{
		mBuffer.Clear();
		return GetNekoNovel()->GetCurrentGame()->LoadFrom(mVersion, mBuffer);
	}

	result nekoSaveData::SaveToFile(const char8 *fileName)
	{
		nekoGame *currentGame = GetNekoNovel()->GetCurrentGame();

		string fullPath = GetNekoNovel()->GetAppFolderPath();
		CreateDirectory((fullPath + "세이브\\").c_str(), NULL);
		FILE *fp = fopen((fullPath + fileName).c_str(), "wb");
		if(fp == NULL)
		{
			LogPrint(LOG_LEVEL_WARN, "세이브 할 파일 '%s'를 열 수 없습니다.", fileName);
			return E_FAIL;
		}

		nekoUtility::WriteString(fp, SAVEDATA_CODENAME);
		int32 temp;
		fwrite(&(temp = ENGINE_VERSION), 1, sizeof(int32), fp);

		// 이미지 저장
		LPD3DXBUFFER buf = currentGame->TakeSaveScreenshot();
		if(buf != 0)
		{
			uint32 length = buf->GetBufferSize();
			fwrite(&length, 1, sizeof(uint32), fp);
			fwrite(buf->GetBufferPointer(), 1, length, fp);
			
			char8 *newData = nekoNew char8[length];
			memcpy(newData, buf->GetBufferPointer(), length);
			mPreview = nekoNew nekoHWTexture(GetNekoNovel()->GetHWResourceManager(), newData, length);

			buf->Release();
		}
		else
		{
			fwrite(&(temp = 0), 1, sizeof(uint32), fp);
		}
		
		// 날짜 저장
		struct tm *t;
		mTime = time(0);
		t = localtime(&mTime);

		mDateString = nekoUtility::FormString("%d년 %d월 %d일 \n%02d시 %02d분 %02d초", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
		t->tm_hour, t->tm_min, t->tm_sec);
		nekoUtility::WriteString(fp, mDateString.c_str());

		// 119
		nekoVariable *variable;
		if(variable = currentGame->GetVariable("슬롯씬이름"))
			mCaption = variable->GetValue();

		nekoUtility::WriteString(fp, mCaption.c_str());
		fwrite(&mTime, 1, sizeof(time_t), fp);

		fwrite(&(mVersion = SAVEDATA_VERSION), 1, sizeof(int32), fp);
		mBuffer.Clear();
		currentGame->DumpTo(mBuffer);
		fwrite(mBuffer.GetBuffer(), mBuffer.GetLength(), 1, fp);

		fclose(fp);
		return S_OK;
	}

	// 필요할 때만 생성
	const char8 *nekoSaveData::GetLiteDateString()
	{
		if(mLiteDateString.empty())
		{
			struct tm *t;
			t = localtime(&mTime);

			mLiteDateString = nekoUtility::FormString("%d.%02d.%02d %02d:%02d:%02d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
				t->tm_hour, t->tm_min, t->tm_sec);
		}

		return mLiteDateString.c_str();
	}

	///////////////////////////////////////////////////////
	// 게임을 전체적으로 관리하는 클래스
	///////////////////////////////////////////////////////

	/// Constructor
	nekoGame::nekoGame(NekoNovel *nekoCore)
		: mIsPlaying(false)
		, mNekoCore(nekoCore)
		, mGameState(EGSS_NONE)
		, mSkipAble(false)
		, mThreadUpdating(false)
		, mPickedInstance(NULL)
		, mBGImageEffect(EBET_NONE)
		, mBGImageEffectSpeed(1.0f)
		, mJumped(false)
		, mFlowState(EGFS_NORMAL)
		, mGameFlags(0)
		, mMsgBoxType(EMT_UNKNOWN)
		, mMsgBoxArgs(0)
	{
		char8 buf[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, buf);
		mRunPath = buf;

		//mShotTacker.Start();

		// 게임 실행 후 단 한번만 초기화 할 값들
		mPlayingEnv.mMaxPage = 6;
		mPlayingEnv.mProjectFlags = 0;
		mPlayingEnv.mTextWindowAlpha = 255;
	}

	/// Destructor
	nekoGame::~nekoGame()
	{
		ClearSelections();

		list<nekoCommands *>::iterator itCmd = mLoadedCommands.begin();
		for(uint32 i = 0;i < mLoadedCommands.size();++i, ++itCmd)
			(*itCmd)->Release();

		list<nekoIBlockInstance *>::iterator iter = mBlockInstanceList.begin();
		for(uint32 i = 0;i < mBlockInstanceList.size();++i, ++iter)
			(*iter)->Release();

		list<nekoIBlockInstance *>::iterator itSkip = mSkipAbleInstances.begin();
		for(;itSkip != mSkipAbleInstances.end();++itSkip)
			(*itSkip)->Release();

		mNekoCore->LoadExternalFonts(mPlayingEnv.mExternalFonts, true);
	}

	result nekoGame::Initialize()
	{
		mChatBox = nekoNew nekoGUIChatBox;
		mSelectionButtons = nekoNew nekoGUISelectionButtons;

		return S_OK;
	}

	/// Add instance
	nekoIBlockInstance *nekoGame::AddInstance(nekoIBlockInstance *inst)
	{
		if(!inst)
			return NULL;

		mBlockInstanceList.push_back(inst);
		return inst;
	}

	nekoIBlockInstance *nekoGame::CreateInstance(E_BLOCK_TYPE blockType, const char8 *name, bool *firstInitialize)
	{
		nekoIBlockInstance *inst = NULL;

		if(stricmp(name, "대사창") == 0)
			return mChatBox;
		else if(blockType == BLOCK_BGMUSIC)
			inst = mBGMusic;
		else if(blockType == BLOCK_VOICE)
			inst = mVoice;
		else
		{
			// Button은 체크할 필요 없음.
			if(!(blockType == BLOCK_TRACKBAR))
			{
				list<nekoIBlockInstance *>::iterator iter = mBlockInstanceList.begin();
				for(;iter != mBlockInstanceList.end();++iter)
				{
					// 찾았어 ?!
					if(stricmp(name, (*iter)->GetName()) == 0)
					{
						// 근데 다른 녀석이야 ?
						if((*iter)->GetType() != blockType)
						{
							// 그럼 얘 지우고 새로 만들자...
							RemoveInstance((*iter));
							break;
						}

						inst = (*iter);

						// 그럼 맨뒤로 옮겨주라!
						mBlockInstanceList.erase(iter);
						mBlockInstanceList.push_back(inst);

						// 배경효과 설정
						if(mRunType != GAME_RUN_PREVIEW && blockType == BLOCK_BGIMAGE && mBGImageEffect != EBET_NONE
							&& !IsFloatZero(mBGImageEffectSpeed))
							((nekoBGImageInstance *)inst)->SetEffect(mBGImageEffect, mBGImageEffectSpeed, true, mBGImageEffectRule);
						break;
					}
				}
			}
		}

		// 이미 있다면~
		if(inst != NULL)
		{
			if(mSkipAble)
			{
				/*
				// 이미 스킵리스트 목록에 있다면 추가할 필요는 없다.
				list<nekoIBlockInstance *>::iterator iter = mSkipAbleInstances.begin();
				for(;iter != mSkipAbleInstances.end();++iter)
				{
					if((*iter) == inst)
						return inst;
				}
				*/

				// 없을 경우..
				inst->AddRef();
				mSkipAbleInstances.push_back(inst);
			}

			if(firstInitialize)
				*firstInitialize = false;

			return inst;
		}

		nekoIBlock *block = mNekoCore->GetBlock(blockType);
		if(block == NULL)
		{
			LogPrint(LOG_LEVEL_WARN, "'%s' 블럭 인스턴스를 생성할 수 없습니다.", name);
			return NULL;
		}

		if(firstInitialize)
			*firstInitialize = true;

		inst = block->CreateInstance();
		inst->SetName(name);

		/// 카메라 무시 적용.
		if(mPlayingEnv.mCameraIgnored)
			inst->AddFlags(EBIF_IGNORE_CAMERA);
		else
			inst->RemoveFlags(EBIF_IGNORE_CAMERA);

		if(mSkipAble)
		{
			inst->AddRef();
			mSkipAbleInstances.push_back(inst);
		}

		if(blockType == BLOCK_BGMUSIC)
		{
			(mBGMusic = (nekoBGMusicInstance *)inst);
			return inst;
		}
		else if(blockType == BLOCK_VOICE)
		{
			(mVoice = (nekoVoiceInstance *)inst);
			return inst;
		}

		// 배경효과 설정
		if(mRunType != GAME_RUN_PREVIEW && blockType == BLOCK_BGIMAGE && mBGImageEffect != EBET_NONE
			&& !IsFloatZero(mBGImageEffectSpeed))
			((nekoBGImageInstance *)inst)->SetEffect(mBGImageEffect, mBGImageEffectSpeed, true, mBGImageEffectRule);

		mBlockInstanceList.push_back(inst);
		return inst;
	}

	bool nekoGame::RemoveInstance(nekoIBlockInstance *inst)
	{
		if(inst == NULL)
			return false;

		bool result = false;

		if(inst == mBGMusic)
		{
			mBGMusic = NULL;
			result = true;
		} else if(inst == mVoice)
		{
			mVoice = NULL;
			result = true;
		} else
		{
			list<nekoIBlockInstance *>::iterator iter = mBlockInstanceList.begin();
			for(uint32 i = 0;i < mBlockInstanceList.size();++i, ++iter)
			{
				if((*iter) == inst)
				{
					(*iter)->Release();
					mBlockInstanceList.erase(iter);
					result = true;
					break;
				}
			}
		}

		if(mSkipAble)
		{
			/// 스킵 리스트에서도 삭제
			list<nekoIBlockInstance *>::iterator itSkip = mSkipAbleInstances.begin();
			for(;itSkip != mSkipAbleInstances.end();++itSkip)
			{
				if((*itSkip) == inst)
				{
					inst->Release();
					mSkipAbleInstances.erase(itSkip);
					break;
				}
			}
		}

		return result;
	}

	nekoIBlockInstance *nekoGame::GetInstance(const char8 *name)	
	{
		list<nekoIBlockInstance *>::iterator iter = mBlockInstanceList.begin();
		for(uint32 i = 0;i < mBlockInstanceList.size();++i, ++iter)
		{
			if(stricmp((*iter)->GetName(), name) == 0)
				return (*iter);
		}

		if(stricmp(name, "배경음악") == 0)
			return mBGMusic;

		else if(stricmp(name, "대사창") == 0)
			return mChatBox;

		else if(stricmp(name, "보이스") == 0)
			return mVoice;

		return NULL;
	}

	void nekoGame::ClearInstances(E_BLOCK_TYPE blockType, nekoIBlockInstance *without, bool excludesNonZeroAlphaDeleting)
	{
		/// 특정 인스턴스 삭제
		list<nekoIBlockInstance *>::iterator it = mBlockInstanceList.begin();
		for(;it != mBlockInstanceList.end();)
		{
			if((*it)->GetType() == blockType && (*it) != without)
			{
				if(excludesNonZeroAlphaDeleting && !(*it)->IsZeroAlphaDeleting())
				{
					++it;
					continue;
				}

				(*it)->Release();
				mBlockInstanceList.erase(it++);
			}
			else
				++it;
		}

		if(mSkipAble)
		{
			/// 스킵가능한 특정 인스턴스 삭제
			list<nekoIBlockInstance *>::iterator itSkip = mSkipAbleInstances.begin();
			for(;itSkip != mSkipAbleInstances.end();)
			{
				if((*itSkip)->GetType() == blockType && (*itSkip) != without)
				{
					(*itSkip)->Release();
					mSkipAbleInstances.erase(itSkip++);
				}
				else
					itSkip++;
			}
		}
	}

	/// Load
	result nekoGame::Load(const char8 *projectInfoData)
	{
		TiXmlDocument doc;

		if(projectInfoData == NULL)
		{
			string fullPath = GetNekoNovel()->GetAppFolderPath();
			if(FAILED(doc.LoadFile((fullPath + "Game.nknovel").c_str())))
			{
				LogPrint(LOG_LEVEL_WARN, "Game.nknovel 파일 로드 실패 : [%d]%s", doc.ErrorRow(), doc.ErrorDesc());
				return E_FAIL;
			}
		}
		else
		{
			doc.Parse(projectInfoData);
			if(doc.Error())
			{
				LogPrint(LOG_LEVEL_WARN, "Game.nknovel 파일 로드 실패 : [%d]%s", doc.ErrorRow(), doc.ErrorDesc());
				return E_FAIL;
			}
		}

		TiXmlElement *eRoot = doc.FirstChildElement("NekoNovel_Game");
		if(!eRoot)
		{
			LogPrint(LOG_LEVEL_WARN, "게임 파일에 루트 노드가 존재하지 않아요!");
			return E_FAIL;
		}

		TiXmlElement *e;
		TiXmlElement *n;
		const char *str;

		if(e = eRoot->FirstChildElement("Game"))
		{
			mName.clear();
			mMadeBy.clear();
			mPlayingEnv.mExternalFonts.clear();

			if(str = e->FirstChildElement("Title")->GetText())
				mName = str;

			if(str = e->FirstChildElement("MadeBy")->GetText())
				mMadeBy	= str;

			mPlayingEnv.mDefFullScreen = (stricmp(e->FirstChildElement("DefaultFullScreen")->GetText(), "true") == 0);
			mPlayingEnv.mDefFXVolume = atoi(e->FirstChildElement("DefaultFXVolume")->GetText());
			mPlayingEnv.mDefBGMVolume = atoi(e->FirstChildElement("DefaultBGMVolume")->GetText());
			mPlayingEnv.mDefVoiceVolume = atoi(e->FirstChildElement("DefaultVoiceVolume")->GetText());

			if((n = e->FirstChildElement("TextWindowAlpha")) && n->GetText())
				mPlayingEnv.mTextWindowAlpha = atoi(n->GetText());

			if(n = e->FirstChildElement("FirstScriptName"))
				mPlayingEnv.mFirstScriptName = n->GetText();
			else
				mPlayingEnv.mFirstScriptName = "타이틀.txt";

			if(n = e->FirstChildElement("CursorType"))
				mPlayingEnv.mCursorType = (E_CHATBOX_CURSOR_TYPE)atoi(n->GetText());
			else
				mPlayingEnv.mCursorType = ECCT_TWINKLE;

			if(n = e->FirstChildElement("CursorAniWidth"))
				mPlayingEnv.mCursorAniWidth = atoi(n->GetText());
			else
				mPlayingEnv.mCursorAniWidth = 1;

			if(n = e->FirstChildElement("CursorAniHeight"))
				mPlayingEnv.mCursorAniHeight = atoi(n->GetText());
			else
				mPlayingEnv.mCursorAniHeight = 1;

			if(n = e->FirstChildElement("ScreenWidth"))
				mPlayingEnv.mWidth = atoi(n->GetText());
			else
				mPlayingEnv.mWidth = 800;

			if(n = e->FirstChildElement("ScreenHeight"))
				mPlayingEnv.mHeight = atoi(n->GetText());
			else
				mPlayingEnv.mHeight = 600;

			if((n = e->FirstChildElement("ExternalFonts")) && n->GetText() != 0)
				mPlayingEnv.mExternalFonts = n->GetText();

			if((n = e->FirstChildElement("MaxPage")) && n->GetText() != 0)
				mPlayingEnv.mMaxPage = atoi(n->GetText());

			if((n = e->FirstChildElement("ProjectFlags")) && n->GetText() != 0)
				mPlayingEnv.mProjectFlags = atoi(n->GetText());

			mNekoCore->LoadExternalFonts(mPlayingEnv.mExternalFonts);
			mNekoCore->OnWindowChanged(mPlayingEnv.mWidth, mPlayingEnv.mHeight, false);
			mNekoCore->SetWindowTitle(nekoUtility::FormString("%s%s%s (nkNovel %.2f)",
				mName.c_str(), (!mMadeBy.empty()) ? " by " : "", mMadeBy.c_str(), ENGINE_VERSION / 100.0f));
		}

		// 프로젝트를 위한 것..
		if(projectInfoData == NULL)
		{
			if(e = eRoot->FirstChildElement("LoadedScripts"))
			{
				TiXmlElement *e2 = e->FirstChildElement("Script");

				while(e2)
				{
					CreateCommands(e2->GetText());
					e2 = e2->NextSiblingElement();
				}
			}
		}

		if(e = eRoot->FirstChildElement("Publish"))
		{
			const char8 *ptr;
			mPlayingEnv.mCompressBGM = (stricmp(e->FirstChildElement("CompressBGM")->GetText(), "true") == 0);
			if(ptr = e->FirstChildElement("LastPublishFolder")->GetText())
				mPlayingEnv.mLastPublishFolder = ptr;
			else
				mPlayingEnv.mLastPublishFolder = "";

			if(ptr = e->FirstChildElement("LastPublishVersion")->GetText())
				mPlayingEnv.mLastPublishVersion = atoi(ptr);
			else
				mPlayingEnv.mLastPublishVersion = 1;
		}

		LogPrint(LOG_LEVEL_INFO, "게임 파일이 로드 되었습니다!");
		return S_OK;
	}

	/// Save
	result nekoGame::Save()
	{
		TiXmlDocument doc("Game.nknovel");

		TiXmlElement *root = new TiXmlElement("NekoNovel_Game");

		TiXmlElement *eGame = new TiXmlElement("Game");
		{
			TiXmlElement *eTitle = new TiXmlElement("Title");
			eTitle->LinkEndChild(new TiXmlText(mName.c_str()));
			eGame->LinkEndChild(eTitle);

			TiXmlElement *eMadeBy = new TiXmlElement("MadeBy");
			eMadeBy->LinkEndChild(new TiXmlText(mMadeBy.c_str()));
			eGame->LinkEndChild(eMadeBy);

			{
				TiXmlElement *e = new TiXmlElement("DefaultFullScreen");
				e->LinkEndChild(new TiXmlText((mPlayingEnv.mDefFullScreen) ? "true" : "false"));
				eGame->LinkEndChild(e);
			}

			{
				TiXmlElement *e = new TiXmlElement("DefaultFXVolume");
				e->LinkEndChild(new TiXmlText(nekoUtility::FormString("%d", mPlayingEnv.mDefFXVolume)));
				eGame->LinkEndChild(e);
			}

			{
				TiXmlElement *e = new TiXmlElement("DefaultBGMVolume");
				e->LinkEndChild(new TiXmlText(nekoUtility::FormString("%d", mPlayingEnv.mDefBGMVolume)));
				eGame->LinkEndChild(e);
			}

			{
				TiXmlElement *e = new TiXmlElement("DefaultVoiceVolume");
				e->LinkEndChild(new TiXmlText(nekoUtility::FormString("%d", mPlayingEnv.mDefVoiceVolume)));
				eGame->LinkEndChild(e);
			}

			{
				TiXmlElement *e = new TiXmlElement("TextWindowAlpha");
				e->LinkEndChild(new TiXmlText(nekoUtility::FormString("%d", mPlayingEnv.mTextWindowAlpha)));
				eGame->LinkEndChild(e);
			}

			{
				TiXmlElement *e = new TiXmlElement("FirstScriptName");
				e->LinkEndChild(new TiXmlText(mPlayingEnv.mFirstScriptName.c_str()));
				eGame->LinkEndChild(e);
			}

			{
				TiXmlElement *e = new TiXmlElement("CursorType");
				e->LinkEndChild(new TiXmlText(nekoUtility::FormString("%d", mPlayingEnv.mCursorType)));
				eGame->LinkEndChild(e);
			}

			{
				TiXmlElement *e = new TiXmlElement("CursorAniWidth");
				e->LinkEndChild(new TiXmlText(nekoUtility::FormString("%d", mPlayingEnv.mCursorAniWidth)));
				eGame->LinkEndChild(e);
			}

			{
				TiXmlElement *e = new TiXmlElement("CursorAniHeight");
				e->LinkEndChild(new TiXmlText(nekoUtility::FormString("%d", mPlayingEnv.mCursorAniHeight)));
				eGame->LinkEndChild(e);
			}

			{
				TiXmlElement *e = new TiXmlElement("ScreenWidth");
				e->LinkEndChild(new TiXmlText(nekoUtility::FormString("%d", mPlayingEnv.mWidth)));
				eGame->LinkEndChild(e);
			}

			{
				TiXmlElement *e = new TiXmlElement("ScreenHeight");
				e->LinkEndChild(new TiXmlText(nekoUtility::FormString("%d", mPlayingEnv.mHeight)));
				eGame->LinkEndChild(e);
			}

			{
				TiXmlElement *e = new TiXmlElement("ExternalFonts");
				e->LinkEndChild(new TiXmlText(mPlayingEnv.mExternalFonts.c_str()));
				eGame->LinkEndChild(e);
			}

			{
				TiXmlElement *e = new TiXmlElement("MaxPage");
				e->LinkEndChild(new TiXmlText(nekoUtility::FormString("%d", mPlayingEnv.mMaxPage)));
				eGame->LinkEndChild(e);
			}

			{
				TiXmlElement *e = new TiXmlElement("ProjectFlags");
				e->LinkEndChild(new TiXmlText(nekoUtility::FormString("%d", mPlayingEnv.mProjectFlags)));
				eGame->LinkEndChild(e);
			}
		}
		root->LinkEndChild(eGame);

		TiXmlElement *eLoadedScripts = new TiXmlElement("LoadedScripts");
		{
			TiXmlElement *e;

			list<nekoCommands *>::iterator iter = mLoadedCommands.begin();
			for(;iter != mLoadedCommands.end();++iter)
			{
				e = new TiXmlElement("Script");
				e->LinkEndChild(new TiXmlText((*iter)->GetName()));
				eLoadedScripts->LinkEndChild(e);
			}
		}
		root->LinkEndChild(eLoadedScripts);

		TiXmlElement *ePublish = new TiXmlElement("Publish");
		{
			{
				TiXmlElement *e = new TiXmlElement("LastPublishFolder");
				e->LinkEndChild(new TiXmlText(mPlayingEnv.mLastPublishFolder.c_str()));
				ePublish->LinkEndChild(e);
			}

			{
				TiXmlElement *e = new TiXmlElement("LastPublishVersion");
				e->LinkEndChild(new TiXmlText(nekoUtility::FormString("%d", mPlayingEnv.mLastPublishVersion)));
				ePublish->LinkEndChild(e);
			}

			{
				TiXmlElement *e = new TiXmlElement("CompressBGM");
				e->LinkEndChild(new TiXmlText((mPlayingEnv.mCompressBGM) ? "true" : "false"));
				ePublish->LinkEndChild(e);
			}
		}
		root->LinkEndChild(ePublish);

		doc.LinkEndChild(root);

		if(!doc.SaveFile())
		{
			LogPrint(LOG_LEVEL_WARN, "'Game.nknovel' 파일 저장에 실패하였습니다!");
			return E_FAIL;
		}

		return S_OK;
	}


	/// Build it!
	result nekoGame::Build()
	{
		if(!mPlayingEnv.mLastPublishFolder.length())
			return E_FAIL;

		// 프로젝트 파일 만들기
		FILE *fp = fopen(nekoUtility::FormString("%s\\Game.nkpublished", mPlayingEnv.mLastPublishFolder.c_str()), "wb");
		if(fp == NULL)
			return E_FAIL;
		int32 temp;
		
		nekoUtility::WriteString(fp, ENGINE_CODENAME);
		fwrite(&(temp = ENGINE_VERSION), 1, sizeof(int32), fp);
		nekoUtility::WriteString(fp, mName.c_str());
		nekoUtility::WriteString(fp, mMadeBy.c_str());
		fwrite(&mPlayingEnv.mDefFullScreen, 1, sizeof(bool), fp);
		fwrite(&mPlayingEnv.mDefFXVolume, 1, sizeof(int32), fp);
		fwrite(&mPlayingEnv.mDefBGMVolume, 1, sizeof(int32), fp);
		fwrite(&mPlayingEnv.mDefVoiceVolume, 1, sizeof(int32), fp);
		fwrite(&mPlayingEnv.mLastPublishVersion, 1, sizeof(int32), fp);
		fwrite(&mPlayingEnv.mCompressBGM, 1, sizeof(int32), fp);
		nekoUtility::WriteString(fp, mPlayingEnv.mFirstScriptName.c_str());
		fwrite(&mPlayingEnv.mCursorType, 1, sizeof(int32), fp);
		fwrite(&mPlayingEnv.mCursorAniWidth, 1, sizeof(int32), fp);
		fwrite(&mPlayingEnv.mCursorAniHeight, 1, sizeof(int32), fp);
		fwrite(&mPlayingEnv.mWidth, 1, sizeof(int32), fp);
		fwrite(&mPlayingEnv.mHeight, 1, sizeof(int32), fp);
		nekoUtility::WriteString(fp, mPlayingEnv.mExternalFonts.c_str());
		fwrite(&mPlayingEnv.mMaxPage, 1, sizeof(int32), fp);
		fwrite(&mPlayingEnv.mProjectFlags, 1, sizeof(uint32), fp);
		fwrite(&mPlayingEnv.mTextWindowAlpha, 1, sizeof(uint32), fp); // 119
		fclose(fp);

		LogPrint(LOG_LEVEL_INFO,"'%s' 위치에 올바르게 빌드했습니다.", mPlayingEnv.mLastPublishFolder.c_str());
		return S_OK;
	}

	/// Run
	result nekoGame::Run()
	{
		// 프로젝트 파일 열기
		string fullPath = GetNekoNovel()->GetAppFolderPath();
		FILE *fp = fopen((fullPath + "Game.nkpublished").c_str(), "rb");
		if(fp == NULL)
			return E_FAIL;

		string str = nekoUtility::ReadString(fp, strlen(ENGINE_CODENAME));
		if(str != ENGINE_CODENAME)
		{
			fclose(fp);
			LogPrint(LOG_LEVEL_WARN, "Game.nkpublished 파일이 손상되었습니다.");
			return E_FAIL;
		}
		int32 temp;

		fread(&temp, 1, sizeof(int32), fp);
		if(ENGINE_VERSION < temp)
		{
			LogPrint(LOG_LEVEL_WARN, "nkNovel 엔진이 게임보다 구버전입니다.");
			return E_FAIL;
		}

		mName = nekoUtility::ReadString(fp);
		mMadeBy = nekoUtility::ReadString(fp);
		fread(&mPlayingEnv.mDefFullScreen, 1, sizeof(bool), fp);
		fread(&mPlayingEnv.mDefFXVolume, 1, sizeof(int32), fp);
		fread(&mPlayingEnv.mDefBGMVolume, 1, sizeof(int32), fp);
		fread(&mPlayingEnv.mDefVoiceVolume, 1, sizeof(int32), fp);
		fread(&mPlayingEnv.mLastPublishVersion, 1, sizeof(int32), fp);
		fread(&mPlayingEnv.mCompressBGM, 1, sizeof(int32), fp);

		/*
		if(!feof(fp))
		{
			int32 version;
			fread(&version, 1, sizeof(int32), fp);


		}
		*/

		if(temp >= 110)
		{
			mPlayingEnv.mFirstScriptName = nekoUtility::ReadString(fp);
			fread(&mPlayingEnv.mCursorType, 1, sizeof(int32), fp);
			fread(&mPlayingEnv.mCursorAniWidth, 1, sizeof(int32), fp);
			fread(&mPlayingEnv.mCursorAniHeight, 1, sizeof(int32), fp);
		}

		if(temp >= 113)
		{

			fread(&mPlayingEnv.mWidth, 1, sizeof(int32), fp);
			fread(&mPlayingEnv.mHeight, 1, sizeof(int32), fp);

			mNekoCore->OnWindowChanged(mPlayingEnv.mWidth, mPlayingEnv.mHeight, false);
		}

		if(temp >= 117)
		{
			mPlayingEnv.mExternalFonts = nekoUtility::ReadString(fp);
			mNekoCore->LoadExternalFonts(mPlayingEnv.mExternalFonts);
		}

		if(temp >= 119)
		{
			fread(&mPlayingEnv.mMaxPage, 1, sizeof(int32), fp);
			fread(&mPlayingEnv.mProjectFlags, 1, sizeof(uint32), fp);
			fread(&mPlayingEnv.mTextWindowAlpha, 1, sizeof(int32), fp);
		}

		fclose(fp);

		string strVersion = nekoUtility::FormString(" Ver %.2f", mPlayingEnv.mLastPublishVersion / 100.0f);
		if(!mPlayingEnv.mLastPublishVersion) strVersion.clear();

		mNekoCore->SetWindowTitle(nekoUtility::FormString("%s%s%s%s (nkNovel %.2f)",
			mName.c_str(), (!mMadeBy.empty()) ? " by " : "", mMadeBy.c_str(), strVersion.c_str(), ENGINE_VERSION / 100.0f));

		return S_OK;
	}

	/// Save slot
	result nekoGame::SaveToSlot(int32 slotNo)
	{
		if(!(slotNo >= 0 && slotNo < MAX_SAVE_COUNT))
		{
			// 퀵세이브를 위한..
			if(slotNo != QUICK_SAVE_IDX)
			return E_FAIL;
		}

		if(mRunType == GAME_RUN_PREVIEW)
			return E_FAIL;

		result res;
		if(slotNo == QUICK_SAVE_IDX)
		{
			mQuickSave = nekoNew nekoSaveData;
			if(FAILED(res = mQuickSave->SaveToFile("세이브\\퀵.nkslot")))
			{
				mQuickSave = NULL;
				return res;
			}
		}
		else
		{
			mSaveDatas[slotNo] = nekoNew nekoSaveData;
			if(FAILED(res = mSaveDatas[slotNo]->SaveToFile(nekoUtility::FormString("세이브\\세이브%02d.nkslot", slotNo))))
			{
				mSaveDatas[slotNo] = NULL;
				return res;
			}
		}

		// '키처리' 커스텀 이벤트 전달
		CallCustomEvent(ECET_SAVED);

		// 이벤트 전달
		nekoEvent e(EVENT_GAME_SAVED);
		e.value[0] = res;
		e.value[1] = slotNo;
		GetNekoNovel()->OnEvent(e);

		return S_OK;
	}

	/// Load from slot
	result nekoGame::LoadFromSlot(int32 slotNo)
	{
		nekoSaveData *saveData = NULL;
		if(!(slotNo >= 0 && slotNo < MAX_SAVE_COUNT))
		{
			// 퀵세이브일 경우..
			if(slotNo == QUICK_SAVE_IDX)
			{
				saveData = mQuickSave;
			}
			else
				return E_FAIL;
		}
		else
			saveData = mSaveDatas[slotNo];

		if(saveData == NULL)
			return E_FAIL;
		
		result res = saveData->LoadToGame();

		if(SUCCEEDED(res))
		{
			CallCustomEvent(ECET_LOADED);
		}

		// 이벤트 전달
		nekoEvent e(EVENT_GAME_LOADED);
		e.value[0] = res;
		e.value[1] = slotNo;
		GetNekoNovel()->OnEvent(e);

		return res;
	}

	/// Remove slot
	result nekoGame::RemoveSlot(int32 slotNo)
	{
		if(!(slotNo >= 0 && slotNo < MAX_SAVE_COUNT))
			return E_FAIL;

		if(mSaveDatas[slotNo] == NULL)
			return E_FAIL;

		mSaveDatas[slotNo] = 0;

		// 세이브 파일 삭제
		DeleteFile(
			nekoUtility::FormString("%s세이브\\세이브%02d.nkslot",
			GetNekoNovel()->GetAppFolderPath(), slotNo));
		return S_OK;
	}

	/// Save auto to slot
	result nekoGame::SaveAutoToSlot()
	{
		if(mRunType == GAME_RUN_PREVIEW)
			return E_FAIL;

		for(int32 i = 0;i < MAX_SAVE_COUNT;++i)
		{
			if(mSaveDatas[i] == NULL)
			{
				return SaveToSlot(i);
			}
		}

		// 0번 슬롯에 저장.
		return SaveToSlot(MAX_SAVE_COUNT - 1);
	}

	/// Create Commands
	nekoCommands *nekoGame::CreateCommands(const char8 *name)
	{
		nekoCommands *cmds = (name) ? GetCommands(name) : NULL;
		if(cmds)
		{
			LogPrint(LOG_LEVEL_WARN, "'%s' 이름의 스크립트는 이미 존재해요.", name);
			return NULL;
		}

		mLoadedCommands.push_back(cmds = nekoNew nekoCommands(name));
		return cmds;
	}

	/// Remove Commands
	bool nekoGame::RemoveCommands(nekoCommands *commands)
	{
		list<nekoCommands *>::iterator iter = mLoadedCommands.begin();
		for(;iter != mLoadedCommands.end();++iter)
		{
			if((*iter) == commands)
			{
				commands->Release();
				mLoadedCommands.erase(iter);
				return true;
			}
		}

		return false;
	}

	/// Get Commands by name
	nekoCommands *nekoGame::GetCommands(const char8 *name)
	{
		list<nekoCommands *>::iterator iter = mLoadedCommands.begin();
		for(;iter != mLoadedCommands.end();++iter)
		{
			if(stricmp(name, (*iter)->GetName()) == 0)
				return (*iter);
		}

		// Find in stack list
		list<nekoThread>::iterator itStack = mStackList.begin();
		for(;itStack != mStackList.end();++itStack)
		{
			if(stricmp(name, (*itStack).mCommands->GetName()) == 0)
				return (*itStack).mCommands;
		}

		return 0;
	}

	/// Change Commands' order.
	bool nekoGame::ChangeCommandsOrder(nekoCommands *commands, uint32 index)
	{
		if(index >= mLoadedCommands.size())
			return false;

		list<nekoCommands *>::iterator iter = mLoadedCommands.begin();
		for(;iter != mLoadedCommands.end();++iter)
		{
			if((*iter) == commands)
			{
				// 삭제하고 재삽입..
				mLoadedCommands.erase(iter);

				iter = mLoadedCommands.begin();
				uint32 i = 0;
				for(;iter != mLoadedCommands.end();++iter, ++i)
				{
					if(i == index)
					{
						mLoadedCommands.insert(iter, commands);
						break;
					}
				}

				if(iter == mLoadedCommands.end())
					mLoadedCommands.push_back(commands);
				return true;
			}
		}

		return false;
	}

	/// Play commands
	result nekoGame::PlayCommands(const char8 *name, const char8 *bookMarkName, bool onlyInLoadedScripts, bool internalRequest)
	{
		if(name && name[0] != '\0')
		{
			if(mStackList.size() >= MAX_STACK_SIZE)
			{
				nekoLogger::Log(LOG_LEVEL_WARN, "최대 스크립트 스택 수(%d)를 초과하였습니다.", MAX_STACK_SIZE);
				return E_FAIL;
			}

			nekoCommands *cmds = GetCommands(name);
			if(cmds == NULL)
			{
				if(onlyInLoadedScripts)
					return E_FAIL;

				/// 파일로 부터 로드
				cmds = nekoNew nekoCommands(name);
				if(FAILED(cmds->LoadFromFile(name)))
					return E_FAIL;

				nekoThread stack(cmds, true);
				cmds->Release();
				mStackList.push_back(stack);
			}
			else
			{
				// 미리보기 모드 일때, 스택 오버플로우를 방지하기 위해..
				if(this->GetGameMode() == GAME_RUN_PREVIEW)
				{
					if(mStackList.empty() == false 
						&& (*(--mStackList.end())).mCommands == cmds)
						return E_FAIL;
				}

				nekoThread stack(cmds, true);
				mStackList.push_back(stack);
			}
		}

		nekoThread &stack = (*(--mStackList.end()));

		list<nekoOneCommand *>::iterator iter = stack.mCommands->GetEndingPos();
		if(bookMarkName == NULL || bookMarkName[0] == '\0')
		{
			// 첫 줄 실행하기!
			iter = stack.mPlayingPos;
			//if(stack.mCommands->GetCommandList().size() > 0)
			//	(*(iter = stack.mPlayingPos))->Begin(&stack, internalRequest);
		}
		else
		{
			if(FAILED(stack.mCommands->SearchBookMark(bookMarkName, iter)))
				return E_FAIL;

			stack.SetPlayingPos(iter);
		}

		if(internalRequest)
		{
			for(;iter != stack.mCommands->GetEndingPos();++iter)
			{
				(*iter)->Begin(iter, &stack, internalRequest);
				(*iter)->End(iter, &stack, internalRequest);
			}

			mStackList.pop_back();
		}
		else
		{
			if(iter != stack.mCommands->GetEndingPos())
				(*iter)->Begin(iter, &stack, internalRequest);
		}

		return S_OK;
	}

	/// Add variable
	nekoVariable *nekoGame::AddVariable(const char8 *name)
	{
		if(name == '\0')
			return NULL;

		// 환경 변수인가 ?
		if(name[0] == '%')
		{
			LogPrint(LOG_LEVEL_WARN, "'변수 이름 : %s' - 임의로 환경 변수를 추가할 수 없습니다.", name);
			return NULL;
		}

		nekoVariable *var = GetVariable(name);
		if(!var)
		{
			var = &(mVariables[name] = nekoVariable());

			if(IsTurnOn(GAME_FLAG_VARTRACE))
			{
				// 이벤트 전송
				nekoEvent e(EVENT_VARIABLE_CHANGED);
				e.value[0] = (int32)name;
				e.value[1] = (int32)var;
				OnEvent(e);
			}
		}

		return var;
	}

	bool nekoGame::RemoveVariable(const nekoVariable *var)
	{
		if(!var)
			return false;

		map<string, nekoVariable>::iterator iter = mVariables.begin();
		for(;iter != mVariables.end();++iter)
		{
			if(&((*iter).second) == var)
			{
				if(IsTurnOn(GAME_FLAG_VARTRACE))
				{
					// 이벤트 전송
					nekoEvent e(EVENT_VARIABLE_CHANGED);
					e.value[0] = (int32)(*iter).first.c_str();
					e.value[1] = 0;
					OnEvent(e);
				}
				
				mVariables.erase(iter);
				return true;
			}
		}

		return false;
	}

	/// Get variable
	nekoVariable *nekoGame::GetVariable(const char8 *name)
	{
		if(!name)
			return NULL;

		// 환경 변수이다.
		if(name[0] == '%')
		{
			static nekoVariable staticVar;

			++name;

			if(strcmp(name, "대기중") == 0)
			{
				if(IsSelecting() || (mGameState >= EGSS_WAIT && mGameState <= EGSS_BUTTON_PRESS_WAIT))
					staticVar.SetValue(nekoUtility::FormString("1"));
				else
					staticVar.SetValue(nekoUtility::FormString("0"));
			}
			else if(strcmp(name, "게임상태") == 0)
			{
				if(mFlowState == EGFS_AUTOTURNING)
					staticVar.SetValue("\"자동넘기기\"");
				else if(mFlowState == EGFS_QUICKSKIPPING)
					staticVar.SetValue("\"빠른스킵\"");
				else
					staticVar.SetValue("\"일반\"");
			}
			else if(strcmp(name, "게임실행횟수") == 0)
			{
				staticVar.SetValue(nekoUtility::FormString("%d", mPlayingEnv.mGameRuntimes));
			}
			else if(strcmp(name, "현재시간") == 0)
			{
				struct tm *t;
				time_t timer = time(0);
				t = localtime(&timer);

				staticVar.SetValue(nekoUtility::FormString("\"%02d:%02d:%02d\"", t->tm_hour, t->tm_min, t->tm_sec));
			}
			else if(strcmp(name, "페이지") == 0)
			{
				staticVar.SetValue(nekoUtility::FormString("%d", mPlayingEnv.mSlotPage));
			}
			else if(strncmp(name, "슬롯날짜", 8) == 0)
			{
				int32 slotNo = atoi(&name[8]) + (mPlayingEnv.mSlotPage - 1) * ONE_PAGE_SAVECOUNT;
				nekoSaveData *slotData = GetSaveData(slotNo - 1);
				if(slotData)
				{
					staticVar.SetStringValue(slotData->GetLiteDateString());
				}
				else
					staticVar.SetStringValue("-");
			}
			else if(strncmp(name, "슬롯씬이름", 10) == 0)
			{
				int32 slotNo = atoi(&name[10]) + (mPlayingEnv.mSlotPage - 1) * ONE_PAGE_SAVECOUNT;
				nekoSaveData *slotData = GetSaveData(slotNo - 1);
				if(slotData)
				{
					staticVar.SetStringValue(slotData->GetCaption());
				}
				else
					staticVar.SetStringValue("-");
			}
			else if(strcmp(name, "메세지") == 0)
			{
				staticVar.SetStringValue(mMsgBoxCaption.c_str());
			}
			else if(strcmp(name, "메세지박스응답") == 0)
			{
				staticVar.SetValue(nekoUtility::FormString("%d", mMsgBoxArgs));
			}
			else if(strcmp(name, "플레이시간") == 0)
			{
				staticVar.SetValue(nekoUtility::FormString("%d", (int32)mPlayingEnv.mGamePlayTime));
			}

			return &staticVar;
		}

		// 목록에서 찾아준다.
		map<string, nekoVariable>::iterator iter = mVariables.begin();
		for(;iter != mVariables.end();++iter)
		{
			if(stricmp((*iter).first.c_str(), name) == 0)
				return &((*iter).second);
		}

		return NULL;
	}

	/// Change Game Mode
	void nekoGame::ChangeGameMode(E_GAME_RUN_TYPE runType)
	{
		if(mRunType == runType)
			return;

		//sGotoLineRequest.Clear();
		
		mRunType = runType;
		mThreadList.clear();

		list<nekoIBlockInstance *>::iterator iter = mBlockInstanceList.begin();
		for(uint32 i = 0;i < mBlockInstanceList.size();++i, ++iter)
		{
			if((*iter)->GetType() == BLOCK_BGIMAGE)
			{
				((nekoBGImageInstance *)(*iter))->ClearEffect();
			}
		}
	}

	/// Update
	void nekoGame::Update(float32 frameTime)
	{
		//mShotTacker.Lock();

		if(mRunType == GAME_RUN_PLAYING)
		{
			mMessageLogger.Update(frameTime);

			mPlayingEnv.mGamePlayTime += frameTime;
		}

		// 쓰레드 업데이트
		if(mRunType == GAME_RUN_PLAYING)
		{
			// 쓰레드의 ㅎㅎ.
			mThreadUpdating = true;
			list<nekoThread>::iterator iter = mThreadList.begin();
			bool threadFinished;
			for(;iter != mThreadList.end();)
			{
				if((*iter).mRunning == true)
				{
					nekoOneCommand *cmd = *((*iter).GetPlayingPos());
					list<nekoOneCommand *>::iterator &sit = (*iter).GetPlayingPos();

					threadFinished = false;
					if(!cmd->Update(&(*iter), frameTime))
					{
						cmd->End(sit, &(*iter), false);

						++sit;
						while(true)
						{
							if(sit == (*iter).mCommands->GetEndingPos() || 
								cmd->cmdType == ECT_THREAD_DECLARE)
							{
								threadFinished = true;
								mThreadList.erase(iter++);
								break;
							}

							cmd = *sit;
							cmd->Begin(sit, &(*iter), false);
							if(cmd->Update(&(*iter), 0))
								break;
							cmd->End(sit, &(*iter), false);

							if(sit == (*iter).mCommands->GetEndingPos() || 
								cmd->cmdType == ECT_THREAD_DECLARE)
								continue;

							++sit;
						}
					}

					if(threadFinished)
						continue;
				}

				// 쓰레드가 유효하지 않다면 삭제!
				if((*iter).mValid == false)
				{
					mThreadList.erase(iter++);
				}
				else
					++iter;
			}
			mThreadUpdating = false;
		}

		// 최근 스택 업데이트 해주기
		if(mStackList.size() > 0 && mRunType == GAME_RUN_PLAYING)
		{
			bool doing = false;
			do
			{
				doing = false;

				if(mJumped)
				{
					int32 stackCount = mStackList.size() - 1;
					for(int32 i = 0;i < stackCount;++i)
						mStackList.pop_front();
					mJumped = false;
				}
				nekoThread &stack = (*(--mStackList.end()));

				if(stack.mPlayingPos != stack.mCommands->GetEndingPos())
				{
					nekoOneCommand *cmd = (*stack.mPlayingPos);
					if(!cmd->Update(&stack, frameTime))
					{
						cmd->End(stack.mPlayingPos, &stack, false);

						++stack.mPlayingPos;
						while(stack.mPlayingPos != stack.mCommands->GetEndingPos())
						{
							cmd = (*stack.mPlayingPos);
							cmd->Begin(stack.mPlayingPos, &stack, false);

							if(mJumped || &stack != &(*(--mStackList.end())))
							{
								doing = true;
								break;
							}

							if(cmd->Update(&stack, 0))
								break;

							cmd->End(stack.mPlayingPos, &stack, false);

							if(stack.mPlayingPos == stack.mCommands->GetEndingPos())
								break;

							++stack.mPlayingPos;
						}
					}
					
				}

				if(!mJumped && stack.mPlayingPos == stack.mCommands->GetEndingPos())
				{
					mStackList.pop_back();

					// 이전 스택 실행 계속하기!
					if(!mStackList.empty())
						doing = true;
				}

			} while(doing);
		}

		if(mBGMusic) mBGMusic->Update(frameTime);

		list<nekoIBlockInstance *>::iterator iter = mBlockInstanceList.begin();
		for(;iter != mBlockInstanceList.end();)
		{
			nekoIBlockInstance *inst = (*iter++);

			inst->Update(frameTime);

			if(inst->IsValid() == false)
			{
				RemoveInstance(inst);
			}
		}

		//mShotTacker.Unlock();

		mGUIManager.Update(frameTime);
		mChatBox->Update(frameTime);
		mSelectionButtons->Update(frameTime);
	}

	/// Draw
	void nekoGame::Draw()
	{
		if(mBGMusic) mBGMusic->Draw();

		list<nekoIBlockInstance *>::iterator iter = mBlockInstanceList.begin();
		for(uint32 i = 0;i < mBlockInstanceList.size();++i)
		{
			nekoIBlockInstance *inst = (*iter++);

			// 포토 모드일때..
			if(mPlayingEnv.mPhotoMode)
			{
				// 일부는 안보이게 한다..
				if(inst->GetType() == BLOCK_SYSTEMBUTTON ||
					inst->GetType() == BLOCK_TRACKBAR ||
					inst->GetType() == BLOCK_LABEL ||
					stricmp(inst->GetName(), "페이스CG") == 0)
					continue;
			}

			// 파티클은 나중에 그리자..
			if(inst->GetType() == BLOCK_PARTICLE)
				continue;

			if(inst->IsVisible())
				inst->Draw();
		}

		if(mPlayingEnv.mPhotoMode == false)
		{
			mChatBox->Draw();
			mSelectionButtons->Draw();
		}

		// 파티클만 그리자..
		iter = mBlockInstanceList.begin();
		for(uint32 i = 0;i < mBlockInstanceList.size();++i)
		{
			nekoIBlockInstance *inst = (*iter++);

			if(inst->GetType() != BLOCK_PARTICLE)
				continue;

			if(inst->IsVisible())
				inst->Draw();
		}
	}

	/// 버튼대기 수행.
	void nekoGame::EnableButtonWait()
	{
		mGameState = EGSS_BUTTON_PRESS_WAIT;
	}

	/// 끝났구려~
	void nekoGame::OnButtonPressed(bool forMsgBox)
	{
		/// 버튼 모두 제거..
		{
			list<nekoIBlockInstance *>::iterator it = mBlockInstanceList.begin();
			for(;it != mBlockInstanceList.end();)
			{
				if((*it)->GetType() == BLOCK_BUTTON)
				{
					if(forMsgBox)
					{
						if(!(static_cast<nekoButtonInstance *>(*it)->GetButtonType() == EBIT_MSGBOX_OK
							|| static_cast<nekoButtonInstance *>(*it)->GetButtonType() == EBIT_MSGBOX_CANCEL))
						{
							++it;
							continue;
						}
					}
					(*it)->Release();
					mBlockInstanceList.erase(it++);
				}
				else
					++it;
			}
		}

		if(mSkipAble)
		{
			/// 스킵 리스트에서도 삭제
			list<nekoIBlockInstance *>::iterator itSkip = mSkipAbleInstances.begin();
			for(;itSkip != mSkipAbleInstances.end();)
			{
				if((*itSkip)->GetType() == BLOCK_BUTTON)
				{
					if(forMsgBox)
					{
						if(!(static_cast<nekoButtonInstance *>(*itSkip)->GetButtonType() == EBIT_MSGBOX_OK
							|| static_cast<nekoButtonInstance *>(*itSkip)->GetButtonType() == EBIT_MSGBOX_CANCEL))
						{
							++itSkip;
							continue;
						}
					}
					(*itSkip)->Release();
					mSkipAbleInstances.erase(itSkip++);
				}
				else
					++itSkip;
			}
		}

		if(forMsgBox)
			return;

		if(mGameState != EGSS_BUTTON_PRESS_WAIT)
			return;

		mGameState = EGSS_NONE;
	}

	/// 대사 넣기
	void nekoGame::WriteText(const char8 *prevVoiceName, const char8 *txt, bool append)
	{
		nekoVariable *varName = GetVariable("회상이름");

		if(append == false)
		{
			GetChatBox()->ClearBuffer();
		}

		if(mRunType == GAME_RUN_PLAYING)
		{
			mMessageLogger.WriteText((varName) ? varName->GetValue() : 0
				, prevVoiceName, txt
				, append);
		}

		GetChatBox()->WriteText(txt);
	}

	void nekoGame::DisableSkipAble()
	{
		// 이미 끝남.
		if(!mSkipAble)
			return;

		mSkipAble = false;

		/// 스킵 리스트에서 삭제
		list<nekoIBlockInstance *>::iterator itSkip = mSkipAbleInstances.begin();
		nekoIBlockInstance *inst;
		for(;itSkip != mSkipAbleInstances.end();)
		{
			inst = (*itSkip++);

			RemoveInstance(inst);
			inst->Release();
		}

		mSkipAbleInstances.clear();

		// 스킵 내에 선택지가 있을 경우를 대비해서..
		mGameState = EGSS_NONE;
		ClearSelections();
	}

	///////////////////////////////////
	/// 미리보기를 위한 메소드!
	///////////////////////////////////

	/// 해당 위치로 이동하기
	void nekoGame::GotoLine(nekoCommands *commands, uint32 lineNumber, bool clearAll)
	{
		if(clearAll || mRunType == GAME_RUN_PREVIEW)
		{
			// 전역 변수를 제외하고 삭제..
			{
				map<string, nekoVariable>::iterator iter = mVariables.begin();
				for(;iter != mVariables.end();)
				{
					if((*iter).first[0] != '$')
						mVariables.erase(iter++);
					else
						++iter;
				}

				// 이벤트 전송
				nekoEvent e(EVENT_VARIABLE_CHANGED);
				e.value[0] = 0;
				e.value[1] = 0;
				OnEvent(e);
			}

			mStackList.clear();

			// 실행 모드에서는 환경 설정은 고유한 값이여야 해!
			if(mRunType == GAME_RUN_PREVIEW)
				mPlayingEnv.Clear();

			GetChatBox()->Clear();
			GetSelectionButtons()->Clear();

			mVoice = NULL;
			mBGMusic = NULL;
			mGameState = EGSS_NONE;
			mFlowState = EGFS_NORMAL;
			ClearSelections();

			mBGImageEffect = EBET_NONE;
			mBGImageEffectSpeed = 1.0f;
			mBGImageEffectRuleName = "";
			mBGImageEffectRule = NULL;

			mMsgBoxArgs = 0;
			mMsgBoxCaption.clear();
			mMsgBoxType = EMT_UNKNOWN;

			mNekoCore->GetVideoDriver()->SetClearColor(0xff000000);

			list<nekoIBlockInstance *>::iterator iter = mBlockInstanceList.begin();
			for(uint32 i = 0;i < mBlockInstanceList.size();++i, ++iter)
				(*iter)->Release();

			mNekoCore->GetVideoDriver()->GetCamera().Clear();
			mNekoCore->GetVideoDriver()->EnableCamera(true);
			mBlockInstanceList.clear();
			mCustomEventLtnList.clear();

			mGUIManager.SetScreen(NULL);
			mHotKeyList.clear();
			mVarTriggerList.clear();

			mMessageLogger.Clear();

			// 필요 없는 플래그 제거
			RemoveFlags(GAME_FLAG_LAST_SKIPPED);

			///////////////////////////////////
			/// 스킵관련 처리~~~~~~~~~~~~~~~~~~~~~~~~~
			///////////////////////////////////
			mSkipAble = false;

			/// 스킵 리스트에서 삭제
			list<nekoIBlockInstance *>::iterator itSkip = mSkipAbleInstances.begin();
			for(;itSkip != mSkipAbleInstances.end();++itSkip)
			{
				(*itSkip)->Release();
			}

			mSkipAbleInstances.clear();
		}

		if(commands == NULL)
			return;

		// 스택에 올리기.
		nekoThread stack(commands, true);
		mStackList.push_back(stack);

		commands->_GotoLine(&(*(--mStackList.end())), (mRunType == GAME_RUN_PREVIEW) ? true : false, lineNumber);
	}

	void nekoGame::DoFrom(int32 lineNumber)
	{
		if(!mStackList.size() || mRunType != GAME_RUN_PLAYING)
			return;

		nekoThread &stack = (*(--mStackList.end()));

		list<nekoOneCommand *> &lst = stack.mCommands->GetCommandList();
		list<nekoOneCommand *>::iterator iter = lst.begin();
		for(uint32 i = 0;i < lst.size();++i, ++iter)
		{
			if((*iter)->startLine >= lineNumber)
				break;
		}

		stack.mPlayingPos = iter;
		if(stack.mPlayingPos != stack.mCommands->GetEndingPos())
		{
			(*stack.mPlayingPos)->Begin(stack.mPlayingPos, &stack, false);
		}
	}

	/// 점핑하기!
	result nekoGame::JumpTo(const char8 *commandsName, const char8 *bookMarkName)
	{
		if(commandsName != NULL && commandsName[0] != '\0')
		{
			if(mStackList.size() >= MAX_STACK_SIZE)
			{
				nekoLogger::Log(LOG_LEVEL_WARN, "최대 스크립트 스택 수(%d)를 초과하였습니다.", MAX_STACK_SIZE);
				return E_FAIL;
			}

			nekoCommands *cmds = GetCommands(commandsName);
			if(cmds == NULL)
			{
				/// 파일로 부터 로드
				cmds = nekoNew nekoCommands(commandsName);
				if(FAILED(cmds->LoadFromFile(commandsName)))
					return E_FAIL;

				nekoThread stack(cmds, true);
				cmds->Release();
				mStackList.push_back(stack);

				mJumped = true; // 점프하면 있다가 어떤 녀석이 지울꺼야!
			}
			else
			{
				// 스택에 올리기.
				nekoThread stack(cmds, true);
				mStackList.push_back(stack);

				mJumped = true; // 점프하면 있다가 어떤 녀석이 지울꺼야!
			}
		}

		if(!mStackList.size())
			return E_FAIL;

		nekoThread &stack = (*(--mStackList.end()));
		if(bookMarkName != NULL && bookMarkName[0] != '\0')
		{
			list<nekoOneCommand *>::iterator iter;
			if(strcmp(bookMarkName, "첫줄") == 0)
				iter = stack.mCommands->GetCommandList().begin();
			else
			{
				if(FAILED(stack.mCommands->SearchBookMark(bookMarkName, iter)))
					return E_FAIL;
			}

			stack.SetPlayingPos(iter);
		}

		if(stack.mPlayingPos == stack.mCommands->GetEndingPos())
			return S_OK;

		if(stack.mCommands->GetCommandList().size() > 0)
			(*stack.mPlayingPos)->Begin(stack.mPlayingPos, &stack, false);

		return S_OK;
	}

	bool nekoGame::OnEvent(const nekoEvent &event)
	{
		// 스크린샷 키 인지 체크
		if(event.type == EVENT_WINDOW_MESSAGE)
		{
			if(event.uMsg == WM_KEYUP)
			{
				switch(event.wParam)
				{
				case VK_SNAPSHOT:
					MakeScreenshot();
					return true;
				//case VK_F5:
				//	SaveAutoToSlot();
				//	return true;
				case VK_F9:
					// 퀵 로드.
					return true;
				}
			}
		}

		if(mGUIManager.OnEvent(event))
			return true;

		if(mPlayingEnv.mPhotoMode == false || IsMsgBoxShown())
		{
			if(event.type == EVENT_WINDOW_MESSAGE)
			{
				// 픽킹 체크하기.
				if(event.uMsg >= WM_MOUSEMOVE && event.uMsg <= WM_MBUTTONDBLCLK)
				{
					nekoIBlockInstance *lastPicked = mPickedInstance;
					mPickedInstance = NULL;

					list<nekoIBlockInstance *>::reverse_iterator iter = mBlockInstanceList.rbegin();
					for(;iter != mBlockInstanceList.rend();++iter)
					{
						
						if(_IsMouseEventReceivable(*iter) &&
							(event.uMsg == WM_MOUSEMOVE && (event.wParam & MK_LBUTTON) &&  lastPicked == (*iter) // 마지막 선택시에 여기에 걸렸었다면..
							|| (*iter)->GetBoundingBox().IsPointIn(LOWORD(event.lParam), HIWORD(event.lParam))))
						{
							mPickedInstance = (*iter);

							if((*iter)->OnEvent(event))
								return true;

							continue;
						}
					}
				}
				else
				{
					list<nekoIBlockInstance *>::reverse_iterator iter = mBlockInstanceList.rbegin();
					for(;iter != mBlockInstanceList.rend();++iter)
					{
						if((*iter)->OnEvent(event))
							return true;
					}
				}
			}

			if(IsMsgBoxShown())
				return false;

			if(IsSelecting())
			{
				if(mSelectionButtons->OnEvent(event))
					return true;
			}

			if(mChatBox->OnEvent(event))
				return true;

			// 대기시에만 체크한다.
			// '선택중'에는 이벤트를 주지 않는다.
			if(mRunType == GAME_RUN_PLAYING && ((mGameState >= EGSS_WAIT && mGameState <= EGSS_BUTTON_PRESS_WAIT)))
			{
				// 핫키 체크
				if(event.type == EVENT_WINDOW_MESSAGE && 
					(event.uMsg == WM_KEYUP || event.uMsg == WM_MBUTTONUP || event.uMsg == WM_RBUTTONUP || event.uMsg == WM_MOUSEWHEEL))
				{
					list<nekoHotKey>::iterator iter = mHotKeyList.begin();
					for(;iter != mHotKeyList.end();++iter)
					{
						uchar8 keyCode = (*iter).mKeyCode;
						if((keyCode < 200 && (event.uMsg == WM_KEYUP && keyCode == event.wParam)) || (keyCode == 200 && (event.uMsg == WM_MBUTTONUP))
							|| (keyCode == 201 && event.uMsg == WM_RBUTTONUP)
							|| (keyCode == 202 && event.uMsg == WM_MOUSEWHEEL && (int32)event.wParam > 0)
							|| (keyCode == 203 && event.uMsg == WM_MOUSEWHEEL && (int32)event.wParam < 0)
							)
						{
							// 핫키 실행

							// 현재 파일 내에서 이동하는 것인가.. ?
							if(mStackList.size() && stricmp((*(--mStackList.end())).mCommands->GetName(), (*iter).mFileName.c_str()) == 0)
							{
								nekoStack &stack = (*(--mStackList.end()));

								list<nekoOneCommand *>::iterator iter2;
								if((*iter).mBookMarkName == "")
									iter2 = stack.mCommands->GetStartingPos();
								else
								{
									if(FAILED(stack.mCommands->SearchBookMark((*iter).mBookMarkName.c_str(), iter2)))
									{
										LogPrint(LOG_LEVEL_WARN, "핫키 '%d' 실행중 '%s' 파일 내의 '%s' 북마크 위치로 이동할 수 없습니다.", (*iter).mKeyCode, (*iter).mFileName.c_str(), (*iter).mBookMarkName.c_str());
										break;
									}
								}

								stack.SetPlayingPos(iter2);
							}
							else
								PlayCommands((*iter).mFileName.c_str(), (*iter).mBookMarkName.c_str());
							return true;
						}
					}
				}
			}

			// 마지막에 녀석한테 이벤트 주기.
			if(mStackList.size() > 0)
			{
				nekoThread &stack = (*(--mStackList.end()));
				if(stack.mCommands->OnEvent(&stack, event))
					return true;
			}

		}

		if(event.type == EVENT_WINDOW_MESSAGE)
		{
			if(event.uMsg == WM_KEYUP)
			{
				if(event.wParam == VK_SPACE)
				{
					mPlayingEnv.mPhotoMode = !mPlayingEnv.mPhotoMode;
				}
			}
			else if(event.uMsg == WM_LBUTTONDOWN)
			{
				if(!IsSelecting() && mFlowState == EGFS_QUICKSKIPPING)
				{
					// 통상 모드로 돌아오게
					mFlowState = EGFS_NORMAL;

					CallCustomEvent(ECET_EXIT_SKIPMODE);
					return true;
				}
			}

			// 포토모드 해제하기..
			else if(event.uMsg == WM_LBUTTONUP || event.uMsg == WM_MBUTTONUP || event.uMsg == WM_RBUTTONUP)
			{
				if(mPlayingEnv.mPhotoMode == true)
				{
					mPlayingEnv.mPhotoMode = false;
					return true;
				}
			}
		}

		return nekoIEventListener::OnEvent(event);
	}

	void nekoGame::ClearAllInstances()
	{
		mBGMusic = NULL;
		mGameState = EGSS_NONE;
		ClearSelections();

		GetChatBox()->ClearBuffer();
		GetSelectionButtons()->ClearButtons();

		/// 일반 인스턴스 삭제
		list<nekoIBlockInstance *>::iterator iter = mBlockInstanceList.begin();
		for(uint32 i = 0;i < mBlockInstanceList.size();++i, ++iter)
			(*iter)->Release();

		mBlockInstanceList.clear();

		/// 스킵가능한 인스턴스 삭제
		list<nekoIBlockInstance *>::iterator itSkip = mSkipAbleInstances.begin();
		for(;itSkip != mSkipAbleInstances.end();++itSkip)
			(*itSkip)->Release();

		mSkipAbleInstances.clear();

		mGUIManager.SetScreen(NULL);

		mNekoCore->GetVideoDriver()->GetCamera().Clear();
		mNekoCore->GetVideoDriver()->EnableCamera(true);
	}

	/// 선택지 추가
	void nekoGame::AddSelection(const string &fileName, const string &bookMarkName)
	{
		if(mRunType != GAME_RUN_PREVIEW)
		{
			nekoSelection *sel = nekoNew nekoSelection;
			if(fileName != "여기")
				sel->mScriptName = fileName;
			sel->mBookMarkName = bookMarkName;
			mSelectionList.push_back(sel);
		}
	}

	void nekoGame::OnSelectionCompleted(int32 index)
	{
		if(!mSelectionList.size() || mRunType != GAME_RUN_PLAYING)
			return;

		// 셀렉션 얻기
		list<nekoSelection *>::iterator it = mSelectionList.begin();
		int32 i = 0;
		for(;it != mSelectionList.end();++it, ++i)
		{
			if(i == index)
			{
				// 현재 최상위 스택으루..
				if((*it)->mScriptName.length() == 0)
				{
					nekoCommands *cmds = (*(--mStackList.end())).mCommands;
					list<nekoOneCommand *>::iterator foundIter;
					if(FAILED(cmds->SearchBookMark((*it)->mBookMarkName.c_str(), foundIter)))
					{
						LogPrint(LOG_LEVEL_WARN, "선택지를 끝냈는데 북마크(%s) 찾기에 실패하였어요 ㅠ.ㅠ", (*it)->mBookMarkName.c_str());
					}
					else
					{
						(*(--mStackList.end())).SetPlayingPos(foundIter);
					}
				}
				else
				{
					/// Commands 로딩해서 어쩌구 저쩌구..
					if(FAILED(JumpTo((*it)->mScriptName.c_str(), (*it)->mBookMarkName.c_str())))
					{
						LogPrint(LOG_LEVEL_WARN, "선택지를 끝냈는데 파일 %s(북마크 %s)로 이동할 수 없네요.", (*it)->mScriptName.c_str(), (*it)->mBookMarkName.c_str());
					}
				}
			}

			delete (*it);
		}
		mSelectionList.clear();
	}

	void nekoGame::ClearSelections()
	{
		list<nekoSelection *>::iterator it = mSelectionList.begin();
		for(;it != mSelectionList.end();++it)
			delete (*it);
		mSelectionList.clear();
	}


	result nekoGame::AddThread(nekoThread &thread)
	{
		if(!mStackList.size() || mRunType != GAME_RUN_PLAYING)
			return E_FAIL;

		nekoThread &stack = (*(--mStackList.end()));

		// 쓰레드의 끝 찾기
		list<nekoOneCommand *>::iterator iter = thread.GetPlayingPos();
		for(;iter != thread.mCommands->GetEndingPos();++iter)
		{
			// 찾았다!
			if((*iter)->cmdType == ECT_THREAD_DECLARE 
				&& (*iter)->arguments.size() == 0)
			{
				//thread.SetEndingPos(iter);
				stack.mPlayingPos = iter;
				thread.mRunning = false;

				mThreadList.push_back(thread);
				return S_OK;
			}
		}

		return E_FAIL;
	}

	result nekoGame::RemoveThread(const char8 *name)
	{
		if(!mStackList.size() || mRunType != GAME_RUN_PLAYING)
			return E_FAIL;

		list<nekoThread>::iterator iter = mThreadList.begin();
		int32 numThread = 0;

		for(;iter != mThreadList.end();++iter)
		{
			if(stricmp((*iter).mName.c_str(), name) == 0)
			{
				++numThread;
				(*iter).mValid = false;
			}
		}


		return (numThread > 0) ? S_OK : E_NOT_EXIST;
	}

	result nekoGame::StartThreads()
	{
		// 쓰레드의 시작하기.
		list<nekoThread>::iterator iter = mThreadList.begin();
		for(;iter != mThreadList.end();++iter)
		{
			if((*iter).GetPlayingPos() == (*iter).mCommands->GetEndingPos())
				continue;

			if((*iter).mRunning)
				continue;

			(*iter).mRunning = true;
			(*(*iter).GetPlayingPos())->Begin((*iter).GetPlayingPos(), &(*iter), false);
		}

		return S_OK;
	}

	bool nekoGame::IsExistThread(const char8 *name)
	{
		list<nekoThread>::iterator iter = mThreadList.begin();
		for(;iter != mThreadList.end();++iter)
		{
			if(stricmp((*iter).mName.c_str(), name) == 0)
				return true;
		}

		return false;
	}

	/// 이펙트 추가
	result nekoGame::SetEffectToBG(E_BGIMAGE_EFFECT_TYPE fadeType, float32 speed, bool fadeIn)
	{
		if(fadeType >= EBET_EMBEDDED_COUNT)
			return E_FAIL;

		if(!fadeIn)
		{
			list<nekoIBlockInstance *>::iterator iter = mBlockInstanceList.begin();
			for(uint32 i = 0;i < mBlockInstanceList.size();++i, ++iter)
			{
				if((*iter)->GetType() == BLOCK_BGIMAGE)
				{
					((nekoBGImageInstance *)(*iter))->SetEffect(fadeType, speed, false, NULL);
				}
			}
		}
		else
		{
			mBGImageEffect = fadeType;
			mBGImageEffectSpeed = speed;
		}

		return S_OK;
	}

	result nekoGame::SetRuleEffectToBG(const string &ruleFileName, float32 speed, bool fadeIn)
	{
		if(!fadeIn)
		{
			nekoHWTexture *tex = GetNekoNovel()->GetHWResourceManager()->LoadTexture((string("이미지\\") + ruleFileName).c_str());

			list<nekoIBlockInstance *>::iterator iter = mBlockInstanceList.begin();
			for(uint32 i = 0;i < mBlockInstanceList.size();++i, ++iter)
			{
				if((*iter)->GetType() == BLOCK_BGIMAGE)
				{
					((nekoBGImageInstance *)(*iter))->SetEffect(EBET_FROMRULE, speed, false, tex);
				}
			}

			tex->Release();
		}
		else
		{
			if(stricmp(mBGImageEffectRuleName.c_str(), ruleFileName.c_str()) != 0)
			{
				nekoHWTexture *tex = GetNekoNovel()->GetHWResourceManager()->LoadTexture((string("이미지\\") + ruleFileName).c_str());

				if(tex)
				{
					mBGImageEffectRule = tex;
					mBGImageEffectRuleName = ruleFileName;
					//mNekoCore->GetVideoDriver()->ConvertGrayscaleToAlphaMap(tex);
				}
				else
				{
					tex->Release();
				}
			}
	
			if(mBGImageEffectRule == NULL)
				return E_FAIL;

			mBGImageEffect = EBET_FROMRULE;
			mBGImageEffectSpeed = speed;
		}

		return S_OK;
	}

	bool nekoGame::HasBGEffect()
	{
		list<nekoIBlockInstance *>::iterator iter = mBlockInstanceList.begin();
		for(uint32 i = 0;i < mBlockInstanceList.size();++i, ++iter)
		{
			if((*iter)->GetType() == BLOCK_BGIMAGE)
			{
				if(((nekoBGImageInstance *)(*iter))->GetEffectType() != EBET_NONE)
					return true;
			}
		}

		return false;
	}

	/// 배포하기
	result nekoGame::StartPublishing()
	{
		mPublisher = nekoNew nekoPublisher;
		mPublisher->Start();
		return S_OK;
	}

	void nekoGame::StopPublishing()
	{
		// 요청한다.
		mPublisher->Stop(false);
	}

	void nekoGame::LoadSaveDatas()
	{
		WIN32_FIND_DATA foundData;

		HANDLE hFile = FindFirstFile(nekoUtility::FormString("%s세이브\\세이브*.nkslot", GetNekoNovel()->GetAppFolderPath()), &foundData);
		if(hFile == NULL)
			return;

		do
		{
			if(foundData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				continue;

			// 01부터 MAX_SAVE_COUNT까지만 로드

			char8 *num = foundData.cFileName + 6;
			if(!(num[0] >= '0' && num[0] <= '9') || !(num[1] >= '0' && num[1] <= '9'))
				continue;

			int32 no = atoi(num);
			if(!(no >= 0 && no < MAX_SAVE_COUNT))
				continue;

			mSaveDatas[no] = nekoNew nekoSaveData();
			if(FAILED(mSaveDatas[no]->Initialize(nekoUtility::FormString("세이브\\%s", foundData.cFileName))))
			{
				mSaveDatas[no] = NULL;
				continue;
			}

		} while (FindNextFile(hFile, &foundData));

		// 퀵세이브 데이터 불러오기
		mQuickSave = nekoNew nekoSaveData();
		if(FAILED(mQuickSave->Initialize("세이브\\퀵.nkslot")))
		{
			mQuickSave = NULL;
		}
	}

	nekoSaveData *nekoGame::GetSaveData(int32 slotNo)
	{
		if(!(slotNo >= 0 && slotNo < MAX_SAVE_COUNT))
		{
			if(slotNo == QUICK_SAVE_IDX)
				return mQuickSave;
			return NULL;
		}

		return mSaveDatas[slotNo];
	}

	/// 덤프 시키기
	void nekoGame::DumpTo(nekoByteBuffer &buf)
	{
		int32 version = SAVEDATA_VERSION;

		/// 환경설정, 게임 상태
		buf << mGameState << mPlayingEnv.mSpeechSpeed << mPlayingEnv.mSpeechColor << mPlayingEnv.mSelectionColor
			<< mPlayingEnv.mSpeechShadowColor << mPlayingEnv.mSpeechShadowType << mPlayingEnv.mSystemMenuEnabled
			<< mPlayingEnv.mPrintX << mPlayingEnv.mPrintY << mSkipAble;

		/// 배경 이펙트 타입, 속도
		buf << mBGImageEffect << mBGImageEffectSpeed
			<< mBGImageEffectRuleName;

		/// CG 초기 페이드인/아웃 속도/등
		buf << mPlayingEnv.mCGFadeInSpeed
			<< mPlayingEnv.mCGFadeOutSpeed
			<< mPlayingEnv.mSmoothPrint
			<< mPlayingEnv.mGamePlayTime; // 1129

		// 배경색
		buf << mNekoCore->GetVideoDriver()->GetClearColor();

		// 카메라 무시여부(1109)
		buf << mPlayingEnv.mCameraIgnored;

		// 무대기 무시(1126)
		buf << mPlayingEnv.mIW2Use;

		/// 선택지 목록 저장하기
		buf << (uint32)mSelectionList.size();
		list<nekoSelection *>::iterator itSel = mSelectionList.begin();
		for(;itSel != mSelectionList.end();++itSel)
		{
			buf << (*itSel)->mScriptName;
			buf << (*itSel)->mBookMarkName;
		}

		/// 점프 됬는가 ?
		buf << mJumped;


		/// 배경음악, 인스턴스 리스트 2개
		if(mBGMusic != NULL)
		{
			buf << (bool)true;
			mBGMusic->Dump(version, buf);
		}
		else
		{
			buf << (bool)false;
		}

		buf << (uint32)mBlockInstanceList.size();
		list<nekoIBlockInstance *>::iterator itBlockInst = mBlockInstanceList.begin();

		for(;itBlockInst != mBlockInstanceList.end();++itBlockInst)
		{
			nekoIBlockInstance *inst = (*itBlockInst);
			nekoByteBuffer miniBuf;

			// 미니 덤프에 넣은 후, 옮기기.
			(*itBlockInst)->Dump(version, miniBuf);

			buf << miniBuf.GetLength();
			buf.Append(miniBuf.GetBuffer(), miniBuf.GetLength());

			if(mSkipAble)
			{
				bool found = false;
				list<nekoIBlockInstance *>::iterator itSBlockInst = mSkipAbleInstances.begin();
				for(;itSBlockInst != mSkipAbleInstances.end();++itSBlockInst)
				{
					if((*itSBlockInst) == (*itBlockInst))
					{
						found = true;
						break;
					}
				}

				buf << (bool)found;
			}
			else
				buf << (bool)false;
		}

		/// 스택 저장하기
		buf << (uint32)mStackList.size();
		list<nekoThread>::iterator itStack = mStackList.begin();
		for(;itStack != mStackList.end();++itStack)
		{
			buf << (*itStack).mCommands->GetName();
			if((*itStack).mPlayingPos != (*itStack).mCommands->GetEndingPos())
			{
				buf << (*(*itStack).mPlayingPos)->startLine
					<< (*(*itStack).mPlayingPos)->cmdType;
			}
			else
			{
				buf << (int32)0
					<< (E_COMMAND_TYPE)0;
			}

			for(uint32 i = 0;i < nekoThread::TempValueCount;++i)
			{
				buf << (*itStack)._mTempFloat[i];
				buf << (*itStack)._mTempInteger[i];
				buf << (*itStack)._mTempTimer[i];
			}
		}


		/// 쓰레드 저장하기
		buf << (uint32)mThreadList.size();
		list<nekoThread>::iterator itThread = mThreadList.begin();
		for(;itThread != mThreadList.end();++itThread)
		{
			buf << (*itThread).mCommands->GetName();
			if((*itThread).mPlayingPos != (*itThread).mCommands->GetEndingPos())
			{
				buf << (*(*itThread).mPlayingPos)->startLine
					<< (*(*itThread).mPlayingPos)->cmdType;
			}
			else
			{
				buf << (int32)0
					<< (E_COMMAND_TYPE)0;
			}
			buf << (*itThread).mRunning;

			for(uint32 i = 0;i < nekoThread::TempValueCount;++i)
			{
				buf << (*itThread)._mTempFloat[i];
				buf << (*itThread)._mTempInteger[i];
				buf << (*itThread)._mTempTimer[i];
			}
		}

		/// 핫키 데이터
		buf << (uint32)mHotKeyList.size();
		list<nekoHotKey>::iterator itHotKey = mHotKeyList.begin();
		for(;itHotKey != mHotKeyList.end();++itHotKey)
		{
			buf << (*itHotKey).mKeyCode;
			buf << (*itHotKey).mFileName;
			buf << (*itHotKey).mBookMarkName;
		}

		/// 변수트리거 데이터(1109)
		buf << (uint32)mVarTriggerList.size();
		list<nekoVariableTrigger>::iterator itVT = mVarTriggerList.begin();
		for(;itVT != mVarTriggerList.end();++itVT)
		{
			buf << (*itVT).mVarName;
			buf << (*itVT).mFileName;
			buf << (*itVT).mBookMarkName;
		}

		/// 회상 저장하기
		mChatBox->Dump(version, buf);

		/// 커스텀 이벤트 저장
		buf << (uint32)mCustomEventLtnList.size();
		map<E_CUSTOM_EVENT_TYPE, nekoCustomEventListener>::iterator itCEvent = mCustomEventLtnList.begin();
		for(;itCEvent != mCustomEventLtnList.end();++itCEvent)
		{
			buf << (*itCEvent).first
			<< (*itCEvent).second.mName
			<< (*itCEvent).second.mBookMarkName;
		}

		const nekoCamera &camera = GetNekoNovel()->GetVideoDriver()->GetCamera();
		buf << camera.mCenterPos
			<< camera.mRotation
			<< camera.mScaleX << camera.mScaleY;

		/// 세이브용 변수 저장
		int32 temp = 0;
		map<string, nekoVariable>::iterator itVar = mVariables.begin();
		for(uint32 i = 0;i < mVariables.size();++i, ++itVar)
		{
			if((*itVar).first[0] != '$' && (*itVar).first[0] != '%')
			{
				++temp;

			}
		}

		buf << temp;

		itVar = mVariables.begin();
		for(uint32 i = 0;i < mVariables.size();++i, ++itVar)
		{
			if((*itVar).first[0] != '$' && (*itVar).first[0] != '%')
			{
				buf << ((*itVar).first.c_str())
					<< ((*itVar).second.GetValue())
					<< ((*itVar).second.GetType());
			}
		}

		/// 선택지 버튼들 저장
		mSelectionButtons->Dump(version, buf);

		mMessageLogger.Dump(version, buf);

		buf.Compress(9);
	}

	// 가져오기
	result nekoGame::LoadFrom(int32 version, nekoByteBuffer buf)
	{
		int32 temp;
		uint32 count;

		// 클리어
		GotoLine(NULL, 0, true);
		
		if(version >= 1124)
			buf.Uncompress();

		uint32 bgColor;
		buf 
		 >> mGameState >> mPlayingEnv.mSpeechSpeed
		 >> mPlayingEnv.mSpeechColor >> mPlayingEnv.mSelectionColor
		 >> mPlayingEnv.mSpeechShadowColor >> mPlayingEnv.mSpeechShadowType
		 >> mPlayingEnv.mSystemMenuEnabled >> mPlayingEnv.mPrintX >> mPlayingEnv.mPrintY >> mSkipAble;

		// E_SCREEN_TYPE
		if(version < 1119)
			buf >> temp;

		buf >> mBGImageEffect >> mBGImageEffectSpeed;
		
		if(version >= 1104)
		{
			buf >> mBGImageEffectRuleName;
			if(mBGImageEffectRuleName.length())
			{
				mBGImageEffectRule = 
					GetNekoNovel()->GetHWResourceManager()->LoadTexture((string("이미지\\") + mBGImageEffectRuleName).c_str());
			}
		}

		if(version >= 1111)
		{
			buf >> mPlayingEnv.mCGFadeInSpeed;
		}

		if(version >= 1112)
		{
			buf >> mPlayingEnv.mCGFadeOutSpeed;
		}

		if(version >= 1114)
		{
			buf >> mPlayingEnv.mSmoothPrint;
		}

		if(version >= 1129)
		{
			buf >> mPlayingEnv.mGamePlayTime;
		}
		
		buf >> bgColor;

		mNekoCore->GetVideoDriver()->SetClearColor(bgColor);

		// 카메라 무시 여부(1109)
		if(version >= 1109)
			buf >> mPlayingEnv.mCameraIgnored;

		// 무대기 무시(1126)
		if(version >= 1126)
			buf >> mPlayingEnv.mIW2Use;

		buf >> count;
		nekoSelection *sel;

		for(uint32 i = 0;i < count;++i)
		{
			sel = nekoNew nekoSelection;
			buf >> sel->mScriptName >> sel->mBookMarkName;
			mSelectionList.push_back(sel);
		}

		buf >> mJumped;
		
		string stackName;
		int32 startLine;
		E_BLOCK_TYPE btype;

		bool btemp;
		buf >> btemp;
		if(btemp)
		{
			mBGMusic = NULL;
			CreateInstance(BLOCK_BGMUSIC, "");

			buf >> btype;
			mBGMusic->Load(version, buf);
		}

		buf >> count;
		nekoIBlockInstance *inst;
		int32 miniBufLen;

		for(uint32 i = 0;i < count;++i)
		{
			if(version >= 1118)
			{
				nekoByteBuffer miniBuf;
				buf >> miniBufLen;
				miniBuf.Append(buf.GetReadPositionBuffer(), miniBufLen);
				buf.Read(0, miniBufLen);

				miniBuf >> btype;
				inst = CreateInstance(btype, "test");

				if(inst)
					inst->Load(version, miniBuf);
			}
			else
			{

				buf >> btype;
				inst = CreateInstance(btype, "test");
				inst->Load(version, buf);
			}

			// 스킵 가능한 객체인지 체크
			buf >> btemp;

			if(inst)
			{
				// 이름에 '%' 가 들어간 녀석은 부르지 않는다.
				if(inst->GetName()[0] == '%')
				{
					RemoveInstance(inst);
					continue;
				}

				if(btemp)
				{
					inst->AddRef();
					mSkipAbleInstances.push_back(inst);
				}
			}
		}

		buf >> count;
		E_COMMAND_TYPE cmdType;
		for(uint32 i = 0;i < count;++i)
		{
			buf >> stackName >> startLine;

			if(version >= 1105)
			{
				buf >> cmdType;
			}

			nekoCommands *cmds = GetCommands(stackName.c_str());
			if(cmds == NULL)
			{
				/// 파일로 부터 로드
				cmds = nekoNew nekoCommands(stackName.c_str());
				if(FAILED(cmds->LoadFromFile(stackName.c_str())))
				{
					cmds->Release();
					nekoLogger::Log(LOG_LEVEL_WARN, "로딩 중에 스택 '%s'(%d)를 불러오지 못했습니다.", stackName.c_str(), startLine);
					return E_FAIL;
				}

				nekoThread stack(cmds, true);
				cmds->Release();
				mStackList.push_back(stack);
			}
			else
			{
				nekoThread stack(cmds, true);
				mStackList.push_back(stack);
			}

			nekoThread &stack = (*(--mStackList.end()));

			for(uint32 a = 0;a < nekoThread::TempValueCount;++a)
			{
				buf >> stack._mTempFloat[a]
				>> stack._mTempInteger[a]
				>> stack._mTempTimer[a];
			}

			if(startLine)
			{
				stack.mPlayingPos = stack.mCommands->GetPosByLine(startLine, true);
				if(stack.mPlayingPos != stack.mCommands->GetEndingPos())
				{
					if(version >= 1105)
					{
						// 명령어가 올바른지 비교
						if(cmdType == (*stack.mPlayingPos)->cmdType)
						{
							(*stack.mPlayingPos)->OnGameLoaded(&stack);
						}
						else
						{
							/// 에이씨.. 명령어 틀리잖아!
							/// 명령어 초기화해 그러면!
							(*stack.mPlayingPos)->Begin(stack.mPlayingPos, &stack, false);
						}
					}
					else
					{
						(*stack.mPlayingPos)->OnGameLoaded(&stack);
					}
				}
			}
			else
				stack.mPlayingPos = stack.mCommands->GetEndingPos();
		}

		buf >> count;
		for(uint32 i = 0;i < count;++i)
		{
			buf >> stackName
				>> startLine
				>> cmdType;

			nekoCommands *cmds = GetCommands(stackName.c_str());
			if(cmds == NULL)
			{
				/// 파일로 부터 로드
				cmds = nekoNew nekoCommands(stackName.c_str());
				if(FAILED(cmds->LoadFromFile(stackName.c_str())))
				{
					cmds->Release();
					nekoLogger::Log(LOG_LEVEL_WARN, "로딩 중에 쓰레드 '%s'(%d)를 불러오지 못했습니다.", stackName.c_str(), startLine);
					return E_FAIL;
				}

				nekoThread stack(cmds, true);
				cmds->Release();
				mThreadList.push_back(stack);
			}
			else
			{
				nekoThread stack(cmds, true);
				mThreadList.push_back(stack);
			}

			nekoThread &th = (*(--mThreadList.end()));

			buf >> th.mRunning;

			for(uint32 a = 0;a < nekoThread::TempValueCount;++a)
			{
				buf >> th._mTempFloat[a]
				>> th._mTempInteger[a]
				>> th._mTempTimer[a];
			}

			if(startLine)
			{
				th.mPlayingPos = th.mCommands->GetPosByLine(startLine);
				if(th.mPlayingPos != th.mCommands->GetEndingPos())
				{
					if(version >= 1105)
					{
						// 명령어가 올바른지 비교
						if(cmdType == (*th.mPlayingPos)->cmdType)
						{
							(*th.mPlayingPos)->OnGameLoaded(&th);
						}
						else
						{
							/// 에이씨.. 명령어 틀리잖아!
							/// 명령어 초기화해 그러면!
							(*th.mPlayingPos)->Begin(th.mPlayingPos, &th, false);
						}
					}
					else
					{
						(*th.mPlayingPos)->OnGameLoaded(&th);
					}
				}
			}
			else
				th.mPlayingPos = th.mCommands->GetEndingPos();

			mThreadList.push_back(th);
		}

		/// 핫키 데이터 얻어오기
		buf >> count;

		char8 keyCode;
		string fileName;
		string bookMarkName;
		for(uint32 i = 0;i < count;++i)
		{
			buf >> keyCode >> fileName >> bookMarkName;

			mHotKeyList.push_back(nekoHotKey(keyCode, fileName, bookMarkName));
		}

		/// 변수트리거 데이터 얻어오기
		buf >> count;

		string varName;
		for(uint32 i = 0;i < count;++i)
		{
			buf >> varName >> fileName >> bookMarkName;

			mVarTriggerList.push_back(nekoVariableTrigger(varName, fileName, bookMarkName));
		}

		mChatBox->Load(version, buf);

		/// 커스텀 이벤트 불러오기
		buf >> count;
		E_CUSTOM_EVENT_TYPE cevtType;
		for(uint32 i = 0;i < count;++i)
		{
			nekoCustomEventListener ltn;
			buf >> cevtType
				>> ltn.mName
				>> ltn.mBookMarkName;

			mCustomEventLtnList[cevtType] = ltn;
		}

		if(version >= 1102)
		{
			nekoCamera &camera = GetNekoNovel()->GetVideoDriver()->GetCamera();
			buf >> camera.mCenterPos
				>> camera.mRotation
				>> camera.mScaleX >> camera.mScaleY;
			camera.Update();
		}

		string name;
		nekoVariable var;

		if(version >= 1103)
		{
			buf >> count;

			for(uint32 i = 0;i < count;++i)
			{
				buf >> name;

				(mVariables[name] = nekoVariable()).Load(version, buf);
			}
		}

		if(version >= 1106)
		{
			/// 선택지 버튼들 불러오기
			mSelectionButtons->Load(version, buf);
		}

		if(version >= 1120)
		{
			mMessageLogger.Load(version, buf);
		}

		return S_OK;
	}

	void nekoGame::SetHotKey(uchar8 keyCode, const string &fileName, const string &bookMarkName)
	{
		if(!mStackList.size())
			return;

		// 목록에서 찾기
		list<nekoHotKey>::iterator iter = mHotKeyList.begin();
		for(;iter != mHotKeyList.end();++iter)
		{
			if((*iter).mKeyCode == keyCode )
			{
				if(fileName.empty() && bookMarkName.empty())
				{
					mHotKeyList.erase(iter);
					return;
				}

				(*iter).mFileName = (fileName == "여기") ? (*(--mStackList.end())).mCommands->GetName() : fileName;
				(*iter).mBookMarkName = (bookMarkName == "첫줄") ? "" : bookMarkName;
				return;
			}
		}

		if(fileName.empty() && bookMarkName.empty())
		{
			return;
		}

		nekoHotKey hk(keyCode, 
			(fileName == "여기") ? (*(--mStackList.end())).mCommands->GetName() : fileName,
			(bookMarkName == "첫줄") ? "" : bookMarkName);
		mHotKeyList.push_back(hk);
	}

	void nekoGame::SetVariableTrigger(const string &varName, const string &fileName, const string &bookMarkName)
	{
		if(!mStackList.size())
			return;

		// 목록에서 찾기
		list<nekoVariableTrigger>::iterator iter = mVarTriggerList.begin();
		for(;iter != mVarTriggerList.end();++iter)
		{
			if(stricmp((*iter).mVarName.c_str(), varName.c_str()) == 0)
			{
				if(fileName.empty() && bookMarkName.empty())
				{
					mVarTriggerList.erase(iter);
					return;
				}

				(*iter).mFileName = (fileName == "여기") ? (*(--mStackList.end())).mCommands->GetName() : fileName;
				(*iter).mBookMarkName = (bookMarkName == "첫줄") ? "" : bookMarkName;
				return;
			}
		}

		if(fileName.empty() && bookMarkName.empty())
			return;

		nekoVariableTrigger vt(varName, 
			(fileName == "여기") ? (*(--mStackList.end())).mCommands->GetName() : fileName,
			(bookMarkName == "첫줄") ? "" : bookMarkName);
		mVarTriggerList.push_back(vt);
	}

	void nekoGame::SetCustomEventListener(E_CUSTOM_EVENT_TYPE type, const string &fileName, const string &bookMarkName)
	{
		nekoCustomEventListener ltn;
		ltn.mName = (fileName != "여기") ? fileName : (*(--mStackList.end())).mCommands->GetName();
		ltn.mBookMarkName = (bookMarkName != "처음") ? bookMarkName : "";

		mCustomEventLtnList[type] = ltn;
	}

	bool nekoGame::RemoveCustomEventListener(E_CUSTOM_EVENT_TYPE type)
	{
		map<E_CUSTOM_EVENT_TYPE, nekoCustomEventListener>::iterator iter = mCustomEventLtnList.find(type);
		if(mCustomEventLtnList.end() == iter)
			return false;

		mCustomEventLtnList.erase(iter);
		return true;
	}

	result nekoGame::CallCustomEvent(E_CUSTOM_EVENT_TYPE type)
	{
		map<E_CUSTOM_EVENT_TYPE, nekoCustomEventListener>::iterator iter = mCustomEventLtnList.find(type);
		if(mCustomEventLtnList.end() == iter)
			return E_NOT_EXIST;

		if(mStackList.size() > 0 && stricmp((*iter).second.mName.c_str(), (*(--mStackList.end())).mCommands->GetName()) == 0)
			JumpTo(NULL, (*iter).second.mBookMarkName.c_str());
		else
			PlayCommands((*iter).second.mName.c_str(), (*iter).second.mBookMarkName.c_str());

		return S_OK;
	}

	void nekoGame::OnVariableChanged(const string &varName, nekoVariable *variable)
	{
		if(IsTurnOn(GAME_FLAG_VARTRACE))
		{
			// 이벤트 전송
			nekoEvent e(EVENT_VARIABLE_CHANGED);
			e.value[0] = (int32)varName.c_str();
			e.value[1] = (int32)variable;
			OnEvent(e);
		}

		// 변수 트리거 에서 검색...
		list<nekoVariableTrigger>::iterator iter = mVarTriggerList.begin();
		for(;iter != mVarTriggerList.end();++iter)
		{
			if(stricmp(varName.c_str(), (*iter).mVarName.c_str()) == 0)
			{
				// 트리거 실행

				// 현재 파일 내에서 이동하는 것인가.. ?
				if(mStackList.size() && stricmp((*(--mStackList.end())).mCommands->GetName(), (*iter).mFileName.c_str()) == 0)
				{
					nekoStack &stack = (*(--mStackList.end()));

					list<nekoOneCommand *>::iterator iter2;
					if((*iter).mBookMarkName == "")
						iter2 = stack.mCommands->GetStartingPos();
					else
					{
						if(FAILED(stack.mCommands->SearchBookMark((*iter).mBookMarkName.c_str(), iter2)))
						{
							LogPrint(LOG_LEVEL_WARN, "변수트리거 '%S' 실행중 '%s' 파일 내의 '%s' 북마크 위치로 이동할 수 없습니다.", varName.c_str(),
								(*iter).mFileName.c_str(), (*iter).mBookMarkName.c_str());
							break;
						}
					}

					stack.SetPlayingPos(iter2);
				}
				else
					PlayCommands((*iter).mFileName.c_str(), (*iter).mBookMarkName.c_str());

				return;
			}
		}
	}

	void nekoGame::OnMsgBoxCompleted(bool answer)
	{
		if(mMsgBoxType == EMT_UNKNOWN)
			return;

		if(!answer)
		{
			mMsgBoxArgs = 0;
			mMsgBoxType = EMT_UNKNOWN;
			return;
		}

		switch(mMsgBoxType)
		{
		case EMT_LOAD:
			this->LoadFromSlot(mMsgBoxArgs);
			break;
		case EMT_OVERSAVE:
			this->SaveToSlot(mMsgBoxArgs);
			break;
		case EMT_REMOVE:
			this->RemoveSlot(mMsgBoxArgs);
			break;
		case EMT_QUICKLOAD:
			this->LoadFromSlot(mMsgBoxArgs);
			break;
		case EMT_QUICKSAVE:
			this->SaveToSlot(mMsgBoxArgs);
			break;
		}

		mMsgBoxArgs = 1;
		mMsgBoxType = EMT_UNKNOWN;
	}

	LPD3DXBUFFER nekoGame::TakeSaveScreenshot()
	{
		nekoVideoDriver *videoDrv =  GetNekoNovel()->GetVideoDriver();
		if(FAILED(videoDrv->BeginTargetRender()))
			return NULL;

		list<nekoIBlockInstance *>::iterator iter = mBlockInstanceList.begin();
		for(uint32 i = 0;i < mBlockInstanceList.size();++i, ++iter)
		{
			if((*iter)->GetType() > BLOCK_BGIMAGE)
				continue;

			if((*iter)->IsVisible() && (*iter)->IsIncludedInSnapShot())
				(*iter)->Draw();
		}

		if(mPlayingEnv.mProjectFlags & EPFT_CHATBOX_IN_THUMBNAIL)
		{
			mChatBox->Draw();
		}

		videoDrv->EndTargetRender();

		LPD3DXBUFFER buf;
		if(FAILED(D3DXSaveSurfaceToFileInMemory(&buf, D3DXIFF_JPG, videoDrv->GetTempTargetSurface(), NULL, NULL)))
			return NULL;

		return buf;
	}

	// 재생
	void nekoGame::PlayVoice(const char8 *name)
	{
		nekoVoiceInstance *voiceInst = (nekoVoiceInstance *)CreateInstance(BLOCK_VOICE, "");
		voiceInst->SetMusic(name, false);
	}

	/// 오마케 추가
	void nekoGame::AddOmake(const string &name, const string &fileName)
	{
		nekoOmake omake;
		omake.mName = name;
		omake.mFileName = fileName;

		mOmakeList.push_back(omake);
	}

	bool nekoGame::RemoveOmake(const string &name)
	{
		list<nekoOmake>::iterator iter = mOmakeList.begin();
		for(;iter != mOmakeList.end();++iter)
		{
			if(stricmp((*iter).mName.c_str(), name.c_str()) == 0)
			{
				mOmakeList.erase(iter);
				return true;
			}
		}

		return false;
	}

	const char8 *nekoGame::GetOmake(const string &name)
	{
		list<nekoOmake>::iterator iter = mOmakeList.begin();
		for(;iter != mOmakeList.end();++iter)
		{
			if(stricmp((*iter).mName.c_str(), name.c_str()) == 0)
				return (*iter).mFileName.c_str();
		}

		return NULL;
	}

	/////////////////////////////////////
	/// 환경 관련 메소드
	/////////////////////////////////////
	result nekoGame::SaveEnvironment()
	{
		// 파일 체크
		string fullPath = GetNekoNovel()->GetAppFolderPath();
		FILE *fp = fopen((fullPath + ENVDATA_FILENAME).c_str(), "wb");
		if(fp == NULL)
		{
			LogPrint(LOG_LEVEL_WARN, "환경 파일을 저장할 수 없습니다.");
			return E_FAIL;
		}

		nekoUtility::WriteString(fp, ENVDATA_CODENAME);
		int32 temp;

		mPlayingEnv.mDefFullScreen = mNekoCore->GetVideoDriver()->IsFullScreen();

		fwrite(&(temp = ENGINE_VERSION), 1, sizeof(int32), fp);
		fwrite(&mPlayingEnv.mDefFullScreen, 1, sizeof(bool), fp);
		fwrite(&mPlayingEnv.mDefFXVolume, 1, sizeof(int32), fp);
		fwrite(&mPlayingEnv.mDefBGMVolume, 1, sizeof(int32), fp);
		fwrite(&mPlayingEnv.mDefVoiceVolume, 1, sizeof(int32), fp);
		fwrite(&mPlayingEnv.mTextWindowAlpha, 1, sizeof(int32), fp);
		fwrite(&mPlayingEnv.mPrintDelay, 1, sizeof(bool), fp);
		fwrite(&mPlayingEnv.mTextSubSpeed, 1, sizeof(float32), fp);

		// 환경 변수들 저장
		fwrite(&(temp = mOmakeList.size()), 1, sizeof(uint32), fp);
		list<nekoOmake>::iterator itOmake = mOmakeList.begin();
		for(uint32 i = 0;i < mOmakeList.size();++i, ++itOmake)
		{
			nekoUtility::WriteString(fp, (*itOmake).mName.c_str());
			nekoUtility::WriteString(fp, (*itOmake).mFileName.c_str());
		}

		/// 세이브용 변수 저장(version < 121)

		/*
		temp = 0;
		for(uint32 i = 0;i < mVariables.size();++i, ++itVar)
		{
			if((*itVar).first[0] == '$')
			{
				++temp;
			}
		}
		fwrite(&temp, 1, sizeof(uint32), fp);

		itVar = mVariables.begin();
		for(uint32 i = 0;i < mVariables.size();++i, ++itVar)
		{
			if((*itVar).first[0] == '$')
			{
				nekoUtility::WriteString(fp, (*itVar).first.c_str());
				nekoUtility::WriteString(fp, (*itVar).second.GetValue());
			}
		}

		*/

		nekoByteBuffer buf;
		map<string, nekoVariable>::iterator itVar = mVariables.begin();
		for(uint32 i = 0;i < mVariables.size();++i, ++itVar)
		{
			if((*itVar).first[0] == '$')
			{
				buf << (*itVar).first
					<< (*itVar).second.GetValue()
					<< (*itVar).second.GetType();
			}
		}

		buf.Compress(9);

		fwrite(&(temp = buf.GetLength()), 1, sizeof(int32), fp);
		fwrite(buf.GetBuffer(), 1, buf.GetLength(), fp);

		// 게임 실행횟수
		fwrite(&mPlayingEnv.mGameRuntimes, 1, sizeof(uint32), fp);
		
		fclose(fp);
		return S_OK;
	}

	result nekoGame::LoadEnvironment()
	{
		// 파일 체크
		string fullPath = GetNekoNovel()->GetAppFolderPath();
		FILE *fp = fopen((fullPath + ENVDATA_FILENAME).c_str(), "rb");
		if(fp == NULL)
		{
			// 풀스크린은 적용시켜주기.
			if(mPlayingEnv.mDefFullScreen)
				mNekoCore->OnWindowChanged(-1, -1,  true);

			return E_NOT_EXIST;
		}

		// 파일 체크
		const string &str = nekoUtility::ReadString(fp, strlen(ENVDATA_CODENAME));
		if(str != ENVDATA_CODENAME)
		{
			fclose(fp);
			LogPrint(LOG_LEVEL_WARN, "환경 파일이 손상되었습니다.");
			return E_FAIL;
		}

		int32 temp;
		int32 version;
		fread(&version, 1, sizeof(int32), fp);
		fread(&mPlayingEnv.mDefFullScreen, 1, sizeof(bool), fp);
		fread(&mPlayingEnv.mDefFXVolume, 1, sizeof(int32), fp);
		fread(&mPlayingEnv.mDefBGMVolume, 1, sizeof(int32), fp);
		fread(&mPlayingEnv.mDefVoiceVolume, 1, sizeof(int32), fp);
		fread(&mPlayingEnv.mTextWindowAlpha, 1, sizeof(int32), fp);
		fread(&mPlayingEnv.mPrintDelay, 1, sizeof(bool), fp);

		if(version >= 118)
			fread(&mPlayingEnv.mTextSubSpeed, 1, sizeof(float32), fp);

		if(mPlayingEnv.mDefFullScreen)
			mNekoCore->OnWindowChanged(-1, -1,  true);

		// 환경 변수들..
		fread(&temp, 1, sizeof(uint32), fp);
		nekoOmake omake;
		for(uint32 i = 0;i < temp;++i)
		{
			omake.mName = nekoUtility::ReadString(fp);
			omake.mFileName = nekoUtility::ReadString(fp);

			mOmakeList.push_back(omake);
		}

		if(version >= 107)
		{
			fread(&temp, 1, sizeof(uint32), fp);

			if(version >= 121)
			{
				nekoByteBuffer buf(temp);
				fread(buf.GetBuffer(), 1, temp, fp);
				buf.Uncompress();

				string name;
				string value;

				while(buf.IsReadEndPosition() == false)
				{
					buf >> name;

					(mVariables[name] = nekoVariable()).Load(version, buf);

				}
			}
			else
			{
				string name;
				for(uint32 i = 0;i < temp;++i)
				{
					name = nekoUtility::ReadString(fp);

					(mVariables[name] = nekoVariable()).SetValue(nekoUtility::ReadString(fp).c_str());
				}
			}
		}

		// 게임 실행 횟수
		if(version >= 116)
		{
			fread(&mPlayingEnv.mGameRuntimes, 1, sizeof(uint32), fp);
		}

		fclose(fp);
		return S_OK;
	}

	void nekoGame::MakeScreenshot()
	{
		D3DDISPLAYMODE d3ddm;
		IDirect3DDevice9 *pDevice = mNekoCore->GetVideoDriver()->GetHandle();

		// Get display's mode.
		if(FAILED(pDevice->GetDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
			return;

		LPDIRECT3DSURFACE9 surf;
		if(FAILED(pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &surf)))
		{
			return;
		}

		struct tm *t;
		time_t timer;
		char8 fileName[100];
		timer = time(0);
		t = localtime(&timer);

		sprintf(fileName, "스크린샷\\nk_%02d%02d_%02d%02d%02d.jpg", t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_mon, t->tm_sec);

		// Create a folder
		CreateDirectory("스크린샷", 0);

		// Save to file.
		D3DXSaveSurfaceToFile(fileName, D3DXIFF_JPG, surf, 0, 0);
		surf->Release();
	}

	/// 텍스트를 파싱한다.(변수가 있으면 치환해줌..)
	const string &nekoGame::ParseString(const char8 *str)
	{
		static string out;
		string varName;

		out.clear();
		out.reserve(strlen(str));

		bool varFlag = false;
		while(*str)
		{
			if(*str == '{' && *(str + 1) == '{')
			{
				varFlag = true;
				++str;
			}
			else
			{
				if(varFlag)
				{
					if(*str == '}' && *(str + 1) == '}')
					{
						varFlag = false;
						++str;

						nekoVariable *var = GetVariable(varName.c_str());
						varName.clear();
						if(var == NULL)
						{
							out += "<변수 없음>";
						}
						else
						{
							out += var->GetValue();
						}
					}
					else
					{
						varName.push_back(*str);
					}
				}
				else
				{
					out.push_back(*str);
				}
			}

			++str;
		}

		return out;
	}

	void nekoGame::OnRunCompleted()
	{
		mPlayingEnv.mGameRuntimes++;
	}
}