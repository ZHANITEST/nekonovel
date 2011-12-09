// ===============================================================
//  nekoCommands - 2009.7.6
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoCommands_h__
#define __nekoCommands_h__

#undef _HAS_ITERATOR_DEBUGGING
#define _HAS_ITERATOR_DEBUGGING 0
/*#undef  _SECURE_SCL
#define _SECURE_SCL 0
#undef  _SECURE_SCL_THROWS
#define _SECURE_SCL_THROWS 1*/
#include <list>
using namespace std;
#include "nekoTimer.h"
#include "nekoIRunnable.h"
#include "nekoIEventListener.h"
#include "nekoLockSection.h"
#include "nekoSaveSpot.h"

namespace neko
{
#	define MAX_COMMAND_SIZE 20

	enum E_COMMAND_TYPE
	{
		ECT_INPUT_WAIT = 0,
		// 대기 [이름]

		ECT_SOUND,
		// 효과음 [이름] [파일명]
		// 해당 효과음을 출력한다.

		ECT_BGMUSIC,
		// 배경음악 [파일명] [반복횟수=1]
		// 해당 배경음악을 재생한다. 반복횟수가 -1이면 영원히 재생한다.

		ECT_PLAY,
		// 재생 [이름]
		// 정지되어있던 것을 재생한다.

		ECT_PAUSE,
		// 일시정지 [이름]
		// 해당 객체를 일시정지 한다.

		ECT_STOP,
		// 정지 [이름]
		// 해당 객체를 정지한다.

		ECT_SET_VARIABLE,
		// 변수 변수이름 [=] 값
		// 해당 변수에 값을 입력한다.
		// 변수 이름이 @ 으로 시작할 경우 전역변수로 인식한다.
		// 연산자에는 = += -= /= *= 가 있다.
		// 랜덤값을 입력하려면 [값] 부분에 랜덤1~7 을 입력한다. 음수도 가능.

		ECT_MOVIE,
		// 동영상 [객체이름] [파일이름] [재생시간=-1]
		// 해당 동영상을 출력한다. 재생시간이 -1이면, 끝까지 재생한다. 

		ECT_FADEIN,
		// 페이드인 [객체이름] [소요시간]
		// 해당 객체를 페이드 인 한다.

		ECT_FADEOUT,
		// 페이드아웃 [객체이름] [소요시간]
		// 해당 객체를 페이드 아웃 한다.

		ECT_ROTATE,
		// 회전 [객체이름] [각도] [속도=0] [가속도=0]
		// 해당 객체를 회전한다.

		ECT_THREAD_START,
		// 쓰레드시작

		ECT_THREAD_WAIT,
		// 쓰레드대기,

		ECT_THREAD_DECLARE,
		// 쓰레드 [이름]

		ECT_CG,
		// CG [이름] [이미지파일명=""] [X] [Y]
		// 해당 이미지의 캐릭터를 표시한다.

		ECT_SPRITE,
		// 스프라이트 [이름] [이미지1] [이미지2] [이미지3] ...
		// 해당 이름의 새로운 스프라이트를 만든다.

		ECT_GIF_SPRITE,
		// GIF스프라이트 [이름] [이미지이름] [X] [Y]
		// 해당 이름의 GIF 스프라이트를 만든다.

		ECT_BOOKMARK,
		// 북마크 이름

		ECT_TIMER,
		// 타이머 [이름] [시간] [타이머완료후 호출할 파일] [이동할 북마크이름]
		// 해당 이름의 타이머를 만든다.

		ECT_PRINTPOS,
		// 시작위치 [X] [Y]

		ECT_TEXT_SPEED,
		// 대사속도 [값]
		// 대사 출력 속도를 조절한다.
		// 하나의 글자가 출력되는데 걸리는 시간(1/1000초)
		
		ECT_TEXT_COLOR,
		// 대사색깔 [R] [G] [B] [A]
		// 대사의 색깔을 조정한다.

		ECT_NEW_LINE,
		// 대사새줄
		// 대사창에서 다음줄로 넘어간다.

		ECT_TEXT_SIZE,
		// 대사크기 [값]
		// 출력될 대사의 크기이다.

		ECT_TEXT_FONT,
		// 대사폰트 [이름]
		// 출력될 대사의 폰트를 조정한다.

		ECT_CLEAR_TEXT,
		// 대사지우기
		// 출력된 모든 대사를 지운다.

		ECT_MOVE,
		// 이동 [이름] [X] [Y] [속도=0] [가속도=0]
		// 출력된 것을 옮긴다.
		// 속도는 1px 이동하는데 걸리는 시간으로 1/1000초 이다.

		ECT_TEXT_SKIN,
		// 대사창스킨 [파일이름]
		// 대사창의 스킨을 변경한다.

		ECT_TEXT_SHADOW_COLOR,
		// 대사그림자색깔 [R] [G] [B] [A=255]
		
		ECT_TEXT_SHADOW_TYPE,
		// 대사그림자 [값]
		// 0~4까지 설정 가능
		
		ECT_TEXT_WINDOW_RECT,
		// 대사창영역 [X] [Y] [가로크기] [세로크기]

		ECT_TEXT_PRINTING_RECT,
		// 대사출력영역 [X] [Y] [가로크기] [세로크기]

		// ★★★★

		ECT_APTEXT,
		ECT_TEXT,
		// 대사 어쩌구저쩌구 내용~~
		// 대사잇기 어쩌구저쩌구 내용~

		ECT_DELAY,
		// 딜레이 [값]
		// 1/1000초 단위로 딜레이를 준다.

		ECT_JUMP,
		// 점프 [파일이름] [북마크이름]
		// 해당 파일의 북마크로 이동한다.
		// 북마크이름이 첫줄 이면 첫줄로 이동한다.
		// 점프 [북마크이름]
		// 현재 파일 내에서 해당 북마크로 이동한다.

		ECT_SELECTION_COLOR,
		// 선택지색깔 [R] [G] [B] [A=255]

		ECT_SELECTION,
		// 선택지 [파일이름] [북마크이름] [선택지 내용]

		ECT_SYSTEM_MENU_ENABLE,
		// 시스템메뉴활성화 [응/아니]

		ECT_CLEAR_ALL,
		// 모두지우기
		// 현재 보이는 모든 것들을 제거한다.

		ECT_EXIT,
		// 프로그램을 종료한다.

		ECT_SCALE,
		// 크기 [이름] [가로배수] [세로배수] [속도=0]

		ECT_SKIP_START,
		// 스킵시작
		// ESC키나 엔터키로 스킵 가능.

		ECT_SKIP_END,
		// 스킵끝

		ECT_GET_TEXT,
		// 입력 [변수명] [X] [Y] [가로크기] [세로크기] [버튼 이미지] [마우스 올림 효과음] [눌렀을때 효과음] [폰트크기=-1] [폰트굵게=-1] [폰트이름=""]

		ECT_MACRO_START, // 사용안함
		// 매크로시작 [이름]
		// 매크로를 시작한다.

		ECT_FUNC_EXECUTE,
		// 함수 [이름] [인수1] [인수2] [인수3] ...
		// 인수를 가지고 함수를 실행한다.

		ECT_BUTTON_WAIT,
		// 버튼대기

		ECT_BUTTON,
		// 버튼 [X] [Y] [너비] [높이] [타입] ... [보통] [마우스가 올려져있을 때] [마우스가 눌려져 있을 때] [마우스를 올렸을 때 나는 효과음] [눌렀을 때 효과음]
		// [타입]에는 다음의 것이 있다.
		// 점프 [파일명] [북마크명]
		// 링크 [인터넷주소]
		// 종료
		// 배경음 [파일명]
		// 효과음 [파일명]
		// 스크립트 [파일명]
		// 없음

		//ECT_MAP_BUTTON,
		// 지도버튼 [CG이름] [X] [Y] [너비] [높이] [마우스가 올려져 있을때] [마우스가 눌려져 있을 때] [마우스를 올렸을때 나는 효과음] [눌렀을 때 효과음]
		
		ECT_TRACKBAR,
		// 트랙바 [X] [Y] [가로크기] [세로크기] [타입] ... [바 이미지]
		// 변수 [최대값] [변수명]
		// 보이스
		// 효과음
		// 배경음
		// 대사창투명도

		ECT_SCRIPT,
		// 스크립트 [파일명]
		// 스크립트를 불러온다.

		ECT_BGFADEIN,
		// 배경페이드인 [효과번호] [속도=1000]
		// 1 페이드
		// 2 블럭
		// 3 커튼.
		// 4 왼쪽부터 페이드
		// 5 오른쪽부터 페이드
		// 6 윗쪽부터 페이드
		// 7 아래쪽부터 페이드

		ECT_BGFADEOUT,
		// 배경페이드아웃 [효과번호] [속도=1000]
		// 1 페이드
		// 2 블럭
		// 3 커튼
		// 4 왼쪽부터 페이드
		// 5 오른쪽부터 페이드
		// 6 윗쪽부터 페이드
		// 7 아래쪽부터 페이드

		ECT_BACKGROUND,
		// 배경

		ECT_FACECG,
		// 페이스CG

		ECT_SCREENCOLOR,
		// 화면색깔 [R] [G] [B] [A]

		ECT_BOUNCE,
		// 튀어오르기 [이름] [X1] [Y1] [X2] [Y2] [속도=1000]

		ECT_ALPHA,
		// 투명도 [이름] [알파값]

		ECT_SYSTEMSCREEN,
		// 시스템화면 [종류]
		// [종류]에는 로드, 세이브 이 있다.
		// 종류가 없으면 화면을 없앤다.

		ECT_SYSTEMBUTTON,
		// 시스템버튼 [X] [Y] [너비] [높이] [타입] ... [보통] [마우스가 올려져 있을 때] [마우스가 눌려져 있을 때] [마우스를 올렸을 때 나는 효과음] [눌렀을 때 효과음]
		// [타입]에는 다음이 있다.
		// 회상
		// 점프 [파일이름] [북마크] // 파일이름에 '여기' 입력시 현재 파일이 된다.
		// 자동넘기기 [응/아니]
		// 빠른스킵 [응/아니]
		// 다음페이지 // 화면이 지원하는 경우에만
		// 이전페이지 // 화면이 지원하는 경우에만
		// 변수 [변수명] [값]
		// 화면모드 [윈도우/풀스크린]
		// 문장흘림 [응/아니]
		// 기본폰트 [이름]
		// 항상위 [응/아니]
		// 종료

		ECT_SYSTEMBUTTON_BYE,
		// 시스템버튼제거

		ECT_VIBE,
		// 진동 [이름] [X세기] [Y세기] [시간=1000] [속도=1000] [X감쇠=0] [Y감쇠=0]

		ECT_BUTTON_BYE,
		// 버튼제거
		// 출력된 일반 버튼들을 모두 제거한다.

		ECT_KEYPROCESS,
		// 키처리 [키코드] [파일이름] [북마크이름]
		// 해당 키를 누르면 해당 위치로 이동한다.

		ECT_HIDE_ONLYCHAT,
		// 대사만감추기 [응/아니]

		ECT_IF,
		// 조건 [A] [=] [B] [파일이름] [북마크이름] [아니면] [파일이름] [북마크이름]
		// [명령]에는 다음이 있다.
		//  [파일명] [북마크이름]

		ECT_OMAKE_ADD,
		// 오마케추가 [이름] [파일이름]

		ECT_OMAKE_CG,
		// 오마케CG [이름] [X] [Y] [가로크기] [세로크기] [없으면 표시할 파일]
		// 해당 이름의 오마케 이미지를 불러와 표시해줍니다.
		// 표시된 오마케는 CG와 같은 기능을 합니다.

		ECT_GAME_STATE,
		// 게임상태 [타입]
		// 자동넘기기
		// 빠른스킵

		ECT_SET_EVENT,
		// 이벤트 [타입] [파일이름] [북마크이름]
		// [타입]에는 다음이 있다.
		// 로드후
		// 세이브후

		ECT_VOICE,
		/// 보이스 [파일이름]
		/// 성우의 목소리를 출력한다.
		/// 

		ECT_TEXT_LINEINTERVAL,
		/// 대사줄간격 [픽셀]
		/// 대사줄간격을 설정합니다.
		/// 음수도 가질 수 있습니다.

		ECT_PRINTING_ORDER,
		/// 출력순서 [이름] [값]
		/// 0~200까지 출력 순서를 정합니다.
		/// CG로 명령어로 두개의 그림을 출력했을 때 앞뒤 위치를 확실히 해주기 위해 사용합니다.
		/// 기본값은 100이고, 작을수록 앞으로 나옵니다.

		ECT_LABEL,
		/// 라벨 [이름] [X] [Y] [R=255] [G=255] [B=255] [A=255] [텍스트]
		/// 화면에 텍스트를 출력합니다.
		/// 변수는 {{변수이름}}을 사용합니다.

		ECT_CAMERA,
		/// 카메라 [속도] [가속도] [목표X] [목표Y] [초당 비율조정량] [확대비율X] [확대비율Y] [초당 각도조정량] [회전각도]

		ECT_HIDE_CHATBOX,
		/// 대사창감추기 [시간]
		/// 대사를 지우고, 대사창을 감춥니다. 시간은 1/1000초 단위이며, 입력하지 않으면 바로 감추어집니다.

		ECT_SHOW_CHATBOX,
		/// 대사창보이기 [시간]
		/// 대사창을 보여줍니다. 시간은 1/1000초 단위이며, 입력하지 않으면 바로 보여집니다.

		ECT_TEXT_BOLD,
		/// 대사굵게 [응,아니]
		/// 대사 글씨를 굵게 한다.

		ECT_FLASH,
		/// 플래시 [이름] [파일이름] [X] [Y] [가로크기] [세로크기]
		///
		/// 플래시를 화면상에 재생합니다.
		///
		/// 예)
		/// // 불러오기
		/// 플래시 엽기토끼 엽기토끼.swf 0 0 800 600
		///
		/// // 없애기
		/// 플래시 엽기토끼

		ECT_BG_RULE_FADEIN,
		/// 배경룰페이드인 [파일이름] [속도]

		ECT_BG_RULE_FADEOUT,
		/// 배경룰페이드아웃 [파일이름] [속도]

		ECT_PARTICLE,
		/// 파티클 [이름] [파일이름]

		ECT_SELECTION_BUTTON,
		/// 선택지버튼 [보통] [마우스가 올려져있을 때] [마우스가 눌려져 있을 때] [마우스를 올렸을 때 나는 효과음] [마우스를 눌렀을 때 나는 효과음] [파일이름] [북마크이름] [내용]

		ECT_SELECTION_BUTTON_RECT,
		/// 선택지버튼영역 [X] [Y] [가로크기] [세로크기]

		ECT_LABEL_PROPERTY,
		/// 라벨속성 [이름] [폰트크기] [굵기] [폰트이름]
		/// 라벨의 글씨 속성을 변경합니다.

		ECT_CLEAR_RECOLLECTION,
		/// 회상초기화
		/// 회상에 있는 내용을 모두 지웁니다.

		ECT_IGNORE_CAMERA,
		/// 카메라무시
		/// 카메라 무시 여부를 결정합니다.

		ECT_VARIABLE_TRIGGER,
		// 변수트리거 [변수이름] [파일이름] [북마크이름]
		// 변수의 값이 바뀌면 해당 스크립트를 실행하고, 다시 돌아옵니다.

		ECT_COLOR,
		// 색조 [이름] [R] [G] [B] [소요시간=0]
		// 캐릭터의 색조를 변경할 수 있습니다.
		
		ECT_TEXT_ALIGN,
		// 텍스트정렬 [방법]
		// [방법] 에는 왼쪽, 가운데, 가운데2, 오른쪽 가 있으며, 생략할 경우 왼쪽으로 지정됩니다.

		ECT_CG_FADEIN,
		// CG페이드인 [속도=0]
		// CG 명령어 사용시 자동적으로 페이드인 시킵니다.
		// 0일 경우, CG를 페이드인 하지 않고 바로 나타냅니다.

		ECT_CG_FADEOUT,
		// CG페이드아웃 [속도=0]
		// 0일 경우, CG를 페이드아웃 하지 않고 바로 없앱니다.

		ECT_TEXT_FADE,
		// 대사페이드 [응/아니]
		// 대사페이드 기능을 켜고 끌 수 있습니다.

		ECT_SYSTEM_OPTION,
		// 시스템옵션 [옵션명] [값]
		// 시스템옵션을 지정할 수 있습니다.
		// [옵션명]에는 다음이 있습니다.
		// 대사넘기기 [응/아니] : 마우스 클릭시 대사가 한번에 넘어가는 기능을 켜고 끕니다.
		// 이미지퀵스킵 [응/아니] : 퀵스킵시 이미지 등도 빠르게 스킵될 수 있도록 합니다.


		ECT_SET_ONE_VARIABLE,
		// 정변수
		// 변수와 기능은 같으나, 존재하지 않을 경우에만 생성하는 변수이다.

		ECT_INITIALIZE_RECOLLECTION,
		// 회상첫설정 [가로크기] [세로크기] [폰트크기] [폰트이름]

		ECT_RECOLLECTION_TEXT,
		// 회상 [이름] [X] [Y] [스크롤바타입=1,2] [스크롤바버튼높이] [스킨파일]

		ECT_RECOLLECTION_SETTING,
		// 회상설정 [라벨/대사간 여백] [페이지 당 간격] [빈 줄삽입=0] [R] [G] [B]
		// 입력하지 않으면 기본 설정을 유지합니다.

		ECT_RECOLLECTION_COLOR,
		// 회상색깔 [R] [G] [B]
		// 입력하지 않으면 기본 색상으로..

		ECT_MSGBOX,
		// 메세지박스 [글귀]

		ECT_MSGBOX_WAIT,
		// 메세지박스대기

		ECT_INPUT_WAIT2,
		// 무대기
		// 무시 가능한 대기

		ECT_IW2_USE,
		// 무대기기능 [응/아니]

		ECT_QUICK_LOAD,
		// 퀵세이브한 게임을 로드합니다.

		ECT_QUICK_SAVE,
		// 현재 게임 진행 상황을 세이브 합니다.

		ECT_THREAD_TERMINIATE,
		// 쓰레드종료 [이름]
		// 해당 이름의 쓰레드를 종료 시킵니다.

		ECT_NOTHING,
		ECT_UNKNOWN,
		ECT_COUNT
	};

	enum E_EDITOR_COLOR_TYPE
	{
		EECT_BACKCOLOR = 0,
		EECT_1 = 1,
		EECT_10 = 10,
		EECT_COMMENT,
		EECT_ENABLED, // 색상을 사용하는가 ? 0 or 1
		EECT_COUNT,
	};

	struct nekoThread;
	struct NEKONOVEL_API nekoOneCommand
	{
		E_COMMAND_TYPE cmdType;
		uint32 startLine;
		list<string> arguments;

		nekoOneCommand() {}
		virtual ~nekoOneCommand() {}

		void Begin(const list<nekoOneCommand *>::iterator &selfIter, nekoThread *thread, bool internalRequest);
		bool Update(nekoThread *cmds, float32 frameTime);
		void End(const list<nekoOneCommand *>::iterator &selfIter, nekoThread *cmds, bool internalRequest);
		bool OnEvent(nekoThread *thread, const nekoEvent &event);

		// 게임이 로딩되었을 때
		void OnGameLoaded(nekoThread *cmds);
	};

	class NEKONOVEL_API nekoGame;
	class NEKONOVEL_API nekoCommands : public nekoIEventListener
	{
	protected:

		enum
		{
			LinesPerVisualSpot = 100,
		};

		string mName;
		nekoLockSection mLockSection;
		list<nekoOneCommand *> mCommandList;
		list<nekoSaveSpot> mVisualSpotList;

		/// 해당 라인으로 이동
		/// internalRequest가 true이면 명령어는 단순히
		/// 내부 처리를 위해서만 사용된다.
		void _GotoLine(nekoThread *thread, bool internalRequest, uint32 lineNumber);

		friend class nekoGame;

	public:
		
		/// Constructor
		nekoCommands(const char8 *name);
		
		/// Destructor
		~nekoCommands();

		/// 이름 얻기
		inline const char8 *GetName() const { return mName.c_str(); }

		/// 파일로 부터 스크립트 가져오기
		result LoadFromFile(const char8 *fileName);

		//////////////////////////////////////
		/// 게임 제작 모드를 위한 메소드
		//////////////////////////////////////
		result OnLineUpdated(uint32 lineNumber, const char8 *message); /// 라인이 변경됨.
		result OnLineRemoved(uint32 startLine, uint32 lineCount); /// 몇 라인이 삭제되었다.
		result OnLineInserted(uint32 lineNumber, const char8 *message); /// 새 라인이 입력되었다.
		void UpdateLineCount(uint32 lineCount);

		/// 새 커맨드 추가
		result AddCommand(uint32 lineNumber, const char8 *message);

		/// 기존 커맨드 수정
		result UpdateCommand(uint32 lineNumber, const char8 *message);

		/*
		/// 비주얼 스팟 추가
		result AddVisualSpot(uint32 lineNumber);
		*/

		/// 커맨드 타입 얻기
		static result GetCommandType(const char8 *message, E_COMMAND_TYPE *cmdType, list<string> &arguments);

		/*
		/// 락 걸기
		inline void Lock()
		{
			mLockedCount++;
		}*/

		/*inline void Unlock() { mLockedCount = max(mLockedCount - 1, 0); }
		inline bool IsLocked() { return (mLockedCount > 0); }*/

		inline list<nekoOneCommand *>::iterator GetStartingPos() { return mCommandList.begin(); }
		inline list<nekoOneCommand *>::iterator GetEndingPos() { return mCommandList.end(); }
		inline list<nekoOneCommand *> &GetCommandList() { return mCommandList; }

		list<nekoOneCommand *>::iterator GetPosByLine(uint32 lineNum, bool findAround = false);
		list<nekoOneCommand *>::iterator SearchFrom(list<nekoOneCommand *>::iterator from, E_COMMAND_TYPE cmdType);
		result SearchBookMark(const char8 *name, list<nekoOneCommand *>::iterator &itOut);

		bool OnEvent(nekoThread *thread, const nekoEvent &event);

		static E_COMMAND_TYPE GetCommandByName(const char8 *name);
		static const char8 *GetCommandString(int32 i);
		static const char8 **GetCommandHelpStrings();

		static result LoadCommandReplacements(uint32 editorColors[] = 0, uint32 commandColors[] = 0);

		inline bool IsVoicePlaying()
		{
			//if(mVoice)
			return false;
		}
	};
}


#endif // __nekoCommands_h__