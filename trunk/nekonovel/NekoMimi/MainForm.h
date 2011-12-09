#pragma once

#include "HelpForm.h"
#include "GamePublishForm.h"
#include "GameSettingForm.h"
#include "MainEventListener.h"
#include "ExtCanvas.h"
#include "ScriptFile.h"
#include "NewScriptFileForm.h"
#include "HelpComboItem.h"
#include "VarListForm.h"
#include "DraggableTabControl.h"

using namespace System;
using namespace System::Text;
using namespace System::IO;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace NekoMimi {


	static uint32 gEditorColors[EECT_COUNT];
	static uint32 gCommandColors[ECT_COUNT];

	/// <summary>
	/// frmMain에 대한 요약입니다.
	///
	/// 경고: 이 클래스의 이름을 변경하면 이 클래스가 의존하는
	///          모든 .resx 파일과 관련된 관리되는 리소스 컴파일러 도구의
	///          '리소스 파일 이름' 속성도 변경해야 합니다. 그렇지 않은 경우
	///          디자이너는 이 폼과 관련된 지역화된 리소스와
	///          올바르게 상호 작용할 수 없습니다.
	/// </summary>
	public ref class frmMain : public System::Windows::Forms::Form
	{
	public:
		private: System::Windows::Forms::ToolStripStatusLabel^  toolStripStatusLabel2;



		private: System::Windows::Forms::ColorDialog^  colorDialog;
		private: System::Windows::Forms::ToolStripMenuItem^  menuEditorUndo;
		private: System::Windows::Forms::ToolStripMenuItem^  menuEditorRedo;


	public: 

		static frmMain ^self;
	private: ScintillaNet::Scintilla^  txtEditor;
	private: System::Windows::Forms::ToolStripMenuItem^  보기VToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  화면비율SToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  로줄이기ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  원래크기로3ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  로줄이기2ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  글씨색깔FToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  주석CToolStripMenuItem;
	private: System::Windows::Forms::ComboBox^  cbHelp;
	private: System::Windows::Forms::TextBox^  txtLogging;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::TextBox^  txtHelpContent;
	private: System::Windows::Forms::Button^  btPutColor;
	private: System::Windows::Forms::ToolStripSeparator^  toolStripMenuItem3;
	private: System::Windows::Forms::ToolStripMenuItem^  왼쪽으로이동LToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  오른쪽으로이동RToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  사용자명령어UToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  사용중인변수리스트VToolStripMenuItem;
	private: System::Windows::Forms::Button^  btAddScript;
	private: System::Windows::Forms::Button^  btCloseScript;
	private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator2;
	private: System::Windows::Forms::ToolStripMenuItem^  스크립트가져오기OToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  작업중인스크립트닫기CToolStripMenuItem;
	private: System::Windows::Forms::ToolStripSeparator^  toolStripMenuItem4;
	private: System::Windows::Forms::ToolStripMenuItem^  이전스크립트로가기PToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  다음스크립트로넘어가기NToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  스크립트빨리보기VToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem10;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem9;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem8;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem7;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem6;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem5;
	private: System::Windows::Forms::ToolStripMenuItem^  번탭1tCtrl1ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator1;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem12;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem11;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem13;










	protected:

	public: 
		static String ^mRunPath;

		frmMain(void)
			: mLastLine(1)
			, mCurrentCommands(NULL)
			, mTestPlaying(false)
			, scriptLoadingNow(false)
			, mZoomLevel(1)
		{
			InitializeComponent();

#ifndef _DEBUG
			frmMain::mRunPath = Application::ExecutablePath->Substring(0, Application::ExecutablePath->LastIndexOf('\\'));
#else
			frmMain::mRunPath = Environment::CurrentDirectory;
#endif
			// 마지막에 역슬래시 넣기
			if(frmMain::mRunPath[frmMain::mRunPath->Length - 1] != L'\\')
				frmMain::mRunPath += L'\\';

			frmMain::self = this;
			mNekoNovel = GetNekoNovel();

			this->picCanvas = (gcnew ExtCanvas());
			this->picCanvas->Location = System::Drawing::Point(8, 52);
			this->picCanvas->Name = L"picCanvas";
			this->picCanvas->Size = System::Drawing::Size(800, 600);
			this->picCanvas->BorderStyle = BorderStyle::None;
			this->picCanvas->TabIndex = 0;
			this->picCanvas->TabStop = false;
			this->picCanvas->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &frmMain::picCanvas_MouseMove);
			this->picCanvas->SizeChanged += gcnew System::EventHandler(this, &frmMain::picCanvas_SizeChanged);
			this->picCanvas->ImeMode = System::Windows::Forms::ImeMode::Hangul;
			this->Controls->Add(this->picCanvas);

#ifndef _DEBUG
			mNekoNovel->SetAppFolderPath(ConvertToChar(frmMain::mRunPath));
#endif

			if(FAILED(mNekoNovel->Initialize((HWND)picCanvas->Handle.ToPointer(), picCanvas->Width, picCanvas->Height, 0)))
				return;

			// 샘플 게임 만들기.	
			mCurrentGame = mNekoNovel->LoadGame();
			if(mCurrentGame == NULL)
			{
				MessageBox::Show(this, "게임을 여는데 있어서 예기치 못한 오류가 발생하였습니다.", MIMI_TITLE, MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
				Application::Exit();
				return;
			}
			// 메모리 초기화 하기
			memset(gEditorColors, 0, sizeof(uint32) * EECT_COUNT);
			memset(gCommandColors, 0, sizeof(uint32) * ECT_COUNT);
			nekoCommands::LoadCommandReplacements(gEditorColors, gCommandColors);

			/// 도움말 불러오기
			int32 i = 0;
			const char8 *p;
			for(int32 i = 0;i < ECT_COUNT;++i)
			{
				p = nekoCommands::GetCommandString(i);
				if(p == NULL)
					break;
				if(*p == '\0')
					continue;
				HelpComboItem ^item = gcnew HelpComboItem(ConvertToString(p), i);
				cbHelp->Items->Add(item);
			}
			cbHelp->SelectedIndex = 0;

			if(gEditorColors[EECT_BACKCOLOR])
			{
				txtEditor->Styles[0]->BackColor = Color::FromArgb(gEditorColors[EECT_BACKCOLOR]);
				txtEditor->Styles[32]->BackColor = Color::FromArgb(gEditorColors[EECT_BACKCOLOR]);
			}
			for(uint32 i = 1;i < 11;++i)
			{
				if(gEditorColors[i])
				{
					txtEditor->Styles[20 + i]->ForeColor = Color::FromArgb(gEditorColors[i]);
					txtEditor->Styles[20 + i]->BackColor = Color::FromArgb(gEditorColors[EECT_BACKCOLOR]);
				}
			}


			OnGameLoaded();

			/// 스타일을 만든다.
			txtEditor->Styles[1]->BackColor = Color::Yellow;
			if(gEditorColors[EECT_COMMENT])
				txtEditor->Styles[2]->ForeColor = Color::FromArgb(gEditorColors[EECT_COMMENT]);
			else
				txtEditor->Styles[2]->ForeColor = Color::FromArgb(0, 128, 0);
			txtEditor->Styles[2]->Bold = true;
			if(gEditorColors[EECT_BACKCOLOR])
				txtEditor->Styles[2]->BackColor = Color::FromArgb(gEditorColors[EECT_BACKCOLOR]);
			txtEditor->Styles[3]->ForeColor = Color::FromArgb(150, 150, 200);
			if(gEditorColors[EECT_BACKCOLOR])
				txtEditor->Styles[3]->BackColor = Color::FromArgb(gEditorColors[EECT_BACKCOLOR]);

			/// 캔버스 사이즈 조정
			picCanvas->Width = mCurrentGame->GetEnvironment().mWidth;
			picCanvas->Height = mCurrentGame->GetEnvironment().mHeight;
		}

	protected:
		/// <summary>
		/// 사용 중인 모든 리소스를 정리합니다.
		/// </summary>
		~frmMain()
		{
			ClearHGlobal();

			if (components)
			{
				delete components;
			}
		}



	private: System::Windows::Forms::MenuStrip^  menuMain;
	private: System::Windows::Forms::ToolStripMenuItem^  파일FToolStripMenuItem;


	private: System::Windows::Forms::ToolStripMenuItem^  현재게임저장SToolStripMenuItem;
	private: System::Windows::Forms::ToolStripSeparator^  toolStripMenuItem1;
	private: System::Windows::Forms::ToolStripMenuItem^  프로그램종료XToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  도움말HToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  네코노벨에대해서ToolStripMenuItem;


	private: System::Windows::Forms::StatusStrip^  statusBar;


	private: System::Windows::Forms::TabPage^  firstPage;
	private: DraggableTabControl^  tabFiles;
	private: System::Windows::Forms::ToolStripMenuItem^  게임GToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  이위치에서실행RToolStripMenuItem;

	private: System::Windows::Forms::ToolStripSeparator^  toolStripMenuItem2;
	private: System::Windows::Forms::ToolStripMenuItem^  게임배포하기PToolStripMenuItem;






	private: System::Windows::Forms::ToolStripMenuItem^  게임설정RToolStripMenuItem;
	private: System::Windows::Forms::ToolStripStatusLabel^  toolStripStatusLabel1;
	private: System::Windows::Forms::ToolStripMenuItem^  선택스크립트만저장SToolStripMenuItem;
	private: System::Windows::Forms::ContextMenuStrip^  menuTabs;

	private: System::Windows::Forms::ToolStripMenuItem^  새스크립트AToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  스크립트가져오기IToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  스크립트삭제DToolStripMenuItem;

	private: System::ComponentModel::IContainer^  components;
	private:
		/// <summary>
		/// 필수 디자이너 변수입니다.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// 디자이너 지원에 필요한 메서드입니다.
		/// 이 메서드의 내용을 코드 편집기로 수정하지 마십시오.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->menuMain = (gcnew System::Windows::Forms::MenuStrip());
			this->파일FToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->선택스크립트만저장SToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->현재게임저장SToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripSeparator2 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->스크립트가져오기OToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->작업중인스크립트닫기CToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem1 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->프로그램종료XToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->보기VToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->화면비율SToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->원래크기로3ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->로줄이기ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->로줄이기2ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->글씨색깔FToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->주석CToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->사용자명령어UToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem4 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->이전스크립트로가기PToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->다음스크립트로넘어가기NToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->스크립트빨리보기VToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem10 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem9 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem8 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem7 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem6 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem5 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem12 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem11 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem13 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->번탭1tCtrl1ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripSeparator1 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->사용중인변수리스트VToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->게임GToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->이위치에서실행RToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem2 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->게임설정RToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->게임배포하기PToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->도움말HToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->네코노벨에대해서ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->menuEditorUndo = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->menuEditorRedo = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->statusBar = (gcnew System::Windows::Forms::StatusStrip());
			this->toolStripStatusLabel1 = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->toolStripStatusLabel2 = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->firstPage = (gcnew System::Windows::Forms::TabPage());
			this->tabFiles = (gcnew DraggableTabControl());
			this->menuTabs = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			this->새스크립트AToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->스크립트가져오기IToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->스크립트삭제DToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem3 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->왼쪽으로이동LToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->오른쪽으로이동RToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->colorDialog = (gcnew System::Windows::Forms::ColorDialog());
			this->txtEditor = (gcnew ScintillaNet::Scintilla());
			this->cbHelp = (gcnew System::Windows::Forms::ComboBox());
			this->txtLogging = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->txtHelpContent = (gcnew System::Windows::Forms::TextBox());
			this->btPutColor = (gcnew System::Windows::Forms::Button());
			this->btAddScript = (gcnew System::Windows::Forms::Button());
			this->btCloseScript = (gcnew System::Windows::Forms::Button());
			this->menuMain->SuspendLayout();
			this->statusBar->SuspendLayout();
			this->tabFiles->SuspendLayout();
			this->menuTabs->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->txtEditor))->BeginInit();
			this->SuspendLayout();
			// 
			// menuMain
			// 
			this->menuMain->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {this->파일FToolStripMenuItem, 
				this->보기VToolStripMenuItem, this->게임GToolStripMenuItem, this->도움말HToolStripMenuItem});
			this->menuMain->Location = System::Drawing::Point(0, 0);
			this->menuMain->Name = L"menuMain";
			this->menuMain->Size = System::Drawing::Size(1231, 24);
			this->menuMain->TabIndex = 3;
			this->menuMain->Text = L"menuStrip1";
			// 
			// 파일FToolStripMenuItem
			// 
			this->파일FToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(7) {this->선택스크립트만저장SToolStripMenuItem, 
				this->현재게임저장SToolStripMenuItem, this->toolStripSeparator2, this->스크립트가져오기OToolStripMenuItem, this->작업중인스크립트닫기CToolStripMenuItem, 
				this->toolStripMenuItem1, this->프로그램종료XToolStripMenuItem});
			this->파일FToolStripMenuItem->Name = L"파일FToolStripMenuItem";
			this->파일FToolStripMenuItem->Size = System::Drawing::Size(84, 20);
			this->파일FToolStripMenuItem->Text = L"네코노벨(&N)";
			// 
			// 선택스크립트만저장SToolStripMenuItem
			// 
			this->선택스크립트만저장SToolStripMenuItem->Name = L"선택스크립트만저장SToolStripMenuItem";
			this->선택스크립트만저장SToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::S));
			this->선택스크립트만저장SToolStripMenuItem->Size = System::Drawing::Size(273, 22);
			this->선택스크립트만저장SToolStripMenuItem->Text = L"편집중인 스크립트만 저장(&S)";
			this->선택스크립트만저장SToolStripMenuItem->Click += gcnew System::EventHandler(this, &frmMain::선택스크립트만저장SToolStripMenuItem_Click);
			// 
			// 현재게임저장SToolStripMenuItem
			// 
			this->현재게임저장SToolStripMenuItem->Name = L"현재게임저장SToolStripMenuItem";
			this->현재게임저장SToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>(((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::Shift) 
				| System::Windows::Forms::Keys::S));
			this->현재게임저장SToolStripMenuItem->Size = System::Drawing::Size(273, 22);
			this->현재게임저장SToolStripMenuItem->Text = L"현재 게임 저장(&S)";
			this->현재게임저장SToolStripMenuItem->Click += gcnew System::EventHandler(this, &frmMain::현재게임저장SToolStripMenuItem_Click);
			// 
			// toolStripSeparator2
			// 
			this->toolStripSeparator2->Name = L"toolStripSeparator2";
			this->toolStripSeparator2->Size = System::Drawing::Size(270, 6);
			// 
			// 스크립트가져오기OToolStripMenuItem
			// 
			this->스크립트가져오기OToolStripMenuItem->Name = L"스크립트가져오기OToolStripMenuItem";
			this->스크립트가져오기OToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::O));
			this->스크립트가져오기OToolStripMenuItem->Size = System::Drawing::Size(273, 22);
			this->스크립트가져오기OToolStripMenuItem->Text = L"스크립트 가져오기(&O)";
			this->스크립트가져오기OToolStripMenuItem->Click += gcnew System::EventHandler(this, &frmMain::스크립트가져오기OToolStripMenuItem_Click);
			// 
			// 작업중인스크립트닫기CToolStripMenuItem
			// 
			this->작업중인스크립트닫기CToolStripMenuItem->Name = L"작업중인스크립트닫기CToolStripMenuItem";
			this->작업중인스크립트닫기CToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::F4));
			this->작업중인스크립트닫기CToolStripMenuItem->Size = System::Drawing::Size(273, 22);
			this->작업중인스크립트닫기CToolStripMenuItem->Text = L"작업중인 스크립트 닫기(&C)";
			this->작업중인스크립트닫기CToolStripMenuItem->Click += gcnew System::EventHandler(this, &frmMain::작업중인스크립트닫기CToolStripMenuItem_Click);
			// 
			// toolStripMenuItem1
			// 
			this->toolStripMenuItem1->Name = L"toolStripMenuItem1";
			this->toolStripMenuItem1->Size = System::Drawing::Size(270, 6);
			// 
			// 프로그램종료XToolStripMenuItem
			// 
			this->프로그램종료XToolStripMenuItem->Name = L"프로그램종료XToolStripMenuItem";
			this->프로그램종료XToolStripMenuItem->Size = System::Drawing::Size(273, 22);
			this->프로그램종료XToolStripMenuItem->Text = L"프로그램 종료(&X)";
			this->프로그램종료XToolStripMenuItem->Click += gcnew System::EventHandler(this, &frmMain::프로그램종료XToolStripMenuItem_Click);
			// 
			// 보기VToolStripMenuItem
			// 
			this->보기VToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(8) {this->화면비율SToolStripMenuItem, 
				this->글씨색깔FToolStripMenuItem, this->toolStripMenuItem4, this->이전스크립트로가기PToolStripMenuItem, this->다음스크립트로넘어가기NToolStripMenuItem, 
				this->스크립트빨리보기VToolStripMenuItem, this->toolStripSeparator1, this->사용중인변수리스트VToolStripMenuItem});
			this->보기VToolStripMenuItem->Name = L"보기VToolStripMenuItem";
			this->보기VToolStripMenuItem->Size = System::Drawing::Size(59, 20);
			this->보기VToolStripMenuItem->Text = L"보기(&V)";
			// 
			// 화면비율SToolStripMenuItem
			// 
			this->화면비율SToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->원래크기로3ToolStripMenuItem, 
				this->로줄이기ToolStripMenuItem, this->로줄이기2ToolStripMenuItem});
			this->화면비율SToolStripMenuItem->Name = L"화면비율SToolStripMenuItem";
			this->화면비율SToolStripMenuItem->Size = System::Drawing::Size(291, 22);
			this->화면비율SToolStripMenuItem->Text = L"화면 비율(&S)";
			// 
			// 원래크기로3ToolStripMenuItem
			// 
			this->원래크기로3ToolStripMenuItem->Name = L"원래크기로3ToolStripMenuItem";
			this->원래크기로3ToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Alt | System::Windows::Forms::Keys::D1));
			this->원래크기로3ToolStripMenuItem->Size = System::Drawing::Size(197, 22);
			this->원래크기로3ToolStripMenuItem->Text = L"원래 크기로(&1)";
			this->원래크기로3ToolStripMenuItem->Click += gcnew System::EventHandler(this, &frmMain::원래크기로3ToolStripMenuItem_Click);
			// 
			// 로줄이기ToolStripMenuItem
			// 
			this->로줄이기ToolStripMenuItem->Name = L"로줄이기ToolStripMenuItem";
			this->로줄이기ToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Alt | System::Windows::Forms::Keys::D2));
			this->로줄이기ToolStripMenuItem->Size = System::Drawing::Size(197, 22);
			this->로줄이기ToolStripMenuItem->Text = L"3/4로 줄이기(&2)";
			this->로줄이기ToolStripMenuItem->Click += gcnew System::EventHandler(this, &frmMain::로줄이기ToolStripMenuItem_Click);
			// 
			// 로줄이기2ToolStripMenuItem
			// 
			this->로줄이기2ToolStripMenuItem->Name = L"로줄이기2ToolStripMenuItem";
			this->로줄이기2ToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Alt | System::Windows::Forms::Keys::D3));
			this->로줄이기2ToolStripMenuItem->Size = System::Drawing::Size(197, 22);
			this->로줄이기2ToolStripMenuItem->Text = L"2/4로 줄이기(&3)";
			this->로줄이기2ToolStripMenuItem->Click += gcnew System::EventHandler(this, &frmMain::로줄이기2ToolStripMenuItem_Click);
			// 
			// 글씨색깔FToolStripMenuItem
			// 
			this->글씨색깔FToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->주석CToolStripMenuItem, 
				this->사용자명령어UToolStripMenuItem});
			this->글씨색깔FToolStripMenuItem->Name = L"글씨색깔FToolStripMenuItem";
			this->글씨색깔FToolStripMenuItem->Size = System::Drawing::Size(291, 22);
			this->글씨색깔FToolStripMenuItem->Text = L"편집창 글자 모양(&F)";
			this->글씨색깔FToolStripMenuItem->Visible = false;
			// 
			// 주석CToolStripMenuItem
			// 
			this->주석CToolStripMenuItem->Name = L"주석CToolStripMenuItem";
			this->주석CToolStripMenuItem->Size = System::Drawing::Size(166, 22);
			this->주석CToolStripMenuItem->Text = L"주석(&C)";
			this->주석CToolStripMenuItem->Click += gcnew System::EventHandler(this, &frmMain::주석CToolStripMenuItem_Click);
			// 
			// 사용자명령어UToolStripMenuItem
			// 
			this->사용자명령어UToolStripMenuItem->Name = L"사용자명령어UToolStripMenuItem";
			this->사용자명령어UToolStripMenuItem->Size = System::Drawing::Size(166, 22);
			this->사용자명령어UToolStripMenuItem->Text = L"사용자 명령어(&U)";
			// 
			// toolStripMenuItem4
			// 
			this->toolStripMenuItem4->Name = L"toolStripMenuItem4";
			this->toolStripMenuItem4->Size = System::Drawing::Size(288, 6);
			// 
			// 이전스크립트로가기PToolStripMenuItem
			// 
			this->이전스크립트로가기PToolStripMenuItem->Name = L"이전스크립트로가기PToolStripMenuItem";
			this->이전스크립트로가기PToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>(((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::Shift) 
				| System::Windows::Forms::Keys::Tab));
			this->이전스크립트로가기PToolStripMenuItem->Size = System::Drawing::Size(291, 22);
			this->이전스크립트로가기PToolStripMenuItem->Text = L"이전 스크립트로 가기(&P)";
			this->이전스크립트로가기PToolStripMenuItem->Click += gcnew System::EventHandler(this, &frmMain::이전스크립트로가기PToolStripMenuItem_Click);
			// 
			// 다음스크립트로넘어가기NToolStripMenuItem
			// 
			this->다음스크립트로넘어가기NToolStripMenuItem->Name = L"다음스크립트로넘어가기NToolStripMenuItem";
			this->다음스크립트로넘어가기NToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::Tab));
			this->다음스크립트로넘어가기NToolStripMenuItem->Size = System::Drawing::Size(291, 22);
			this->다음스크립트로넘어가기NToolStripMenuItem->Text = L"다음 스크립트로 가기(&N)";
			this->다음스크립트로넘어가기NToolStripMenuItem->Click += gcnew System::EventHandler(this, &frmMain::다음스크립트로넘어가기NToolStripMenuItem_Click);
			// 
			// 스크립트빨리보기VToolStripMenuItem
			// 
			this->스크립트빨리보기VToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(10) {this->toolStripMenuItem10, 
				this->toolStripMenuItem9, this->toolStripMenuItem8, this->toolStripMenuItem7, this->toolStripMenuItem6, this->toolStripMenuItem5, 
				this->toolStripMenuItem12, this->toolStripMenuItem11, this->toolStripMenuItem13, this->번탭1tCtrl1ToolStripMenuItem});
			this->스크립트빨리보기VToolStripMenuItem->Name = L"스크립트빨리보기VToolStripMenuItem";
			this->스크립트빨리보기VToolStripMenuItem->Size = System::Drawing::Size(291, 22);
			this->스크립트빨리보기VToolStripMenuItem->Text = L"스크립트 탭 단축키(&T)";
			// 
			// toolStripMenuItem10
			// 
			this->toolStripMenuItem10->Name = L"toolStripMenuItem10";
			this->toolStripMenuItem10->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::D1));
			this->toolStripMenuItem10->Size = System::Drawing::Size(172, 22);
			this->toolStripMenuItem10->Tag = L"1";
			this->toolStripMenuItem10->Text = L"1번 탭(&1)";
			this->toolStripMenuItem10->Click += gcnew System::EventHandler(this, &frmMain::toolStripMenuItem10_Click);
			// 
			// toolStripMenuItem9
			// 
			this->toolStripMenuItem9->Name = L"toolStripMenuItem9";
			this->toolStripMenuItem9->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::D2));
			this->toolStripMenuItem9->Size = System::Drawing::Size(172, 22);
			this->toolStripMenuItem9->Tag = L"2";
			this->toolStripMenuItem9->Text = L"2번 탭(&2)";
			this->toolStripMenuItem9->Click += gcnew System::EventHandler(this, &frmMain::toolStripMenuItem10_Click);
			// 
			// toolStripMenuItem8
			// 
			this->toolStripMenuItem8->Name = L"toolStripMenuItem8";
			this->toolStripMenuItem8->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::D3));
			this->toolStripMenuItem8->Size = System::Drawing::Size(172, 22);
			this->toolStripMenuItem8->Tag = L"3";
			this->toolStripMenuItem8->Text = L"3번 탭(&3)";
			this->toolStripMenuItem8->Click += gcnew System::EventHandler(this, &frmMain::toolStripMenuItem10_Click);
			// 
			// toolStripMenuItem7
			// 
			this->toolStripMenuItem7->Name = L"toolStripMenuItem7";
			this->toolStripMenuItem7->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::D4));
			this->toolStripMenuItem7->Size = System::Drawing::Size(172, 22);
			this->toolStripMenuItem7->Tag = L"4";
			this->toolStripMenuItem7->Text = L"4번 탭(&4)";
			this->toolStripMenuItem7->Click += gcnew System::EventHandler(this, &frmMain::toolStripMenuItem10_Click);
			// 
			// toolStripMenuItem6
			// 
			this->toolStripMenuItem6->Name = L"toolStripMenuItem6";
			this->toolStripMenuItem6->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::D5));
			this->toolStripMenuItem6->Size = System::Drawing::Size(172, 22);
			this->toolStripMenuItem6->Tag = L"5";
			this->toolStripMenuItem6->Text = L"5번 탭(&5)";
			this->toolStripMenuItem6->Click += gcnew System::EventHandler(this, &frmMain::toolStripMenuItem10_Click);
			// 
			// toolStripMenuItem5
			// 
			this->toolStripMenuItem5->Name = L"toolStripMenuItem5";
			this->toolStripMenuItem5->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::D6));
			this->toolStripMenuItem5->Size = System::Drawing::Size(172, 22);
			this->toolStripMenuItem5->Tag = L"6";
			this->toolStripMenuItem5->Text = L"6번 탭(&6)";
			this->toolStripMenuItem5->Click += gcnew System::EventHandler(this, &frmMain::toolStripMenuItem10_Click);
			// 
			// toolStripMenuItem12
			// 
			this->toolStripMenuItem12->Name = L"toolStripMenuItem12";
			this->toolStripMenuItem12->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::D7));
			this->toolStripMenuItem12->Size = System::Drawing::Size(172, 22);
			this->toolStripMenuItem12->Tag = L"7";
			this->toolStripMenuItem12->Text = L"7번 탭(&7)";
			this->toolStripMenuItem12->Click += gcnew System::EventHandler(this, &frmMain::toolStripMenuItem10_Click);
			// 
			// toolStripMenuItem11
			// 
			this->toolStripMenuItem11->Name = L"toolStripMenuItem11";
			this->toolStripMenuItem11->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::D8));
			this->toolStripMenuItem11->Size = System::Drawing::Size(172, 22);
			this->toolStripMenuItem11->Tag = L"8";
			this->toolStripMenuItem11->Text = L"8번 탭(&8)";
			this->toolStripMenuItem11->Click += gcnew System::EventHandler(this, &frmMain::toolStripMenuItem10_Click);
			// 
			// toolStripMenuItem13
			// 
			this->toolStripMenuItem13->Name = L"toolStripMenuItem13";
			this->toolStripMenuItem13->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::D9));
			this->toolStripMenuItem13->Size = System::Drawing::Size(172, 22);
			this->toolStripMenuItem13->Tag = L"9";
			this->toolStripMenuItem13->Text = L"9번 탭(&9)";
			this->toolStripMenuItem13->Click += gcnew System::EventHandler(this, &frmMain::toolStripMenuItem10_Click);
			// 
			// 번탭1tCtrl1ToolStripMenuItem
			// 
			this->번탭1tCtrl1ToolStripMenuItem->Name = L"번탭1tCtrl1ToolStripMenuItem";
			this->번탭1tCtrl1ToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::D0));
			this->번탭1tCtrl1ToolStripMenuItem->Size = System::Drawing::Size(172, 22);
			this->번탭1tCtrl1ToolStripMenuItem->Tag = L"10";
			this->번탭1tCtrl1ToolStripMenuItem->Text = L"10번 탭(&0)";
			this->번탭1tCtrl1ToolStripMenuItem->Click += gcnew System::EventHandler(this, &frmMain::toolStripMenuItem10_Click);
			// 
			// toolStripSeparator1
			// 
			this->toolStripSeparator1->Name = L"toolStripSeparator1";
			this->toolStripSeparator1->Size = System::Drawing::Size(288, 6);
			// 
			// 사용중인변수리스트VToolStripMenuItem
			// 
			this->사용중인변수리스트VToolStripMenuItem->Name = L"사용중인변수리스트VToolStripMenuItem";
			this->사용중인변수리스트VToolStripMenuItem->ShortcutKeys = System::Windows::Forms::Keys::F4;
			this->사용중인변수리스트VToolStripMenuItem->Size = System::Drawing::Size(291, 22);
			this->사용중인변수리스트VToolStripMenuItem->Text = L"사용중인 변수 리스트(&V)";
			this->사용중인변수리스트VToolStripMenuItem->Click += gcnew System::EventHandler(this, &frmMain::사용중인변수리스트VToolStripMenuItem_Click);
			// 
			// 게임GToolStripMenuItem
			// 
			this->게임GToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {this->이위치에서실행RToolStripMenuItem, 
				this->toolStripMenuItem2, this->게임설정RToolStripMenuItem, this->게임배포하기PToolStripMenuItem});
			this->게임GToolStripMenuItem->Name = L"게임GToolStripMenuItem";
			this->게임GToolStripMenuItem->Size = System::Drawing::Size(59, 20);
			this->게임GToolStripMenuItem->Text = L"게임(&G)";
			// 
			// 이위치에서실행RToolStripMenuItem
			// 
			this->이위치에서실행RToolStripMenuItem->Name = L"이위치에서실행RToolStripMenuItem";
			this->이위치에서실행RToolStripMenuItem->ShortcutKeys = System::Windows::Forms::Keys::F5;
			this->이위치에서실행RToolStripMenuItem->Size = System::Drawing::Size(230, 22);
			this->이위치에서실행RToolStripMenuItem->Text = L"이 위치에서 실행/정지(&R)";
			this->이위치에서실행RToolStripMenuItem->Click += gcnew System::EventHandler(this, &frmMain::이위치에서실행RToolStripMenuItem_Click);
			// 
			// toolStripMenuItem2
			// 
			this->toolStripMenuItem2->Name = L"toolStripMenuItem2";
			this->toolStripMenuItem2->Size = System::Drawing::Size(227, 6);
			// 
			// 게임설정RToolStripMenuItem
			// 
			this->게임설정RToolStripMenuItem->Name = L"게임설정RToolStripMenuItem";
			this->게임설정RToolStripMenuItem->Size = System::Drawing::Size(230, 22);
			this->게임설정RToolStripMenuItem->Text = L"게임 설정(&R)";
			this->게임설정RToolStripMenuItem->Click += gcnew System::EventHandler(this, &frmMain::게임설정RToolStripMenuItem_Click);
			// 
			// 게임배포하기PToolStripMenuItem
			// 
			this->게임배포하기PToolStripMenuItem->Name = L"게임배포하기PToolStripMenuItem";
			this->게임배포하기PToolStripMenuItem->ShortcutKeys = System::Windows::Forms::Keys::F9;
			this->게임배포하기PToolStripMenuItem->Size = System::Drawing::Size(230, 22);
			this->게임배포하기PToolStripMenuItem->Text = L"게임 배포하기(&P)";
			this->게임배포하기PToolStripMenuItem->Click += gcnew System::EventHandler(this, &frmMain::게임배포하기PToolStripMenuItem_Click);
			// 
			// 도움말HToolStripMenuItem
			// 
			this->도움말HToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->네코노벨에대해서ToolStripMenuItem});
			this->도움말HToolStripMenuItem->Name = L"도움말HToolStripMenuItem";
			this->도움말HToolStripMenuItem->Size = System::Drawing::Size(72, 20);
			this->도움말HToolStripMenuItem->Text = L"도움말(&H)";
			// 
			// 네코노벨에대해서ToolStripMenuItem
			// 
			this->네코노벨에대해서ToolStripMenuItem->Name = L"네코노벨에대해서ToolStripMenuItem";
			this->네코노벨에대해서ToolStripMenuItem->Size = System::Drawing::Size(203, 22);
			this->네코노벨에대해서ToolStripMenuItem->Text = L"네코 노벨에 대해서...(&A)";
			this->네코노벨에대해서ToolStripMenuItem->Click += gcnew System::EventHandler(this, &frmMain::네코노벨에대해서ToolStripMenuItem_Click);
			// 
			// menuEditorUndo
			// 
			this->menuEditorUndo->Name = L"menuEditorUndo";
			this->menuEditorUndo->Size = System::Drawing::Size(32, 19);
			// 
			// menuEditorRedo
			// 
			this->menuEditorRedo->Name = L"menuEditorRedo";
			this->menuEditorRedo->Size = System::Drawing::Size(32, 19);
			// 
			// statusBar
			// 
			this->statusBar->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->toolStripStatusLabel1, 
				this->toolStripStatusLabel2});
			this->statusBar->Location = System::Drawing::Point(0, 772);
			this->statusBar->Name = L"statusBar";
			this->statusBar->Size = System::Drawing::Size(1231, 22);
			this->statusBar->TabIndex = 7;
			this->statusBar->Text = L"statusStrip1";
			// 
			// toolStripStatusLabel1
			// 
			this->toolStripStatusLabel1->AutoSize = false;
			this->toolStripStatusLabel1->Name = L"toolStripStatusLabel1";
			this->toolStripStatusLabel1->Size = System::Drawing::Size(200, 17);
			// 
			// toolStripStatusLabel2
			// 
			this->toolStripStatusLabel2->AutoSize = false;
			this->toolStripStatusLabel2->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
			this->toolStripStatusLabel2->Name = L"toolStripStatusLabel2";
			this->toolStripStatusLabel2->Size = System::Drawing::Size(122, 17);
			// 
			// firstPage
			// 
			this->firstPage->Location = System::Drawing::Point(4, 22);
			this->firstPage->Name = L"firstPage";
			this->firstPage->Padding = System::Windows::Forms::Padding(3);
			this->firstPage->Size = System::Drawing::Size(1035, 0);
			this->firstPage->TabIndex = 0;
			this->firstPage->Text = L"새로운 시작 *";
			this->firstPage->UseVisualStyleBackColor = true;
			// 
			// tabFiles
			// 
			this->tabFiles->ContextMenuStrip = this->menuTabs;
			this->tabFiles->Controls->Add(this->firstPage);
			this->tabFiles->Location = System::Drawing::Point(8, 32);
			this->tabFiles->Name = L"tabFiles";
			this->tabFiles->SelectedIndex = 0;
			this->tabFiles->Size = System::Drawing::Size(1043, 21);
			this->tabFiles->TabIndex = 2;
			this->tabFiles->Selecting += gcnew System::Windows::Forms::TabControlCancelEventHandler(this, &frmMain::tabFiles_Selecting);
			this->tabFiles->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &frmMain::tabFiles_MouseDown);
			this->tabFiles->SelectedIndexChanged += gcnew System::EventHandler(this, &frmMain::tabFiles_SelectedIndexChanged);
			// 
			// menuTabs
			// 
			this->menuTabs->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(6) {this->새스크립트AToolStripMenuItem, 
				this->스크립트가져오기IToolStripMenuItem, this->스크립트삭제DToolStripMenuItem, this->toolStripMenuItem3, this->왼쪽으로이동LToolStripMenuItem, this->오른쪽으로이동RToolStripMenuItem});
			this->menuTabs->Name = L"menuTabs";
			this->menuTabs->Size = System::Drawing::Size(186, 120);
			// 
			// 새스크립트AToolStripMenuItem
			// 
			this->새스크립트AToolStripMenuItem->Name = L"새스크립트AToolStripMenuItem";
			this->새스크립트AToolStripMenuItem->Size = System::Drawing::Size(185, 22);
			this->새스크립트AToolStripMenuItem->Text = L"새 스크립트(&A)";
			this->새스크립트AToolStripMenuItem->Click += gcnew System::EventHandler(this, &frmMain::새스크립트AToolStripMenuItem_Click);
			// 
			// 스크립트가져오기IToolStripMenuItem
			// 
			this->스크립트가져오기IToolStripMenuItem->Name = L"스크립트가져오기IToolStripMenuItem";
			this->스크립트가져오기IToolStripMenuItem->Size = System::Drawing::Size(185, 22);
			this->스크립트가져오기IToolStripMenuItem->Text = L"스크립트 가져오기(&I)";
			this->스크립트가져오기IToolStripMenuItem->Click += gcnew System::EventHandler(this, &frmMain::스크립트가져오기IToolStripMenuItem_Click);
			// 
			// 스크립트삭제DToolStripMenuItem
			// 
			this->스크립트삭제DToolStripMenuItem->Name = L"스크립트삭제DToolStripMenuItem";
			this->스크립트삭제DToolStripMenuItem->Size = System::Drawing::Size(185, 22);
			this->스크립트삭제DToolStripMenuItem->Text = L"스크립트 닫기(&C)";
			this->스크립트삭제DToolStripMenuItem->Click += gcnew System::EventHandler(this, &frmMain::스크립트삭제DToolStripMenuItem_Click);
			// 
			// toolStripMenuItem3
			// 
			this->toolStripMenuItem3->Name = L"toolStripMenuItem3";
			this->toolStripMenuItem3->Size = System::Drawing::Size(182, 6);
			// 
			// 왼쪽으로이동LToolStripMenuItem
			// 
			this->왼쪽으로이동LToolStripMenuItem->Name = L"왼쪽으로이동LToolStripMenuItem";
			this->왼쪽으로이동LToolStripMenuItem->Size = System::Drawing::Size(185, 22);
			this->왼쪽으로이동LToolStripMenuItem->Text = L"왼쪽으로 이동(&L)";
			this->왼쪽으로이동LToolStripMenuItem->Click += gcnew System::EventHandler(this, &frmMain::왼쪽으로이동LToolStripMenuItem_Click);
			// 
			// 오른쪽으로이동RToolStripMenuItem
			// 
			this->오른쪽으로이동RToolStripMenuItem->Name = L"오른쪽으로이동RToolStripMenuItem";
			this->오른쪽으로이동RToolStripMenuItem->Size = System::Drawing::Size(185, 22);
			this->오른쪽으로이동RToolStripMenuItem->Text = L"오른쪽으로 이동(&R)";
			this->오른쪽으로이동RToolStripMenuItem->Click += gcnew System::EventHandler(this, &frmMain::오른쪽으로이동RToolStripMenuItem_Click);
			// 
			// txtEditor
			// 
			this->txtEditor->Location = System::Drawing::Point(816, 64);
			this->txtEditor->Margins->Left = 0;
			this->txtEditor->Margins->Margin0->Width = 30;
			this->txtEditor->Margins->Margin3->Width = -10;
			this->txtEditor->Name = L"txtEditor";
			this->txtEditor->Size = System::Drawing::Size(408, 696);
			this->txtEditor->Styles->LineNumber->BackColor = System::Drawing::SystemColors::ActiveBorder;
			this->txtEditor->TabIndex = 13;
			this->txtEditor->TextDeleted += gcnew System::EventHandler<ScintillaNet::TextModifiedEventArgs^ >(this, &frmMain::txtEditor_TextDeleted);
			this->txtEditor->BeforeTextInsert += gcnew System::EventHandler<ScintillaNet::TextModifiedEventArgs^ >(this, &frmMain::txtEditor_BeforeTextInsert);
			this->txtEditor->BeforeTextDelete += gcnew System::EventHandler<ScintillaNet::TextModifiedEventArgs^ >(this, &frmMain::txtEditor_BeforeTextDelete);
			this->txtEditor->SelectionChanged += gcnew System::EventHandler(this, &frmMain::txtEditor_SelectionChanged);
			this->txtEditor->TextInserted += gcnew System::EventHandler<ScintillaNet::TextModifiedEventArgs^ >(this, &frmMain::txtEditor_TextInserted);
			this->txtEditor->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &frmMain::txtEditor_KeyUp);
			this->txtEditor->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &frmMain::txtEditor_KeyDown);
			// 
			// cbHelp
			// 
			this->cbHelp->DropDownHeight = 365;
			this->cbHelp->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cbHelp->FormattingEnabled = true;
			this->cbHelp->IntegralHeight = false;
			this->cbHelp->Location = System::Drawing::Point(501, 664);
			this->cbHelp->Name = L"cbHelp";
			this->cbHelp->Size = System::Drawing::Size(192, 20);
			this->cbHelp->Sorted = true;
			this->cbHelp->TabIndex = 20;
			this->cbHelp->SelectedIndexChanged += gcnew System::EventHandler(this, &frmMain::cbHelp_SelectedIndexChanged);
			// 
			// txtLogging
			// 
			this->txtLogging->Location = System::Drawing::Point(8, 664);
			this->txtLogging->Multiline = true;
			this->txtLogging->Name = L"txtLogging";
			this->txtLogging->ReadOnly = true;
			this->txtLogging->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->txtLogging->Size = System::Drawing::Size(432, 96);
			this->txtLogging->TabIndex = 19;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->BackColor = System::Drawing::Color::Transparent;
			this->label1->Location = System::Drawing::Point(448, 670);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(49, 12);
			this->label1->TabIndex = 23;
			this->label1->Text = L"도움말 :";
			// 
			// txtHelpContent
			// 
			this->txtHelpContent->Location = System::Drawing::Point(448, 688);
			this->txtHelpContent->Multiline = true;
			this->txtHelpContent->Name = L"txtHelpContent";
			this->txtHelpContent->ReadOnly = true;
			this->txtHelpContent->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->txtHelpContent->Size = System::Drawing::Size(360, 72);
			this->txtHelpContent->TabIndex = 21;
			// 
			// btPutColor
			// 
			this->btPutColor->Location = System::Drawing::Point(698, 662);
			this->btPutColor->Name = L"btPutColor";
			this->btPutColor->Size = System::Drawing::Size(104, 24);
			this->btPutColor->TabIndex = 22;
			this->btPutColor->Text = L"색깔 넣기(&C)";
			this->btPutColor->UseVisualStyleBackColor = true;
			this->btPutColor->Click += gcnew System::EventHandler(this, &frmMain::btPutColor_Click);
			// 
			// btAddScript
			// 
			this->btAddScript->Location = System::Drawing::Point(1057, 31);
			this->btAddScript->Name = L"btAddScript";
			this->btAddScript->Size = System::Drawing::Size(84, 23);
			this->btAddScript->TabIndex = 24;
			this->btAddScript->Text = L"열기(&O)";
			this->btAddScript->UseVisualStyleBackColor = true;
			this->btAddScript->Click += gcnew System::EventHandler(this, &frmMain::btAddScript_Click);
			// 
			// btCloseScript
			// 
			this->btCloseScript->Location = System::Drawing::Point(1140, 31);
			this->btCloseScript->Name = L"btCloseScript";
			this->btCloseScript->Size = System::Drawing::Size(84, 23);
			this->btCloseScript->TabIndex = 25;
			this->btCloseScript->Text = L"닫기(&L)";
			this->btCloseScript->UseVisualStyleBackColor = true;
			this->btCloseScript->Click += gcnew System::EventHandler(this, &frmMain::btCloseScript_Click);
			// 
			// frmMain
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1231, 794);
			this->Controls->Add(this->btCloseScript);
			this->Controls->Add(this->btAddScript);
			this->Controls->Add(this->cbHelp);
			this->Controls->Add(this->txtLogging);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->txtHelpContent);
			this->Controls->Add(this->btPutColor);
			this->Controls->Add(this->statusBar);
			this->Controls->Add(this->tabFiles);
			this->Controls->Add(this->menuMain);
			this->Controls->Add(this->txtEditor);
			this->MainMenuStrip = this->menuMain;
			this->Name = L"frmMain";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"네코노벨 미미 Ver 1.0 ( 엔진 Ver 1.0 )";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &frmMain::frmMain_FormClosing);
			this->Resize += gcnew System::EventHandler(this, &frmMain::frmMain_Resize);
			this->menuMain->ResumeLayout(false);
			this->menuMain->PerformLayout();
			this->statusBar->ResumeLayout(false);
			this->statusBar->PerformLayout();
			this->tabFiles->ResumeLayout(false);
			this->menuTabs->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->txtEditor))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

	protected:


		ExtCanvas^  picCanvas;
		ScriptFile ^currentScriptFile;

		int32 mLastLine;
		nekoGame *mCurrentGame;
		nekoCommands *mCurrentCommands;
		NekoNovel *mNekoNovel;
		bool mTestPlaying;
		bool scriptLoadingNow;
		float32 mZoomLevel;

	protected:

		/// 타이틀 갱신
		void RefreshTitle()
		{
			this->Text = ConvertToString(mCurrentGame->GetName()) + " - 네코노벨 미미 Ver " + String::Format("{0:##.00}", MIMI_VERSION * 0.01f)
				+ " ( 엔진 Ver " + String::Format("{0:##.00}", mNekoNovel->GetVersion() * 0.01f) + " )";

			if(mTestPlaying)
				this->Text += " [테스트 플레이중]";
		}

		/// 게임이 새로 로드 되었을 때
		void OnGameLoaded()
		{
			RefreshTitle();

			tabFiles->TabPages->Clear();

			list<nekoCommands *> &lst = mCurrentGame->GetCommandsList();
			list<nekoCommands *>::iterator iter = lst.begin();

			if(lst.size() > 0)
				mCurrentCommands = (*iter);
			else
				mCurrentCommands = NULL;

			TabPage ^page;
			ScriptFile ^sf;
			nekoCommands *cmds;
			for(;iter != lst.end();)
			{
				cmds = (*iter++);

				page = gcnew TabPage(ConvertToString(cmds->GetName()));
				sf = gcnew ScriptFile();

				// 여기서 파일 열어줘야함.
				{
					try
					{
						StreamReader ^streamReader = gcnew StreamReader(frmMain::mRunPath + "스크립트\\" + ConvertToString(cmds->GetName()), System::Text::Encoding::GetEncoding("EUC-KR"));
						sf->sourceFile = streamReader->ReadToEnd();

						streamReader->Close();
					} catch(FileNotFoundException ^)
					{	
						MessageBox::Show(this, "'" + ConvertToString(cmds->GetName()) + "' 스크립트 파일을 열 수 없습니다!", MIMI_TITLE, MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
						mCurrentGame->RemoveCommands(cmds);

						continue;
					}
				}

				sf->commands = cmds;
				page->Tag = sf;
				tabFiles->TabPages->Add(page);
				tabFiles->SelectedTab = page;
			}

			if(lst.size() > 0)
			{
				/// 0번 페이지로 이동.
				tabFiles->SelectedIndex = 0;

				/// 탭이 한개 인경우 위 문장은 효용 없음.
				if(lst.size() == 1)
				{
					tabFiles_Selecting(tabFiles, gcnew TabControlCancelEventArgs(tabFiles->SelectedTab, 0, false, TabControlAction::Selecting));
					tabFiles_SelectedIndexChanged(tabFiles, nullptr);
				}

				//txtEditor->Parent = tabFiles->SelectedTab;
				txtEditor->Visible = true;
				//txtEditor->Dock = DockStyle::Right;
				currentScriptFile = (ScriptFile ^)tabFiles->SelectedTab->Tag;
			}
		}

	private: System::Void tabFiles_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
			 {
				 if(tabFiles->IsUpdating)
					 return;

				 if(tabFiles->SelectedTab)
				 {
					 //txtEditor->Parent = tabFiles->SelectedTab;
					 txtEditor->Visible = true;
					 //txtEditor->Dock = DockStyle::Fill;
					 currentScriptFile = (ScriptFile ^)tabFiles->SelectedTab->Tag;

					 txtEditor->Selection->Start = currentScriptFile->selectionStart;
					 txtEditor->Selection->Length = currentScriptFile->selectionLength;
					 //SetScrollPos((HWND)txtEditor->Handle.ToPointer(), SB_HORZ, currentScriptFile->hScroll, true);
					 //SetScrollPos((HWND)txtEditor->Handle.ToPointer(), SB_VERT, currentScriptFile->vScroll, true);
					 txtEditor->Scrolling->ScrollBy(0, currentScriptFile->vScroll);
				 }
				 else
				 {
					 txtEditor->Visible = false;
				 }

				 //txtEditor->Select(0, 0);
				 //txtEditor->Selection->Start = 0;
				 //txtEditor->Selection->Length = 0;
				 //txtEditor->ClearUndo();
				 txtEditor->UndoRedo->EmptyUndoBuffer();
				 txtEditor->Focus();

				 mLastLine = 1;
			 }
			private: String ^nkANSISubString(int32 startIndex, int32 endIndex)
			 {
				 return txtEditor->GetRange(startIndex, endIndex)->Text;
			 }
			private: System::Void txtEditor_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e)
			 {
				 int32 curLine = txtEditor->Lines->Current->Number + 1;

				 // 다음 행으로 넘어갔을 때
				 if(mLastLine != curLine)
				 {
					 if(e->KeyCode == Keys::Left || e->KeyCode == Keys::Right ||
						 e->KeyCode == Keys::Up || e->KeyCode == Keys::Down)
					 {
						 // 현재 있던 라인 컴파일하기
						 if(FAILED(mCurrentCommands->OnLineUpdated(curLine, ConvertToChar(txtEditor->Lines[curLine - 1]->Text))))
						 {
							 txtEditor->GetRange(txtEditor->Lines[curLine - 1]->StartPosition, txtEditor->Lines[curLine - 1]->EndPosition)->SetStyle(1);
						 }
						 else
						 {
							 txtEditor->GetRange(txtEditor->Lines[curLine - 1]->StartPosition, txtEditor->Lines[curLine - 1]->EndPosition)->SetStyle(0);
						 }

						 mLastLine = curLine;

						 return;
					 }

					 //mCurrentGame->GotoLineRequest(mCurrentCommands, curLine);
					 mCurrentGame->GotoLine(mCurrentCommands, curLine, false);
				 }

				 mLastLine = curLine;
			 }
	private: System::Void txtEditor_KeyUp(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e)
			 {
				 int32 curLine = txtEditor->Lines->Current->Number + 1;

				 if(mLastLine != curLine)
				 {
					 // 마지막에 있던 라인 컴파일하기
					 if(txtEditor->Lines->Count >= mLastLine)
					 {
						 if(FAILED(mCurrentCommands->OnLineUpdated(mLastLine, ConvertToChar(txtEditor->Lines[mLastLine - 1]->Text))))
						 {
							 txtEditor->GetRange(txtEditor->Lines[mLastLine - 1]->StartPosition, txtEditor->Lines[mLastLine - 1]->EndPosition)->SetStyle(1);
						 }
						 else
						 {
							 txtEditor->GetRange(txtEditor->Lines[mLastLine - 1]->StartPosition, txtEditor->Lines[mLastLine - 1]->EndPosition)->SetStyle(0);
						 }
					 }

					 //mCurrentGame->GotoLineRequest(mCurrentCommands, curLine);
					 mCurrentGame->GotoLine(mCurrentCommands, curLine, false);
				 }

				 mLastLine = curLine;
			 }

	private:
		delegate void TestDelegate(String ^txt);

	public:

		void OnLogRealReceived(String ^txt)
		{
			if(txtLogging->Text->Length == 0)
				txtLogging->Text = txt;
			else
				txtLogging->Text += Environment::NewLine + txt;
			txtLogging->Select(txtLogging->Text->Length, 0);
			txtLogging->ScrollToCaret();
		}

		static void OnLogReceived(const nekoEvent &e)
		{
			String ^str = ConvertToString(e.GetText(0));

			try
			{
				self->OnLogRealReceived(str);
			} catch(Exception ^)
			{
				try
				{
					TestDelegate ^realPut = gcnew TestDelegate(self , &frmMain::OnLogRealReceived);
					//self->Invoke(realPut, gcnew array<Object ^>{str});
					self->Invoke(realPut, gcnew array<Object ^>{str});
				} catch(Exception ^)
				{

				}
			}
		}

		/// 백업 파일을 만든다.
		void SaveEmergencyFiles()
		{
			System::Windows::Forms::DialogResult res = 
				MessageBox::Show(this, "죄송합니다. 네코노벨 엔진에 문제가 생겼습니다.\n프로그램이 종료되어야만 하는 상황입니다.\n\n현재 프로젝트를 저장하시고 종료하시겠습니까 ?", MIMI_TITLE, MessageBoxButtons::YesNo, MessageBoxIcon::Exclamation);
			if(res == System::Windows::Forms::DialogResult::No)
			{
				return;
			}

			현재게임저장SToolStripMenuItem_Click(nullptr, nullptr);
		}

	private: System::Void 네코노벨에대해서ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
			 {
				 (gcnew HelpForm())->ShowDialog(this);
			 }

	private: System::Void 게임배포하기PToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
			 {
				 System::Windows::Forms::DialogResult res = 
					 MessageBox::Show(this, "게임을 배포하기 전에 저장하는 편이 좋습니다.\n저장하시고 배포 작업을 하시겠습니까 ?", MIMI_TITLE, MessageBoxButtons::YesNo, MessageBoxIcon::Exclamation);
				 if(res == System::Windows::Forms::DialogResult::Yes)
				 {
					 현재게임저장SToolStripMenuItem_Click(sender, e);
				 }

				 (gcnew GamePublishForm())->ShowDialog(this);
			 }
	private: System::Void frmMain_Resize(System::Object^  sender, System::EventArgs^  e)
			 {
				 tabFiles->Width = ((this->Width - 195) - tabFiles->Left);
				 txtEditor->Width = ((this->Width - 16) - txtEditor->Left);
				 txtEditor->Height = ((this->Height - 64) - txtEditor->Top);

				 txtLogging->Height = ((this->Height - 64) - txtLogging->Top);
				 txtHelpContent->Height = ((this->Height - 64) - txtHelpContent->Top);

				 btAddScript->Left = (this->Width - 190);
				 btCloseScript->Left = (this->Width - 107);
			 }
	private: System::Void 이위치에서실행RToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
			 {
				 mTestPlaying = !mTestPlaying;

				 txtEditor->Enabled = !mTestPlaying;
				 btPutColor->Enabled = !mTestPlaying;

				 // 플레이 시작 ?!
				 int32 curLine = txtEditor->Lines->FromPosition(txtEditor->Lines->Current->StartPosition)->Number + 1;
				 if(mTestPlaying)
				 {
					 mCurrentGame->ChangeGameMode(GAME_RUN_PLAYING);
					 mCurrentGame->DoFrom(curLine + 1);
					 tabFiles->Enabled = false;
					 btAddScript->Enabled = false;
					 btCloseScript->Enabled = false;
				 }
				 else
				 {
					 mCurrentGame->ChangeGameMode(GAME_RUN_PREVIEW);
					 mCurrentGame->GotoLine(mCurrentCommands, curLine, false);
					 tabFiles->Enabled = true;
					 btAddScript->Enabled = true;
					 btCloseScript->Enabled = true;
				 }

				 RefreshTitle();
			 }
	private: System::Void 게임설정RToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
			 {
				 (gcnew GameSettingForm())->ShowDialog(this);
				 RefreshTitle();

				 // 해상도 변경 ?
				 picCanvas->Width = mCurrentGame->GetEnvironment().mWidth * mZoomLevel;
				 picCanvas->Height = mCurrentGame->GetEnvironment().mHeight * mZoomLevel;

				 mNekoNovel->OnWindowChanged(mCurrentGame->GetEnvironment().mWidth,
					 mCurrentGame->GetEnvironment().mHeight, false);
			 }
	private: System::Void picCanvas_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
			 {
				 statusBar->Items[0]->Text = String::Format("마우스 좌표 : ({0}, {1})", (int)(e->X / mZoomLevel), (int)(e->Y / mZoomLevel));
			 }

	private: System::Void 프로그램종료XToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
			 {
				 Application::Exit();
			 }
	private: System::Void 현재게임저장SToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
			 {
				 mCurrentGame->Save();

				 // iterate through all the tab pages
				 for(int i = 0; i < tabFiles->TabCount; i++)
				 {
					 ScriptFile ^sf = (ScriptFile ^)tabFiles->TabPages[i]->Tag;

					 if(sf->changed)
					 {
						 try
						 {
							 // 저장하기
							 StreamWriter ^streamWriter = gcnew StreamWriter(frmMain::mRunPath + "스크립트\\" + ConvertToString(sf->commands->GetName()), FALSE, System::Text::Encoding::GetEncoding("EUC-KR"));
							 if(i == tabFiles->SelectedIndex)
								 sf->sourceFile = txtEditor->Text;

							 streamWriter->Write(sf->sourceFile);
							 streamWriter->Close();

							 tabFiles->TabPages[i]->Text = ConvertToString(sf->commands->GetName());
						 } catch(Exception ^)
						 {
							 MessageBox::Show(this, "'" +  ConvertToString(sf->commands->GetName()) + "' 스크립트를 저장할 수 없습니다.", MIMI_TITLE, MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
							 return;
						 }

						 sf->changed = false;
					 }
				 }
			 }
	private: System::Void tabFiles_Selecting(System::Object^  sender, System::Windows::Forms::TabControlCancelEventArgs^  e)
			 {
				 if(tabFiles->IsUpdating)
					 return;

				 if(currentScriptFile)
				 {
					 // 현재 에디터 속성 가져오기
					 ScriptFile ^sf = (ScriptFile ^)currentScriptFile;
					 sf->sourceFile = txtEditor->Text;
					 sf->selectionStart = txtEditor->Selection->Start;
					 sf->selectionLength = txtEditor->Selection->Length;
					 sf->hScroll = GetScrollPos((HWND)txtEditor->Handle.ToPointer(), SB_HORZ);
					 sf->vScroll = txtEditor->Lines->FromPosition(txtEditor->PositionFromPoint(0, 0))->Number;//GetScrollPos((HWND)txtEditor->Handle.ToPointer(), SB_VERT);
					 

					 //txtEditor->Lines->Current->Text->ToCharArray();

					 int32 curLine = (txtEditor->Lines->Current->Number + 1);

					 // 현재 있던 라인 컴파일하기
					 mCurrentCommands->OnLineUpdated(curLine, 
						 ConvertToChar(nkANSISubString(txtEditor->Lines[curLine - 1]->StartPosition, txtEditor->Lines[curLine - 1]->EndPosition))
						 );
				 }

				 scriptLoadingNow = true;
				 txtEditor->Text = "";

				 mCurrentCommands = NULL;
				 if(e->TabPage)
				 {
					 ScriptFile ^sf = (ScriptFile ^)e->TabPage->Tag;

					 mCurrentCommands = sf->commands;

					 if(sf->sourceFile)
					 {
						 txtEditor->Text = sf->sourceFile;

						 array<String ^> ^lines = sf->sourceFile->Split('\n');

						 int32 lastIndex = -1;
						 int32 returnCharCount = 0;
						 while((lastIndex = sf->sourceFile->IndexOf('\n', lastIndex + 1)) >= 0)
							 ++returnCharCount;

						 if(returnCharCount > 0)
						 {
							 for(int32 i = 0;i < lines->Length;++i)
							 {
								 String ^str = lines[i];

								 // 새로 붙인 라인 컴파일 하기
								 if(FAILED(mCurrentCommands->OnLineUpdated(i + 1, ConvertToChar(lines[i]))))
								 {
									 txtEditor->GetRange(txtEditor->Lines[i]->StartPosition, txtEditor->Lines[i]->EndPosition)->SetStyle(1);
								 }
								 else
								 {
									 // 색깔 바꾸기
									 txtEditor->GetRange(txtEditor->Lines[i]->StartPosition, txtEditor->Lines[i]->EndPosition)->SetStyle(0);

									 // 주석이구만!
									 if(str->Length >= 2 && str[0] == '/' && str[1] == '/')
									 {
										 txtEditor->GetRange(txtEditor->Lines[i]->StartPosition, txtEditor->Lines[i]->EndPosition)->SetStyle(2);
									 }
									 else
									 {
										 SetCommandFade(i + 1);
									 }
								 }
							 }
						}	
					 }

					 mCurrentGame->GotoLine(mCurrentCommands, 1, false);
				 }

				 scriptLoadingNow = false;
			 }
	private: System::Void frmMain_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e)
			 {
				 System::Windows::Forms::DialogResult res = MessageBox::Show(this, "현재 작업중인 게임을 저장하고 종료하시겠어요?", MIMI_TITLE, MessageBoxButtons::YesNoCancel, MessageBoxIcon::Exclamation);

				 if(res == System::Windows::Forms::DialogResult::Yes)
				 {
					 현재게임저장SToolStripMenuItem_Click(sender, e);
					 return;
				 } else if(res == System::Windows::Forms::DialogResult::No)
				 {
					 return;
				 }

				 e->Cancel = true;
			 }
	private: System::Void 선택스크립트만저장SToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
			 {
				 ScriptFile ^sf = (ScriptFile ^)tabFiles->SelectedTab->Tag;

				 try
				 {
					 // 저장하기
					 StreamWriter ^streamWriter = gcnew StreamWriter(frmMain::mRunPath + "스크립트\\" + ConvertToString(sf->commands->GetName()), false, System::Text::Encoding::GetEncoding("EUC-KR"));
					 streamWriter->Write(txtEditor->Text);
					 streamWriter->Close();

					 tabFiles->SelectedTab->Text = ConvertToString(sf->commands->GetName());
				 } catch(Exception ^exp)
				 {
					 MessageBox::Show(this, "'" +  ConvertToString(sf->commands->GetName()) + "' 스크립트를 저장할 수 없습니다.\n예외 : " + exp->ToString(), MIMI_TITLE, MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
					 return;
				 }

				 sf->changed = false;
			 }
	private: System::Void 새스크립트AToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
			 {
				 NewScriptFileForm ^frm = gcnew NewScriptFileForm();
				 if(frm->ShowDialog(this) == System::Windows::Forms::DialogResult::Cancel)
					 return;

				 nekoCommands *cmds = mCurrentGame->CreateCommands(ConvertToChar(frm->mFileName));
				 if(cmds == NULL)
				 {
					 MessageBox::Show(this, "'" +  frm->mFileName + "' 이름의 스크립트는 이미 존재해요.", MIMI_TITLE, MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
					 return;
				 }

				 TabPage ^tp = gcnew TabPage(frm->mFileName);
				 ScriptFile ^sf = gcnew ScriptFile();
				 sf->commands = cmds;
				 tp->Tag = sf;
				 tabFiles->TabPages->Add(tp);
			 }
	private: System::Void 스크립트가져오기IToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
			 {
				 if(mTestPlaying)
					 return;

				 OpenFileDialog ^dlg = gcnew OpenFileDialog();
				 dlg->Title = "스크립트 파일 가져오기";
				 dlg->Multiselect = true;
				 dlg->Filter = "EUC-KR 형식의 텍스트 파일(*.txt)|*.txt";

				 if(dlg->ShowDialog(this) == System::Windows::Forms::DialogResult::OK)
				 {
					 String ^fileName;
					 nekoCommands *cmds;
					 TabPage ^page;

					 for(int32 i = 0;i < dlg->FileNames->Length;++i)
					 {
						 fileName = Path::GetFileName(dlg->FileNames[i]);

						 cmds = mCurrentGame->CreateCommands(ConvertToChar(fileName));
						 if(cmds == NULL)
						 {
							 continue;
						 }

						 page = gcnew TabPage(ConvertToString(cmds->GetName()));
						 ScriptFile ^sf = gcnew ScriptFile();

						 try
						 {
							 Stream ^stream = System::IO::File::OpenRead(dlg->FileNames[i]);

							 StreamReader ^streamReader = gcnew StreamReader(stream, System::Text::Encoding::GetEncoding("EUC-KR"));
							 sf->sourceFile = streamReader->ReadToEnd();

							 streamReader->Close();
							 stream->Close();
						 } catch(FileNotFoundException ^)
						 {
							 MessageBox::Show(this, "'" + fileName + "' 스크립트 파일을 열 수 없습니다!", MIMI_TITLE, MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
							 mCurrentGame->RemoveCommands(cmds);

							 continue;
						 }

						 // 다른 폴더에 있을 경우에만 복사.
						 if(String::Compare(dlg->FileNames[i], Application::StartupPath + "\\스크립트\\" + fileName, StringComparison::CurrentCultureIgnoreCase) != 0)
							 System::IO::File::Copy(dlg->FileNames[i], Application::StartupPath + "\\스크립트\\" + fileName, true);

						 sf->commands = cmds;
						 page->Tag = sf;
						 tabFiles->TabPages->Add(page);
					 }

					 Directory::SetCurrentDirectory(mRunPath);
				 }
			 }
	private: System::Void 스크립트삭제DToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
			 {
				 if(mTestPlaying)
					 return;


				 if(!tabFiles->SelectedTab)
					 return;

				 if(tabFiles->TabCount == 1)
				 {
					 MessageBox::Show(this, "마지막 '잎새' 입니다! ㅠ_ㅠ..", MIMI_TITLE, MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
					 return;
				 }

				 // 파일 저장 여부 물어보기.
				 ScriptFile ^sf = (ScriptFile ^)tabFiles->SelectedTab->Tag;

				 if(sf->changed)
				 {
					 System::Windows::Forms::DialogResult res = 
						 MessageBox::Show(this, "해당 스크립트 파일을 저장하고 닫으시겠습니까 ?", MIMI_TITLE, MessageBoxButtons::YesNoCancel, MessageBoxIcon::Exclamation);

					 if(res == System::Windows::Forms::DialogResult::Yes)
					 {
						 try
						 {
							 // 저장하기
							 StreamWriter ^streamWriter = gcnew StreamWriter(Application::StartupPath + "\\스크립트\\" + ConvertToString(sf->commands->GetName()), false, System::Text::Encoding::GetEncoding("EUC-KR"));
							 streamWriter->Write(txtEditor->Text);

							 tabFiles->SelectedTab->Text = ConvertToString(sf->commands->GetName());
						 } catch(Exception ^)
						 {
							 MessageBox::Show(this, "'" +  ConvertToString(sf->commands->GetName()) + "' 스크립트를 저장할 수 없습니다.", MIMI_TITLE, MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
							 return;
						 }
					 }
					 else if(res == System::Windows::Forms::DialogResult::Cancel)
					 {
						 return;
					 }
				 }

				 int32 selectedIndex = tabFiles->SelectedIndex;

				 mCurrentGame->RemoveCommands(sf->commands);
				 tabFiles->TabPages->Remove(tabFiles->SelectedTab);

				 tabFiles->SelectedIndex = max(min(selectedIndex, tabFiles->TabPages->Count - 1), 0);
			 }
	private: System::Void tabFiles_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
			 {
				 // check if the right mouse button was pressed
				 if(e->Button == System::Windows::Forms::MouseButtons::Right)
				 {
					 // iterate through all the tab pages
					 for(int i = 0; i < tabFiles->TabCount; i++)
					 {
						 // get their rectangle area and check if it contains the mouse cursor
						 System::Drawing::Rectangle r = tabFiles->GetTabRect(i);
						 if (r.Contains(e->Location))
						 {
							 tabFiles->SelectedIndex = i;
						 }
					 }
				 }

			 }
	private: System::Void txtEditor_SelectionChanged(System::Object^  sender, System::EventArgs^  e)
			 {
				 int32 curLine = (txtEditor->Lines->Current->Number + 1);
				 statusBar->Items[1]->Text = "편집 라인 : " + curLine;

				 if(mLastLine != curLine)
				 {
					 /// 기존 라인을 컴파일한다.
					 if(curLine != mLastLine)
					 {
						 String ^str = txtEditor->Lines[mLastLine - 1]->Text;

						 // 마지막에 있던 라인 컴파일하기
						 if(FAILED(mCurrentCommands->OnLineUpdated(mLastLine, ConvertToChar(str))))
						 {
							 txtEditor->GetRange(txtEditor->Lines[mLastLine - 1]->StartPosition, txtEditor->Lines[mLastLine - 1]->EndPosition)->SetStyle(1);
						 }
						 else
						 {
							 txtEditor->GetRange(txtEditor->Lines[mLastLine - 1]->StartPosition, txtEditor->Lines[mLastLine - 1]->EndPosition)->SetStyle(0);

							 // 주석이구만!
							 if(str->Length >= 2 &&
								 str[0] == '/' && str[1] == '/')
							 {
								 txtEditor->GetRange(txtEditor->Lines[mLastLine - 1]->StartPosition, txtEditor->Lines[mLastLine - 1]->EndPosition)->SetStyle(2);
							 }
							 else
							 {
								 SetCommandFade(mLastLine);
							 }
						 }

						 //mCurrentGame->GotoLineRequest(mCurrentCommands, curLine);
						 mCurrentGame->GotoLine(mCurrentCommands, curLine, false);
					 }

				 }
				 mLastLine = curLine;

				 // 도움말 참조

				 if(txtEditor->Selection->Length > 1 && txtEditor->Selection->Length < MAX_COMMAND_SIZE * 3/2)
				 {
					 int32 res = cbHelp->FindString(txtEditor->Selection->Text);
					 if(res < 0)
						 return;

					 cbHelp->SelectedIndex = res;
				 }
			 }
	private: System::Void cbHelp_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
			 {
				 const char8 **cmdHelpStrings = nekoCommands::GetCommandHelpStrings();
				 HelpComboItem ^item = (HelpComboItem ^)cbHelp->SelectedItem;

				 if(cmdHelpStrings[item->mValue] != NULL)
					 txtHelpContent->Text = item->mName + " " + ConvertToString(cmdHelpStrings[item->mValue])->Replace("\n", Environment::NewLine);
				 else
					 txtHelpContent->Text = "";
			 }
	private: System::Void btPutColor_Click(System::Object^  sender, System::EventArgs^  e)
			 {
				 colorDialog->AllowFullOpen = true;
				 colorDialog->ShowHelp = true;

				 if(colorDialog->ShowDialog(this) == System::Windows::Forms::DialogResult::OK)
				 {
					 String ^str;
					 str = colorDialog->Color.R.ToString() + " " + colorDialog->Color.G.ToString() + " " + colorDialog->Color.B.ToString();

					 txtEditor->Selection->Text = str;
				 }
			 }
	private:

		private: System::Void txtEditor_BeforeTextInsert(System::Object^  sender, ScintillaNet::TextModifiedEventArgs^  e)
		{
			if(scriptLoadingNow)
				return;

			String^ newData = e->Text;
			int32 returnCharCount = 0;
			int32 lastIndex = -1;

			int32 startIndex = txtEditor->Selection->Start;

			// 기존 녀석 제거.
			if(txtEditor->Selection->Length > 0)
			{
				while((lastIndex = txtEditor->Selection->Text->IndexOf('\n', lastIndex + 1)) >= 0)
					++returnCharCount;

				if(returnCharCount > 0)
				{
					int32 selLine = txtEditor->Lines->FromPosition(txtEditor->Selection->Start)->Number + 1;

					mCurrentCommands->OnLineRemoved(selLine + 1, returnCharCount);
				}
			}

			returnCharCount = 0;
			lastIndex = -1;

			int32 selLine = txtEditor->Lines->FromPosition(startIndex)->Number + 1;
		}
		private: System::Void txtEditor_BeforeTextDelete(System::Object^  sender, ScintillaNet::TextModifiedEventArgs^  e)
		{
			if(scriptLoadingNow)
				return;

			int32 returnCharCount = 0;
			int32 lastIndex = -1;

			if(e->Length > 1)
			{
				String ^str = txtEditor->GetRange(e->Position, e->Position + e->Length)->Text;
				while((lastIndex = str->IndexOf('\n', lastIndex + 1)) >= 0)
					++returnCharCount;

				if(returnCharCount > 0)
				{
					int32 selLine = txtEditor->Lines->FromPosition(e->Position)->Number + 1;

					mCurrentCommands->OnLineRemoved(selLine + 1, returnCharCount);
				}
			}
			else
			{
				int32 selLine = txtEditor->Lines->FromPosition(e->Position)->Number + 1;
			
				if(txtEditor->CharAt(e->Position) == '\n')
				{
					mCurrentCommands->OnLineRemoved(selLine + 1, 1);
					mCurrentCommands->OnLineUpdated(selLine, ConvertToChar(txtEditor->Lines[selLine - 1]->Text));
				}
			}
			
		}
		private: void SetCommandFade(int32 lastLine)
		{
			ScintillaNet::Range ^range = txtEditor->FindReplace->Find(gcnew ScintillaNet::Range(txtEditor->Lines[lastLine - 1]->StartPosition,
				txtEditor->Lines[lastLine - 1]->EndPosition, txtEditor), " ", 0);

		 if(!range || !range->Length)
			 txtEditor->GetRange(txtEditor->Lines[lastLine - 1]->StartPosition, txtEditor->Lines[lastLine - 1]->EndPosition)->SetStyle(3);
		 else
		 {
			 
			 ScintillaNet::Range ^range2 = txtEditor->GetRange(txtEditor->Lines[lastLine - 1]->StartPosition, range->Start);
			 E_COMMAND_TYPE cmdType = nekoCommands::GetCommandByName(ConvertToChar(range2->Text));

			 if(cmdType < ECT_NOTHING && gCommandColors[cmdType])
			 {
				 range2->SetStyle(20 + gCommandColors[cmdType]);
			 }
			 else
			 {
				 range2->SetStyle(3);
			 }

			 txtEditor->GetRange(range->Start, txtEditor->Lines[lastLine - 1]->EndPosition)->SetStyle(0);
		 }
		}
		private: System::Void txtEditor_TextInserted(System::Object^  sender, ScintillaNet::TextModifiedEventArgs^  e)
		{
			if(scriptLoadingNow)
				return;

			int32 lastIndex = -1;
			int32 returnCharCount = 0;
			int32 selLine = txtEditor->Lines->FromPosition(e->Position)->Number + 1;

			// 추가!
			array<String ^> ^lines = e->Text->Split('\n');

			while((lastIndex =  e->Text->IndexOf('\n', lastIndex + 1)) >= 0)
				++returnCharCount;

			if(returnCharCount > 0)
			{
				int32 startIndex;
				int32 endIndex;
				for(int32 i = 0;i < lines->Length;++i)
				{
					startIndex = (txtEditor->Lines[selLine + i - 1])->StartPosition;
					endIndex = (txtEditor->Lines[selLine + i - 1])->EndPosition;

					String ^str;

					if(!i)
						str = txtEditor->Lines[selLine + i - 1]->Text;
					else
						str = lines[i];

					// 새로 붙인 라인 컴파일 하기
					if((!i && FAILED(mCurrentCommands->OnLineUpdated(selLine + i, ConvertToChar(str)))) ||
						(i && (FAILED(mCurrentCommands->OnLineInserted(selLine + i - 1, ConvertToChar(str)))))
						)
					{
						txtEditor->GetRange(startIndex, endIndex)->SetStyle(1);
					}
					else
					{
						txtEditor->GetRange(startIndex, endIndex)->SetStyle(0);

						// 주석이구만!
						if(str->Length >= 2 &&
							str[0] == '/' && str[1] == '/')
						{
							txtEditor->GetRange(startIndex, endIndex)->SetStyle(2);
						}
						else
						{
							SetCommandFade(selLine + i);
						}
					}
					
				}
			}
			else
			{
				int32 curLine = selLine;

				/// 기존 라인을 컴파일한다.
				if(curLine != mLastLine)
				{
					String ^str = txtEditor->Lines[mLastLine - 1]->Text;

					// 마지막에 있던 라인 컴파일하기
					if(FAILED(mCurrentCommands->OnLineUpdated(mLastLine, ConvertToChar(str))))
					{
						txtEditor->GetRange(txtEditor->Lines[mLastLine - 1]->StartPosition, txtEditor->Lines[mLastLine - 1]->EndPosition)->SetStyle(1);
					}
					else
					{
						txtEditor->GetRange(txtEditor->Lines[mLastLine - 1]->StartPosition, txtEditor->Lines[mLastLine - 1]->EndPosition)->SetStyle(0);

						// 주석이구만!
						if(str->Length >= 2 &&
							str[0] == '/' && str[1] == '/')
						{
							txtEditor->GetRange(txtEditor->Lines[mLastLine - 1]->StartPosition, txtEditor->Lines[mLastLine - 1]->EndPosition)->SetStyle(2);
						}
						else
						{
							SetCommandFade(mLastLine);
						}
					}
				}

				mLastLine = curLine;
			};

			ScriptFile ^sf = (ScriptFile ^)tabFiles->SelectedTab->Tag;

			if(sf->changed == false)
			{
				tabFiles->SelectedTab->Text += " *";
				sf->changed = true;
			}
		}
		private: System::Void txtEditor_TextDeleted(System::Object^  sender, ScintillaNet::TextModifiedEventArgs^  e)
		{
			if(scriptLoadingNow)
				return;

			ScriptFile ^sf = (ScriptFile ^)tabFiles->SelectedTab->Tag;

			if(sf->changed == false)
			{
				tabFiles->SelectedTab->Text += " *";
				sf->changed = true;
			}
		}
		private: System::Void 원래크기로3ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
		{
			mZoomLevel = 1;
			picCanvas->Width = mCurrentGame->GetEnvironment().mWidth;
			picCanvas->Height = mCurrentGame->GetEnvironment().mHeight;
		}
		private: System::Void 로줄이기ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
		{
			mZoomLevel = 0.75f;
			picCanvas->Width = mCurrentGame->GetEnvironment().mWidth * mZoomLevel;
			picCanvas->Height = mCurrentGame->GetEnvironment().mHeight * mZoomLevel;
		}
		private: System::Void 로줄이기2ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
		{
			mZoomLevel = 0.5f;
			picCanvas->Width = mCurrentGame->GetEnvironment().mWidth * mZoomLevel;
			picCanvas->Height = mCurrentGame->GetEnvironment().mHeight * mZoomLevel;
		}
		private: System::Void picCanvas_SizeChanged(System::Object^  sender, System::EventArgs^  e) {

			mNekoNovel->SetScreenZoom(mZoomLevel);

			cbHelp->Left = picCanvas->Right - 360 + (cbHelp->Left - txtHelpContent->Left);
			btPutColor->Left = picCanvas->Right - 360 + (btPutColor->Left - txtHelpContent->Left);
		    txtHelpContent->Left = picCanvas->Right - 360;
			label1->Left = txtHelpContent->Left;
			txtLogging->Width = txtHelpContent->Left - 20;
			txtEditor->Left = picCanvas->Right + 10;
			txtEditor->Width = ((this->Width - 16) - txtEditor->Left);

			txtLogging->Top = picCanvas->Bottom + 10;
			cbHelp->Top = picCanvas->Bottom + 10;
			btPutColor->Top = picCanvas->Bottom + 10;
			label1->Top = picCanvas->Bottom +  10;
			txtHelpContent->Top = cbHelp->Bottom + 10;

			frmMain_Resize(nullptr, nullptr);
		}
		private: System::Void 주석CToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
		{
			
		}
		private: System::Void 왼쪽으로이동LToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
		{
			if(!tabFiles->SelectedTab || tabFiles->SelectedIndex == 0)
				return;

			int32 idx = tabFiles->SelectedIndex;
			TabPage ^page = tabFiles->SelectedTab;
			tabFiles->TabPages->Remove(page);
			tabFiles->TabPages->Insert(idx - 1, page);
			tabFiles->SelectedTab = page;

			mCurrentGame->ChangeCommandsOrder(((ScriptFile ^)page->Tag)->commands, idx - 1);
		}
		private: System::Void 오른쪽으로이동RToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
		{
			if(!tabFiles->SelectedTab || tabFiles->SelectedIndex == tabFiles->TabPages->Count - 1)
				return;

			int32 idx = tabFiles->SelectedIndex;
			TabPage ^page = tabFiles->SelectedTab;
			tabFiles->TabPages->Remove(page);
			tabFiles->TabPages->Insert(idx + 1, page);
			tabFiles->SelectedTab = page;

			mCurrentGame->ChangeCommandsOrder(((ScriptFile ^)page->Tag)->commands, idx + 1);
		}
		private: System::Void 사용중인변수리스트VToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
		{
			(gcnew VarListForm())->Show();
		}
		private: System::Void btAddScript_Click(System::Object^  sender, System::EventArgs^  e)
		{
			스크립트가져오기IToolStripMenuItem_Click(sender, e);
		}
		private: System::Void btCloseScript_Click(System::Object^  sender, System::EventArgs^  e)
		{
			스크립트삭제DToolStripMenuItem_Click(sender, e);
		}
		private: System::Void 이전스크립트로가기PToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
		{
			if(mTestPlaying)
				return;

			if(tabFiles->SelectedIndex - 1 < 0)
				tabFiles->SelectedIndex = tabFiles->TabPages->Count - 1;
			else
				tabFiles->SelectedIndex--;
		}
		private: System::Void 다음스크립트로넘어가기NToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
		{
			if(mTestPlaying)
				return;

			if(tabFiles->SelectedIndex + 1 >= tabFiles->TabPages->Count)
				tabFiles->SelectedIndex = 0;
			else
				tabFiles->SelectedIndex++;
		}
		private: System::Void 작업중인스크립트닫기CToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
		{
			if(mTestPlaying)
				return;

			스크립트삭제DToolStripMenuItem_Click(sender, e);
		}
		private: System::Void 스크립트가져오기OToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
		{
			스크립트가져오기IToolStripMenuItem_Click(sender, e);
		}

		// 탭간 빠른 이동
		private: System::Void toolStripMenuItem10_Click(System::Object^  sender, System::EventArgs^  e)
		{
			if(mTestPlaying)
				return;

			int32 newIdx = int::Parse( (String ^)((ToolStripMenuItem ^)sender)->Tag) - 1;
			
			if(newIdx >= tabFiles->TabPages->Count)
				return;

			tabFiles->SelectedIndex = newIdx;
		}
};
}

