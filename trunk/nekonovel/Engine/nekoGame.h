// ===============================================================
//  nekoGame - 2009.7.6
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoGame_h__
#define __nekoGame_h__

#include <map>
using namespace std;

#include "nekoCommands.h"
#include "nekoIBlockInstance.h"
#include "nekoVariable.h"
#include "nekoGUIManager.h"
#include "nekoBGImage.h"
#include "nekoBGMusic.h"
#include "nekoVoice.h"
#include "nekoGUISelectionButtons.h"
#include "nekoGUIChatBox.h"
#include "nekoMessageLogger.h"
#include "nekoPublisher.h"
#include "nekoLockSection.h"
#include "nekoButton.h"

namespace neko
{
#	define MAX_SAVE_COUNT 400 // 20 * 20
#	define ONE_PAGE_SAVECOUNT 20
#	define QUICK_SAVE_IDX -1

	struct nekoStack
	{
		nekoAutoPtr<nekoCommands> mCommands;
		list<nekoOneCommand *>::iterator mPlayingPos;

		inline void SetPlayingPos(list<nekoOneCommand *>::iterator iter) { mPlayingPos = iter; }
		inline list<nekoOneCommand *>::iterator &GetPlayingPos() { return mPlayingPos; }

		nekoStack(nekoCommands *cmds) : mCommands(cmds) { cmds->AddRef(); mPlayingPos = mCommands->GetCommandList().begin(); }
		~nekoStack() {}
	};

	struct nekoThread : public nekoStack
	{
		string mName;
		bool mRunning; // 스텍은 영향 받지 않음.
		bool mIsStack; // 스택인가 쓰레드인가!
		bool mValid;

		enum
		{
			TempValueCount = 8,
		};

		/// Command 처리를 위한 임시 변수들(멀티 쓰레드에 안전하지 않음.)
		nekoTimer _mTempTimer[TempValueCount];
		int32 _mTempInteger[TempValueCount];
		float64 _mTempFloat[TempValueCount];
		string _mTempString; // 이것은 세이브 파일에 저장되지 않으니, 주의!
		nekoAutoPtr<nekoIBlockInstance> _mTempInst; // 락 걸어둔 인스턴스. 이것도 세이브 파일에 저장되지 않는다.

		nekoThread(nekoCommands *cmds, bool isStack = false) : nekoStack(cmds), mRunning(false), mIsStack(isStack), mValid(true) {}
		~nekoThread() {}
	};

	enum E_GAME_RUN_TYPE
	{
		GAME_RUN_PREVIEW = 0,
		// 프리뷰 모드에서는 변수 입력을 사용하지 않는다.

		GAME_RUN_PLAYING,
	};

	enum E_GAME_PROGRESSING_TYPE
	{
		EGPT_NORMAL = 0,
		EGPT_AUTO,
		EGPT_SKIP,
	};

	enum E_PROJECT_FLAG_TYPE
	{
		// 채팅박스를 썸네일 이미지에 표시하기
		EPFT_CHATBOX_IN_THUMBNAIL = 0x00000001,

		// 커서를 채팅박스 오른쪽 아래에 붙이기
		EPFT_NEWTYPE_CURSOR		  = 0x00000002,
	};

	struct NEKONOVEL_API nekoGameEnvironment
	{
		float32 mSpeechSpeed;
		uint32 mSpeechColor;
		uint32 mSelectionColor;
		uint32 mSpeechShadowColor;
		int32 mSpeechShadowType;

		bool mSystemMenuEnabled;
		E_GAME_PROGRESSING_TYPE mProgressingType;

		int32 mPrintX;
		int32 mPrintY;

		/// 프로젝트 설정
		int32 mWidth;
		int32 mHeight;
		bool mDefFullScreen;
		int32 mDefFXVolume;
		int32 mDefBGMVolume;
		int32 mDefVoiceVolume;

		string mLastPublishFolder;
		int32 mLastPublishVersion;
		bool mCompressBGM;

		/// 문장 흘림
		bool mPrintDelay;

		/// 텍스트 윈도우 투명도(기본은 아닌데, 기본으로 승격!)
		int32 mTextWindowAlpha;

		/// 포토 모드..
		bool mPhotoMode;

		/// 첫 스크립트 파일
		string mFirstScriptName;

		/// 커서 옵션
		E_CHATBOX_CURSOR_TYPE mCursorType;
		int32 mCursorAniWidth;
		int32 mCursorAniHeight;

		/// 카메라 속성 강제 무시 여부
		/// 새로 만들어지는 객체부터 적용
		bool mCameraIgnored;

		/// 처음 CG가 나타날때 페이드인 속도
		float32 mCGFadeInSpeed;

		/// CG가 사라질 때 페이드아웃 속도
		float32 mCGFadeOutSpeed;

		/// 부드러운 문장 흘림
		bool mSmoothPrint;

		/// 게임 실행 횟수
		uint32 mGameRuntimes;

		/// 외부 폰트 리스트
		string mExternalFonts;

		/// 대사 부가속도
		float32 mTextSubSpeed;

		E_TEXT_ALIGN_TYPE mTextAlign;

		/// 현재 저장 슬롯 페이지
		int32 mSlotPage;

		/// 최대 페이지 수
		int32 mMaxPage;

		/// 프로젝트 플래그
		uint32 mProjectFlags;

		/// '무대기' 무시 ?!
		bool mIW2Use;

		/// 오토모드 대기시간
		float32 mAutoWaitTime;

		/// 총 게임 플레이 시간
		float32 mGamePlayTime;

		nekoGameEnvironment(){ Clear(); }
		~nekoGameEnvironment() {}

		/// Clear
		void Clear();

		inline float32 GetSpeechSpeed() { return max(mSpeechSpeed + mTextSubSpeed, 0); }
	};

	struct NEKONOVEL_API nekoSelection
	{
		string mScriptName;
		string mBookMarkName;

		nekoSelection() {}
		~nekoSelection() {}
	};

	enum E_GAME_SYSTEM_STATE
	{
		EGSS_NONE = 0,
		EGSS_SELECTING, /* 사용 안함 */
		EGSS_WAIT,
		EGSS_BUTTON_PRESS_WAIT,
		EGSS_INPUT_WAIT,	/* 텍스트박스 입력대기 */
		EGSS_UNKNOWN,
	};

	enum E_GAME_FLOW_STATE
	{
		EGFS_NORMAL = 0,
		EGFS_QUICKSKIPPING, // 빠른 스킵
		EGFS_AUTOTURNING, // 자동 넘기기
	};

	struct nekoMacro
	{
		string mName;
		list<nekoOneCommand *> mCommandList;

		nekoMacro() {}
		~nekoMacro() {}
	};

#define SAVEDATA_CODENAME	"NEKONOVEL_SAVEDATA"
#define SAVEDATA_VERSION	1129
#define PUBLISHEDFILE_VERSION	1000
	enum
	{
		SaveThumnailWidth = 150,
		SaveThumnailHeight = 113,
	};

	class NEKONOVEL_API nekoSaveData : public nekoIUnknown
	{
	protected:

		string mFileName;
		string mDateString;
		string mLiteDateString;
		string mCaption;
		nekoAutoPtr<nekoHWTexture> mPreview;
		nekoByteBuffer mBuffer;
		time_t mTime;
		int32 mVersion;

	public:

		nekoSaveData();
		~nekoSaveData();

		result Initialize(const char8 *fileName);

		result LoadToGame();
		result SaveToFile(const char8 *fileName);

		inline nekoHWTexture *GetPreview() { return mPreview; }
		inline const char8 *GetDateString() { return mDateString.c_str(); }

		// 필요할 때만 생성
		const char8 *GetLiteDateString();

		inline const char8 *GetCaption() { return mCaption.c_str(); }
	};

	struct nekoHotKey
	{
		uchar8 mKeyCode;
		string mFileName;
		string mBookMarkName;

		nekoHotKey(char8 keyCode, const string &fileName, const string &bookMarkName) 
			: mKeyCode(keyCode), mFileName(fileName), mBookMarkName(bookMarkName) {}
		~nekoHotKey() {}
	};
	
	struct nekoVariableTrigger
	{
		string mVarName;
		string mFileName;
		string mBookMarkName;

		nekoVariableTrigger(const string &varName, const string &fileName, const string &bookMarkName) 
			: mVarName(varName), mFileName(fileName), mBookMarkName(bookMarkName) {}
		~nekoVariableTrigger() {}
	};

	// 오마케
	struct nekoOmake
	{
		string mName;
		string mFileName;

		nekoOmake() {}
		~nekoOmake() {}
	};

	enum E_CUSTOM_EVENT_TYPE
	{
		ECET_LOADED = 0,
		ECET_SAVED,
		ECET_RECOLLECTED,
		ECET_EXIT_SKIPMODE,
		ECET_GOTO_PREV_SLOT,
		ECET_GOTO_NEXT_SLOT,
		ECET_MSGBOX,
		ECET_COUNT,
	};

	// 커스텀 이벤트
	struct nekoCustomEventListener
	{
		string mName;
		string mBookMarkName;

		nekoCustomEventListener() {}
		~nekoCustomEventListener() {}
	};

	enum E_GAME_FLAG_TYPE
	{
		GAME_FLAG_VARTRACE		= 0x00000001,

		// 마지막 '대기' 이후로 스킵 되었는가 ?
		GAME_FLAG_LAST_SKIPPED	= 0x00000002,
	};

	enum E_MESSAGEBOX_TYPE
	{
		EMT_LOAD		= 0,	// 해당 로드 데이터를 불러올 것인가?
		EMT_OVERSAVE,			// 해당 세이브 데이터를 덮어쓸 것인지 ?
		EMT_REMOVE,				// 해당 데이터를 지울 것인지?
		EMT_QUICKLOAD,			// 퀵로드를 할 것인지 ?
		EMT_QUICKSAVE,			// 퀵세이브를 할 것인지 ?
		EMT_CUSTOM,				// 커스텀
		EMT_UNKNOWN,
	};

	class NEKONOVEL_API NekoNovel;
	class NEKONOVEL_API nekoGame : public nekoIEventListener
	{
	protected:

		uint32 mGameFlags;

		/// 기본 이펙트
		E_BGIMAGE_EFFECT_TYPE mBGImageEffect;
		nekoAutoPtr<nekoHWTexture> mBGImageEffectRule;
		string mBGImageEffectRuleName;
		float32 mBGImageEffectSpeed;

		string mName;
		string mMadeBy;
		string mIdentifier;
		string mRunPath;
		NekoNovel *mNekoCore;
		list<nekoThread> mStackList;
		list<nekoThread> mThreadList;
		bool mIsPlaying;
		bool mThreadUpdating;
		E_GAME_RUN_TYPE mRunType;

		bool mSkipAble;

		/// 인스턴스들...
		list<nekoIBlockInstance *> mSkipAbleInstances;
		list<nekoIBlockInstance *> mBlockInstanceList;
		map<string, nekoVariable> mVariables;
		nekoAutoPtr<nekoVoiceInstance> mVoice;
		nekoAutoPtr<nekoBGMusicInstance> mBGMusic;
		list<nekoIBlockInstance *> mTimerList;

		list<nekoCommands *> mLoadedCommands;

		nekoGUIManager mGUIManager;
		nekoAutoPtr<nekoGUIChatBox> mChatBox;
		nekoAutoPtr<nekoGUISelectionButtons> mSelectionButtons;

		// 메세지 로거
		nekoMessageLogger mMessageLogger;

		// 게임 환경 설정
		nekoGameEnvironment mPlayingEnv;

		// 게임 상태
		E_GAME_SYSTEM_STATE mGameState;
		E_GAME_FLOW_STATE mFlowState;

		list<nekoSelection *> mSelectionList;

		/// 픽킹된 인스턴스
		nekoIBlockInstance *mPickedInstance;

		/// 점프됨
		bool mJumped;

		nekoAutoPtr<nekoPublisher> mPublisher;
		nekoAutoPtr<nekoSaveData> mSaveDatas[MAX_SAVE_COUNT]; /// 세이브 데이터들..
		nekoAutoPtr<nekoSaveData> mQuickSave;

		list<nekoHotKey> mHotKeyList; // 핫키 데이터
		list<nekoVariableTrigger> mVarTriggerList; // 변수 트리거 데이터
		list<nekoOmake> mOmakeList;	// 오마케 리스트
		map<E_CUSTOM_EVENT_TYPE, nekoCustomEventListener> mCustomEventLtnList; // 커스텀 이벤트 리스트

		inline bool _IsMouseEventReceivable(const nekoIBlockInstance *inst)
		{
			if(IsMsgBoxShown())
				return (inst->GetType() == BLOCK_BUTTON
				&& (static_cast<const nekoButtonInstance *>(inst)->GetButtonType() == EBIT_MSGBOX_OK
				|| static_cast<const nekoButtonInstance *>(inst)->GetButtonType() == EBIT_MSGBOX_CANCEL));

			return ((inst->GetType() >= BLOCK_BUTTON && inst->GetType() <= BLOCK_TRACKBAR) 
				|| (inst->GetType() >= BLOCK_TEXTBOX && inst->GetType() <= BLOCK_MESSAGELOG));
		}

		/// 메세지박스 내용
		string mMsgBoxCaption;
		E_MESSAGEBOX_TYPE mMsgBoxType;
		int32 mMsgBoxArgs;

	public:
		
		/// Constructor
		nekoGame(NekoNovel *nekoCore);
		
		/// Destructor
		virtual ~nekoGame();

		inline void AddFlags(uint32 flags) { mGameFlags |= flags; }
		inline void RemoveFlags(uint32 flags) { mGameFlags &= ~flags; }
		inline bool IsTurnOn(uint32 flags) { return ((mGameFlags & flags) > 0); }

		result Initialize();

		nekoIBlockInstance *AddInstance(nekoIBlockInstance *inst);
		nekoIBlockInstance *CreateInstance(E_BLOCK_TYPE blockType, const char8 *name, bool *firstInitialize = 0);
		bool RemoveInstance(nekoIBlockInstance *inst);
		nekoIBlockInstance *GetInstance(const char8 *name);
		void ClearInstances(E_BLOCK_TYPE blockType, nekoIBlockInstance *without = NULL, bool excludesNonZeroAlphaDeleting = false);

		void ClearSkipAbleInstances();

		////////////////////////////////
		////////// 디버깅용....
		////////////////////////////////

		/// Load
		/// projectInfoData가 있으면 런타임으로 실행.
		result Load(const char8 *projectInfoData = NULL);

		/// Save
		result Save();

		////////////////////////////////
		///////// 실제 가동용....
		////////////////////////////////

		/// Build it!
		result Build();

		/// Run
		result Run();

		/// Save slot
		result SaveToSlot(int32 slotNo);

		/// Load from slot
		result LoadFromSlot(int32 slotNo);

		/// Remove slot
		result RemoveSlot(int32 slotNo);

		/// Save auto to slot
		result SaveAutoToSlot();

		/// Create Commands
		nekoCommands *CreateCommands(const char8 *name);

		/// Remove Commands
		bool RemoveCommands(nekoCommands *commands);

		/// Get Commands by name
		nekoCommands *GetCommands(const char8 *name);

		/// Change Commands' order.
		bool ChangeCommandsOrder(nekoCommands *commands, uint32 index);

		/// Play commands
		result PlayCommands(const char8 *name, const char8 *bookMarkName = NULL, bool onlyInLoadedScripts = false, bool internalRequest = false);

		/// Get Commands list
		inline list<nekoCommands *> &GetCommandsList() { return mLoadedCommands; }


		
		nekoVariable *AddVariable(const char8 *name);
		bool RemoveVariable(const nekoVariable *var);
		nekoVariable *GetVariable(const char8 *name);
		map<string, nekoVariable> &GetVariableList() { return mVariables; }


		/// Change Game Mode
		inline E_GAME_RUN_TYPE GetGameMode() { return mRunType; }
		void ChangeGameMode(E_GAME_RUN_TYPE runType);

		/// Update
		void Update(float32 frameTime);

		/// Draw
		void Draw();

		void EnableButtonWait(); // 버튼 대기 수행
		void OnButtonPressed(bool forMsgBox = false); // 버튼이 선택 되었다.

		inline void SetGameSystemState(E_GAME_SYSTEM_STATE state) { mGameState = state; }
		inline void SetGameFlowState(E_GAME_FLOW_STATE state) { mFlowState = state; }

		void WriteText(const char8 *prevVoiceName, const char8 *txt, bool append);

		/// GUI 얻어!
		inline nekoGUIChatBox *GetChatBox() { return mChatBox; }
		inline nekoGUISelectionButtons *GetSelectionButtons() { return mSelectionButtons; }

		/// 스킵 관련...
		inline void EnableSkipAble() { mSkipAble = true; }
		void DisableSkipAble();
		inline bool IsSkipAble() { return mSkipAble; }

		/// 좀 얻어와바 -_-a!!
		inline nekoVoiceInstance *GetVoice() { return mVoice; }
		inline nekoBGMusicInstance *GetBGMusic() { return mBGMusic; }
		inline nekoIBlockInstance *GetPickedInstance() { return mPickedInstance; }
		inline list<nekoIBlockInstance *> &GetBlockInstanceList() { return mBlockInstanceList; }
		inline void SetName(const char8 *name) { mName = name; }
		inline const char8 *GetName() { return mName.c_str(); }
		inline void SetMadeBy(const char8 *value) { mMadeBy = value; }
		inline const char8 *GetMadeBy() { return mMadeBy.c_str(); }
		inline nekoGameEnvironment &GetEnvironment() { return mPlayingEnv; }
		inline E_GAME_SYSTEM_STATE GetSystemState() { return mGameState; }
		inline E_GAME_FLOW_STATE GetFlowState() { return mFlowState; }

		/// 해당 위치로 이동하기
		/// clearAll은 mRunType가 GAME_RUN_PUBLISHED일 때만 유효함.
		/// mRunType가 GAME_RUN_PREVIEW에서는 clearAll은 항상 true 이다.
		void GotoLine(nekoCommands *commands, uint32 lineNumber, bool clearAll);

		/// 해당 위치부터 실행.
		void DoFrom(int32 lineNumber);

		/// 점핑하기!
		result JumpTo(const char8 *commandsName, const char8 *bookMarkName);

		bool OnEvent(const nekoEvent &event);

		void ClearAllInstances();

		/// 선택지 관련...
		void AddSelection(const string &fileName, const string &bookMarkName); /// 선택지 추가
		void OnSelectionCompleted(int32 index);
		void ClearSelections();

		/// 쓰레드 관련
		result AddThread(nekoThread &thread);
		result RemoveThread(const char8 *name);
		result StartThreads();
		bool IsExistThread(const char8 *name);
		inline uint32 GetThreadCount() { return mThreadList.size(); }

		/// 이펙트 추가
		result SetEffectToBG(E_BGIMAGE_EFFECT_TYPE fadeType, float32 speed, bool fadeIn);
		result SetRuleEffectToBG(const string &ruleFileName, float32 speed, bool fadeIn);
		inline nekoHWTexture *GetBGEffectRule() { return mBGImageEffectRule; }
		bool HasBGEffect();	// 이펙트가 진행중인가..
		inline E_BGIMAGE_EFFECT_TYPE GetFadeInEffect() { return mBGImageEffect; }
		
		/// 배포하기
		result StartPublishing();
		void StopPublishing();

		// 선택지 관련
		inline bool IsSelecting() { return (!mSelectionList.empty());}

		// 기본
		inline const char8 *GetRunPath() { return mRunPath.c_str(); }

		// 세이브 & 로드
		void LoadSaveDatas();
		inline nekoSaveData *GetSaveData(int32 slotNo);

		void DumpTo(nekoByteBuffer &buf); /// 덤프 시키기
		result LoadFrom(int32 version, nekoByteBuffer buf); // 가져오기
		
		// 이벤트 관련
		void SetHotKey(uchar8 keyCode, const string &fileName, const string &bookMarkName);
		void SetVariableTrigger(const string &varName, const string &fileName, const string &bookMarkName);
		void SetCustomEventListener(E_CUSTOM_EVENT_TYPE type, const string &fileName, const string &bookMarkName);
		bool RemoveCustomEventListener(E_CUSTOM_EVENT_TYPE type);
		result CallCustomEvent(E_CUSTOM_EVENT_TYPE type);
		void OnVariableChanged(const string &varName, nekoVariable *variable);
		void OnMsgBoxCompleted(bool answer);
		inline bool ShowMsgBox(E_MESSAGEBOX_TYPE msgBoxType, const char8 *msg, int32 args)
		{
			mMsgBoxCaption = msg;
			mMsgBoxType = msgBoxType;
			mMsgBoxArgs = args;

			if(FAILED(CallCustomEvent(ECET_MSGBOX)))
			{
				mMsgBoxType = EMT_UNKNOWN;
				return false;
			}

			return true;
		}
		inline bool IsMsgBoxShown() { return (mMsgBoxType != EMT_UNKNOWN); }
		inline E_MESSAGEBOX_TYPE GetMsgBoxType() { return mMsgBoxType; }

		inline void OnTextInputCompleted()
		{
			if(mGameState != EGSS_INPUT_WAIT)
				return;

			mGameState = EGSS_NONE;
			ClearInstances(BLOCK_TEXTBOX);
		}

		LPD3DXBUFFER TakeSaveScreenshot();

		inline nekoMessageLogger *GetMessageLogger() { return &mMessageLogger; }

		inline void GoPrevSlotPage()
		{
			mPlayingEnv.mSlotPage--;
			if(mPlayingEnv.mSlotPage < 1)
			{
				mPlayingEnv.mSlotPage = 1;
				return;
			}
				
			CallCustomEvent(ECET_GOTO_PREV_SLOT);
		}
		inline void GoNextSlotPage()
		{
			mPlayingEnv.mSlotPage++;
			if(mPlayingEnv.mSlotPage > mPlayingEnv.mMaxPage)
			{
				mPlayingEnv.mSlotPage = mPlayingEnv.mMaxPage;
				return;
			}

			CallCustomEvent(ECET_GOTO_NEXT_SLOT);
		}
		inline void GoSlotPage(int32 slotPage)
		{
			slotPage = min(max(slotPage, 1), mPlayingEnv.mMaxPage);
			if(mPlayingEnv.mSlotPage < slotPage)
			{
				mPlayingEnv.mSlotPage = slotPage;
				CallCustomEvent(ECET_GOTO_NEXT_SLOT);
			}
			else if(mPlayingEnv.mSlotPage > slotPage)
			{
				mPlayingEnv.mSlotPage = slotPage;
				CallCustomEvent(ECET_GOTO_PREV_SLOT);
			}
		}

		// 재생
		void PlayVoice(const char8 *name);

		///////////////////////////////
		/// 오마케
		//////////////////////////////
		void AddOmake(const string &name, const string &fileName);
		bool RemoveOmake(const string &name);
		const char8 *GetOmake(const string &name);

		#define ENVDATA_FILENAME "Config.nkenv"
		#define ENVDATA_CODENAME "NEKONOVEL_ENVIONRMENT"

		/////////////////////////////////////
		/// 환경 관련 메소드
		/////////////////////////////////////
		result SaveEnvironment();
		result LoadEnvironment();
	

		/// 기타
		void MakeScreenshot();

		/// 보이스 관련
		inline bool IsVoicePlaying() { return (mVoice && mVoice->IsPlaying()); }


		/// 텍스트를 파싱한다.(변수가 있으면 치환해줌..)
		const string &ParseString(const char8 *str);

		/// 이벤트들
		void OnRunCompleted();
	};
}


#endif // __nekoGame_h__