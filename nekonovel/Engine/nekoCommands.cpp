#include "nekoPrecompiledHeader.h"
#include "nekoCommands.h"
#include "nekoLogger.h"
#include "NekoNovel.h"
#include "nekoImage.h"
#include "nekoUtility.h"
#include "nekoDefined.h"
#include "nekoSound.h"
#include "nekoSprite.h"
#include "nekoFileSystem.h"
#include "nekoButton.h"
#include "nekoSystemButton.h"
#include "nekoTrackBar.h"
#include "nekoLabel.h"
#include "nekoFlash.h"
#include "nekoParticle.h"
#include "nekoTextBox.h"
#include "nekoMovie.h"
#include "nekoMessageLog.h"
#include "nekoVideoDriver.h"
#include "nekoGIFSprite.h"
#include <tinyxml.h>

namespace neko
{
#define LimitMinArguments(value) if(arguments.size() < value) \
								{ \
									LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어는 인수가 적어도 %d개 필요해요.", startLine, gCommandStrings[cmdType], value); \
									return; \
								} \

#define mTempTimer (thread->_mTempTimer)
#define mTempInteger (thread->_mTempInteger)
#define mTempFloat (thread->_mTempFloat)
#define mTempString (thread->_mTempString)
#define mTempInst (thread->_mTempInst)

	static list<string> realArguments;

	static char8 gCommandStrings[ECT_COUNT][MAX_COMMAND_SIZE] =
	{
		"대기",
		"효과음",
		"배경음악",
		"재생",
		"일시정지",
		"정지",
		"변수",
		"동영상",
		"페이드인",
		"페이드아웃",
		"회전",
		"쓰레드시작",
		"쓰레드대기",
		"쓰레드",
		"CG",
		"스프라이트",
		"GIF스프라이트",
		"북마크",
		"타이머",
		"시작위치",
		"대사속도",
		"대사색깔",
		"대사새줄",
		"대사크기",
		"대사폰트",
		"대사지우기",
		"이동",
		"대사창스킨",
		"대사그림자색깔",
		"대사그림자",
		"대사창영역",
		"대사출력영역",
		"대사잇기",
		"대사",
		"딜레이",
		"점프",
		"선택지색깔",
		"선택지",
		"", // 시스템메뉴활성화
		"모두지우기",
		"종료",
		"크기",
		"스킵시작",
		"스킵끝",
		"입력",
		"", // 매크로시작
		"함수",
		"버튼대기",
		"버튼",
		"트랙바",
		"스크립트",
		"배경페이드인",
		"배경페이드아웃",
		"배경",
		"페이스CG",
		"화면색깔",
		"튀어오르기",
		"투명도",
		"", // 시스템화면
		"시스템버튼",
		"시스템버튼제거",
		"진동",
		"버튼제거",
		"키처리",
		"대사만감추기",
		"조건",
		"오마케추가",
		"오마케CG",
		"게임상태",
		"이벤트",
		"보이스",
		"대사줄간격",
		"출력순서",
		"라벨",
		"카메라",
		"대사창감추기",
		"대사창보이기",
		"대사굵게",
		"플래시",
		"배경룰페이드인",
		"배경룰페이드아웃",
		"파티클",
		"선택지버튼",
		"선택지버튼영역",
		"라벨속성",
		"회상초기화",
		"카메라무시",
		"변수트리거",
		"색조",
		"대사정렬",
		"CG페이드인",
		"CG페이드아웃",
		"대사페이드",
		"시스템옵션",
		"정변수",
		"회상첫설정",
		"회상",
		"회상설정",
		"회상색깔",
		"메세지박스",
		"메세지박스대기",
		"무대기",
		"무대기기능",
		"퀵로드",
		"퀵세이브",
		"쓰레드종료",
		0,
		0,
	};

	static int32 gCommandStringsLen[ECT_COUNT];

	static const char8 *gCommandHelps[] =
	{
		"\n게임을 즐기는 사람이 키를 누를 때 까지 기다립니다.",
		"[이름] [파일명(생략가능)] [반복]\n사운드를 재생합니다. 파일명이 생략되면 사운드가 종료됩니다.\n[반복]은 재생되는 횟수이며 '-1'이나 '반복'을 입력하면 끝없이 재생 됩니다.\n예)\n사운드 흥! 흥.wav",
		"[반복] [파일명]\n해당 배경음악을 재생합니다.[반복]은 재생되는 횟수이며 '-1'이나 '반복'을 입력하면 끝없이 재생 됩니다.\n예)배경음악 반복 렛츠고하앙이.mp3\n배경음악 렛츠고하앙이.mp3",
		"[이름]\n멈춰 있던 것(사운드, 동영상 ..)을 움직이게 합니다.",
		"[이름]\n어떤 것(사운드, 동영상 ..)을 일시적으로 정지시킵니다.",
		"[이름]\n어떤 것(사운드, 동영상 ..)을 정지시킵니다. 그리고 처음의 위치로 돌아갑니다.",
		"[이름] [=] [값]\n변수를 설정합니다. 만들어진 모든 변수는 세이브 할때 같이 저장됩니다. 이름 맨 앞에 $가 있으면 세이브 파일에 관계없이 게임 전체에 저장됩니다.\n예)\n변수 호감도 = 10\n변수 호감도 += 10\n\n*랜덤 기능은 다음과 같이 사용해주세요. 다음 예제는 1~6의 숫자 중 랜덤값을 얻는 방법입니다.\n예)\n변수 테스트 = 랜덤(1~6)\n\n*다음의 변수는 게임 프로그램 내부에서 지원하고 있습니다.\n%대기중 : 1 또는 0\n%게임상태 : \"자동넘기기\" 또는 \"빠른스킵\"\n%페이지 : 현재의 로드 혹은 세이브 슬롯 페이지\n%슬롯날짜[번호 1~20] : 저장된 슬롯의 날짜\n%슬롯씬이름[번호 1~20] : 저장된 슬롯의 씬이름(변수 '슬롯씬이름'으로부터 기록)\n%메세지 : 커스텀 메세지박스에 띄워진 글\n%메세지박스응답 : 커스텀 메세지박스의 결과(0 또는 1)\n%플레이시간 : 게임을 켠후 종료 때까지의 총 플레이 시간(초 단위, 세이브 파일에 기록됨.)n%게임실행횟수\n%현재시간",
		"[이름] [X] [Y] [가로크기] [세로크기] [파일이름] [스킵가능시간=0]\n동영상을 재생합니다.\n해당 컴퓨터에 코덱이 설치되어 있어야 합니다.\n[스킵가능시간]초 만큼 재생 후 스킵이 가능토록합니다. -1 입력시에는 스킵이 아예 불가능합니다.\n예)\n동영상 오프닝 0 0 800 600 하앙이AV.avi",
		"[이름] [소요시간=0]\n\n해당 녀석을 페이드인(점점 밝아지게) 합니다. 소요시간(1/1000초 단위)이 없으면 바로 적용입니다.\n참고 : 이름 부분에는 '대사창' '배경음악' '보이스' '페이스CG'도 들어갈 수 있습니다.",
		"[이름] [소요시간=0]\n\n해당 녀석을 페이드아웃(점점 어두워지게) 합니다. 소요시간(1/1000초 단위)이 없으면 바로 적용입니다.\n페이드아웃된 후에는 일반적으로 해당 녀석은 사라집니다.\n참고 : 이름 부분에는 '대사창' '배경음악' '보이스' '페이스CG'도 들어갈 수 있습니다.",
		"[이름] [각도] [속도] [가속도=0]\n해당 녀석을 회전합니다. 각도는 60분법을 사용하며, 가속도는 생략할 수 있습니다 ^^.",
		"\n지금까지 만든 쓰레드를 모두 작동시킵니다.",
		"\n현재 진행중인 쓰레드가 모두 끝날 때 까지 기다립니다.",
		"[이름]\n새로운 쓰레드를 만듭니다.\n예)\n쓰레드 이쁜쓰레드\n~어쩌구저쩌구 샬랴샬라~\n쓰레드 // 쓰레드 만드는 것을 끝냅니다.",
		"[이름] [파일명] [X] [Y] [세이브화면에 표시여부=\"응\"] [출력순서=100]\n\nSCG(캐릭터 이미지)를 출력합니다.\n\n파일명이 없으면 출력을 해제합니다.\n[X]와 [Y]를 입력하지 않으면 시작 위치로 설정된 곳에서 이미지가 나타납니다.\n",
		"[이름] [지연시간] [파일명1] [파일명2] [파일명3] ...\n이미지들의 스프라이트를 만듭니다. 지연시간은 1/1000초 단위 입니다.",
		"[이름] [파일명] [반복횟수=1] [X] [Y]\nGIF 파일을 불러와 스프라이트를 만들어 재생합니다.",
		"[이름]\n이곳에 북마크를 지정합니다.",
		"[이름] [시간] [호출 파일] [북마크이름]\n<지원예정중 입니다..>타이머를 설정합니다. 타이머가 끝나면 [호출 파일]의 [북마크이름]으로 이동합니다.\n예)\n타이머 폭발타이머 10 폭발되었다.txt 처음 // 타이머 설정\n타이머 폭발타이머 // 타이머 해제",
		"[X] [Y]\nCG 라던가 스프라이트 등의 시작 위치를 지정합니다. 이미 출력된 것은 영향을 받지 않습니다.",
		"[시간]\n대사 출력 속도를 조절합니다. 시간은 하나의 글자가 출력되는데 걸리는 시간으로 1/1000초 단위입니다. 작을수록 빨리 출력됩니다.",
		"[빨강] [초록] [파랑] [투명도=255]\n앞으로 출력될 대사의 색깔을 지정합니다. 투명도는 입력하지 않으면 255가 됩니다.",
		"\n대사가 출력되는 커서를 다음줄로 옮깁니다.",
		"[크기]\n앞으로 출력될 대사의 크기를 지정합니다.",
		"[이름]\n출력될 대사의 폰트를 지정합니다.",
		"\n현재 출력된 모든 대사를 지웁니다.",
		"[이름] [X] [Y] [속도=0] [가속도=0]\n출력된 녀석을 이동시킵니다. [속도]를 입력하지 않으면 바로 이동됩니다.",
		"[파일명]\n대사창의 스킨을 변경합니다.", 
		"[빨강] [초록] [파랑] [투명도=255]\n대사의 그림자 색깔을 지정합니다.",
		"[타입]\n대사의 그림자 모습(0~3)을 지정합니다. 0은 없음, 1은 오른쪽 아래 그림자, 2는 외곽선, 3은 밖으로 갈수록 옅어지는 모습, 4는 두꺼운 외곽선 입니다.",
		"[X] [Y] [가로크기] [세로크기]\n대사창이 표시될 위치를 지정합니다. 대사창영역의 스킨은 '대사창스킨' 명령어로 바꿀 수 있습니다.",
		"[X] [Y] [가로크기] [새로크기]\n대사창 영역과 별도로, 실제 대사가 출력되는 영역을 지정합니다.",
		"[내용]\n방금 전에 출력한 대사에 이어서 출력합니다.\n예)대사 나는 잘생긴 하앙이\n대사잇기 라고 말한 적 없음.",
		"[내용]\n대사를 화면에 출력합니다.\n변수를 표시하려면 {{변수이름}} 을 사용하세요!.\n예)대사 나는 잘생긴 하앙이다!",
		"[시간]\n게임을 잠시 멈춥니다. 시간은 1/1000초 단위입니다.", 
		"[파일명] [북마크이름]\n점프 [북마크이름] // 현재 파일 내에서 이동\n\n해당 위치로 점프합니다.\n[북마크이름]에 첫줄(혹은 끝줄)을 입력하면 첫줄(혹은 끝줄)로 갑니다.",
		"[빨강] [초록] [파랑] [투명도=255]\n앞으로 출력될 선택지의 색깔을 지정합니다. 투명도는 입력하지 않으면 255가 됩니다.",
		"[파일이름] [북마크이름] [선택지내용]\n선택지를 만듭니다. 그냥 '선택지시작' '선택지끝' 같은 것 없이 바로 선택지만 입력하면 됩니다. 현재 파일에서 이동하려면 [파일이름]에 여기라고 입력해주세요.",
		"[응/아니]\n시스템메뉴를 활성화 할 것인지 하지 말 것인지 결정합니다. 시스템메뉴에는 게임을 저장, 불러올 수 있는 기능이 있습니다.",
		"\n현재 보이는 모든 것을 지웁니다. (잔인한 녀석.. -_-+)", 
		"\n프로그램을 종료합니다.",
		"[이름] [가로배수] [세로배수] [시간=0]\n해당 녀석의 크기를 늘립니다. 시간은 1/1000초 단위입니다.\n예)크기 이쁜그림 1.5 1.5 1000 // '이쁜그림'을 1초 동안 가로 1.5배, 세로 1.5배 늘립니다.",
		"\n현재 부터 '스킵끝'까지 ESC키로 스킵할 수 있도록 합니다",
		"\n이곳에 오면 스킵 가능한 영역이 끝나겠지요 ^^",
		"[변수명] [X] [Y] [가로크기] [세로크기] [최대길이=-1] [버튼 이미지] [마우스 올림 효과음] [눌렀을때 효과음] [R] [G] [B] [폰트크기] [폰트굵게=-1] [폰트이름=""]\n\n입력할 수 있는 텍스트박스와 버튼을 만듭니다.\n[마우스 올림 효과음] 부터는 생략할 수 있습니다.\n[최대길이]는 바이트수 이며, -1일 경우 무제한입니다.",
		"사용할 수 없습니다.",
		"[파일명] [인수1] [인수2] [인수3] ...\n함수를 실행합니다. 인수의 개수는 제한이 없으며, 각각의 인수는 변수 {{_1}}, {{_2}}, {{_3}} ... 식으로 대응됩니다.\n함수의 인수에서는 다음을 조심해주세요.\n * 문자열을 인수로 할때에는 \"내용\"으로 해주세요.\n * {{내용}} 은 '내용' 변수 안에 있는 값을 변수로 취급합니다.\n * 인수에 변수를 전달하시고 싶으시면 그냥 변수 이름을 입력해주세요.",
		"\n현재 보여진 버튼이 선택될 때 까지 기다립니다.",
		"[이름] [X] [Y] [가로크기=0] [세로크기=0] [타입] ... [보통 이미지] [롤오버 이미지] [마우스 누를 때이미지] [마우스 올렸을 때 효과음] [눌렀을 때 효과음]\n[타입]에 따라 가운데 인수가 다르게 결정됩니다. 타입에는 다음의 것이 있습니다.\n점프 [파일명] [북마크명]\n링크 [인터넷주소]\n종료\n배경음 [파일명]\n효과음 [파일명]\n스크립트 [파일명] : 해당 스크립트를 실행합니다.\n포토모드 : 포토모드의 작동을 토글로 설정합니다.\n메세지확인 : 메세지에 '예' 라고 응답합니다.\n메세지취소 : 메세지에 '아니오'라고 응답합니다.\n없음",
		"[X] [Y] [가로크기] [세로크기] [타입] ... [바 이미지]\n볼륨 등을 조절할 수 있는 바를 만듭니다.\n[타입]에 따라 가운데 인수가 다르게 결정됩니다. 타입에는 다음의 것이 있습니다.\n변수 [최대값] [변수명]\n보이스\n효과음\n배경음\n대사창투명도\n대사속도",
		"[파일명] [북마크이름]\n[파일명]의 스크립트를 불러와 처음부터 작동시킵니다. 불러온 스크립트가 모두 끝나면 '스크립트' 명령어의 바로 다음줄로 복귀 합니다.\n[북마크이름]은 생략 가능합니다.",
		"[효과번호] [속도]\n배경을 페이드인(점점 밝아지게) 합니다. 배경 페이드인은 앞으로 '배경' 명령어가 나올때 마다 실행됩니다.\n속도는 기본이 1000 입니다.\n[효과번호]\n0 : 없음\n1 : 페이드\n2 : 블럭\n3 : 왼쪽부터 블라인드\n4 : 오른쪽부터 블라인드\n5 : 왼쪽 슬라이드\n6 : 오른쪽 슬라이드\n7 : 윗쪽 슬라이드\n8 : 아랫쪽 슬라이드\n9 : 4조각으로 나눠서 위 아래로\n10 : 회전하면서 커지게\n11 : 배경을 매우 크게 확대\n12 : 왼쪽에서부터 이미지가 나타남\n13 : 오른쪽에서부터 이미지가 나타남\n14 : 윗쪽에서부터 이미지가 나타남\n15 : 아랫쪽에서부터 이미지가 나타남\n",
		"[효과번호] [속도]\n배경을 페이드아웃(점점 희미해지게) 합니다. 방금전 까지 보여진 배경에 대해서만 적용됩니다.\n속도는 기본이 1000 입니다.\n[효과번호]\n0 : 없음\n1 : 페이드\n2 : 블럭\n3 : 왼쪽부터 블라인드\n4 : 오른쪽부터 블라인드\n5 : 왼쪽 슬라이드\n6 : 오른쪽 슬라이드\n7 : 윗쪽 슬라이드\n8 : 아랫쪽 슬라이드\n9 : 4조각으로 나눠서 위 아래로\n10 : 회전하면서 작아지게\n11 : 배경을 매우 크게 확대\n12 : 왼쪽에서부터 이미지가 잘려나감\n13 : 오른쪽에서부터 이미지가 잘려나감\n14 : 윗쪽에서부터 이미지가 잘려나감\n15 : 아랫쪽에서부터 이미지가 잘려나감\n",
		"[이름] [파일명] [자동삭제=\"응\"]\n배경 화면을 설정합니다. 별거 없습니다 :)\n[자동삭제]는 배경전환이나 페이드아웃이 되면 자동적으로 삭제되는 지를 결정하는 인수입니다.\n\n예)\n배경 배경1 hellokity.png",
		"[파일명] [X=0] [Y=0]\n캐릭터의 얼굴을 설정합니다. 위치 비 입력시 (0, 0)에 위치합니다.",
		"[빨강] [초록] [파랑] [투명도=255]\n게임에서 아무것도 띄우지 않은 상태에서의 화면 색깔을 선택합니다.",
		"[이름] [X1] [Y1] [X2] [Y2] [속도=1000]\n해당 [이름]의 녀석을 [X1,Y1] 위치로 갔다가 [X2,Y2]위치로 튕기게 합니다.",
		"[이름] [투명도=255]\n해당 녀석의 투명도를 지정해줍니다.",
		"[종류]\n종류 : 세이브, 로드, 회상\n예)\n// 로드 화면을 띄운다.\n시스템화면 로드",
		"[이름] [X] [Y] [가로크기=0] [세로크기=0] [타입] ... [보통 이미지] [롤오버 이미지] [마우스 누를 때이미지] [마우스 올렸을 때 효과음] [눌렀을 때 효과음]\n[타입]에 따라 가운데 인수가 다르게 결정됩니다. 타입에는 다음의 것이 있습니다.\n회상\n점프 [파일이름] [북마크]\n스크립트 [파일명] : 해당 스크립트를 실행합니다.\n자동넘기기 [응/아니]\n빠른스킵 [응/아니]\n다음페이지 : 로드/세이브 슬롯 상에서 페이지 이동\n이전페이지 : 로드/세이브 슬롯 상에서 페이지 이동\n페이지 [번호] : 로드/세이브 슬롯상에서 페이지 이동[1~6]\n변수 [변수명] [값]\n화면모드 [윈도우/풀스크린]\n문장흘림 [응/아니]\n기본폰트 [이름]\n항상위 [응/아니]\n종료\n로드슬롯 [번호] : [번호]의 범위는 1~20.\n세이브슬롯 [번호] : [번호]의 범위는 1~20.\n슬롯삭제 [번호] : [번호]의 범위는 1~20.",
		"\n화면에 나타난 시스템 버튼이나 트랙바를 제거합니다.",
		"[이름] [X세기] [Y세기] [시간=1000] [속도=1000] [X감쇠=0] [Y감쇠=0]\n해당 시간과 속도로 [이름]의 녀석을 흔들어줍니다.\n* 감쇠를 통해 진동이 점점 약해지게 할 수 있습니다.",
		"\n현재 화면에 보인 일반 버튼들을 모두 제거합니다.",
		"[키코드] [파일이름] [북마크이름]\n해당 키를 누르면 해당 위치로 이동하여 실행하도록 합니다. 실행이 완료된 후 다시 현재 파일로 돌아옵니다. [키코드]는 홈페이지를 참조해주세요.",
		"[응/아니]\n현재 대사창에서 대사창이 아닌 대사 글씨만 감추게 합니다.",
		"[A] [=] [B] [파일이름] [북마크이름] [아니면] [파일이름] [북마크이름]\n해당 조건이 맞는지 확인한 후 명령이 실행되도록 합니다. [아니면] 이후는 생략할 수 있습니다.\nA와 B를 비교할때에 = 대신에 !=(같지 않음), >, >=, <, <=, %(문자열 포함 확인), %%(대소문자 무시, 문자열 포함 확인) 가 들어갈 수 있습니다.\n* 변수의 존재여부 확인은 없음과 비교해주세요.\n\n예)\n변수 후후 = 1.5\n조건 후후 = 1.5 여기 A 아니면 여기 B",
		"[이름] [파일이름]\n오마케 CG 목록에 해당 파일을 추가합니다.",
		"[이름] [X] [Y] [가로크기] [세로크기] [없으면 표시할 파일]\n해당 이름으로 등록된 오마케 CG를 불러와 화면에 보여줍니다.",
		"[타입]\n현재 게임 상태를 변경해줍니다. [타입]에는 자동넘기기와 빠른스킵이 있으며, 생략됬을 경우 보통 상태로 돌아옵니다.",
		"[타입] [파일이름] [북마크이름]\n게임 프로그램에서 이벤트가 발생하면 해당 스크립트를 실행합니다. [타입]에는 다음이 있습니다.\n로드후 : 게임이 로드된 후\n세이브후 : 게임이 세이브된 후\n회상후 : 마우스 휠로 회상이 끝난 후\n스킵모드해제후 : 스킵모드가 해제된 후\n이전페이지 : 슬롯이 이전 페이지로 넘어갔을 경우\n다음페이지 : 슬롯이 다음 페이지로 넘어갔을 경우\n메세지박스 : 시스템 메세지 박스가 나타날 때\n\n만약 다음과 같이 한다면 세이브가 된 후에 해당 스크립트 파일을 실행합니다.\n예)\n이벤트 세이브후 세이브후.txt 처음",
		"[파일이름]\n성우의 목소리를 출력합니다. [파일이름]이 생략되면 재생을 중지합니다.",
		"[픽셀]\n대사줄간격을 설정합니다.\n음수도 가질 수 있습니다.",
		"[이름] [값]\n-200 ~ +200까지 출력 순서를 정합니다.\nCG 명령어로 두개의 그림을 출력했을 때 앞뒤 위치를 확실히 해주기 위해 사용합니다.\n기본값은 100이고, 작을수록 앞으로 나옵니다.\n참고로 '대사창'의 출력순서 기본값은 50, '라벨'은 0, '버튼'은 -120 입니다.",
		"[이름] [X] [Y] [R] [G] [B] [A] [텍스트]\n화면에 텍스트를 보여줍니다. 변수를 표시하기 위해서는 {{변수이름}}을 사용합시다.\n[이름]이 '%메세지라벨' 인 경우 특수용도로 사용되는 라벨로써, [X],[Y] 좌표가 라벨의 중심이 됩니다.",
		"[속도] [가속도] [목표X] [목표Y] [초당 비율조정량] [확대비율X] [확대비율Y] [초당 각도조정량] [회전각도]\n\n화면의 중심좌표는 (400, 300)입니다.\n\n확대를 사용하고 싶으지 않으시면 [초당 비율조정량]을 0으로 설정하세요.\n회전을 사용하고 싶으지 않으시면 [초당 각도조정량]을 0으로 설정하세요.\n\n[확대비율X] 와 [확대비율Y]의 기본값은 1.0입니다.\n\n회전각도는 60분법을 사용합니다. 음수도 가능합니다.\n\n예)\n// 간단한 확대후 축소!\n카메라 10 1000 400 300 1 1.5 1.5 2 30\n카메라 10 10000 400 300 1 1.0 1.0 1 0\n",
		"[시간]\n대사를 지우고, 대사창을 감춥니다. 시간은 1/1000초 단위이며, 입력하지 않으면 바로 감추어집니다.",
		"[시간]\n대사창을 보여줍니다. 시간은 1/1000초 단위이며, 입력하지 않으면 바로 보여집니다.",
		"[응,아니]\n대사 글씨를 굵게 한다.\n예)\n대사굵게 응",
		"[이름] [파일이름] [X] [Y] [가로크기] [세로크기] [스킵가능시간=0]\n\n플래시를 화면상에 재생합니다.\n[스킵가능시간]초 만큼 재생 후 스킵이 가능토록합니다. -1 입력시에는 스킵이 아예 불가능합니다.\n예)\n// 불러오기\n플래시 엽기토끼 엽기토끼.swf 0 0 800 600\n\n// 없애기\n플래시 엽기토끼",
		"[파일이름] [속도]\n룰 파일을 사용하여 배경 페이드인을 진행합니다.\n앞으로 '배경' 명령어가 사용될때 마다 보이게 됩니다.\n더이상 페이드인을 싶지 않다면 [파일이름]과 [소요시간] 입력하지 마세요.",
		"[파일이름] [속도]\n룰 파일을 사용하여 배경 페이드아웃을 진행합니다.\n지금까지 보인 배경들을 페이드아웃 합니다.\n배경페이드아웃된 후에는 일반적으로 모든 배경은 사라집니다.",
		"[이름] [파일이름] [생성위치 X(생략가능)] [생성위치 Y(생략가능)]\n'파티클' 폴더에 있는 nkparticle 파일을 불러와 화면에 보여줍니다.\n\n예)\n파티클 눈 눈.nkparticle",
		"[보통 이미지] [마우스가 위에 있을 때의 이미지] [클릭시 이미지] [마우스가 위에 있을 때의 효과음] [클릭시 효과음] [파일 이름] [북마크 이름] [내용]\n\n선택지 버튼을 구성합니다.\n사용하고 싶지 않은 부분에는 '없음' 을 입력해주세요.\n예)\n대사 흠.. 뭘 할지 고민이군....\n선택지버튼 버튼.PNG 버튼2.PNG 버튼3.PNG 없음 클릭시.wav 루트1_1.txt 첫줄 잠이나 자야겠다!\n선택지버튼 버튼.PNG 버튼2.PNG 버튼3.PNG 없음 클릭시.wav 루트1_2.txt 첫줄 게시판에서 낚시나 해볼까!!",
		"[X] [Y] [가로크기] [세로크기]\n선택지 버튼이 나타나는 영역을 설정합니다. 기본 설정 영역은 [0, 100, 800, 400] 입니다.",
		"[이름] [폰트크기] [굵기] [폰트이름]\n화면에 표시된 라벨 텍스트의 속성을 변경합니다. [폰트크기] 혹은 [굵기]에 -1을 입력하면 기존 설정이 유지됩니다.",
		"\n회상에 있는 내용을 모두 지웁니다.",
		"[응,아니]\n카메라 속성을 무시할지 적용할지를 결정합니다.\n'카메라무시' 명령어 이후에 생성되는 CG나 배경만 영향을 받습니다.",
		"[변수이름] [파일이름] [북마크이름]\n해당 이름의 변수의 값이 바뀌면 해당 스크립트를 실행하고, 다시 돌아옵니다.",
		"[이름] [R] [G] [B] [소요시간=0]\n캐릭터의 색조를 변경할 수 있습니다.\n[소요시간]은 아시다시피 1/1000초 입니다 ^^.",
		"[방법]\n[방법]에는 왼쪽, 가운데, 가운데2, 오른쪽이 있으며, 생략할 경우 왼쪽으로 지정됩니다.",
		"[속도=0]\nCG 명령어 사용시 자동적으로 페이드인 시킵니다.\n[속도]가 0일 경우, CG를 페이드인 하지 않고 바로 나타냅니다.",
		"[속도=0]\nCG를 제거 할 때 자동적으로 페이드아웃 시킵니다.\n[속도]가 0일 경우, CG를 페이드아웃 하지 않고 바로 없앱니다.",
		"[응/아니]\n대사흘림 기능을 켜고 끌 수 있습니다.",
		"",
		"[이름] [=] [값]\n'변수' 명령어와 기능은 같으나, 독특한 점은 해당 [이름]의 변수가 이미 존재할 경우 이 명령은 실행되지 않습니다.",
		"[가로크기] [세로크기] [오른쪽여백=5] [폰트크기] [폰트이름]\n회상을 처음 사용할때 필요한 초기화 명령어입니다.\n여기서의 [가로크기]와 [세로크기]는 회상창이 차지하는 크기입니다.\n[오른쪽여백]은 회상 글씨들과 스크롤바 사이의 여백입니다.\n\n'회상' 명령어를 사용하기 전에 넣어주세요.",
		"[이름] [X] [Y] [스크롤바고정크기=0] [스크롤바버튼높이=22] [스킨파일]\n회상 창을 화면에 표시합니다.\n그만 표시하고 싶을 땐 [이름]만 입력하세요.\n스크롤바의 크기가 자동적으로 조절되길 원한다면 [스크롤바고정크기]에 0을 입력하세요.",
		"[일반대사여백] [라벨/대사간 여백] [페이지 당 간격] [빈 줄삽입=0] [R] [G] [B]\n회상에 나타날 텍스트의 설정을 변경합니다\n마지막 [R] [G] [B]는 기본 회상 텍스트 색깔 입니다..",
		"[R] [G] [B]\n앞으로 '대사' 명령 사용시 회상에 나타낼 글씨의 색상입니다.\n아무 것도 입력하지 않으면, 기본 색상을 사용합니다.",
		"[물어볼 글]\n\n'이벤트' 명령어의 '메세지박스'로 등록된 파일에 가서 메세지 박스를 실행합니다.\n메세지 박스가 실행되고 난 뒤에는 변수 '%메세지박스응답'(0 또는 1) 으로 응답을 얻을 수 있습니다.",
		"\n게임내의 커스텀 메세지박스가 닫힐때 까지 대기합니다.",
		"\n'무시 가능한 대기'의 줄임말 입니다.\n\n일반적으로 '대기'와 기능은 비슷하지만 '[대사][무대기][대사잇기][대기]' 와 같은 스크립트에서 스킵할 경우 [대사잇기] 까지 스킵이 진행 됩니다.",
		"[응/아니]\n'응' 일 경우 대사 스킵을 수행할 때 '무대기' 명령 또한 스킵 하도록 합니다. \n기본값은 '응' 입니다.",
		"마지막으로 퀵세이브한 게임을 로드합니다.",
		"현재 게임 진행 상황을 세이브 합니다.",
		"[이름]\n현재 실행중인 쓰레드를 종료합니다.",
		0,
		0,
	};

	// 파라메터는 최대 파라메터 개수를 뜻한다.
	static int32 gCmdParametersCount[ECT_COUNT] = 
	{
		0, // 대기
		3, // 효과음
		2, 1, 1, 1, 3,
		7, // 동영상
		2, 2,
		4, // 회전
		0, 0, 1,
		6, // CG
		-1,
		5, // GIF스프라이트
		1, 4, 2,
		1, 4, 0, 1, 1, 0, 5, 1, 4, 1, 4, 4,
		1, 1, 1, 2, 4, 3, 1, 0, 0, 4, 0, 0,
		15, // 입력
		1, // 매크로시작(사용 안함)
		-1, // 함수
		0,
		-1, // 버튼
		-1,
		2, // 스크립트
		2, 2, 3, 3, 4, 6, 2, 1,
		-1, // 시스템버튼
		0, 7, 0, 3, 1, -1, 2, 6, 1, 3, 1, 1, 2, 8,
		// 카메라
		9, 1, 1, 1,
		7, // 플래시
		2, 2,
		4, // 파티클
		8, // 선택지버튼
		4,
		// 라벨속성
		4,
		0, // 회상지우기
		1, // 카메라무시
		3, // 변수트리거
		5, // 색조
		1, // 대사정렬
		1, // CG페이드인
		1, // CG페이드아웃
		1, // 대사흘림
		1, // 시스템옵션
		3, // 정변수
		5, // 회상초기화
		6, // 회상
		7, // 회상설정
		3, // 회상색깔
		1, // 메세지박스
		0, // 메세지박스대기
		0, // 무대기
		1, // 무대기기능
		0, // 퀵로드
		0, // 퀵세이브
		1, // 쓰레드종료
		0, 0
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////

	inline bool ApprochValue(float32 &which, float32 object,float32 frameTime)
	{
		if(!IsFloatZero(which - object))
		{
			if(which > object)
			{
				which -= frameTime;

				if(which <= object)
				{
					which = object;
					return true;
				}
			}
			if(which < object)
			{
				which += frameTime;

				if(which >= object)
				{
					which = object;
					return true;
				}
			}
			return false;
		}

		return true;
	}

	inline bool ApprochValue(float64 &which, float32 object,float32 frameTime)
	{
		float32 w = which;
		bool res = ApprochValue(w, object, frameTime);
		which = w;
		return res;
	}

	void nekoOneCommand::Begin(const list<nekoOneCommand *>::iterator &selfIter, nekoThread *thread, bool internalRequest)
	{
		nekoGame *currentGame = GetNekoNovel()->GetCurrentGame();

		realArguments = arguments;
		// 변수에 대응
		if(internalRequest == false)
		{
			list<string>::iterator siter = realArguments.begin();
			for(;siter != realArguments.end();++siter)
			{
				(*siter) = currentGame->ParseString((*siter).c_str());
			}
		}

		const list<string> &arguments = realArguments;
		list<string>::const_iterator iter = realArguments.begin();

		// 선택지버튼 명령어 미리보기에서 벗어난다..
		if(cmdType != ECT_SELECTION_BUTTON)
			currentGame->GetSelectionButtons()->ClearButtons();

		switch(cmdType)
		{
		case ECT_TEXT:
		case ECT_APTEXT:
			{
				if(arguments.size() == 1)
				{
					const char8 *prevVoiceName = 0;
					if(!internalRequest
						&& selfIter != thread->mCommands->GetStartingPos())
					{
						list<nekoOneCommand *>::iterator iter = selfIter;
						--iter;

						if((*iter)->cmdType == ECT_VOICE
							&& !(*iter)->arguments.empty())
						{
							prevVoiceName = (*(*iter)->arguments.begin()).c_str();
						}
					}
					if(cmdType == ECT_TEXT)
					{
						currentGame->GetChatBox()->ClearBuffer();
						currentGame->WriteText(prevVoiceName, (*iter).c_str(), false);
					}
					else
					{
						currentGame->WriteText(prevVoiceName, (*iter).c_str(), true);
					}
				}
				else
				{
					if(cmdType == ECT_TEXT)
					{
						currentGame->GetChatBox()->ClearBuffer();
						currentGame->WriteText(0, "", false);
					}
				}

				if(internalRequest == false)
				{
					if(cmdType == ECT_TEXT)
					{
						currentGame->RemoveFlags(GAME_FLAG_LAST_SKIPPED);
					}
					else
					{
						// 스킵 시켜주기!!
						if(currentGame->IsTurnOn(GAME_FLAG_LAST_SKIPPED))
						{
							currentGame->GetChatBox()->Skip();
							break;
						}
					}

					if(IsFloatZero(currentGame->GetEnvironment().GetSpeechSpeed()))
						currentGame->GetChatBox()->Skip();
				}
			}
			break;
		case ECT_NEW_LINE:
			{
				currentGame->WriteText(0, "\n", true);

				if(internalRequest == false)
				{
					// 스킵 시켜주기!!
					if(currentGame->IsTurnOn(GAME_FLAG_LAST_SKIPPED))
					{
						currentGame->GetChatBox()->Skip();
						break;
					}

					if(IsFloatZero(currentGame->GetEnvironment().GetSpeechSpeed()))
						currentGame->GetChatBox()->Skip();
				}
			}
			break;
		case ECT_INPUT_WAIT:
		case ECT_INPUT_WAIT2:
			{
				mTempInteger[0] = 0;
				mTempInteger[1] = 0; // 무시 불가능

				if(internalRequest == false)
				{
					if(currentGame->GetSystemState() == EGFS_QUICKSKIPPING)
						return;

					currentGame->SetGameSystemState(EGSS_WAIT);

					// 자동 모드시 2초간 기다림.
					mTempTimer[1].SetTime(2.0f);
					mTempTimer[1].Clear();

					currentGame->GetChatBox()->SetCursorVisible(true);	

					// 무대기 무시 기능이 켜지고!! 무대기 일때!
					if(currentGame->GetEnvironment().mIW2Use == true && cmdType == ECT_INPUT_WAIT2)
						mTempInteger[1] = 1;
					else
						currentGame->RemoveFlags(GAME_FLAG_LAST_SKIPPED);
				}
			}
			break;
		case ECT_CG:
			{
				bool firstInit = false;

				mTempInst = 0;
				if(arguments.size() >= 1)
				{
					const string &name = (*iter);
					nekoImageInstance *image = (nekoImageInstance *)currentGame->CreateInstance(BLOCK_IMAGE, name.c_str(), &firstInit);
					if(image)
					{
						++iter;
						if(iter != arguments.end() && (*iter).length() > 0)
						{
							// 처음에만 좌표 부여!
							if(firstInit)
								image->GetBoundingBox().MoveTo(currentGame->GetEnvironment().mPrintX, currentGame->GetEnvironment().mPrintY);

							(mTempInst = image)->AddRef();
							image->SetImage((*iter++).c_str());
							
							if(iter != arguments.end())
							{
								image->GetBoundingBox().left = image->GetBoundingBox().right = atoi((*iter++).c_str());

								if(iter != arguments.end())
								{
									image->GetBoundingBox().top = image->GetBoundingBox().bottom = atoi((*iter++).c_str());

									// 세이브 화면에 출력 여부 설정
									if(iter != arguments.end())
									{
										image->SetIncludedInSnapShot((*iter++) != "아니");

										// 출력 순서 설정
										if(iter != arguments.end())
										{
											image->SetZOrder(atoi((*iter).c_str()));
										}

									}
								}
							}

							if(internalRequest == false)
							{
								if(selfIter != thread->mCommands->GetEndingPos())
								{
									// 바로 다음줄이 페이드인 인가 ?
									list<nekoOneCommand *>::iterator nextIter = selfIter;
									if(++nextIter != thread->mCommands->GetEndingPos() &&
										(*nextIter)->cmdType == ECT_FADEIN && !(*nextIter)->arguments.empty())
									{
										// 만약 같다면..
										if(stricmp((*(*nextIter)->arguments.begin()).c_str(), name.c_str()) == 0)
										{
											image->IgnoreAutoFadeInOut();
											
											// 쓰레드에서 때문에..
											image->SetAlpha(0);
										}
									}
								}
							}
						}
						else
						{
							// 제거 요청
							if(!image->RequestToRemove())
							{
								currentGame->RemoveInstance(image);
							}
							else
							{
								// 이쪽에는 internalRequest == false 인 경우에만 온다..
								(mTempInst = image)->AddRef();

								if(selfIter != thread->mCommands->GetEndingPos())
								{
									// 바로 다음줄이 페이드아웃 인가 ?
									list<nekoOneCommand *>::iterator nextIter = selfIter;
									if(++nextIter != thread->mCommands->GetEndingPos() &&
										(*nextIter)->cmdType == ECT_FADEOUT && !(*nextIter)->arguments.empty())
									{
										// 만약 같다면..
										if(stricmp((*(*nextIter)->arguments.begin()).c_str(), name.c_str()) == 0)
										{
											image->IgnoreAutoFadeInOut();
										}
									}
								}
							}
						}
					}
				}
				else
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어는 인수를 적어도 1개 가져요.", startLine, gCommandStrings[ECT_CG]);
				}
			}
			break;
		case ECT_PRINTPOS:
			{
				if(arguments.size() == 2)
				{
					currentGame->GetEnvironment().mPrintX = atoi((*iter++).c_str());
					currentGame->GetEnvironment().mPrintY = atoi((*iter++).c_str());
				}
				else
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어는 인수를 2개 가져요.", startLine, gCommandStrings[ECT_PRINTPOS]);
				}
			}
			break;
		case ECT_BACKGROUND:
			{
				if(arguments.size() >= 2)
				{
					nekoBGImageInstance *image = (nekoBGImageInstance *)currentGame->CreateInstance(BLOCK_BGIMAGE, (*iter++).c_str());
					image->SetImage((*iter++).c_str());

					if(iter != arguments.end())
					{
						image->SetZeroAlphaDeleting((*iter) != "아니");
					}

					(mTempInst = image)->AddRef();
				}
				else if(arguments.size() == 1)
				{
					nekoBGImageInstance *image = (nekoBGImageInstance *)currentGame->GetInstance((*iter).c_str());
					if(image)
						currentGame->RemoveInstance(image);
				}
			}
			break;
		case ECT_BGMUSIC:
			{
				if(internalRequest)
					return;

				nekoBGMusicInstance *music = (nekoBGMusicInstance *)currentGame->CreateInstance(BLOCK_BGMUSIC, "");
				if(arguments.size() == 2)
				{
					uint32 repeat = ((*iter) == "반복") ? -1 : atoi((*iter).c_str());
					++iter;

					music->SetMusic((*iter).c_str(), repeat);
				}
				else if(arguments.size() == 1)
				{
					music->SetMusic((*iter).c_str(), 1);
				}
				else
				{
					currentGame->RemoveInstance(music);
					return;
				}
			}
			break;
		case ECT_SOUND:
			{
				if(internalRequest)
					return;

				if(arguments.size() < 1)
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어는 인수를 1개 이상 가져요.", startLine, gCommandStrings[ECT_SOUND]);
					return;
				}
				
				nekoSoundInstance *sound = (nekoSoundInstance *)currentGame->CreateInstance(BLOCK_SOUND, (*iter++).c_str());
				if(arguments.size() >= 2)
				{
					uint32 repeat = 1;
					const string &fileName = (*iter++);

					if(iter != arguments.end())
					{
						repeat = ((*iter) == "반복") ? -1 : atoi((*iter).c_str());
					}

					sound->SetSound(fileName.c_str(), repeat);
				}
				else
				{
					currentGame->RemoveInstance(sound);
				}

			}
			break;
		case ECT_DELAY:
			{
				if(arguments.size() != 1)
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어는 인수를 1개 가져요.", startLine, gCommandStrings[ECT_DELAY]);
					return;
				}

				if(internalRequest)
					return;

				mTempTimer[0].SetTime(atoi((*iter).c_str()) * 0.001f);
				mTempTimer[0].Clear();

				//currentGame->GetChatBox()->SetCursorVisible(false);
			}
			break;
		case ECT_TEXT_SPEED:
			{
				if(arguments.size() != 1)
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어는 인수를 1개 가져요.", startLine, gCommandStrings[ECT_TEXT_SPEED]);
					return;
				}

				GetNekoNovel()->GetCurrentGame()->GetEnvironment().mSpeechSpeed = atoi((*iter).c_str()) * 0.001f;
			}
			break;
		case ECT_FADEIN:
		case ECT_FADEOUT:
			{
				if(!arguments.size())
				{
					mTempInst = 0;
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어는 인수가 적어도 1개 필요해요.", startLine, gCommandStrings[cmdType]);
					return;
				}
				
				nekoIBlockInstance *inst = currentGame->GetInstance((*iter).c_str());
				if(inst == NULL)
				{
					mTempInst = 0;
					//if(internalRequest == false)
					//	LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어에서 '%s' 라는 녀석을 찾을 수 없는걸요!", startLine, gCommandStrings[cmdType], (*iter).c_str());
					return;
				}

				(mTempInst = inst)->AddRef();

				if(internalRequest)
					return;

				// 소요 시간도 있을 경우
				if(arguments.size() >= 2)
				{
					++iter;
					mTempTimer[0].SetTime(atoi((*iter).c_str()) * 0.001f);

					if(mTempTimer[0].GetTimerTime() > 0)
					{
						mTempFloat[1] = (cmdType == ECT_FADEIN) ? 0 : (inst->GetAlpha());

						if(cmdType == ECT_FADEIN)
							mTempFloat[0] = +(255 - mTempFloat[1]) / mTempTimer[0].GetTimerTime();
						else
							mTempFloat[0] = -(mTempFloat[1]) / mTempTimer[0].GetTimerTime();

						inst->SetAlpha(mTempFloat[1]);
					}
				}
				else
				{
					mTempTimer[0].SetTime(0);
				}

				mTempTimer[0].Clear();
			}
			break;
		case ECT_PLAY:
			{
				nekoIBlockInstance *inst = currentGame->GetInstance((*iter).c_str());
				if(inst == NULL)
				{
					mTempInteger[0] = -1;
					if(internalRequest == false)
						LogPrint(LOG_LEVEL_WARN, "%d줄 - %s에서 '%s' 라는 녀석을 찾을 수 없는걸요!", startLine, gCommandStrings[cmdType], (*iter).c_str());
					return;
				}

				inst->Play();
			}
			break;
		case ECT_STOP:
			{
				nekoIBlockInstance *inst = currentGame->GetInstance((*iter).c_str());
				if(inst == NULL)
				{
					mTempInteger[0] = -1;
					if(internalRequest == false)
						LogPrint(LOG_LEVEL_WARN, "%d줄 - %s에서 '%s' 라는 녀석을 찾을 수 없는걸요!", startLine, gCommandStrings[cmdType], (*iter).c_str());
					return;
				}

				inst->Stop();
			}
			break;
		case ECT_PAUSE:
			{
				nekoIBlockInstance *inst = currentGame->GetInstance((*iter).c_str());
				if(inst == NULL)
				{
					mTempInteger[0] = -1;
					if(internalRequest == false)
						LogPrint(LOG_LEVEL_WARN, "%d줄 - %s에서 '%s' 라는 녀석을 찾을 수 없는걸요!", startLine, gCommandStrings[cmdType], (*iter).c_str());
					return;
				}

				inst->Stop(true);
			}
			break;
		case ECT_ROTATE:
			{
				if(arguments.size() < 2)
				{
					mTempInteger[0] = -1;
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어는 인수가 적어도 2개 필요해요.", startLine, gCommandStrings[cmdType]);
					return;
				}

				nekoIBlockInstance *inst = currentGame->GetInstance((*iter).c_str());
				if(inst == NULL)
				{
					mTempInteger[0] = -1;
					if(internalRequest == false)
						LogPrint(LOG_LEVEL_WARN, "%d줄 - %s에서 '%s' 라는 녀석을 찾을 수 없는걸요!", startLine, gCommandStrings[cmdType], (*iter).c_str());
					return;
				}

				mTempInteger[0] = (int32)inst;

				++iter;
				float64 angle = D3DXToRadian(atof((*iter).c_str()));
				mTempFloat[1] = angle;

				if(internalRequest)
					return;

				mTempFloat[0] = inst->GetRotation();

				// 속도도 잇을 경우엔...
				if(arguments.size() >= 3)
				{
					mTempFloat[2] = atof((*++iter).c_str());

					/// 가속도
					if(arguments.size() >= 4)
						mTempFloat[3] = atof((*++iter).c_str());
					else
						mTempFloat[3] = 0;
				}
				else
				{
					mTempFloat[2] = mTempFloat[3] = 0;
				}
			}
			break;
		case ECT_SCALE:
			{
				if(arguments.size() < 3)
				{
					mTempInteger[0] = -1;
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어는 인수가 적어도 3개 필요해요.", startLine, gCommandStrings[cmdType]);
					return;
				}

				nekoIBlockInstance *inst = currentGame->GetInstance((*iter).c_str());
				if(inst == NULL)
				{
					mTempInteger[0] = -1;
					if(internalRequest == false)
						LogPrint(LOG_LEVEL_WARN, "%d줄 - %s에서 '%s' 라는 녀석을 찾을 수 없는걸요!", startLine, gCommandStrings[cmdType], (*iter).c_str());
					return;
				}

				mTempInteger[0] = (int32)inst;

				++iter;
				mTempFloat[4] = atof((*iter++).c_str());
				mTempFloat[5] = atof((*iter).c_str());

				if(internalRequest)
					return;

				// 소요 시간도 있을 경우
				if(arguments.size() >= 4)
				{
					++iter;
					mTempTimer[0].SetTime(atoi((*iter).c_str()) * 0.001f);

					if(mTempTimer[0].GetTimerTime() > 0)
					{
						nekoPointF &scale = inst->GetScale();
						mTempFloat[2] = scale.x;
						mTempFloat[3] = scale.y;
						mTempFloat[0] = (mTempFloat[4] - scale.x) / mTempTimer[0].GetTimerTime();
						mTempFloat[1] = (mTempFloat[5] - scale.y) / mTempTimer[0].GetTimerTime();
					}
				}
				else
				{
					mTempTimer[0].SetTime(0);
				}

				mTempTimer[0].Clear();
			}
			break;
		case ECT_MOVE:
			{
				mTempInst = 0;

				if(arguments.size() < 3)
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어는 인수가 적어도 3개 필요해요.", startLine, gCommandStrings[cmdType]);
					return;
				}

				nekoIBlockInstance *inst = currentGame->GetInstance((*iter).c_str());
				if(inst == NULL)
				{
					if(internalRequest == false)
						LogPrint(LOG_LEVEL_WARN, "%d줄 - %s에서 '%s' 라는 녀석을 찾을 수 없는걸요!", startLine, gCommandStrings[cmdType], (*iter).c_str());
					return;
				}

				(mTempInst = inst)->AddRef();

				++iter;
				mTempFloat[0] = inst->GetBoundingBox().min.x;
				mTempFloat[1] = inst->GetBoundingBox().min.y;
				mTempFloat[4] = atoi((*iter++).c_str());
				mTempFloat[5] = atoi((*iter).c_str());
				nekoPointF dir = nekoPointF(mTempFloat[4] - inst->GetBoundingBox().min.x, 
					mTempFloat[5] - inst->GetBoundingBox().min.y) ;
				dir.Normalize();
				mTempFloat[6] = abs(dir.x);
				mTempFloat[7] = abs(dir.y);

				if(internalRequest)
					return;

				// 속도도 있을 경우엔...
				if(arguments.size() >= 4)
				{
					mTempFloat[2] = atof((*++iter).c_str());

					/// 가속도
					if(arguments.size() >= 5)
						mTempFloat[3] = atof((*++iter).c_str());
					else
						mTempFloat[3] = 0;
				}
				else
				{
					mTempFloat[2] = mTempFloat[3] = 0;
				}

				//mTempTimer[0].Clear();
			}
			break;
		case ECT_TEXT_SKIN:
			{
				if(arguments.size() == 1)
				{
					GetNekoNovel()->GetCurrentGame()->GetChatBox()->SetSkin((*iter).c_str());
				}
				else
				{
					GetNekoNovel()->GetCurrentGame()->GetChatBox()->SetSkin(NULL);
				}
			}
			break;
		case ECT_TEXT_COLOR:
			{
				if(arguments.size() < 3)
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어는 인수가 3개 필요해요.", startLine, gCommandStrings[cmdType]);
					return;
				}
				uint32 color = 0xff000000;
				color |= ((atoi((*iter).c_str()) & 0xff) << 16);
				++iter;
				color |= ((atoi((*iter).c_str()) & 0xff) << 8);
				++iter;
				color |= (atoi((*iter).c_str()) & 0xff);

				if(arguments.size() == 4)
				{
					++iter;
					color = (color & 0xffffff) | ((atoi((*iter).c_str()) & 0xff) << 24);
				}

				GetNekoNovel()->GetCurrentGame()->GetEnvironment().mSpeechColor = color;
				GetNekoNovel()->GetCurrentGame()->GetChatBox()->PushNewBlock();
			}
			break;
		case ECT_TEXT_SHADOW_COLOR:
			{
				if(arguments.size() < 3)
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어는 인수가 3개 필요해요.", startLine, gCommandStrings[cmdType]);
					return;
				}
				uint32 color = 0x0;
				color |= ((atoi((*iter).c_str()) & 0xff) << 16);
				++iter;
				color |= ((atoi((*iter).c_str()) & 0xff) << 8);
				++iter;
				color |= (atoi((*iter++).c_str()) & 0xff);
				if(iter != arguments.end())
					color |= ((atoi((*iter).c_str()) & 0xff) << 24);
				else
					color |= 0xff000000;

				GetNekoNovel()->GetCurrentGame()->GetEnvironment().mSpeechShadowColor = color;
				GetNekoNovel()->GetCurrentGame()->GetChatBox()->PushNewBlock();
			}
			break;
		case ECT_TEXT_SHADOW_TYPE:
			{
				if(arguments.size() < 1)
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어는 인수가 1개 필요해요.", startLine, gCommandStrings[cmdType]);
					return;
				}

				GetNekoNovel()->GetCurrentGame()->GetEnvironment().mSpeechShadowType = atoi((*iter).c_str());
				GetNekoNovel()->GetCurrentGame()->GetChatBox()->PushNewBlock();
			}
			break;
		case ECT_TEXT_SIZE:
			{
				if(arguments.size() != 1)
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어는 인수가 1개 필요해요.", startLine, gCommandStrings[cmdType]);
					return;
				}

				GetNekoNovel()->GetCurrentGame()->GetChatBox()->SetFont(NULL, atoi((*iter).c_str()), -1);
			}
			break;
		case ECT_TEXT_FONT:
			{
				if(arguments.size() != 1)
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어는 인수가 1개 필요해요.", startLine, gCommandStrings[cmdType]);
					return;
				}

				GetNekoNovel()->GetCurrentGame()->GetChatBox()->SetFont((*iter).c_str(), -1, -1);
			}
			break;
		case ECT_CLEAR_ALL:
			{
				GetNekoNovel()->GetCurrentGame()->ClearAllInstances();
			}
			break;
		case ECT_CLEAR_TEXT:
			{
				GetNekoNovel()->GetCurrentGame()->GetChatBox()->ClearBuffer();
				if(internalRequest == false)
					GetNekoNovel()->GetCurrentGame()->GetMessageLogger()->WriteText(0, 0, "\n", true);
			}
			break;
		case ECT_SELECTION:
			{
				if(arguments.size() < 3)
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어는 인수가 3개 필요해요.", startLine, gCommandStrings[cmdType]);
					return;
				}

				const string &arg1 = (*iter++); const string &arg2 = (*iter++);

				/// 게임에 추가..
				GetNekoNovel()->GetCurrentGame()->AddSelection(arg1, arg2);

				/// 실제로 대사창에 추가..
				currentGame->GetChatBox()->WriteText((*iter).c_str(), ETT_SELECTION);

				if(!internalRequest)
					currentGame->GetChatBox()->SetCursorVisible(false);
			}
			break;
		case ECT_SELECTION_COLOR:
			{
				if(arguments.size() < 3)
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어는 인수가 적어도 3개 필요해요.", startLine,gCommandStrings[cmdType]);
					return;
				}
				uint32 color = 0x0;
				color |= ((atoi((*iter).c_str()) & 0xff) << 16);
				++iter;
				color |= ((atoi((*iter).c_str()) & 0xff) << 8);
				++iter;
				color |= (atoi((*iter).c_str()) & 0xff);
				if(iter != arguments.end())
					color |= ((atoi((*iter).c_str()) & 0xff) << 24);
				else
					color |= 0xff000000;

				GetNekoNovel()->GetCurrentGame()->GetEnvironment().mSelectionColor = color;
			}
			break;
		case ECT_THREAD_DECLARE:
			{
				if(internalRequest)
					return;

				if(arguments.size() == 1)
				{
					nekoThread newThread(thread->mCommands, false);
					newThread.mName = (*iter).c_str();
					newThread.SetPlayingPos(thread->GetPlayingPos());
					++newThread.GetPlayingPos();
					if(FAILED(currentGame->AddThread(newThread)))
					{
						LogPrint(LOG_LEVEL_WARN, "%d줄 - 쓰레드(%s)의 끝이 보이지 않습니다! 먼산..)", startLine, (*iter).c_str());
						return;
					}
				}
			}
			break;
		case ECT_THREAD_START:
			{
				if(internalRequest)
					return;

				currentGame->StartThreads();
			}
			break;
		case ECT_SPRITE:
			{
				mTempInst = 0;

				if(arguments.size() < 1)
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어는 인수가 적어도 1개 필요해요.", startLine, gCommandStrings[cmdType]);
					return;
				}

				if(arguments.size() >= 3)
				{
					bool firstInitialize;
					nekoSpriteInstance *inst = (nekoSpriteInstance *)currentGame->CreateInstance(BLOCK_SPRITE, (*iter++).c_str(), &firstInitialize);
					float32 delayTime = atof((*iter++).c_str());

					inst->SetDelayTimer(delayTime * 0.001f);

					for(;iter != arguments.end();++iter)
					{
						inst->AddTexture((*iter).c_str());
					}

					
					// 처음 객체가 생성되는 것이라면..
					if(firstInitialize == true)
					{
						inst->GetBoundingBox().MoveTo(currentGame->GetEnvironment().mPrintX, currentGame->GetEnvironment().mPrintY);
					}
				}
				else
				{
					nekoIBlockInstance *inst = currentGame->GetInstance((*iter).c_str());
					if(inst == NULL)
					{
						if(!internalRequest)
							LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어에서 '%s' 녀석을 찾지 못하였습니다.", startLine, gCommandStrings[cmdType], (*iter).c_str());
						return;
					}
					if(inst->GetType() != BLOCK_SPRITE)
					{
						if(!internalRequest)
							LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어에서 '%s'는 스프라이트가 아닙니다.", startLine, gCommandStrings[cmdType], (*iter).c_str());
						return;
					}

					currentGame->RemoveInstance(inst);
				}
			}
			break;
		case ECT_SKIP_START:
			{
				GetNekoNovel()->GetCurrentGame()->EnableSkipAble();
			}
			break;
		case ECT_SKIP_END:
			{
				GetNekoNovel()->GetCurrentGame()->DisableSkipAble();
			}
			break;
		case ECT_JUMP:
			{
				if(internalRequest)
					return;
				
				if(!thread->mIsStack)
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - 쓰레드 내에서는 '점프' 명령어를 사용할 수 없습니다.", startLine);
					return;
				}

				if(arguments.size() == 1)
				{
					list<nekoOneCommand *>::iterator iter2;
					if(FAILED(thread->mCommands->SearchBookMark((*iter).c_str(), iter2)))
					{
						LogPrint(LOG_LEVEL_WARN, "%d줄 - 북마크 '%s'를 찾지 못했습니다.", startLine, (*iter).c_str());
						return;
					}

					thread->SetPlayingPos(iter2);
					if(iter2 != thread->mCommands->GetEndingPos())
						(*iter2)->Begin(iter2, thread, internalRequest);
				}
				else if(arguments.size() == 2)
				{
					// 사라질 수 있으니까..
					string arg1 = (*iter++);
					string arg2 = (*iter);
					thread->SetPlayingPos(thread->mCommands->GetEndingPos());
					if(FAILED(currentGame->JumpTo(arg1.c_str(), (arg2 == "첫줄") ? NULL : arg2.c_str())))
					{
						LogPrint(LOG_LEVEL_WARN, "%d줄 - '%s' 파일의 북마크 '%s'를 찾지 못했습니다.", startLine, arg1.c_str(), arg2.c_str());
						return;
					}
					return;
				}
			}
			break;
		case ECT_SYSTEM_MENU_ENABLE:
			{
				/*
				if(arguments.size() == 1)
				{
					if((*iter) == "아니")
					{
						currentGame->EnableSystemMenu(false);
						return;
					}
				}

				currentGame->EnableSystemMenu(true);
				*/
			}
			break;
		case ECT_SCRIPT:
			{
				if(!thread->mIsStack)
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - 쓰레드 내에서는 '%s' 명령어를 사용할 수 없습니다.", startLine, gCommandStrings[cmdType]);
					return;
				}

				if(arguments.size() < 1)
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - '%s' 명령어는 인수가 적어도 1개 필요합니다.", startLine, gCommandStrings[cmdType]);
					return;
				}

				const string &scriptName = (*iter++);
				const char8 *bookMarkName = 0;
				if(iter != arguments.end())
					bookMarkName = (*iter).c_str();

				currentGame->PlayCommands(scriptName.c_str(), bookMarkName, (internalRequest) ? true : false, internalRequest);
			}
			break;
		case ECT_BUTTON:
			{
				LimitMinArguments(6);

				const string &name = (*iter++);

				nekoPointF pt;
				pt.x = atoi((*iter++).c_str());
				pt.y = atoi((*iter++).c_str());

				nekoPoint size;
				size.x = atoi((*iter++).c_str());
				size.y = atoi((*iter++).c_str());

				const string &type = (*iter++);

				nekoButtonInstance *inst = (nekoButtonInstance *)currentGame->CreateInstance(BLOCK_BUTTON, name.c_str());

				if(type == "점프")
				{
					if(arguments.size() < 8)
					{
						LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어 중 '점프'는 인수가 적어도 8개 필요합니다.", startLine, gCommandStrings[cmdType]);
						return;
					}

					const string &arg1 = (*iter++);
					const string &arg2 = (*iter++);

					inst->SetJump(arg1, arg2);
				}
				else if(type == "링크")
				{
					if(arguments.size() < 7)
					{
						LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어 중 '링크'는 인수가 적어도 7개 필요합니다.", startLine, gCommandStrings[cmdType]);
						return;
					}

					const string &arg1 = (*iter++);
					inst->SetLink(arg1);
				}
				else if(type == "로드")
					inst->SetButtonType(EBIT_LOAD);
				else if(type == "옵션")
					inst->SetButtonType(EBIT_OPTION);
				else if(type == "종료")
					inst->SetButtonType(EBIT_END);
				else if(type == "배경음")
				{
					if(arguments.size() < 7)
					{
						LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어 중 '배경음'는 인수가 적어도 7개 필요합니다.", startLine, gCommandStrings[cmdType]);
						return;
					}

					inst->SetMusic((*iter++));
				}
				else if(type == "효과음")
				{
					if(arguments.size() < 7)
					{
						LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어 중 '효과음'은 인수가 적어도 7개 필요합니다.", startLine, gCommandStrings[cmdType]);
						return;
					}

					inst->SetFXSound((*iter++));
				}
				else if(type == "스크립트")
				{
					if(arguments.size() < 7)
					{
						LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어 중 '스크립트'는 인수가 적어도 7개 필요합니다.", startLine, gCommandStrings[cmdType]);
						return;
					}

					inst->SetScriptType((*iter++));
				}
				else if(type == "포토모드")
					inst->SetButtonType(EBIT_PHOTOMODE);
				else if(type == "메세지확인")
					inst->SetButtonType(EBIT_MSGBOX_OK);
				else if(type == "메세지취소")
					inst->SetButtonType(EBIT_MSGBOX_CANCEL);
				else if(type == "없음")
					inst->SetButtonType(EBIT_NOTHING);
				else
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어에서 '%s' 타입은 존재하지 않아요.", startLine, gCommandStrings[cmdType], type.c_str());

					currentGame->RemoveInstance(inst);
					return;
				}

				/// 버튼 이미지 지정
				if(iter != arguments.end())
				{
					inst->SetNormalImage((*iter++));

					if(iter != arguments.end())
						inst->SetRolloverImage((*iter++));

					if(iter != arguments.end())
						inst->SetPressedImage((*iter++));

					// 버튼 사운드 지정
					if(iter != arguments.end())
						inst->SetRolloverSound((*iter++));

					// 버튼 사운드 지정
					if(iter != arguments.end())
						inst->SetPressedSound((*iter++));
				}

				inst->GetBoundingBox().MoveTo(pt.x, pt.y);

				if(size.x > 0)
					inst->GetBoundingBox().SetWidth(size.x);
				else
					inst->AddFlags(EBIF_AUTO_SIZE_X);

				if(size.y > 0)
					inst->GetBoundingBox().SetHeight(size.y);
				else
					inst->AddFlags(EBIF_AUTO_SIZE_Y);

			}
			break;
		case ECT_TRACKBAR:
			{
				if(arguments.size() < 5)
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어는 인수가 적어도 5개 필요합니다.", startLine, gCommandStrings[cmdType]);
					return;
				}

				nekoRectangle rect;
				rect.min.x = atoi((*iter++).c_str());
				rect.min.y = atoi((*iter++).c_str());
				rect.max.x = rect.min.x + atoi((*iter++).c_str());
				rect.max.y = rect.min.y + atoi((*iter++).c_str());
				const string &type = (*iter++);

				nekoTrackBarInstance *inst = (nekoTrackBarInstance *)currentGame->CreateInstance(BLOCK_TRACKBAR, "");

				if(type == "보이스")
				{
					inst->SetTrackBarType(ETBIT_VOICE);
				}
				else if(type == "효과음")
				{
					inst->SetTrackBarType(ETBIT_FX);
				}
				else if(type == "배경음")
				{
					inst->SetTrackBarType(ETBIT_BGM);
				}
				else if(type == "대사창투명도")
				{
					inst->SetTrackBarType(ETBIT_TEXTWINDOW);
				}
				else if(type == "변수")
				{
					if(arguments.size() < 7)
					{
						LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어 중 '변수'는 인수가 적어도 7개 필요합니다.", startLine, gCommandStrings[cmdType]);
						return;
					}		

					inst->SetTrackBarType(ETBIT_VARIABLE);
					inst->SetMaxValue(atoi((*iter++).c_str()));
					inst->SetTrackBarVariable(*iter++);
				}
				else if(type == "대사속도")
				{
					inst->SetTrackBarType(ETBIT_SUB_TEXTSPEED);
				}
				else if(type == "오토딜레이")
				{
					inst->SetTrackBarType(ETBIT_AUTO_DELAY);
				}
				else if(type == "없음")
				{

				}
				else
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어에서 '%s' 타입은 존재하지 않아요.", startLine, gCommandStrings[cmdType], type.c_str());

					currentGame->RemoveInstance(inst);
					return;
				}

				/// 바 이미지 지정
				if(iter != arguments.end())
				{
					inst->SetBarTexture((*iter++));
				}

				inst->GetBoundingBox() = rect;
			}
			break;
		case ECT_BUTTON_WAIT:
			{
				if(internalRequest)
					return;

				currentGame->EnableButtonWait();
			}
			break;
		case ECT_SET_VARIABLE:
		case ECT_SET_ONE_VARIABLE:
			{
				if(internalRequest) return;

				if(arguments.size() < 1)
					return;

				const string &varName = (*iter++).c_str();
				nekoVariable *variable = currentGame->GetVariable(varName.c_str());

				if(iter == arguments.end())
				{
					if(variable)
						currentGame->RemoveVariable(variable);
					return;
				}

				E_OPERATOR_TYPE opType = GetOperatorFrom((*iter++).c_str());

				if(iter == arguments.end())
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어에서 연산자 오른쪽에 값이 없습니다.", startLine, gCommandStrings[cmdType]);
					return;
				}

				if(variable == NULL)
					variable = currentGame->AddVariable(varName.c_str());
				else
				{
					// '정변수' 명령 일 경우
					if(cmdType == ECT_SET_ONE_VARIABLE)
						return;
				}

				string val;

				// 혹시 랜덤임 ?
				if(strncmp((*iter).c_str(), "랜덤", 4) == 0)
				{
					const char8 *p = (*iter).c_str() + 5;
					int32 beginNum = atoi(currentGame->ParseString(p).c_str());
					int32 endNum = beginNum;
					
					while(*p != '\0')
					{
						if(*p == '~')
						{
							++p;
							endNum = atoi(currentGame->ParseString(p).c_str());
							break;
						}

						++p;
					}

					val = nekoUtility::FormString("%d", beginNum + ((endNum - beginNum + 1) ? (rand() % (endNum - beginNum + 1)) : 0));
				}
				else
				{
					val = (*iter);
				}

				switch(opType)
				{
				case EOT_EQUAL:
					{
						variable->SetValue(val.c_str());
					}
					break;
				case EOT_ADDEQUAL:
				case EOT_SUBTRACTEQUAL:
				case EOT_MULTIPLYEQUAL:
				case EOT_DIVIDEEQUAL:
				case EOT_DIVIDELEFTEQUAL:
					{
						if(FAILED(variable->OperateValue(opType, val.c_str())))
						{
							LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어에서 연산이 실패하였습니다.", startLine, gCommandStrings[cmdType]);
							return;
						}
					}
					break;
				default:
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어에서 연산자가 잘못되었습니다.", startLine, gCommandStrings[cmdType]);
					return;
				}

				currentGame->OnVariableChanged(varName, variable);
			}
			break;
		case ECT_BGFADEIN:
			{
				if(!arguments.size())
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어에서 인수가 적어도 1개 필요합니다.", startLine, gCommandStrings[cmdType]);
					return;
				}

				float32 speed = 1.0f;
				int32 fadeType = atoi((*iter++).c_str());
				if(arguments.size() >= 2)
					speed = atof((*iter).c_str()) * 0.001f;

				currentGame->SetEffectToBG((E_BGIMAGE_EFFECT_TYPE)fadeType, speed, true);
			}
			break;
		case ECT_BGFADEOUT:
			{
				if(internalRequest)
					return;

				if(!arguments.size())
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어에서 인수가 적어도 1개 필요합니다.", startLine, gCommandStrings[cmdType]);
					return;
				}

				float32 speed = 1.0f;
				int32 fadeType = atoi((*iter++).c_str());
				if(arguments.size() >= 2)
					speed = atof((*iter).c_str()) * 0.001f;

				currentGame->SetEffectToBG((E_BGIMAGE_EFFECT_TYPE)fadeType, speed, false);
			}
			break;
		case ECT_TEXT_WINDOW_RECT:
			{
				if(arguments.size() < 4)
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어에서 인수가 적어도 4개 필요합니다.", startLine, gCommandStrings[cmdType]);
					return;
				}

				const string &arg1 = (*iter++); const string &arg2 = (*iter++);
				const string &arg3 = (*iter++); const string &arg4 = (*iter++);

				currentGame->GetChatBox()->GetBoundingBox().MoveTo(atoi(arg1.c_str()), atoi(arg2.c_str()));
				currentGame->GetChatBox()->GetBoundingBox().SetSize(atoi(arg3.c_str()), atoi(arg4.c_str()));
			}
			break;
		case ECT_TEXT_PRINTING_RECT:
			{
				if(arguments.size() < 4)
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어에서 인수가 적어도 4개 필요합니다.", startLine, gCommandStrings[cmdType]);
					return;
				}

				const string &arg1 = (*iter++); const string &arg2 = (*iter++);
				const string &arg3 = (*iter++); const string &arg4 = (*iter++);

				currentGame->GetChatBox()->GetPrintingBox().MoveTo(atoi(arg1.c_str()), atoi(arg2.c_str()));
				currentGame->GetChatBox()->GetPrintingBox().SetSize(atoi(arg3.c_str()), atoi(arg4.c_str()));
			}
			break;
		case ECT_FACECG:
			{
				nekoImageInstance *image = (nekoImageInstance *)currentGame->CreateInstance(BLOCK_IMAGE, "페이스CG");
				if(arguments.size())
				{
					if(image)
					{
						image->GetBoundingBox().MoveTo(0, 0);
						image->SetImage((*iter++).c_str());

						int32 x = 0, y = 0;
						if(iter != arguments.end())
						{
							x = atoi((*iter++).c_str());

							if(iter != arguments.end())
							{
								y = atoi((*iter).c_str());
							}
						}

						image->GetBoundingBox().MoveTo(x, y);
					}
				}
				else
				{
					if(image)
					{
						currentGame->RemoveInstance(image);
					}
				}
			}
			break;
		case ECT_SCREENCOLOR:
			{
				if(arguments.size() < 3)
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어는 인수가 적어도 3개 필요해요.", startLine,gCommandStrings[cmdType]);
					return;
				}
				uint32 color = 0x0;
				color |= ((atoi((*iter).c_str()) & 0xff) << 16);
				++iter;
				color |= ((atoi((*iter).c_str()) & 0xff) << 8);
				++iter;
				color |= (atoi((*iter).c_str()) & 0xff);
				if(iter != arguments.end())
					color |= ((atoi((*iter).c_str()) & 0xff) << 24);
				else
					color |= 0xff000000;

				GetNekoNovel()->GetVideoDriver()->SetClearColor(color);
			}
			break;
		case ECT_BOUNCE:
			{
				if(arguments.size() < 5)
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어는 인수가 적어도 5개 필요해요.", startLine,gCommandStrings[cmdType]);
					return;
				}

				nekoIBlockInstance *inst = currentGame->GetInstance((*iter).c_str());
				if(inst == NULL)
				{
					mTempInteger[0] = -1;

					if(!internalRequest)
						LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어에서 '%s' 녀석을 찾지 못하였습니다.", startLine, gCommandStrings[cmdType], (*iter).c_str());

					return;
				}
				mTempInteger[0] = (int32)inst;
				mTempInteger[1] = 0; // 처음 단계인 것 을 표시.

				++iter;
				mTempFloat[0] = inst->GetBoundingBox().min.x;
				mTempFloat[1] = inst->GetBoundingBox().min.y;
				mTempFloat[2] = atof((*iter++).c_str());
				mTempFloat[3] = atof((*iter++).c_str());
				mTempFloat[4] = atof((*iter++).c_str());
				mTempFloat[5] = atof((*iter++).c_str());
				mTempFloat[6] = (iter != arguments.end()) ? atof((*iter).c_str()) * 0.001f : 1.0f;
				mTempFloat[7] = 0;
			}
			break;
		case ECT_ALPHA:
			{
				if(arguments.size() < 1)
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어는 인수가 적어도 1개 필요해요.", startLine,gCommandStrings[cmdType]);
					return;
				}

				nekoIBlockInstance *inst = currentGame->GetInstance((*iter).c_str());
				if(inst == NULL)
				{
					if(!internalRequest)
						LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어에서 '%s' 녀석을 찾지 못하였습니다.", startLine, gCommandStrings[cmdType], (*iter).c_str());

					return;
				}

				++iter;
				inst->SetAlpha((iter != arguments.end()) ? atoi((*iter).c_str()) : 255);
			}
			break;
		case ECT_SYSTEMBUTTON:
			{
				LimitMinArguments(6);

				const string &name = (*iter++);

				nekoPointF pt;
				pt.x = atoi((*iter++).c_str());
				pt.y = atoi((*iter++).c_str());

				nekoPoint size;
				size.x = atoi((*iter++).c_str());
				size.y = atoi((*iter++).c_str());

				const string &type = (*iter++);

				nekoSystemButtonInstance *inst = (nekoSystemButtonInstance *)currentGame->CreateInstance(BLOCK_SYSTEMBUTTON, name.c_str());

				if(type == "점프")
				{
					if(arguments.size() < 8)
					{
						LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어 중 '점프'는 인수가 적어도 8개 필요합니다.", startLine, gCommandStrings[cmdType]);
						return;
					}

					const string &arg1 = (*iter++);
					const string &arg2 = (*iter++);
					inst->SetJump((arg1 == "여기") ? thread->mCommands->GetName() : arg1, arg2);
				}
				else if(type == "없음")
					inst->SetButtonType(ESBIT_NOTHING);
				else if(type == "회상")
					inst->SetButtonType(ESBIT_RECOLLECTION);
				else if(type == "화면모드")
				{
					inst->SetWindowMode((*iter++));
				}
				else if(type == "빠른스킵")
				{
					if(arguments.size() < 7)
					{
						LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어 중 '빠른스킵'은 인수가 적어도 7개 필요합니다.", startLine, gCommandStrings[cmdType]);
						return;
					}

					const string &arg1 = (*iter++);
					inst->SetQuickSkipMode(arg1);
				}
				else if(type == "자동넘기기")
				{
					if(arguments.size() < 7)
					{
						LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어 중 '자동넘기기'은 인수가 적어도 7개 필요합니다.", startLine, gCommandStrings[cmdType]);
						return;
					}

					const string &arg1 = (*iter++);
					inst->SetAutoTurnMode(arg1);
				}
				else if(type == "스크립트")
				{
					if(arguments.size() < 7)
					{
						LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어 중 '스크립트'은 인수가 적어도 7개 필요합니다.", startLine, gCommandStrings[cmdType]);
						return;
					}

					const string &arg1 = (*iter++);
					inst->SetScriptMode(arg1);
				}
				else if(type == "문장흘림")
				{
					if(arguments.size() < 7)
					{
						LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어 중 '문장흘림'은 인수가 적어도 7개 필요합니다.", startLine, gCommandStrings[cmdType]);
						return;
					}

					const string &arg1 = (*iter++);
					inst->SetPrintDelay(arg1);
				}
				else if(type == "다음페이지")
				{
					inst->SetButtonType(ESBIT_NEXT_PAGE);
				}
				else if(type == "이전페이지")
				{
					inst->SetButtonType(ESBIT_PREV_PAGE);
				}
				else if(type == "페이지")
				{
					if(arguments.size() < 7)
					{
						LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어 중 '페이지'은 인수가 적어도 7개 필요합니다.", startLine, gCommandStrings[cmdType]);
						return;
					}

					inst->SetSlotPage(atoi((*iter++).c_str()));
				}
				else if(type == "종료")
				{
					inst->SetButtonType(ESBIT_EXIT);
				}
				else if(type == "로드슬롯")
				{
					if(arguments.size() < 7)
					{
						LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어 중 '로드슬롯'은 인수가 적어도 7개 필요합니다.", startLine, gCommandStrings[cmdType]);
						return;
					}

					const string &arg1 = (*iter++).c_str();

					int32 slotNo = atoi(arg1.c_str());

					if(!(slotNo >= 1 && slotNo <= MAX_SAVE_COUNT))
					{
						LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어 중 '로드슬롯'의 범위는 1~%d 입니다.", startLine, gCommandStrings[cmdType], ONE_PAGE_SAVECOUNT);
						return;
					}

					inst->SetLoadSaveSlotMode(ESBIT_LOAD_SLOT, slotNo - 1);
				}
				else if(type == "세이브슬롯")
				{
					if(arguments.size() < 7)
					{
						LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어 중 '세이브슬롯'은 인수가 적어도 7개 필요합니다.", startLine, gCommandStrings[cmdType]);
						return;
					}

					const string &arg1 = (*iter++).c_str();

					int32 slotNo = atoi(arg1.c_str());

					if(!(slotNo >= 1 && slotNo <= MAX_SAVE_COUNT))
					{
						LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어 중 '세이브슬롯'의 범위는 1~%d 입니다.", startLine, gCommandStrings[cmdType], ONE_PAGE_SAVECOUNT);
						return;
					}

					inst->SetLoadSaveSlotMode(ESBIT_SAVE_SLOT, slotNo - 1);
				}
				else if(type == "슬롯삭제")
				{
					if(arguments.size() < 7)
					{
						LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어 중 '슬롯삭제'은 인수가 적어도 7개 필요합니다.", startLine, gCommandStrings[cmdType]);
						return;
					}

					const string &arg1 = (*iter++).c_str();

					int32 slotNo = atoi(arg1.c_str());

					if(!(slotNo >= 1 && slotNo <= MAX_SAVE_COUNT))
					{
						LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어 중 '슬롯삭제'의 범위는 1~%d 입니다.", startLine, gCommandStrings[cmdType], ONE_PAGE_SAVECOUNT);
						return;
					}

					inst->SetLoadSaveSlotMode(ESBIT_DELETE_SLOT, slotNo - 1);
				}
				else if(type == "항상위")
				{
					if(arguments.size() < 7)
					{
						LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어 중 '항상위'은 인수가 적어도 7개 필요합니다.", startLine, gCommandStrings[cmdType]);
						return;
					}

					const string &arg1 = (*iter++).c_str();

					inst->SetAlwaysTopMode(arg1 == "응");
				}
				else if(type == "변수")
				{
					if(arguments.size() < 8)
					{
						LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어 중 '변수'은 인수가 적어도 8개 필요합니다.", startLine, gCommandStrings[cmdType]);
						return;
					}

					const string &arg1 = (*iter++).c_str();
					const string &arg2 = (*iter++).c_str();

					inst->SetVariableSetMode(arg1, arg2);
				}
				else
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어에서 '%s' 타입은 존재하지 않아요.", startLine, gCommandStrings[cmdType], type.c_str());

					currentGame->RemoveInstance(inst);
					return;
				}

				/// 버튼 이미지 지정
				if(iter != arguments.end())
				{
					inst->SetNormalImage((*iter++));

					if(iter != arguments.end())
						inst->SetRolloverImage((*iter++));

					if(iter != arguments.end())
						inst->SetPressedImage((*iter++));

					// 버튼 사운드 지정
					if(iter != arguments.end())
						inst->SetRolloverSound((*iter++));

					// 버튼 사운드 지정
					if(iter != arguments.end())
						inst->SetPressedSound((*iter++));
				}

				inst->GetBoundingBox().MoveTo(pt.x, pt.y);

				if(size.x > 0)
					inst->GetBoundingBox().SetWidth(size.x);
				else
					inst->AddFlags(EBIF_AUTO_SIZE_X);

				if(size.y > 0)
					inst->GetBoundingBox().SetHeight(size.y);
				else
					inst->AddFlags(EBIF_AUTO_SIZE_Y);
			}
			break;
		case ECT_SYSTEMBUTTON_BYE:
			{
				currentGame->ClearInstances(BLOCK_SYSTEMBUTTON);
				currentGame->ClearInstances(BLOCK_TRACKBAR);
			}
			break;
		case ECT_VIBE:
			{
				if(arguments.size() < 4)
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어는 인수가 적어도 4개 필요합니다.", startLine, gCommandStrings[cmdType]);
					return;
				}

				if(internalRequest)
					return;

				nekoIBlockInstance *inst = currentGame->GetInstance((*iter).c_str());

				if(inst == NULL)
				{
					mTempInteger[0] = -1;
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어에서 '%s' 녀석을 찾을 수 없어요.", startLine, gCommandStrings[cmdType], (*iter).c_str());
					return;
				}

				++iter;

				mTempInteger[0] = (int32)inst;
				mTempInteger[1] = inst->GetBoundingBox().min.x;
				mTempInteger[2] = inst->GetBoundingBox().min.y;
				mTempFloat[0] = abs(atof((*iter++).c_str()));
				mTempFloat[1] = abs(atof((*iter++).c_str()));
				mTempFloat[2] = 0;
				mTempFloat[3] = 1.0f;
				mTempFloat[4] = mTempFloat[5] = 0.0f;

				mTempTimer[0].Clear();

				// 시간
				if(iter == arguments.end())
					mTempTimer[0].SetTime(1.0f);
				else
				{
					mTempTimer[0].SetTime(atoi((*iter++).c_str()) * 0.001f);

					// 속도
					if(iter != arguments.end())
					{
						mTempFloat[3] = atoi((*iter++).c_str()) * 0.001f;

						if(iter != arguments.end())
						{
							mTempFloat[4] = atof((*iter++).c_str());

							if(iter != arguments.end())
								mTempFloat[5] = atof((*iter).c_str());
						}
					}
				}
			}
			break;
		case ECT_KEYPROCESS:
			{
				if(arguments.size() < 1)
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어는 인수가 적어도 1개 필요합니다.", startLine, gCommandStrings[cmdType]);
					return;
				}

				int32 keyCode = atoi((*iter++).c_str());
				keyCode = min(max(keyCode, 0), 255);

				if(arguments.size() == 1)
					currentGame->SetHotKey(keyCode, string(), string());
				else if(arguments.size() == 3)
				{
					const string &arg1 = (*iter++);
					const string &arg2 = (*iter);

					currentGame->SetHotKey(keyCode, arg1, arg2);
				}
			}
			break;
		case ECT_BUTTON_BYE:
			{
				currentGame->OnButtonPressed();
			}
			break;
		case ECT_HIDE_ONLYCHAT:
			{
				if(arguments.size() == 1)
				{
					if((*iter) == "아니")
					{
						currentGame->GetChatBox()->SetChatVisible(true);
						return;
					}
				}

				currentGame->GetChatBox()->SetChatVisible(false);
			}
			break;
		case ECT_IF:
			{
				if(internalRequest) return;

				if(arguments.size() < 5)
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어는 인수가 적어도 5개 필요합니다.", startLine, gCommandStrings[cmdType]);
					return;
				}

				const string &arg1 = (*iter++);
				const string &arg2 = (*iter++);
				const string &arg3 = (*iter++);

				// 연산자 체크
				E_OPERATOR_TYPE opType = GetOperatorFrom(arg2.c_str());
				if(opType >= EOT_COUNT)
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어에 알 수 없는 연산자가 있습니다.", startLine, gCommandStrings[cmdType], arg2.c_str());
					return;
				}

				string fileName = (*iter++);
				string bookMarkName = (*iter++);

				fileName = fileName.c_str();
				bookMarkName = bookMarkName.c_str();

				if(nekoVariable::If(opType, arg1, arg3) == false)
				{
					if(iter == arguments.end() || (*iter) != "아니면")
						return;

					++iter;

					if(iter == arguments.end())
						return;

					fileName = (*iter++);

					if(iter == arguments.end())
					{
						LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어에 '아니면' 부분에 북마크 이름이 없습니다.", startLine, gCommandStrings[cmdType]);
						return;
					}

					bookMarkName = (*iter);
				}

				if(bookMarkName == "첫줄")
					bookMarkName = "";
				
				if(fileName == "여기")
				{
					// 실행하기
					currentGame->JumpTo(NULL, bookMarkName.c_str());
				}
				else
				{
					// 실행하기
					currentGame->PlayCommands(fileName.c_str(), bookMarkName.c_str());
				}
			}
			break;
		case ECT_OMAKE_ADD:
			{
				if(internalRequest)
					return;

				if(arguments.size() < 1)
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어는 인수가 적어도 1개 필요합니다.", startLine, gCommandStrings[cmdType]);
					return;
				}

				const string &arg1 = (*iter++);

				if(arguments.size() == 1)
				{
					currentGame->RemoveOmake(arg1);
				}
				else
				{
					const string &arg2 = (*iter);
					currentGame->AddOmake(arg1, arg2);
				}
			}
			break;
		case ECT_OMAKE_CG:
			{
				if(internalRequest)
					return;
				
				if(arguments.size() < 5)
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어는 인수가 적어도 5개 필요합니다.", startLine, gCommandStrings[cmdType]);
					return;
				}

				const string &omakeName = (*iter++);
				const char8 *fileName = currentGame->GetOmake(omakeName);

				nekoRectangle rect;
				rect.left = atoi((*iter++).c_str());
				rect.top = atoi((*iter++).c_str());
				rect.right = rect.left + atoi((*iter++).c_str());
				rect.bottom = rect.top + atoi((*iter++).c_str());

				nekoImageInstance *image = (nekoImageInstance *)currentGame->CreateInstance(BLOCK_IMAGE, omakeName.c_str());
				if(image)
				{
					// 처음 위치 맞춰주고 이미지 선택하기
					image->GetBoundingBox() = rect;

					// 만약 없을 때는 대체 이미지를 표시하거나, 표시하지 않음.
					image->SetImage(fileName);
					if(fileName == NULL && iter != arguments.end())
					{
						image->SetImage((*iter).c_str());
					}
				}
			}
			break;
		case ECT_GAME_STATE:
			{
				if(internalRequest) return;

				if(arguments.size() == 1)
				{
					if((*iter) == "자동넘기기")
						currentGame->SetGameFlowState(EGFS_AUTOTURNING);
					else if((*iter) == "빠른스킵")
						currentGame->SetGameFlowState(EGFS_QUICKSKIPPING);
					else
						LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령에 '%s' 라는 상태는 존재하지 않습니다.", startLine, gCommandStrings[cmdType], (*iter).c_str());
				}
				else
				{
					currentGame->SetGameFlowState(EGFS_NORMAL);
				}
			}
			break;
		case ECT_SET_EVENT:
			{
				if(arguments.size() != 1 && arguments.size() != 3)
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어는 인수가 1개 또는 3개 필요합니다.", startLine, gCommandStrings[cmdType]);
					return;
				}

				const string &name = (*iter++);
				E_CUSTOM_EVENT_TYPE evtType;

				if(name == "로드후")
					evtType = ECET_LOADED;
				else if(name == "세이브후")
					evtType = ECET_SAVED;
				else if(name == "회상후")
					evtType = ECET_RECOLLECTED;
				else if(name == "스킵모드해제후")
					evtType = ECET_EXIT_SKIPMODE;
				else if(name == "이전페이지")
					evtType = ECET_GOTO_PREV_SLOT;
				else if(name == "다음페이지")
					evtType = ECET_GOTO_NEXT_SLOT;
				else if(name == "메세지박스")
					evtType = ECET_MSGBOX;
				else
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어에서 [타입] 부분에 '%s' 는 지원되지 않아요~", startLine, gCommandStrings[cmdType], name.c_str());
					return;
				}

				if(arguments.size() == 1)
				{
					currentGame->RemoveCustomEventListener(evtType);
				}
				else
				{

					const string &arg1 = (*iter++);
					const string &arg2 = (*iter);

					currentGame->SetCustomEventListener(evtType, arg1, arg2);
				}

			}
			break;
		case ECT_VOICE:
			{
				if(internalRequest)
					return;

				nekoVoiceInstance *music = (nekoVoiceInstance *)currentGame->CreateInstance(BLOCK_VOICE, "");
				if(arguments.size() == 1)
				{
					music->SetMusic((*iter).c_str(), false);
				}
				else
				{
					currentGame->RemoveInstance(music);
				}
				
			}
			break;
		case ECT_TEXT_LINEINTERVAL:
			{
				if(arguments.size() != 1)
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어는 인수가 1개 필요합니다.", startLine, gCommandStrings[cmdType]);
					return;
				}

				currentGame->GetChatBox()->SetLineInterval(atoi((*iter).c_str()));
			}
			break;
		case ECT_PRINTING_ORDER:
			{
				if(arguments.size() != 2)
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어는 인수가 2개 필요합니다.", startLine, gCommandStrings[cmdType]);
					return;
				}

				nekoIBlockInstance *inst = currentGame->GetInstance((*iter).c_str());
				if(inst)
				{
					++iter;
					inst->SetZOrder(atoi((*iter).c_str()));
				}
				else
				{
					if(internalRequest == false)
					{
						LogPrint(LOG_LEVEL_WARN, "%d줄 - '%s'를 찾을 수 없어요.", startLine, (*iter).c_str());
					}
				}
			}
			break;
		case ECT_LABEL:
			{
				////////////////////////////////////////////////////////////////////////

				if(arguments.size() != 1 && arguments.size() != 8)
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어는 인수가 1개 혹은 8개가 필요합니다.", startLine, gCommandStrings[cmdType]);
					return;
				}

				const string &name = (*iter++).c_str();
				nekoIBlockInstance *inst = currentGame->GetInstance(name.c_str());

				if(arguments.size() == 1)
				{
					if(inst)
						currentGame->RemoveInstance(inst);
				}
				else
				{
					if(inst && inst->GetType() != BLOCK_LABEL)
					{
						LogPrint(LOG_LEVEL_WARN, "%d줄 - '%s'는 '라벨'이 아닙니다.", startLine, inst->GetName());
						return;
					}

					// 없으면 만들어~
					if(inst == NULL)
					{
						inst = currentGame->CreateInstance(BLOCK_LABEL, name.c_str());
					}

					{
						const string &arg1 = (*iter++);
						const string &arg2 = (*iter++);

						inst->GetBoundingBox().MoveTo(atoi(arg1.c_str()), atoi(arg2.c_str()));
					}

					{
						const string &arg1 = (*iter++);
						const string &arg2 = (*iter++);
						const string &arg3 = (*iter++);
						const string &arg4 = (*iter++);
						const string &txt = (*iter);


						// 변수가 적용되지 않은 순수 스크립트 인수로 가져온다.
						((nekoLabelInstance *)inst)->SetCaption((*this->arguments.rbegin()).c_str(),
							D3DCOLOR_ARGB(atoi(arg4.c_str()), atoi(arg1.c_str()), atoi(arg2.c_str()), atoi(arg3.c_str())));
					}
				}
			}
			break;
		case ECT_LABEL_PROPERTY:
			{
				if(arguments.size() != 4)
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어는 인수가 4개 필요합니다.", startLine, gCommandStrings[cmdType]);
					return;
				}

				nekoIBlockInstance *inst = currentGame->GetInstance((*iter).c_str());
				if(inst == NULL)
				{
					if(internalRequest == false)
						LogPrint(LOG_LEVEL_WARN, "%d줄 - %s에서 '%s' 라는 녀석을 찾을 수 없는걸요!", startLine, gCommandStrings[cmdType], (*iter).c_str());
					return;
				}

				++iter;

				if(inst->GetType() != BLOCK_LABEL)
				{
					if(internalRequest == false)
						LogPrint(LOG_LEVEL_WARN, "%d줄 - %s에서 '%s' 라는 녀석은 '라벨'이 아닙니다.'", startLine, gCommandStrings[cmdType], (*iter).c_str());
					return;
				}

				nekoLabelInstance *instLabel = (nekoLabelInstance *)inst;
				int32 size = -1;
				int32 bold = -1; // -1이면 기존 설정 그대로

				size = atoi((*iter++).c_str());
				if((*iter) == "응")
					bold = 1;
				else if((*iter) == "아니")
					bold = 0;

				++iter;
				instLabel->SetFont(*iter, size, bold);
			}
			break;
		case ECT_CAMERA:
			{
				if(iter != arguments.end())
				{
					mTempFloat[0] = atof((*iter).c_str());
					++iter;
				}
				else
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어는 인수가 최소 2개 필요합니다.", startLine, gCommandStrings[cmdType]);
					return;
				}

				if(iter != arguments.end())
				{
					mTempFloat[1] = atof((*iter).c_str());
					++iter;
				}
				else
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어는 인수가 최소 2개 필요합니다.", startLine, gCommandStrings[cmdType]);
					return;
				}

				if(iter != arguments.end())
				{
					mTempInteger[0] = atof((*iter).c_str());
					++iter;
				}
				else
				{
					mTempInteger[0] = 400;
				}

				if(iter != arguments.end())
				{
					mTempInteger[1] = atof((*iter).c_str());
					++iter;
				}
				else
				{
					mTempInteger[1] = 300;
				}

				if(iter != arguments.end())
				{
					mTempFloat[5] = atof((*iter).c_str());
					++iter;
				}
				else
				{
					mTempFloat[5] = 0.0f;
				}

				if(iter != arguments.end())
				{
					mTempFloat[2] = atof((*iter).c_str());
					++iter;
				}
				else
				{
					mTempFloat[2] = 1.0f;
				}

				if(iter != arguments.end())
				{
					mTempFloat[3] = atof((*iter).c_str());
					++iter;
				}
				else
				{
					mTempFloat[3] = 1.0f;
				}

				if(iter != arguments.end())
				{
					mTempFloat[6] = atof((*iter).c_str());
					++iter;
				}
				else
				{
					mTempFloat[6] = 1.0f;
				}

				if(iter != arguments.end())
				{
					mTempFloat[4] = atof((*iter).c_str()) / 180 * MATH_PI;
					++iter;
				}
				else
				{
					mTempFloat[4] = 0.0f;
				}


			}
			break;
		case ECT_HIDE_CHATBOX:
			{
				currentGame->GetChatBox()->ClearBuffer();

				if(internalRequest)
					return;

				mTempFloat[1] = currentGame->GetChatBox()->GetAlpha();

				if(iter != arguments.end())
				{
					mTempTimer[0].SetTime(atoi((*iter).c_str()) * 0.001f);

					if(mTempTimer[0].GetTimerTime() > 0)
					{
						mTempFloat[0] = -(mTempFloat[1]) / mTempTimer[0].GetTimerTime();
					}
				}
				else
				{
					mTempTimer[0].SetTime(0.0f);
				}
			}
			break;
		case ECT_SHOW_CHATBOX:
			{
				if(internalRequest)
					return;

				mTempFloat[1] = 0.0f;

				if(iter != arguments.end())
				{
					mTempTimer[0].SetTime(atoi((*iter).c_str()) * 0.001f);

					if(mTempTimer[0].GetTimerTime() > 0)
					{
						mTempFloat[0] = +255.0f / mTempTimer[0].GetTimerTime();
					}
				}
				else
				{
					mTempTimer[0].SetTime(0.0f);
				}
			}
			break;
		case ECT_TEXT_BOLD:
			{
				bool bold;
				bold = (iter != arguments.end() && (*iter) == "응");

				GetNekoNovel()->GetCurrentGame()->GetChatBox()->SetFont(NULL, -1, bold);
			}
			break;
		case ECT_FLASH:
			{
				mTempInst = 0;
				if(internalRequest)
					return;

				if(!arguments.size())
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어는 인수가 1개 이상 필요합니다.", startLine, gCommandStrings[cmdType]);
					return;
				}

				const string &str = (*iter++);
				
				// 삭제 요청..
				if(iter == arguments.end())
				{
					currentGame->RemoveInstance(currentGame->GetInstance(str.c_str()));
					return;
				}

				nekoFlashInstance *flash = (nekoFlashInstance *)currentGame->CreateInstance(BLOCK_FLASH, str.c_str());

				const string &flashName = (*iter++);

				int32 x = 0;
				int32 y = 0;
				if(iter != arguments.end())
				{
					x = atoi((*iter).c_str());
					++iter;
				}

				if(iter != arguments.end())
				{
					y = atoi((*iter).c_str());
					++iter;
				}
				
				flash->GetBoundingBox().MoveTo(x, y);

				x = 800;
				y = 600;
				if(iter != arguments.end())
				{
					x = atoi((*iter).c_str());
					++iter;
				}

				if(iter != arguments.end())
				{
					y = atoi((*iter).c_str());
					++iter;
				}

				if(x > 800 || y > 600)
					LogPrint(LOG_LEVEL_WARN, "'%s'는 플래시의 최대 크기 (800, 600)를 초과하였습니다.", flash->GetName());

				x = max(min(x, 800), 1);
				y = max(min(y, 600), 1);

				flash->GetBoundingBox().SetSize(x, y);
				flash->OnSizeUpdated(x, y);

				float32 skipAbleTime = 0;
				if(iter != arguments.end())
				{
					skipAbleTime = atof((*iter).c_str());
				}

				flash->SetFlash(flashName.c_str(), skipAbleTime);

				(mTempInst = flash)->AddRef();
			}
			break;
		case ECT_BG_RULE_FADEIN:
			{
				if(!arguments.size())
				{
					currentGame->SetEffectToBG(EBET_NONE, 0.0f, true);
					return;
				}

				float32 speed = 1.0f;
				const string &fileName = (*iter++).c_str();
				if(arguments.size() >= 2)
					speed = atof((*iter).c_str()) * 0.001f;

				currentGame->SetRuleEffectToBG(fileName, speed, true);
			}
			break;
		case ECT_BG_RULE_FADEOUT:
			{
				if(internalRequest)
					return;

				if(!arguments.size())
				{
					currentGame->SetEffectToBG(EBET_NONE, 0.0f, false);
					return;
				}

				float32 speed = 1.0f;
				const string &fileName = (*iter++).c_str();
				if(arguments.size() >= 2)
					speed = atof((*iter).c_str()) * 0.001f;

				currentGame->SetRuleEffectToBG(fileName, speed, false);
			}
			break;
		case ECT_PARTICLE:
			{
				if(internalRequest)
					return;

				if(arguments.size() == 1)
				{
					nekoIBlockInstance *inst = currentGame->GetInstance((*iter).c_str());
					currentGame->RemoveInstance(inst);
				}
				else if(arguments.size() >= 2)
				{
					nekoParticleInstance *inst = (nekoParticleInstance *)currentGame->CreateInstance(BLOCK_PARTICLE, (*iter++).c_str());
					
					if(FAILED(inst->LoadParticle((*iter).c_str(), true)))
					{
						currentGame->RemoveInstance(inst);
						LogPrint(LOG_LEVEL_WARN, "%d줄 - '%s' 파티클 파일을 불러오는 데 실패하였습니다.", startLine, (*iter).c_str());
						return;
					}

					++iter;

					if(iter != arguments.end())
					{
						inst->SetAllSpwanPosX(atof((*iter++).c_str()));
					}

					if(iter != arguments.end())
					{
						inst->SetAllSpwanPosY(atof((*iter++).c_str()));
					}
				}
				else
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어는 인수가 1개 이상이 필요합니다.", startLine, gCommandStrings[cmdType]);
				}
			}
			break;
		case ECT_SELECTION_BUTTON:
			{
				if(arguments.size() < 8)
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어는 인수가 8개 필요해요.", startLine, gCommandStrings[cmdType]);
					return;
				}

				const string *imageNames[] = {&(*iter++), &(*iter++), &(*iter++)};
				const string *soundNames[] = {&(*iter++), &(*iter++)};
				const string *args[] = {&(*iter++), &(*iter++)};
				const string &caption = (*iter++);


				/// 게임에 추가..
				currentGame->AddSelection(*args[0], *args[1]);


				/// 버튼 관리 객체에 추가
				currentGame->GetSelectionButtons()->AddButton(caption.c_str(), imageNames, soundNames);
			}
			break;
		case ECT_SELECTION_BUTTON_RECT:
			{
				if(arguments.size() < 4)
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어는 인수가 4개 필요해요.", startLine, gCommandStrings[cmdType]);
					return;
				}

				const string *args[4] = {&(*iter++), &(*iter++), &(*iter++), &(*iter)};
				nekoRectangle rect = nekoRectangle(atoi(args[0]->c_str()), atoi(args[1]->c_str()));
				rect.SetSize(atoi(args[2]->c_str()), atoi(args[3]->c_str()));

				currentGame->GetSelectionButtons()->GetBoundingBox() = rect;
			}
			break;
		case ECT_CLEAR_RECOLLECTION:
			{
				currentGame->GetMessageLogger()->ClearBuffer();
			}
			break;
		case ECT_IGNORE_CAMERA:
			{
				if(arguments.size() == 1)
				{
					currentGame->GetEnvironment().mCameraIgnored = ((*iter) == "응");
				}
				else
				{
					currentGame->GetEnvironment().mCameraIgnored = true;
				}
			}
			break;
		case ECT_VARIABLE_TRIGGER:
			{
				if(internalRequest) return;

				if(arguments.size() == 2)
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어는 인수가 1개 또는 3개가 필요해요.", startLine, gCommandStrings[cmdType]);
					return;
				}

				if(arguments.size() == 1)
					currentGame->SetVariableTrigger((*iter), string(), string());
				else if(arguments.size() == 3)
				{
					const string &arg1 = (*iter++);
					const string &arg2 = (*iter++);
					const string &arg3 = (*iter);

					currentGame->SetVariableTrigger(arg1, arg2, arg3);
				}
			}
			break;
		case ECT_COLOR:
			{
				if(arguments.size() < 4)
				{
					mTempInteger[1] = 0xffffff;
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어는 인수가 4개 이상이 필요해요.", startLine, gCommandStrings[cmdType]);
					return;
				}

				const string &name = (*iter++);
				nekoIBlockInstance *inst = currentGame->GetInstance(name.c_str());

				if(inst == NULL)
				{
					mTempInteger[0] = -1;
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어에서 '%s' 녀석을 찾을 수 없어요.", startLine, gCommandStrings[cmdType], name.c_str());
					return;
				}
				
				int32 r = (atoi((*iter++).c_str()) & 0xff);
				int32 g = (atoi((*iter++).c_str()) & 0xff);
				int32 b = (atoi((*iter).c_str()) & 0xff);
				uint32 color = (r << 16) | (g << 8) | b;
				uint32 ocolor;

				if(inst->GetType() == BLOCK_IMAGE)
				{
					ocolor = static_cast<nekoImageInstance *>(inst)->GetHue();
				}
				else if(inst->GetType() == BLOCK_BGIMAGE)
				{
					ocolor = static_cast<nekoBGImageInstance *>(inst)->GetHue();
				}
				else
				{
					mTempInteger[0] = -1;
					LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어에서 '%s' 녀석은 배경 혹은 CG가 아닙니다.", startLine, gCommandStrings[cmdType], name.c_str());
					return;
				}

				mTempInteger[0] = (int32)inst;
				mTempInteger[1] = color;

				if(arguments.size() == 5)
				{
					mTempTimer[0].SetTime(atoi((*++iter).c_str()) * 0.001f);
					if(mTempTimer[0].GetTimerTime() > 0)
					{
						mTempFloat[0] = (ocolor >> 16) & 0xff;
						mTempFloat[1] = (ocolor >> 8) & 0xff;
						mTempFloat[2] = (ocolor >> 0) & 0xff;
						mTempFloat[3] = abs((r - (int32)((ocolor >> 16) & 0xff)) / mTempTimer[0].GetTimerTime());
						mTempFloat[4] = abs((g - (int32)((ocolor >> 8) & 0xff)) / mTempTimer[0].GetTimerTime());
						mTempFloat[5] = abs((b - (int32)((ocolor >> 0) & 0xff)) / mTempTimer[0].GetTimerTime());
					}
				}
				else
				{
					mTempTimer[0].SetTime(0);
				}
			}
			break;
		case ECT_TEXT_ALIGN:
			{
				E_TEXT_ALIGN_TYPE newAlign = ETAT_LEFT;
				if(arguments.size() == 1)
				{
					if((*iter) == "왼쪽")
						newAlign = ETAT_LEFT;
					else if((*iter) == "가운데")
						newAlign = ETAT_CENTER;
					else if((*iter) == "가운데2")
						newAlign = ETAT_CENTER2;
					else if((*iter) == "오른쪽")
						newAlign = ETAT_RIGHT;
					else
					{
						LogPrint(LOG_LEVEL_WARN, "%d줄 - %s 명령어에서 '%s' 는 올바른 인수가 아니예요.", startLine, gCommandStrings[cmdType], (*iter).c_str());
					}
				}

				if(newAlign != currentGame->GetEnvironment().mTextAlign)
				{
					currentGame->GetEnvironment().mTextAlign = newAlign;
					currentGame->GetChatBox()->PushNewBlock();
				}
			}
			break;
		case ECT_CG_FADEIN:
			{
				float32 speed = 0.0f;
				if(arguments.size() == 1)
				{
					speed = atof((*iter).c_str());
				}
				
				currentGame->GetEnvironment().mCGFadeInSpeed = speed;
			}
			break;
		case ECT_CG_FADEOUT:
			{
				float32 speed = 0.0f;
				if(arguments.size() == 1)
				{
					speed = atof((*iter).c_str());
				}

				currentGame->GetEnvironment().mCGFadeOutSpeed = speed;
			}
			break;
		case ECT_TEXT_FADE:
			{
				LimitMinArguments(1);

				currentGame->GetEnvironment().mSmoothPrint = ((*iter) == "응");
			}
			break;
		case ECT_GET_TEXT:
			{
				LimitMinArguments(7);

				if(internalRequest)
					return;

				nekoTextBoxInstance *inst = (nekoTextBoxInstance *)currentGame->CreateInstance(BLOCK_TEXTBOX, "텍스트박스");
				nekoRectangle rect;
				const string &varName = (*iter++);
				const string &str1 = (*iter++);
				const string &str2 = (*iter++);
				const string &str3 = (*iter++);
				const string &str4 = (*iter++);
				const char8 *imageName = 0;
				const char8 *fx1 = 0;
				const char8 *fx2 = 0;
				int32 fontSize = -1;
				int32 fontBold = -1;
				const char8 *fontName = 0;
				uint32 color = 0xff000000;
				uint32 maxLength;

				// 경계박스 설정
				rect.MoveTo(atoi(str1.c_str()), atoi(str2.c_str()));
				rect.SetSize(atoi(str3.c_str()), atoi(str4.c_str()));

				maxLength = atoi((*iter++).c_str());

				// 이미지 이름
				imageName = (*iter++).c_str();

				if(iter != arguments.end())
				{
					fx1 = (*iter++).c_str();
				}
				if(iter != arguments.end())
					fx2 = (*iter++).c_str();

				if(iter != arguments.end())
					color |= ((atoi((*iter++).c_str()) & 0xff)  << 16);

				if(iter != arguments.end())
					color |= ((atoi((*iter++).c_str()) & 0xff) << 8);

				if(iter != arguments.end())
					color |= (atoi((*iter++).c_str()) & 0xff);

				if(iter != arguments.end())
					fontSize = atoi((*iter++).c_str());
				if(iter != arguments.end())
					fontBold = atoi((*iter++).c_str());
				if(iter != arguments.end())
					fontName = (*iter).c_str();

				inst->Initialize(varName, rect, maxLength, imageName, fx1, fx2, color, fontSize, fontBold, fontName);
				mTempInteger[0] = (int32)inst;

				currentGame->SetGameSystemState(EGSS_INPUT_WAIT);
			}
			break;
		case ECT_MOVIE:
			{
				mTempInst = 0;

				LimitMinArguments(6);

				if(internalRequest)
					return;

				nekoMovieInstance *movie = (nekoMovieInstance *)currentGame->CreateInstance(BLOCK_MOVIE, (*iter++).c_str());

				int32 x = atoi((*iter++).c_str());
				int32 y = atoi((*iter++).c_str());
				int32 width = atoi((*iter++).c_str());
				int32 height = atoi((*iter++).c_str());
				nekoRectangle rect = nekoRectangle(x, y, x + width, y + height);
				const char8 *fileName = (*iter++).c_str();

				float32 skipAbleTime = 0.0f;
				if(iter != arguments.end())
					skipAbleTime = atof((*iter).c_str());

				movie->GetBoundingBox() = rect;
				if(FAILED(movie->SetMovie(fileName, skipAbleTime)))
				{
					currentGame->RemoveInstance(movie);
					return;
				}

				(mTempInst = movie)->AddRef();
			}
			break;
		case ECT_INITIALIZE_RECOLLECTION:
			{
				LimitMinArguments(5);

				int32 width = atoi((*iter++).c_str());
				int32 height = atoi((*iter++).c_str());
				int32 rightMargin = atoi((*iter++).c_str());;
				int32 fontSize = atoi((*iter++).c_str());
				const char8 *fontName = (*iter).c_str();

				currentGame->GetMessageLogger()->Initialize(width, height, rightMargin, fontName, fontSize);
			}
			break;
		case ECT_RECOLLECTION_TEXT:
			{
				if(arguments.size() == 1)
				{
					currentGame->RemoveInstance(currentGame->GetInstance((*iter).c_str()));
					return;
				}

				LimitMinArguments(6);

				if(internalRequest)
					return;


				nekoMessageLogInstance *messageLog = static_cast<nekoMessageLogInstance *>(currentGame->CreateInstance(BLOCK_MESSAGELOG, (*iter++).c_str()));
				int32 x = atoi((*iter++).c_str());
				int32 y = atoi((*iter++).c_str());

				messageLog->GetBoundingBox().MoveTo(x, y);

				// 스크롤바 고정 크기
				x = atoi((*iter++).c_str());

				int32 scrollBarButtonHeight = atoi((*iter++).c_str());
				if(scrollBarButtonHeight <= 0)
					scrollBarButtonHeight = 22;

				messageLog->SetScrollBar(x, scrollBarButtonHeight);

				const char8 *skinFileName = (*iter++).c_str();
				messageLog->Initialize(skinFileName);
			}
			break;
		case ECT_RECOLLECTION_SETTING:
			{
				if(iter != arguments.end())
				{
					currentGame->GetMessageLogger()->SetNormalLeftMargin(atoi((*iter++).c_str()));
				} else return;

				if(iter != arguments.end())
				{
					currentGame->GetMessageLogger()->SetLeftMargin(atoi((*iter++).c_str()));
				} else return;

				if(iter != arguments.end())
				{
					currentGame->GetMessageLogger()->SetPageMargin(atoi((*iter++).c_str()));
				} else return;

				if(iter != arguments.end())
				{
					currentGame->GetMessageLogger()->SetEmptyPage(atoi((*iter++).c_str()));
				} else return;

				if(arguments.size() >= 7)
				{
					uint32 color = 0xff000000;
					color |= ((atoi((*iter++).c_str()) & 0xff) << 16);
					color |= ((atoi((*iter++).c_str()) & 0xff) << 8);
					color |=  (atoi((*iter++).c_str()) & 0xff);
					currentGame->GetMessageLogger()->SetDefaultColor(color);
				}
			}
			break;
		case ECT_RECOLLECTION_COLOR:
			{
				if(arguments.size() >= 3)
				{
					uint32 color = 0xff000000;
					color |= ((atoi((*iter++).c_str()) & 0xff) << 16);
					color |= ((atoi((*iter++).c_str()) & 0xff) << 8);
					color |=  (atoi((*iter++).c_str()) & 0xff);
					currentGame->GetMessageLogger()->SetColor(color);
				}
				else
				{
					currentGame->GetMessageLogger()->SetColor(0);
				}
			}
			break;
		case ECT_GIF_SPRITE:
			{
				mTempInst = 0;

				LimitMinArguments(1);

				if(internalRequest)
					return;

				nekoIBlockInstance *inst = currentGame->GetInstance((*iter).c_str());
				if(arguments.size() == 1)
				{
					currentGame->RemoveInstance(inst);
				}
				else
				{
					nekoGIFSpriteInstance *gifInst = (nekoGIFSpriteInstance *)inst;
					if(!inst || inst->GetType() != BLOCK_GIFSPRITE)
					{
						gifInst = (nekoGIFSpriteInstance *)currentGame->CreateInstance(BLOCK_GIFSPRITE, (*iter++).c_str());
					}

					const string &fileName = (*iter++);

					uint32 repeatCount = 1;
					if(iter != arguments.end())
					{
						repeatCount = atoi((*iter++).c_str());
					}

					if(FAILED(gifInst->LoadGIF(fileName.c_str(), repeatCount)))
					{
						currentGame->RemoveInstance(inst);
						return;
					}

					int32 x = currentGame->GetEnvironment().mPrintX;
					int32 y = currentGame->GetEnvironment().mPrintY;

					if(iter != arguments.end())
					{
						x = atoi((*iter++).c_str());
						
					}

					if(iter != arguments.end())
					{
						y = atoi((*iter).c_str());
					}

					gifInst->GetBoundingBox().MoveTo(x, y);

					(mTempInst = gifInst)->AddRef();
				}
			}
			break;
		case ECT_MSGBOX:
			{
				LimitMinArguments(1);

				if(internalRequest)
					return;

				if(FAILED(currentGame->ShowMsgBox(EMT_CUSTOM, (*iter).c_str(), 0)))
				{
					LogPrint(LOG_LEVEL_WARN, "%d줄 - '커스텀' 메세지박스 스크립트를 작성하고, 이벤트 명령어 '메세지박스'에 등록해주세요! ", startLine, gCommandStrings[cmdType]);
					return;
				}
			}
			break;
		case ECT_IW2_USE:
			{
				LimitMinArguments(1);

				currentGame->GetEnvironment().mIW2Use = ((*iter) == "응");
			}
			break;
		case ECT_QUICK_LOAD:
			{

			if(internalRequest)
				return;

			nekoSaveData *saveData = currentGame->GetSaveData(QUICK_SAVE_IDX);

			if(!saveData)
				return;

			if(currentGame->ShowMsgBox(EMT_QUICKLOAD
				, nekoUtility::FormString("%s\n%s\n게임을 정말 퀵로드 하시겠습니까 ?", saveData->GetCaption(), saveData->GetLiteDateString())
				, QUICK_SAVE_IDX) == false)
			{
				currentGame->LoadFromSlot(QUICK_SAVE_IDX);	
			}

			}
			break;
		case ECT_QUICK_SAVE:
			{

			if(internalRequest)
				return;

			nekoSaveData *saveData = currentGame->GetSaveData(QUICK_SAVE_IDX);

			if(saveData && currentGame->ShowMsgBox(EMT_QUICKLOAD
				, nekoUtility::FormString("이전 퀵 세이브 게임\n%s\n%s\n에 정말 덮어쓰시겠습니까 ?", saveData->GetCaption(), saveData->GetLiteDateString())
				, QUICK_SAVE_IDX) == true)
			{
				
			}
			else
			{
				currentGame->SaveToSlot(QUICK_SAVE_IDX);	
			}

			}
			break;
		case ECT_FUNC_EXECUTE:
			{
				if(internalRequest)
					return;

				LimitMinArguments(1);

				const string &scriptName = (*iter++);

				uint32 i = 1;
				nekoVariable *var;
				for(;iter != arguments.end();++iter, ++i)
				{
					if(var = currentGame->AddVariable(nekoUtility::FormString("_%u", i)))
					{
						var->SetValue((*iter).c_str());
					}
				}

				currentGame->PlayCommands(scriptName.c_str(), 0, (internalRequest) ? true : false, internalRequest);

			}
			break;
		case ECT_THREAD_TERMINIATE:
			{
				if(internalRequest)
					return;

				LimitMinArguments(1);

				const string &threadName = (*iter++);

				currentGame->RemoveThread(threadName.c_str());
			}
			break;
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool nekoOneCommand::Update(nekoThread *thread, float32 frameTime)
	{
		//list<string>::iterator iter = arguments.begin();
		nekoGame *currentGame = GetNekoNovel()->GetCurrentGame();

		switch(cmdType)
		{
		case ECT_TEXT:
		case ECT_APTEXT:
		case ECT_NEW_LINE:
			{
				if(currentGame->GetChatBox()->IsWaitingTextLeft())
					return true;

				if(currentGame->GetChatBox()->IsSmoothTexting())
				{
					if(IsFloatZero(currentGame->GetEnvironment().GetSpeechSpeed()))
						return false;
					if(currentGame->IsTurnOn(GAME_FLAG_LAST_SKIPPED))
						return false;
					return true;
				}

				return false;
			}
			break;
		case ECT_INPUT_WAIT:
		case ECT_INPUT_WAIT2:
			{
				if(mTempInteger[0] ||
					currentGame->GetFlowState() == EGFS_QUICKSKIPPING ||
					(GetNekoNovel()->IsActive() && GetKeyState(VK_CONTROL) < 0)
					)
					return false;


				// 무시 가능한가요 ?!
				if(mTempInteger[1])
				{
					if(currentGame->IsTurnOn(GAME_FLAG_LAST_SKIPPED)
						|| IsFloatZero(currentGame->GetEnvironment().GetSpeechSpeed()))
					{
						return false;
					}
				}

				if(currentGame->GetFlowState() == EGFS_AUTOTURNING)
				{
					// 음성이 재생중이면 조금 기다리기.
					if(currentGame->IsVoicePlaying())
						return true;

					mTempTimer[1].Update(frameTime);
					if(mTempTimer[1].IsCompleted())
					{
						return false;
					}
					return true;
				}

				currentGame->SetGameSystemState(EGSS_WAIT);
				return true;
			}
			break;
		case ECT_DELAY:
			{
				mTempTimer[0].Update(frameTime);

				// 다 끝나서 업데이트를 못해 이제~
				if(mTempTimer[0].IsCompleted())
					return false;

				return true;
			}
			break;
		case ECT_FADEIN:
		case ECT_FADEOUT:
			{
				// 잘못된 요청.)
				if(!mTempInst)
					return false;

				mTempTimer[0].Update(frameTime);
				if(mTempTimer[0].IsCompleted())
					return false;

				mTempFloat[1] += mTempFloat[0] * frameTime;
				mTempInst->SetAlpha(mTempFloat[1]);

				return true;
			}
			break;
		case ECT_ROTATE:
			{
				// 잘못된 요청.)
				if(mTempInteger[0] == -1)
					return false;

				nekoIBlockInstance *inst = (nekoIBlockInstance *)mTempInteger[0];

				int32 yes;
				mTempFloat[2] += mTempFloat[3] * frameTime;

				// 최소 속도
				if(mTempFloat[2] < 0.0f)
					mTempFloat[2] = 0.1f;
				float32 x = mTempFloat[0];
				yes = (int32)ApprochValue(x, mTempFloat[1], mTempFloat[2] * frameTime);
				mTempFloat[0] = x;

				inst->SetRotation(x);

				if(yes == 1)
					return false;

				return true;
			}
			break;
		case ECT_SCALE:
			{
				// 잘못된 요청.)
				if(mTempInteger[0] == -1)
					return false;

				mTempTimer[0].Update(frameTime);
				if(mTempTimer[0].IsCompleted())
					return false;

				nekoIBlockInstance *inst = (nekoIBlockInstance *)mTempInteger[0];

				mTempFloat[2] += mTempFloat[0] * frameTime;
				mTempFloat[3] += mTempFloat[1] * frameTime;
				inst->SetScale(nekoPointF(mTempFloat[2], mTempFloat[3]));

				return true;
			}
			break;
		case ECT_MOVE:
			{
				// 잘못된 요청.)
				if(!mTempInst)
					return false;

				nekoIBlockInstance *inst = mTempInst;

				int32 yes;
				mTempFloat[2] += mTempFloat[3] * frameTime;

				// 최소 속도
				if(mTempFloat[2] < 20.0f)
					mTempFloat[2] = 20.0f;
				float32 x = mTempFloat[0];
				float32 y = mTempFloat[1];
				yes = (int32)ApprochValue(x, mTempFloat[4], (float64)mTempFloat[6] * mTempFloat[2] * frameTime);
				yes += (int32)ApprochValue(y, mTempFloat[5], (float64)mTempFloat[7] * mTempFloat[2] * frameTime);
				mTempFloat[0] = x;
				mTempFloat[1] = y;

				//mTempFloat[2] += mTempFloat[0] * frameTime;
				//mTempFloat[3] += mTempFloat[1] * frameTime;
				inst->GetBoundingBox().MoveTo(mTempFloat[0], mTempFloat[1]);

				if(yes == 2)
					return false;

				return true;
			}
			break;
		case ECT_SELECTION:
			{
				list<nekoOneCommand *>::iterator iter = thread->GetPlayingPos();
				if((++iter) != thread->mCommands->GetEndingPos())
					if((*iter)->cmdType != ECT_SELECTION)
						return true;
			}
			break;
		case ECT_SCRIPT:
			{
				// 한바퀴 돌고나서니까..
				if(frameTime > 0)
					return false;

				// 이렇게 해야 nekoGame이 소유권을 갖게된다.
				return true;
			}
			break;
		case ECT_BUTTON_WAIT:
			{
				// 기달려바 ㅋㅋ
				if(currentGame->GetSystemState() == EGSS_BUTTON_PRESS_WAIT)
					return true;
			}
			break;
		case ECT_BGFADEOUT:
		case ECT_BG_RULE_FADEOUT:
			{
				if(currentGame->HasBGEffect())
					return true;
			}
			break;
		case ECT_BACKGROUND:
			{
				if(currentGame->HasBGEffect())
					return true;
			}
			break;
		case ECT_THREAD_WAIT:
			{
				if(currentGame->GetThreadCount() > 0)
					return true;
			}
			break;
		case ECT_BOUNCE:
			{
				if(mTempInteger[0] == -1)
					return false;

				nekoIBlockInstance *inst = (nekoIBlockInstance *)mTempInteger[0];

				if(mTempInteger[1] == 0)
				{
					int32 v = 0;

					mTempFloat[7] += frameTime;
					frameTime += mTempFloat[7];

					if(mTempFloat[0] > mTempFloat[2])
					{
						mTempFloat[0] -= frameTime * mTempFloat[6] * 50; // 속도임.

						if(mTempFloat[0] <= mTempFloat[2])
							mTempFloat[0] = mTempFloat[2];

					}
					else if(mTempFloat[0] < mTempFloat[2])
					{
						mTempFloat[0] += frameTime * mTempFloat[6] * 50;

						if(mTempFloat[0] >= mTempFloat[2])
							mTempFloat[0] = mTempFloat[2];
					}
					else
						v++;

					if(mTempFloat[1] > mTempFloat[3])
					{
						mTempFloat[1] -= frameTime * mTempFloat[6] * 50; // 속도임.

						if(mTempFloat[1] <= mTempFloat[3])
							mTempFloat[1] = mTempFloat[3];

					}
					else if(mTempFloat[1] < mTempFloat[3])
					{
						mTempFloat[1] += frameTime * mTempFloat[6] * 50;

						if(mTempFloat[1] >= mTempFloat[3])
							mTempFloat[1] = mTempFloat[3];
					}
					else
						v++;

					inst->GetBoundingBox().MoveTo(mTempFloat[0], mTempFloat[1]);

					if(v == 2)
						mTempInteger[1]++;

					return true;
				}
				else
				{
					int32 v = 0;

					mTempFloat[7] = max(mTempFloat[7] - frameTime / 4.0f, 0.4f);
					frameTime += mTempFloat[7];

					if(mTempFloat[0] > mTempFloat[4])
					{
						mTempFloat[0] -= frameTime * mTempFloat[6] * 50; // 속도임.

						if(mTempFloat[0] <= mTempFloat[4])
							mTempFloat[0] = mTempFloat[4];

					}
					else if(mTempFloat[0] < mTempFloat[4])
					{
						mTempFloat[0] += frameTime * mTempFloat[6] * 50;

						if(mTempFloat[0] >= mTempFloat[4])
							mTempFloat[0] = mTempFloat[4];
					}
					else
						v++;

					if(mTempFloat[1] > mTempFloat[5])
					{
						mTempFloat[1] -= frameTime * mTempFloat[6] * 50; // 속도임.

						if(mTempFloat[1] <= mTempFloat[5])
							mTempFloat[1] = mTempFloat[5];

					}
					else if(mTempFloat[1] < mTempFloat[5])
					{
						mTempFloat[1] += frameTime * mTempFloat[6] * 50;

						if(mTempFloat[1] >= mTempFloat[5])
							mTempFloat[1] = mTempFloat[5];
					}
					else
						v++;

					inst->GetBoundingBox().MoveTo(mTempFloat[0], mTempFloat[1]);

					// 끝났다!
					if(v == 2)
						return false;

					return true;
				}
			}
			break;
		case ECT_VIBE:
			{
				if(mTempInteger[0] == -1)
					return false;

				mTempTimer[0].Update(frameTime);
				if(mTempTimer[0].IsCompleted())
					return false;

				mTempFloat[2] += frameTime * 50.0f * mTempFloat[3];

				// 감쇠 적용.
				if(mTempFloat[4])
				{
					mTempFloat[0] -= mTempFloat[4] * frameTime;
					if(mTempFloat[0] < 0) { mTempFloat[0] = 0; mTempFloat[4] = 0; }
				}

				if(mTempFloat[5])
				{
					mTempFloat[1] -= mTempFloat[5] * frameTime;
					if(mTempFloat[1] < 0) { mTempFloat[1] = 0; mTempFloat[5] = 0; }
				}

				nekoIBlockInstance *inst = (nekoIBlockInstance *)mTempInteger[0];
				inst->GetBoundingBox().MoveTo(mTempInteger[1] + mTempFloat[0] * sin(mTempFloat[2]),
					mTempInteger[2] + mTempFloat[1] * cos(mTempFloat[2]));

				return true;
			}
			break;
		case ECT_HIDE_CHATBOX:
		case ECT_SHOW_CHATBOX:
			{
				mTempTimer[0].Update(frameTime);
				if(mTempTimer[0].IsCompleted())
					return false;

				mTempFloat[1] += mTempFloat[0] * frameTime;
				currentGame->GetChatBox()->SetAlpha(mTempFloat[1]);

				return true;
			}
			break;
		case ECT_CAMERA:
			{
				mTempFloat[0] += mTempFloat[1] * frameTime;

				nekoCamera &camera = GetNekoNovel()->GetVideoDriver()->GetCamera();

				frameTime *= mTempFloat[0];

				int32 ret = 0;
				ret -= (ApprochValue(camera.mCenterPos.x, mTempInteger[0], frameTime)) ? 0 : 1;
				ret -= (ApprochValue(camera.mCenterPos.y, mTempInteger[1], frameTime)) ? 0 : 1;
				if(mTempFloat[5])
				{
					ret -= (ApprochValue(camera.mScaleX, mTempFloat[2], frameTime / 1000.0f * mTempFloat[5])) ? 0 : 1;
					ret -= (ApprochValue(camera.mScaleY, mTempFloat[3], frameTime / 1000.0f * mTempFloat[5])) ? 0 : 1;
				}
				if(mTempFloat[6])
				{
					ret -= (ApprochValue(camera.mRotation, mTempFloat[4], frameTime / 1000.0f * mTempFloat[6])) ? 0 : 1;
				}
				camera.Update();

				if(ret < 0)
					return true;
				return false;
			}
			break;
		case ECT_FLASH:
			{
				if(!mTempInst)
					return false;

				if(!((nekoFlashInstance *)(nekoIBlockInstance *)mTempInst)->IsPlaying())
					return false;

				return true;
			}
			break;
		case ECT_SELECTION_BUTTON:
			{
				list<nekoOneCommand *>::iterator iter = thread->GetPlayingPos();
				if((++iter) != thread->mCommands->GetEndingPos())
					if((*iter)->cmdType != ECT_SELECTION_BUTTON)
						return true;
			}
			break;
		case ECT_COLOR:
			{
				if(mTempInteger[0] == -1)
					return false;

				mTempTimer[0].Update(frameTime);
				if(mTempTimer[0].IsCompleted())
					return false;

				nekoIBlockInstance *inst = (nekoIBlockInstance *)mTempInteger[0];

				ApprochValue(mTempFloat[0], (mTempInteger[1] >> 16) & 0xff, mTempFloat[3] * frameTime);
				ApprochValue(mTempFloat[1], (mTempInteger[1] >> 8) & 0xff, mTempFloat[4] * frameTime);
				ApprochValue(mTempFloat[2], (mTempInteger[1] >> 0) & 0xff, mTempFloat[5] * frameTime);

				if(inst->GetType() == BLOCK_IMAGE)
				{
					static_cast<nekoImageInstance *>(inst)->SetHue(
						D3DCOLOR_XRGB((int32)mTempFloat[0],(int32) mTempFloat[1], (int32)mTempFloat[2]));
				}
				else if(inst->GetType() == BLOCK_BGIMAGE)
				{
					static_cast<nekoBGImageInstance *>(inst)->SetHue(
						D3DCOLOR_XRGB((int32)mTempFloat[0],(int32) mTempFloat[1], (int32)mTempFloat[2]));
				}

				return true;
			}
			break;
		case ECT_CG:
			{
				if(!mTempInst)
					return false;

				if(((nekoImageInstance *)(nekoIBlockInstance *)mTempInst)->IsDoing())
					return true;
			}
			break;
		case ECT_GET_TEXT:
			{
				if(currentGame->GetSystemState() == EGSS_INPUT_WAIT)
					return true;

			}
			break;
		case ECT_MOVIE:
			{
				if(!mTempInst)
					return false;

				if(((nekoMovieInstance *)(nekoIBlockInstance *)mTempInst)->IsPlaying())
					return true;

				return false;
			}
			break;
		case ECT_GIF_SPRITE:
			{
				if(!mTempInst)
					return false;

				if(((nekoGIFSpriteInstance *)(nekoIBlockInstance *)mTempInst)->IsPlaying())
					return true;

				return false;
			}
			break;
		case ECT_MSGBOX_WAIT:
			{
				if(currentGame->GetMsgBoxType() == EMT_UNKNOWN)
					return false;

				return true;
			}
			break;
		}

		return false;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////


	void nekoOneCommand::End(const list<nekoOneCommand *>::iterator &selfIter, nekoThread *thread, bool internalRequest)
	{
		nekoGame *currentGame = GetNekoNovel()->GetCurrentGame();
		//list<string>::const_iterator iter = arguments.begin();

		switch(cmdType)
		{
		case ECT_SELECTION:
		case ECT_SELECTION_BUTTON:
			{
				//if(!internalRequest)
				//	currentGame->GetChatBox()->SetCursorVisible(true);
			}
			break;
		case ECT_CG:
			{
				if(!mTempInst || arguments.size() > 1 || internalRequest)
					return;

				currentGame->RemoveInstance(mTempInst);
			}
			break;
		case ECT_FADEIN:
			{
				if(!mTempInst)
					return;

				mTempInst->SetAlpha(255);
			}
			break;
		case ECT_FADEOUT:
			{
				if(!mTempInst)
					return;

				mTempInst->SetAlpha(0);

				if(mTempInst->IsZeroAlphaDeleting())
					currentGame->RemoveInstance(mTempInst);
			}
			break;
		case ECT_MOVE:
			{
				if(!mTempInst)
					return;

				mTempInst->GetBoundingBox().MoveTo(mTempFloat[4], mTempFloat[5]);
			}
			break;
		case ECT_ROTATE:
			{
				if(mTempInteger[0] == -1)
					return;

				nekoIBlockInstance *inst = (nekoIBlockInstance *)mTempInteger[0];
				inst->SetRotation(mTempFloat[1]);
			}
			break;
		case ECT_SCALE:
			{
				if(mTempInteger[0] == -1)
					return;

				nekoIBlockInstance *inst = (nekoIBlockInstance *)mTempInteger[0];
				inst->SetScale(nekoPointF(mTempFloat[4], mTempFloat[5]));
			}
			break;
		case ECT_EXIT:
			{
				GetNekoNovel()->Exit();
			}
			break;
		case ECT_BOUNCE:
			{
				if(mTempInteger[0] == -1)
					return;

				nekoIBlockInstance *inst = (nekoIBlockInstance *)mTempInteger[0];
				if(inst == NULL)
					return;

				inst->GetBoundingBox().MoveTo(mTempFloat[4], mTempFloat[5]);
			}
			break;
		case ECT_VIBE:
			{
				if(internalRequest || mTempInteger[0] == -1)
					return;

				nekoIBlockInstance *inst = (nekoIBlockInstance *)mTempInteger[0];
				inst->GetBoundingBox().MoveTo(mTempInteger[1], mTempInteger[2]);
			}
			break;
		case ECT_INPUT_WAIT:
		case ECT_INPUT_WAIT2:
			{
				if(internalRequest == false)
				{
					if(currentGame->GetFlowState() == EGFS_QUICKSKIPPING)
						return;

					currentGame->SetGameSystemState(EGSS_NONE);

					currentGame->GetChatBox()->SetCursorVisible(false);
				}
			}
			break;
		case ECT_SHOW_CHATBOX:
			{
				currentGame->GetChatBox()->SetAlpha(255);
			}
			break;
		case ECT_HIDE_CHATBOX:
			{
				currentGame->GetChatBox()->SetAlpha(0);
			}
			break;
		case ECT_CAMERA:
			{
				nekoCamera &camera = GetNekoNovel()->GetVideoDriver()->GetCamera();
				camera.mCenterPos = nekoPointF(mTempInteger[0], mTempInteger[1]);
				camera.mScaleX = mTempFloat[2];
				camera.mScaleY = mTempFloat[3];
				camera.mRotation = mTempFloat[4];
				camera.Update();
			}
			break;
		case ECT_FLASH:
			{
				if(internalRequest)
					return;

				currentGame->RemoveInstance(mTempInst);
			}
			break;
		case ECT_BACKGROUND:
			{
				if(arguments.size() >= 2)
				{
					if(currentGame->GetFadeInEffect() == EBET_NONE)
						return;

					currentGame->ClearInstances(BLOCK_BGIMAGE, mTempInst, true);
				}
			}
			break;
		case ECT_BGFADEOUT:
		case ECT_BG_RULE_FADEOUT:
			{
				currentGame->ClearInstances(BLOCK_BGIMAGE, NULL, true);
			}
			break;
		case ECT_COLOR:
			{
				if(mTempInteger[0] == -1)
					return;

				nekoIBlockInstance *inst = (nekoIBlockInstance *)mTempInteger[0];
				if(inst->GetType() == BLOCK_IMAGE)
				{
					static_cast<nekoImageInstance *>(inst)->SetHue(mTempInteger[1]);
				}
				else if(inst->GetType() == BLOCK_BGIMAGE)
				{
					static_cast<nekoBGImageInstance *>(inst)->SetHue(mTempInteger[1]);
				}
			}
			break;
		case ECT_MOVIE:
			{
				if(!mTempInst)
					return;

				currentGame->RemoveInstance(mTempInst);
			}
			break;
		case ECT_BUTTON_WAIT:
			{
				if(internalRequest == true)
				{
					currentGame->ClearInstances(BLOCK_BUTTON);
				}
			}
			break;
		case ECT_GIF_SPRITE:
			{
				if(internalRequest == false)
				{
					currentGame->RemoveInstance(mTempInst);
				}
			}
			break;
		}

		mTempInst = 0;
	}

	bool nekoOneCommand::OnEvent(nekoThread *thread, const nekoEvent &event)
	{
		if(event.type == EVENT_WINDOW_MESSAGE)
		{
			nekoGame *currentGame = GetNekoNovel()->GetCurrentGame();

			if(event.uMsg == WM_KEYUP)
			{
				switch(cmdType)
				{
				case ECT_INPUT_WAIT:
				case ECT_INPUT_WAIT2:
					{
						if(event.wParam == VK_RETURN)
						{
							mTempInteger[0] = 1;
							return true;
						}
					}
					break;

				case ECT_APTEXT:
				case ECT_TEXT:
					{
						if(event.wParam == VK_RETURN)
						{
							if(currentGame->GetChatBox()->Skip())
							{
								// 다음 페이지가 또 남았을 경우..
								if(currentGame->GetChatBox()->IsWaitingTextLeft() == true)
									break;

								currentGame->AddFlags(GAME_FLAG_LAST_SKIPPED);
							}
						}
					}
					break;
				case ECT_NEW_LINE:
					{
						if(event.wParam == VK_RETURN)
						{
							currentGame->GetChatBox()->Skip();
						}
					};
					break;
				case ECT_MOVIE:
					if(event.wParam == VK_RETURN && mTempInst)
					{
						if(currentGame->GetGameMode() != GAME_RUN_PLAYING)
							break;

						nekoMovieInstance *movie = (nekoMovieInstance *)(nekoIBlockInstance *)mTempInst;
						if(movie->IsSkipAble())
							movie->Stop();
					}
					break;
				case ECT_FLASH:
					if(event.wParam == VK_RETURN && mTempInst)
					{
						if(GetNekoNovel()->GetCurrentGame()->GetGameMode() != GAME_RUN_PLAYING)
							break;

						nekoFlashInstance *flash = (nekoFlashInstance *)(nekoIBlockInstance *)mTempInst;
						if(flash->IsSkipAble())
							flash->Stop();
					}
					break;
				}

				if(event.wParam == VK_ESCAPE)
				{
					/// 스킵 가능한 경우..
					if(GetNekoNovel()->GetCurrentGame()->IsSkipAble())
					{
						// 스킵 관련 자료 모두 삭제.
						GetNekoNovel()->GetCurrentGame()->DisableSkipAble();

						thread->SetPlayingPos(thread->mCommands->SearchFrom(thread->GetPlayingPos(), ECT_SKIP_END));
						return true;
					}
				}
			}
			else if(event.uMsg == WM_LBUTTONDOWN || (event.uMsg == WM_MOUSEWHEEL && (int32)event.wParam < 0))
			{
				switch(cmdType)
				{
				case ECT_INPUT_WAIT:
				case ECT_INPUT_WAIT2:
					{
						mTempInteger[0] = 1;
						return true;
					}
					break;

				// 대사가 너무 길어서 아직 전부다 출력 못 한 경우.
				case ECT_APTEXT:
				case ECT_TEXT:
					{
						if(currentGame->GetChatBox()->Skip())
						{
							// 다음 페이지가 또 남았을 경우..
							if(currentGame->GetChatBox()->IsWaitingTextLeft() == true)
								break;

							currentGame->AddFlags(GAME_FLAG_LAST_SKIPPED);
						}
					}
					break;
				case ECT_NEW_LINE:
					{
						GetNekoNovel()->GetCurrentGame()->GetChatBox()->Skip();
					};
					break;
				case ECT_MOVIE:
					if(mTempInst)
					{
						if(GetNekoNovel()->GetCurrentGame()->GetGameMode() != GAME_RUN_PLAYING)
							break;
					
						nekoMovieInstance *movie = (nekoMovieInstance *)(nekoIBlockInstance *)mTempInst;
						if(movie->IsSkipAble())
							movie->Stop();
					}
					break;
				case ECT_FLASH:
					if(mTempInst)
					{
						if(GetNekoNovel()->GetCurrentGame()->GetGameMode() != GAME_RUN_PLAYING)
							break;

						nekoFlashInstance *flash = (nekoFlashInstance *)(nekoIBlockInstance *)mTempInst;
						if(flash->IsSkipAble())
							flash->Stop();
					}
					break;
				}
			}
		}

		return false;
	}

	// 게임이 로딩되었을 때
	void nekoOneCommand::OnGameLoaded(nekoThread *thread)
	{
		nekoGame *currentGame = GetNekoNovel()->GetCurrentGame();

		realArguments = arguments;
		// 변수에 대응
		list<string>::iterator siter = realArguments.begin();
		for(;siter != realArguments.end();++siter)
		{
			(*siter) = currentGame->ParseString((*siter).c_str());
		}

		const list<string> &arguments = realArguments;
		list<string>::const_iterator iter = realArguments.begin();

		switch(cmdType)
		{
		case ECT_ROTATE:
		case ECT_SCALE:
		case ECT_BOUNCE:
		case ECT_VIBE:
		case ECT_COLOR:
			{
				if(arguments.empty())
				{
					mTempInteger[0] = -1;
					return;
				}

				nekoIBlockInstance *inst = currentGame->GetInstance((*iter).c_str());
				if(inst == NULL)
				{
					mTempInteger[0] = -1;
					return;
				}

				mTempInteger[0] = (int32)inst;

			}
			break;
		case ECT_MOVE:
		case ECT_FADEIN:
		case ECT_FADEOUT:
			{
				mTempInst = 0;

				if(arguments.empty())
					return;

				if(mTempInst = currentGame->GetInstance((*iter).c_str()))
					mTempInst->AddRef();
			}
			break;
		case ECT_GIF_SPRITE:
			{
				mTempInst = 0;

				if(arguments.empty())
					return;

				nekoIBlockInstance *inst = currentGame->GetInstance((*iter).c_str());
				if(inst && inst->GetType() == BLOCK_GIFSPRITE)
					(mTempInst = inst)->AddRef();
			}
			break;
		case ECT_CG:
			{
				if(!arguments.size())
				{
					mTempInst = 0;
					return;
				}

				nekoIBlockInstance *inst = currentGame->GetInstance((*iter).c_str());
				if(inst == NULL || inst->GetType() != BLOCK_IMAGE)
				{
					mTempInst = 0;
					return;
				}

				(mTempInst = inst)->AddRef();
			}
			break;
		case ECT_FLASH:
			{
				mTempInst = 0;

				if(iter == arguments.end())
					return;

				nekoIBlockInstance *inst = currentGame->GetInstance((*iter).c_str());
				if(inst && inst->GetType() == BLOCK_FLASH)
					(mTempInst = inst)->AddRef();
			}
			break;
		case ECT_MOVIE:
			{
				mTempInst = 0;

				if(iter == arguments.end())
					return;

				nekoIBlockInstance *inst = currentGame->GetInstance((*iter).c_str());
				if(inst && inst->GetType() == BLOCK_MOVIE)
					(mTempInst = inst)->AddRef();
			}
			break;
		}
	}

	/// 해당 라인으로 이동
	void nekoCommands::_GotoLine(nekoThread *thread, bool internalRequest, uint32 lineNumber)
	{
		int32 spotStartLine = 0;

		if(internalRequest)
		{
			/*
			// 앞쪽 비주얼 스팟으로 이동.
			list<nekoSaveSpot>::iterator iter = mVisualSpotList.begin();
			for(uint32 i = 0;i < mVisualSpotList.size();++i, ++iter)
			{
				if((*iter).GetStartLine() > lineNumber)
					break;
			}

			// 맨 앞 부분이면..
			// 불러올 녀석이 없는거구만 뭐..
			if(iter == mVisualSpotList.begin())
				iter = mVisualSpotList.end();
			else
			{
				--iter;

				// 해당 비주얼 스팟의 상태 로드하기
				GetNekoNovel()->GetCurrentGame()->LoadSpot(*iter);

				//spotStartLine = (*iter).GetStartLine();
			}
			*/
		}
		else
		{
			spotStartLine = lineNumber - 1;
		}

		/// lineNumber까지 실행한다음. 
		list<nekoOneCommand *>::iterator iterCmd = mCommandList.begin();
		list<nekoOneCommand *>::iterator iterLastText = mCommandList.end();
		bool started = false;
		bool executed = false;

		for(uint32 i = 0;i < mCommandList.size();++i, ++iterCmd)
		{
			if(started == false)
			{
				if((*iterCmd)->startLine >= spotStartLine + 1)
					started = true;
				else
					continue;
			}

			if((*iterCmd)->startLine > lineNumber)
				break;

			/*
			// Text의 경우는 패스
			if((*iterCmd)->cmdType == ECT_TEXT)
			{
				iterLastText = iterCmd;
				continue;	
			}

			if((*iterCmd)->cmdType == ECT_APTEXT 
				|| (*iterCmd)->cmdType == ECT_CLEAR_TEXT
				|| (*iterCmd)->cmdType == ECT_NEW_LINE
				)
				continue;
			*/

			/// 실행하자!
			(*iterCmd)->Begin(iterCmd, thread, internalRequest);
			(*iterCmd)->End(iterCmd, thread, internalRequest);
			executed = true;
		}

		/*
		/// 마지막 Text만 실행
		if(iterLastText != mCommandList.end())
		{
			iterCmd = iterLastText;

			for(;iterCmd != mCommandList.end();++iterCmd)
			{
				if((*iterCmd)->startLine > lineNumber)
					break;

				// Text의 경우는 패스
				if(!((*iterCmd)->cmdType == ECT_TEXT
					|| (*iterCmd)->cmdType == ECT_APTEXT 
					|| (*iterCmd)->cmdType == ECT_CLEAR_TEXT
					|| (*iterCmd)->cmdType == ECT_NEW_LINE
					|| (*iterCmd)->cmdType == ECT_TEXT_SIZE
					|| (*iterCmd)->cmdType == ECT_TEXT_BOLD
					|| (*iterCmd)->cmdType == ECT_TEXT_FONT
					|| (*iterCmd)->cmdType == ECT_TEXT_COLOR
					|| (*iterCmd)->cmdType == ECT_TEXT_SHADOW_COLOR
					|| (*iterCmd)->cmdType == ECT_TEXT_SHADOW_TYPE)
					)
					continue;

				/// 실행하자!
				(*iterCmd)->Begin(iterCmd, thread, internalRequest);
				(*iterCmd)->End(iterCmd, thread, internalRequest);
				executed = true;
			}
		}
		*/

		if(internalRequest == false)
		{
			thread->mPlayingPos = iterCmd;
			if(executed == false)
			{
				if(thread->mPlayingPos != thread->mCommands->GetEndingPos())
				{
					(*iterCmd)->Begin(iterCmd, thread, internalRequest);
				}
			}
		}
	}

	/// Constructor
	nekoCommands::nekoCommands(const char8 *name)
		: mName(name)
	{
		static bool firstInit = false;
		if(firstInit == false)
		{
			for(int32 i = 0;i < ECT_COUNT;++i)
			{
				if(gCommandStrings[i])
					gCommandStringsLen[i] = strlen(gCommandStrings[i]);
				else
					gCommandStringsLen[i] = 0;
			}
			firstInit = true;
		}
	}

	/// Destructor
	nekoCommands::~nekoCommands()
	{
		list<nekoOneCommand *>::iterator iter = mCommandList.begin();
		for(uint32 i = 0;i < mCommandList.size();++i, ++iter)
			delete (*iter);
	}

	/// 파일로 부터 스크립트 가져오기
#	define MAX_LINE_SIZE 8192
	result nekoCommands::LoadFromFile(const char8 *fileName)
	{
		char8 *fileData;
		static char8 lineBuf[MAX_LINE_SIZE];
		int32 length;

		if(FAILED(GetNekoNovel()->GetFileSystem()->LoadData(nekoUtility::FormString("스크립트\\%s", fileName), &fileData, &length)))
		{
			LogPrint(LOG_LEVEL_WARN, "'%s' 스크립트 파일을 로드할 수 없습니다.", fileName);
			return E_FAIL;
		}
		
		switch(GetNekoNovel()->GetFileSystem()->GetMode())
		{
		// 순수 텍스트 파일
		//case EFM_LOCALFILE:
		default:
			{
				int32 a = 0;
				int32 b = 0;
				int32 line = 1;
				while(fileData[a] != '\0')
				{
					// 한줄이 끝났을 경우
					if(fileData[a] == '\n' || fileData[a] == '\r')
					{
						lineBuf[b] = '\0';

						// '\r' 건너뛰기.
						if(fileData[a] == '\r' && fileData[a + 1] == '\n')
							++a;

						if(b != 0)	
						{
							// 컴파일하기
							if(FAILED(AddCommand(line, lineBuf)))
							{
								LogPrint(LOG_LEVEL_WARN, "[%s:%d] 스크립트 명령어가 잘못 되었어요. 건너 뛸께요..", fileName, line);
							}
						}
						
						line++;
						b = 0;
						a++; // '\n' 건너 뛰기
						continue;
					}

					lineBuf[b++] = fileData[a++];
					if(b >= MAX_LINE_SIZE)
					{
						delete []fileData;
						LogPrint(LOG_LEVEL_WARN, "[%s:%d] 한줄의 최대 길이 %d 바이트를 초과하였습니다.", fileName, line, MAX_LINE_SIZE);
						return E_FAIL;
					}
				}

				// 마지막 내용이 있었을 경우
				if(b > 0)
				{
					lineBuf[b++] = '\0';
					// 컴파일하기
					if(FAILED(AddCommand(line, lineBuf)))
					{
						LogPrint(LOG_LEVEL_WARN, "[%s:%d] 스크립트 명령어가 잘못 되었어요. 건너 뛸께요..", fileName, line);
					}
				}
			}
			break;
		// 컴파일된 파일
		//case EFM_PACKAGE:
			{

			}
			break;
		}

		delete []fileData;
		return S_OK;
	}

	result nekoCommands::OnLineUpdated(uint32 lineNumber, const char8 *message)
	{
		result hr = S_OK;

		hr = UpdateCommand(lineNumber, message);
		//OutputDebugString(nekoUtility::FormString("%d 라인 수정됨. %s\n", lineNumber, message));

		return hr;
	}

	/// 몇 라인이 삭제되었다.
	result nekoCommands::OnLineRemoved(uint32 startLine, uint32 lineCount)
	{
		result hr = S_OK;

		//OutputDebugString(nekoUtility::FormString("%d 부터 %d까지 라인 삭제됨.\n", startLine, startLine + lineCount - 1));

		list<nekoOneCommand *>::iterator iter = mCommandList.begin();
		for(;iter != mCommandList.end();)
		{
			if((*iter)->startLine >= startLine)
			{
				if((*iter)->startLine < startLine + lineCount)
				{
					delete (*iter);
					mCommandList.erase(iter++);
					continue;
				}
				else
				{
					(*iter)->startLine -= lineCount;
				}
			}

			++iter;
		}

		return hr;
	}

	/// 새 라인이 입력되었다.
	result nekoCommands::OnLineInserted(uint32 lineNumber, const char8 *message)
	{
		result hr = S_OK;

		hr = AddCommand(lineNumber, message);
		//OutputDebugString(nekoUtility::FormString("%d 라인 %s\n", lineNumber, message));

		return hr;
	}

	void nekoCommands::UpdateLineCount(uint32 lineCount)
	{
		/*
		while(mVisualSpotList.size() * 100 < lineCount)
		{
			nekoSaveSpot saveSpot(ESSF_BLOCK_STATE, mVisualSpotList.size() * 100 + 1);
			mVisualSpotList.push_back(saveSpot);
		}

		while(mVisualSpotList.size() * 100 > lineCount + 100)
		{
			mVisualSpotList.pop_back();
		}
		*/
	}

	/// 새 커맨드 추가
	result nekoCommands::AddCommand(uint32 lineNumber, const char8 *message)
	{
		result hr;
		E_COMMAND_TYPE cmdType;
		list<string> arguments;

		// 명령어 파악하기
		hr = GetCommandType(message, &cmdType, arguments);

		// 해당 위치에 삽입하기
		list<nekoOneCommand *>::iterator iter = mCommandList.begin();
		list<nekoOneCommand *>::iterator iterFound = mCommandList.end();
		for(uint32 i = 0;i < mCommandList.size();++i, ++iter)
		{
			if((*iter)->startLine <= lineNumber)
			{
				iterFound = iter;
				++iterFound;
			}
			else
			{
				(*iter)->startLine++;
			}
		}

		if(SUCCEEDED(hr) && cmdType < ECT_NOTHING)
		{
			nekoOneCommand *cmd = nekoNew nekoOneCommand;
			cmd->arguments = arguments;
			cmd->cmdType = cmdType;
			cmd->startLine = lineNumber + 1;
			mCommandList.insert(iterFound, cmd);
		}

		return hr;
	}

	/// 기존 커맨드 수정
	result nekoCommands::UpdateCommand(uint32 lineNumber, const char8 *message)
	{
		result hr;
		E_COMMAND_TYPE cmdType;
		list<string> arguments;

		// 명령어 파악하기
		hr = GetCommandType(message, &cmdType, arguments);

		// 해당 위치에 삽입하기
		list<nekoOneCommand *>::iterator iter = mCommandList.begin();
		list<nekoOneCommand *>::iterator iterFound = mCommandList.end();
		bool found = false;
		for(uint32 i = 0;i < mCommandList.size();++i, ++iter)
		{
			if((*iter)->startLine == lineNumber)
			{
				if(FAILED(hr) || cmdType >= ECT_NOTHING)
				{
					delete (*iter);
					mCommandList.erase(iter);

					return hr;
				}

				(*iter)->arguments = arguments;
				(*iter)->cmdType = cmdType;

				found = true;
				return hr;
			}
			else if((*iter)->startLine < lineNumber)
			{
				iterFound = iter;
			}
		}

		if(found == false && SUCCEEDED(hr) && cmdType < ECT_NOTHING)
		{
			nekoOneCommand *cmd = nekoNew nekoOneCommand;
			cmd->arguments = arguments;
			cmd->cmdType = cmdType;
			cmd->startLine = lineNumber;

			if(iterFound == mCommandList.end())
				mCommandList.push_front(cmd);
			else
				mCommandList.insert(++iterFound, cmd);
		}

		return hr;
	}

	/*
	/// 비주얼 스팟 추가
	result nekoCommands::AddVisualSpot(uint32 lineNumber)
	{
		mLockSection.Begin();

		list<nekoSaveSpot>::iterator iter = mVisualSpotList.begin();
		for(uint32 i = 0;i < mVisualSpotList.size();++i, ++iter)
		{
			// 비주얼 스팟이 이미 그 자리에 있었다면
			if((*iter).GetStartLine() == i)
			{
				// 다음 라인으로 넘겨버리기
				(*iter).SetStartLine((*iter).GetStartLine() + 1);
			}
		}

		nekoSaveSpot vSpot(ESSF_BLOCK_STATE, lineNumber);
		mVisualSpotList.push_back(vSpot);

		mLockSection.End();

		return S_OK;
	}
	*/

#define IF_CMD(x, size) (size > 0 && _strnicmp(message, x, size) == 0 && (message += size))

	/// 커맨드 타입 얻기
	result nekoCommands::GetCommandType(const char8 *message, E_COMMAND_TYPE *cmdType, list<string> &arguments)
	{
		*cmdType = ECT_NOTHING;

		if(!message || message[0] == '\0' || message[0] == '\n')
			return S_OK;
		
		// 명령어 체크
		for(int32 i = 0;i < ECT_COUNT && gCommandStrings[i];++i)
		{
			if(IF_CMD((const char8 *)gCommandStrings[i], gCommandStringsLen[i]))
			{
				if(message[0] != '\0' && message[0] != ' '
					&& message[0] != '\r' && message[0] != '\n')
				{
					message -= gCommandStringsLen[i];
					continue;
				}
				*cmdType = (E_COMMAND_TYPE)i;
				break;
			}
		}
		
		if(*cmdType == ECT_NOTHING)
		{
			const char8 *p = message;

			// 일단 공백 없애봐.
			while(*p == ' ' && *p != '\n')
				++p;

			// 주석이구만!
			if(strncmp(p, "//", 2) == 0)
				return S_OK;
			else if(*p == '\n')
				return S_OK;
			
			*cmdType = ECT_UNKNOWN;
			//LogPrint(LOG_LEVEL_WARN, "'%s' 가 무슨 명령인지 알 수 없어요!", p);
			return E_FAIL;
		}


		// 공백일 경우, 다음에 arguments가 나온다.
		if(*message == ' ')
		{
			int32 cnt = 0;
			static char8 tmpBuffer[MAX_LINE_SIZE];

			++message;

			const char8 *p = message;
			char8 *o = tmpBuffer;
			int32 argCnt = 1;
			int32 size = MAX_LINE_SIZE;

			while(*p != '\r' && *p != '\n' && *p != '\0')
			{
				if((gCmdParametersCount[*cmdType] == -1 || argCnt < gCmdParametersCount[*cmdType]) && *p == ' ')
				{
					*o = '\0';
					arguments.push_back(tmpBuffer);
					++argCnt;

					size = MAX_LINE_SIZE;
					o = tmpBuffer;

					p++;
					continue;
				}

				*o++ = *p++;
				if(!--size) return E_TOO_LONG;
			}
			*o = '\0';

			// 설마 없어?!
			if(tmpBuffer[0] != '\0')
				arguments.push_back(tmpBuffer);
		}
		else
		{
			if(*message != '\r' &&*message != '\n' && *message != '\0')
			{
				*cmdType = ECT_UNKNOWN;
				return E_FAIL;
			}
		}

		return S_OK;
	}

	list<nekoOneCommand *>::iterator nekoCommands::GetPosByLine(uint32 lineNum, bool findAround)
	{
		list<nekoOneCommand *>::iterator iter = mCommandList.begin();

		if(findAround)
		{
			for(;iter != mCommandList.end();++iter)
			{
				if((*iter)->startLine >= lineNum)
					break;
			}
		}
		else
		{
			for(;iter != mCommandList.end();++iter)
			{
				if((*iter)->startLine == lineNum)
					break;
			}
		}

		return iter;
	}

	list<nekoOneCommand *>::iterator nekoCommands::SearchFrom(list<nekoOneCommand *>::iterator from, E_COMMAND_TYPE cmdType)
	{
		list<nekoOneCommand *>::iterator iter = from;
		for(;iter != mCommandList.end();++iter)
		{
			if((*iter)->cmdType == cmdType)
				break;
		}

		return iter;
	}

	result nekoCommands::SearchBookMark(const char8 *name, list<nekoOneCommand *>::iterator &itOut)
	{
		if(stricmp(name, "첫줄") == 0)
		{
			itOut = mCommandList.begin();
			return S_OK;
		}
		else if(stricmp(name, "끝줄") == 0)
		{
			itOut = mCommandList.end();
			return S_OK;
		}

		list<nekoOneCommand *>::iterator iter = mCommandList.begin();
		for(;iter != mCommandList.end();++iter)
		{
			if((*iter)->cmdType == ECT_BOOKMARK &&
				(*iter)->arguments.size() == 1)
			{
				if(stricmp((*(*iter)->arguments.begin()).c_str(), name) == 0)
				{
					itOut = iter;
					return S_OK;
				}
			}
		}

		return E_FAIL;
	}

	bool nekoCommands::OnEvent(nekoThread *thread, const nekoEvent &event)
	{
		if(thread->mPlayingPos != mCommandList.end())
		{
			if((*(thread->mPlayingPos))->OnEvent(thread, event))
				return true;
		}

		return nekoIEventListener::OnEvent(event);
	}

	E_COMMAND_TYPE nekoCommands::GetCommandByName(const char8 *name)
	{
		if(!name)
			return ECT_UNKNOWN;

		for(uint32 i = 0;i < ECT_NOTHING;++i)
		{
			if(stricmp(name, gCommandStrings[i]) == 0)
				return (E_COMMAND_TYPE)i;
		}

		return ECT_UNKNOWN;
	}

	const char8 *nekoCommands::GetCommandString(int32 i)
	{
		return gCommandStrings[i];
	}

	const char8 **nekoCommands::GetCommandHelpStrings()
	{
		return gCommandHelps;
	}

	result nekoCommands::LoadCommandReplacements(uint32 editorColors[], uint32 commandColors[])
	{
		char8 *data;

		if(FAILED(GetNekoNovel()->GetFileSystem()->LoadData("Command.xml", &data, NULL, true)))
		{
			return E_FAIL;
		}

		TiXmlDocument doc;
		doc.Parse(data);

		if(doc.Error())
		{
			delete []data;
			LogPrint(LOG_LEVEL_WARN, "대체 명령어 파일에 오류가 있네요.. %d줄 - %s", doc.ErrorRow(), doc.ErrorDesc());
			return E_FAIL;
		}

		TiXmlElement *root = doc.FirstChildElement("NekoNovel_Command");
		if(root == NULL)
		{
			delete []data;
			LogPrint(LOG_LEVEL_WARN, "대체 명령어 파일에 오류가 있네요.. - 루트 노드가 존재하지 않아요.");
			return E_FAIL;
		}

		TiXmlElement *e = root->FirstChildElement();
		int32 destLen;
		const char8 *temp;
		while(e)
		{
			if(strcmp(e->Value(), "Node") == 0)
			{
				const char8 *src = e->Attribute("Name");
				const char8 *dest = e->Attribute("Replacement");
				if(!src || !dest)
					continue;

				// 목록에서 src 찾기
				for(int32 i = 0;i < ECT_COUNT;++i)
				{
					if(src && _stricmp(src, (const char8 *)gCommandStrings[i]) == 0)
					{
						destLen = strlen(dest);
						if(destLen >= 20)
						{
							LogPrint(LOG_LEVEL_WARN, "대체 명령어 '%s' 가 20바이트를 넘네요.. 죄송합니다. 패스합니다.", dest);
							continue;
						}

						// 대체하기
						strcpy((char8 *)gCommandStrings[i], dest);
						gCommandStringsLen[i] = strlen(dest);

						// 만약 색깔도 있다면..
						if(commandColors && (temp = e->Attribute("Color")))
						{
							commandColors[i] = atoi(temp);
							commandColors[i] = min(max(commandColors[i], 0), EECT_10);
						}
						break;
					}
				}
			} else if(editorColors && strcmp(e->Value(), "BgColor") == 0)
			{
				editorColors[0] = nekoUtility::TextToColor(e->GetText());
			} else if(editorColors && strcmp(e->Value(), "CommentColor") == 0)
			{
				editorColors[EECT_COMMENT] = nekoUtility::TextToColor(e->GetText());
			} else if(editorColors && strcmp(e->Value(), "Color") == 0)
			{
				if(temp = e->Attribute("Index"))
				{
					int32 idx = atoi(temp);
					editorColors[min(max(idx, 1), EECT_10)] = nekoUtility::TextToColor(e->GetText());
					editorColors[EECT_ENABLED] = 1;
				}
			}

			e = e->NextSiblingElement();
		}

#ifdef _DEBUG
		/*
		// 도움말 페이지 출력
		FILE *fp = fopen("help.html", "w");
		if(fp)
		{
			for(int32 i = 0;i < ECT_UNKNOWN - 1;++i)
			{
				if(!gCommandStringsLen[i])
					continue;
				fprintf(fp, "* <b><font color=\"#9A54CE\">%s</font></b><br> %s<br>---------------------------------<br>", gCommandStrings[i], gCommandHelps[i]);
			}
			
			fclose(fp);
		}
		*/

		// 내장 도움말 페이지 출력
		/*
		FILE *fp = fopen("help.txt", "w");
		if(fp)
		{
		for(int32 i = 0;i < ECT_UNKNOWN - 1;++i)
		{
			if(!gCommandStringsLen[i])
			continue;
			fprintf(fp, "NAME : %s\n\"\n%s\n\"\n,\n\n\n", gCommandStrings[i], gCommandHelps[i]);
		}

		fclose(fp);
		}
		*/
#endif
		
		delete []data;
		LogPrint(LOG_LEVEL_INFO, "대체 명령어 파일을 모두 읽었습니다~");
		return S_OK;
	}

}