#pragma once

#include "nekoParticle.h"
#include <nekoUtility.h>

namespace ParticleTool {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Form1에 대한 요약입니다.
	///
	/// 경고: 이 클래스의 이름을 변경하면 이 클래스가 의존하는
	///          모든 .resx 파일과 관련된 관리되는 리소스 컴파일러 도구의
	///          '리소스 파일 이름' 속성도 변경해야 합니다. 그렇지 않은 경우
	///          디자이너는 이 폼과 관련된 지역화된 리소스와
	///          올바르게 상호 작용할 수 없습니다.
	/// </summary>
	public ref class MainForm : public System::Windows::Forms::Form
	{
	protected:

		NekoNovel *mNekoNovel;
	private: System::Windows::Forms::ComboBox^  cmbDestBlend;
	protected: 
		nekoGame *mCurrentGame;
		nekoParticleInstance *mParticle;
	private: System::Windows::Forms::OpenFileDialog^  openFileDlg;
	private: System::Windows::Forms::TextBox^  txtEventTime;
	private: System::Windows::Forms::TextBox^  txtEventRotSpeed;
	private: System::Windows::Forms::TextBox^  txtEventMinX;
	private: System::Windows::Forms::TextBox^  txtEventGravityY;




	private: System::Windows::Forms::TextBox^  txtEventGravityX;
	private: System::Windows::Forms::TextBox^  txtEventColorB;


	private: System::Windows::Forms::TextBox^  txtEventColorG;

	private: System::Windows::Forms::TextBox^  txtEventColorR;

	private: System::Windows::Forms::TextBox^  txtColor4B;

	private: System::Windows::Forms::TextBox^  txtColor4G;

	private: System::Windows::Forms::TextBox^  txtColor4R;

	private: System::Windows::Forms::Label^  label23;
	private: System::Windows::Forms::TextBox^  txtColor2B;

	private: System::Windows::Forms::TextBox^  txtColor2G;

	private: System::Windows::Forms::TextBox^  txtColor2R;

	private: System::Windows::Forms::Label^  label22;
	private: System::Windows::Forms::TextBox^  txtMaxLifeTime;
	private: System::Windows::Forms::TextBox^  txtColor2A;
	private: System::Windows::Forms::TextBox^  txtColor1A;
	private: System::Windows::Forms::TextBox^  txtColor4A;
	private: System::Windows::Forms::TextBox^  txtColor3A;
	private: System::Windows::Forms::TextBox^  txtMinDirY;

	private: System::Windows::Forms::TextBox^  txtMinDirX;
	private: System::Windows::Forms::TextBox^  txtMaxDirY;



	private: System::Windows::Forms::TextBox^  txtMaxDirX;
	private: System::Windows::Forms::TextBox^  txtRadiusY;

	private: System::Windows::Forms::TextBox^  txtRadiusX;

	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::CheckBox^  cbRepeat;
	private: System::Windows::Forms::TextBox^  txtMaxScale;
	private: System::Windows::Forms::TextBox^  txtEventMaxY;

	private: System::Windows::Forms::TextBox^  txtEventMinY;

	private: System::Windows::Forms::TextBox^  txtEventMaxX;
	private: System::Windows::Forms::TextBox^  txtEventColorA;





	protected:
		nekoBGImageInstance *mBGImage;
		nekoParticleEmitter *mEmitter;
		nekoParticleEvent *mEvent;
	private: System::Windows::Forms::TextBox^  txtEventScaleSpeed;
	protected: 
	private: System::Windows::Forms::CheckBox^  cbEventScaleSpeed;
	private: System::Windows::Forms::Label^  label17;

	public:
		static String ^mRunPath;
		static MainForm ^self;
	private: System::Windows::Forms::StatusStrip^  statusBar; 
	private: System::Windows::Forms::ToolStripStatusLabel^  toolStripStatusLabel1;
	private: System::Windows::Forms::ToolStripMenuItem^  보기VToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  테스트배경화면불러오기F5ToolStripMenuItem;
	private: System::Windows::Forms::TextBox^  txtFirstAppearMax;
	private: System::Windows::Forms::TextBox^  txtFirstAppearMin;
	private: System::Windows::Forms::Label^  label24;

		// 최근 저장하거나 열은 파일 이름
		String ^mFileName;
	public:
		MainForm(void)
			: mEmitter(NULL)
			, mEvent(NULL)
			, mFileName("")
		{
			InitializeComponent();

			MainForm::self = this;
			mNekoNovel = GetNekoNovel();

#ifndef _DEBUG
			MainForm::mRunPath = Application::ExecutablePath->Substring(0, Application::ExecutablePath->LastIndexOf('\\')) + "\\";
#else
			MainForm::mRunPath = Environment::CurrentDirectory + "\\";
#endif

#ifndef _DEBUG
			mNekoNovel->SetAppFolderPath(ConvertToChar(MainForm::mRunPath));
#endif

			if(FAILED(mNekoNovel->Initialize((HWND)picCanvas->Handle.ToPointer(), picCanvas->Width, picCanvas->Height, 0)))
				return;

			// 샘플 게임 불러오기.	
			mCurrentGame = mNekoNovel->LoadGame();
			if(mCurrentGame == NULL)
			{
				MessageBox::Show(this, "게임을 여는데 있어서 예기치 못한 오류가 발생하였습니다.", PAPA_TITLE, MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
				Application::Exit();
				return;
			}

			mCurrentGame->GetChatBox()->SetVisible(false);

			mBGImage = (nekoBGImageInstance *)mCurrentGame->CreateInstance(BLOCK_BGIMAGE, "배경이미지");
			mParticle = (nekoParticleInstance *)mCurrentGame->CreateInstance(BLOCK_PARTICLE, "파티클");
			AddParticle("시작");
			UpdateCaption();

			cmbSrcBlend->SelectedIndex = 2;
			cmbDestBlend->SelectedIndex = 1;
		}

	protected:
		/// <summary>
		/// 사용 중인 모든 리소스를 정리합니다.
		/// </summary>
		~MainForm()
		{
			if (components)
			{
				delete components;
			}
		}

	protected: 
















	private: System::Windows::Forms::Button^  button2;
	private: System::Windows::Forms::Button^  button3;
	private: System::Windows::Forms::ToolStripMenuItem^  정보AToolStripMenuItem;
	private: System::Windows::Forms::ToolStripSeparator^  toolStripMenuItem1;
	private: System::Windows::Forms::ToolStripMenuItem^  프로그램종료XToolStripMenuItem;



	private: System::Windows::Forms::ToolStripMenuItem^  다른이름으로저장하기AToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  저장하기SToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  불러오기OToolStripMenuItem;
	private: System::Windows::Forms::ListBox^  lstEmitters;




	private: System::Windows::Forms::PictureBox^  picCanvas;



	private: System::Windows::Forms::ToolStripMenuItem^  새로만들기NToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  파일FToolStripMenuItem;

	private: System::Windows::Forms::MenuStrip^  mainMenu;
	private: System::Windows::Forms::TabControl^  tabControl1;
	private: System::Windows::Forms::TabPage^  tabPage1;


	private: System::Windows::Forms::TabPage^  tabPage2;
	private: System::Windows::Forms::Label^  label14;
	private: System::Windows::Forms::Label^  label13;
	private: System::Windows::Forms::Label^  label12;
	private: System::Windows::Forms::Label^  label11;
	private: System::Windows::Forms::Label^  label10;
private: System::Windows::Forms::LinkLabel^  labHelpTip;


	private: System::Windows::Forms::Label^  label9;
	private: System::Windows::Forms::Label^  label8;

	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::ListBox^  lstEvents;
private: System::Windows::Forms::Button^  btDelEvent;

private: System::Windows::Forms::Button^  btAddEvent;

	private: System::Windows::Forms::Button^  button1;

	private: System::Windows::Forms::Label^  label16;
	private: System::Windows::Forms::Label^  label15;
	private: System::Windows::Forms::Label^  label18;
	private: System::Windows::Forms::Label^  label20;
	private: System::Windows::Forms::Label^  label19;
private: System::Windows::Forms::CheckBox^  cbEventColor;

private: System::Windows::Forms::CheckBox^  cbEventGravity;

private: System::Windows::Forms::CheckBox^  cbEventChangeDir;


private: System::Windows::Forms::CheckBox^  cbEventRotSpeed;



	private: System::Windows::Forms::TextBox^  txtMaxCount;


	private: System::Windows::Forms::TextBox^  txtRevival;

private: System::Windows::Forms::TextBox^  txtGravityY;
private: System::Windows::Forms::TextBox^  txtGravityX;






private: System::Windows::Forms::TextBox^  txtSpawnPosY;

private: System::Windows::Forms::TextBox^  txtSpawnPosX;
private: System::Windows::Forms::TextBox^  txtMinScale;



private: System::Windows::Forms::ComboBox^  cmbSrcBlend;
private: System::Windows::Forms::TextBox^  txtMinLifeTime;

private: System::Windows::Forms::TextBox^  txtColor1R;
private: System::Windows::Forms::TextBox^  txtColor1B;





private: System::Windows::Forms::TextBox^  txtColor1G;
private: System::Windows::Forms::TextBox^  txtColor3B;


private: System::Windows::Forms::TextBox^  txtColor3G;

private: System::Windows::Forms::TextBox^  txtColor3R;
private: System::Windows::Forms::TextBox^  txtScaleSpeed;

private: System::Windows::Forms::TextBox^  txtRotationSpeed;





private: System::Windows::Forms::ToolStripMenuItem^  네코노벨에대해서AToolStripMenuItem;
private: System::Windows::Forms::TextBox^  txtImageName;

private: System::Windows::Forms::Label^  label21;
private: System::Windows::Forms::Button^  btFind;



	private:
		/// <summary>
		/// 필수 디자이너 변수입니다.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// 디자이너 지원에 필요한 메서드입니다.
		/// 이 메서드의 내용을 코드 편집기로 수정하지 마십시오.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(MainForm::typeid));
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->button3 = (gcnew System::Windows::Forms::Button());
			this->정보AToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->네코노벨에대해서AToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem1 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->프로그램종료XToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->다른이름으로저장하기AToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->저장하기SToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->불러오기OToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->lstEmitters = (gcnew System::Windows::Forms::ListBox());
			this->picCanvas = (gcnew System::Windows::Forms::PictureBox());
			this->새로만들기NToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->파일FToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->mainMenu = (gcnew System::Windows::Forms::MenuStrip());
			this->보기VToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->테스트배경화면불러오기F5ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
			this->tabPage1 = (gcnew System::Windows::Forms::TabPage());
			this->txtFirstAppearMax = (gcnew System::Windows::Forms::TextBox());
			this->txtFirstAppearMin = (gcnew System::Windows::Forms::TextBox());
			this->label24 = (gcnew System::Windows::Forms::Label());
			this->cbRepeat = (gcnew System::Windows::Forms::CheckBox());
			this->txtRadiusY = (gcnew System::Windows::Forms::TextBox());
			this->txtRadiusX = (gcnew System::Windows::Forms::TextBox());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->txtColor4B = (gcnew System::Windows::Forms::TextBox());
			this->txtColor4G = (gcnew System::Windows::Forms::TextBox());
			this->txtColor4R = (gcnew System::Windows::Forms::TextBox());
			this->txtColor2B = (gcnew System::Windows::Forms::TextBox());
			this->txtColor2G = (gcnew System::Windows::Forms::TextBox());
			this->txtColor2R = (gcnew System::Windows::Forms::TextBox());
			this->btFind = (gcnew System::Windows::Forms::Button());
			this->txtImageName = (gcnew System::Windows::Forms::TextBox());
			this->label21 = (gcnew System::Windows::Forms::Label());
			this->txtColor3B = (gcnew System::Windows::Forms::TextBox());
			this->txtColor4A = (gcnew System::Windows::Forms::TextBox());
			this->txtColor3A = (gcnew System::Windows::Forms::TextBox());
			this->txtColor2A = (gcnew System::Windows::Forms::TextBox());
			this->txtColor1A = (gcnew System::Windows::Forms::TextBox());
			this->txtColor1B = (gcnew System::Windows::Forms::TextBox());
			this->txtColor3G = (gcnew System::Windows::Forms::TextBox());
			this->txtColor3R = (gcnew System::Windows::Forms::TextBox());
			this->txtColor1G = (gcnew System::Windows::Forms::TextBox());
			this->txtColor1R = (gcnew System::Windows::Forms::TextBox());
			this->txtScaleSpeed = (gcnew System::Windows::Forms::TextBox());
			this->txtRotationSpeed = (gcnew System::Windows::Forms::TextBox());
			this->txtMaxLifeTime = (gcnew System::Windows::Forms::TextBox());
			this->txtMinLifeTime = (gcnew System::Windows::Forms::TextBox());
			this->cmbDestBlend = (gcnew System::Windows::Forms::ComboBox());
			this->cmbSrcBlend = (gcnew System::Windows::Forms::ComboBox());
			this->txtSpawnPosY = (gcnew System::Windows::Forms::TextBox());
			this->txtMaxDirY = (gcnew System::Windows::Forms::TextBox());
			this->txtMinDirY = (gcnew System::Windows::Forms::TextBox());
			this->txtGravityY = (gcnew System::Windows::Forms::TextBox());
			this->txtSpawnPosX = (gcnew System::Windows::Forms::TextBox());
			this->txtMaxDirX = (gcnew System::Windows::Forms::TextBox());
			this->txtMinDirX = (gcnew System::Windows::Forms::TextBox());
			this->txtGravityX = (gcnew System::Windows::Forms::TextBox());
			this->txtMaxScale = (gcnew System::Windows::Forms::TextBox());
			this->txtMinScale = (gcnew System::Windows::Forms::TextBox());
			this->txtMaxCount = (gcnew System::Windows::Forms::TextBox());
			this->txtRevival = (gcnew System::Windows::Forms::TextBox());
			this->label14 = (gcnew System::Windows::Forms::Label());
			this->label13 = (gcnew System::Windows::Forms::Label());
			this->label12 = (gcnew System::Windows::Forms::Label());
			this->label11 = (gcnew System::Windows::Forms::Label());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->labHelpTip = (gcnew System::Windows::Forms::LinkLabel());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label22 = (gcnew System::Windows::Forms::Label());
			this->label23 = (gcnew System::Windows::Forms::Label());
			this->tabPage2 = (gcnew System::Windows::Forms::TabPage());
			this->txtEventColorA = (gcnew System::Windows::Forms::TextBox());
			this->txtEventColorB = (gcnew System::Windows::Forms::TextBox());
			this->txtEventColorG = (gcnew System::Windows::Forms::TextBox());
			this->txtEventColorR = (gcnew System::Windows::Forms::TextBox());
			this->txtEventGravityY = (gcnew System::Windows::Forms::TextBox());
			this->txtEventGravityX = (gcnew System::Windows::Forms::TextBox());
			this->txtEventMaxY = (gcnew System::Windows::Forms::TextBox());
			this->txtEventMinY = (gcnew System::Windows::Forms::TextBox());
			this->txtEventMaxX = (gcnew System::Windows::Forms::TextBox());
			this->txtEventMinX = (gcnew System::Windows::Forms::TextBox());
			this->txtEventScaleSpeed = (gcnew System::Windows::Forms::TextBox());
			this->txtEventRotSpeed = (gcnew System::Windows::Forms::TextBox());
			this->txtEventTime = (gcnew System::Windows::Forms::TextBox());
			this->cbEventColor = (gcnew System::Windows::Forms::CheckBox());
			this->cbEventGravity = (gcnew System::Windows::Forms::CheckBox());
			this->cbEventChangeDir = (gcnew System::Windows::Forms::CheckBox());
			this->cbEventScaleSpeed = (gcnew System::Windows::Forms::CheckBox());
			this->cbEventRotSpeed = (gcnew System::Windows::Forms::CheckBox());
			this->label20 = (gcnew System::Windows::Forms::Label());
			this->label19 = (gcnew System::Windows::Forms::Label());
			this->label18 = (gcnew System::Windows::Forms::Label());
			this->label17 = (gcnew System::Windows::Forms::Label());
			this->label16 = (gcnew System::Windows::Forms::Label());
			this->label15 = (gcnew System::Windows::Forms::Label());
			this->lstEvents = (gcnew System::Windows::Forms::ListBox());
			this->btDelEvent = (gcnew System::Windows::Forms::Button());
			this->btAddEvent = (gcnew System::Windows::Forms::Button());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->openFileDlg = (gcnew System::Windows::Forms::OpenFileDialog());
			this->statusBar = (gcnew System::Windows::Forms::StatusStrip());
			this->toolStripStatusLabel1 = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->picCanvas))->BeginInit();
			this->mainMenu->SuspendLayout();
			this->tabControl1->SuspendLayout();
			this->tabPage1->SuspendLayout();
			this->tabPage2->SuspendLayout();
			this->statusBar->SuspendLayout();
			this->SuspendLayout();
			// 
			// button2
			// 
			this->button2->Location = System::Drawing::Point(816, 104);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(128, 24);
			this->button2->TabIndex = 2;
			this->button2->Text = L"추가(&A)";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &MainForm::button2_Click);
			// 
			// button3
			// 
			this->button3->Location = System::Drawing::Point(952, 104);
			this->button3->Name = L"button3";
			this->button3->Size = System::Drawing::Size(128, 24);
			this->button3->TabIndex = 3;
			this->button3->Text = L"삭제(&R)";
			this->button3->UseVisualStyleBackColor = true;
			this->button3->Click += gcnew System::EventHandler(this, &MainForm::button3_Click);
			// 
			// 정보AToolStripMenuItem
			// 
			this->정보AToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->네코노벨에대해서AToolStripMenuItem});
			this->정보AToolStripMenuItem->Name = L"정보AToolStripMenuItem";
			this->정보AToolStripMenuItem->Size = System::Drawing::Size(71, 20);
			this->정보AToolStripMenuItem->Text = L"도움말(&H)";
			// 
			// 네코노벨에대해서AToolStripMenuItem
			// 
			this->네코노벨에대해서AToolStripMenuItem->Name = L"네코노벨에대해서AToolStripMenuItem";
			this->네코노벨에대해서AToolStripMenuItem->Size = System::Drawing::Size(210, 22);
			this->네코노벨에대해서AToolStripMenuItem->Text = L"네코 노벨에 대해서...(&A)";
			// 
			// toolStripMenuItem1
			// 
			this->toolStripMenuItem1->Name = L"toolStripMenuItem1";
			this->toolStripMenuItem1->Size = System::Drawing::Size(253, 6);
			// 
			// 프로그램종료XToolStripMenuItem
			// 
			this->프로그램종료XToolStripMenuItem->Name = L"프로그램종료XToolStripMenuItem";
			this->프로그램종료XToolStripMenuItem->Size = System::Drawing::Size(256, 22);
			this->프로그램종료XToolStripMenuItem->Text = L"프로그램 종료(&X)";
			this->프로그램종료XToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::프로그램종료XToolStripMenuItem_Click);
			// 
			// 다른이름으로저장하기AToolStripMenuItem
			// 
			this->다른이름으로저장하기AToolStripMenuItem->Name = L"다른이름으로저장하기AToolStripMenuItem";
			this->다른이름으로저장하기AToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::A));
			this->다른이름으로저장하기AToolStripMenuItem->Size = System::Drawing::Size(256, 22);
			this->다른이름으로저장하기AToolStripMenuItem->Text = L"다른이름으로 저장하기(&A)";
			this->다른이름으로저장하기AToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::다른이름으로저장하기AToolStripMenuItem_Click);
			// 
			// 저장하기SToolStripMenuItem
			// 
			this->저장하기SToolStripMenuItem->Name = L"저장하기SToolStripMenuItem";
			this->저장하기SToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::S));
			this->저장하기SToolStripMenuItem->Size = System::Drawing::Size(256, 22);
			this->저장하기SToolStripMenuItem->Text = L"저장하기(&S)";
			this->저장하기SToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::저장하기SToolStripMenuItem_Click);
			// 
			// 불러오기OToolStripMenuItem
			// 
			this->불러오기OToolStripMenuItem->Name = L"불러오기OToolStripMenuItem";
			this->불러오기OToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::O));
			this->불러오기OToolStripMenuItem->Size = System::Drawing::Size(256, 22);
			this->불러오기OToolStripMenuItem->Text = L"불러오기(&O)";
			this->불러오기OToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::불러오기OToolStripMenuItem_Click);
			// 
			// lstEmitters
			// 
			this->lstEmitters->FormattingEnabled = true;
			this->lstEmitters->ItemHeight = 12;
			this->lstEmitters->Location = System::Drawing::Point(816, 64);
			this->lstEmitters->Name = L"lstEmitters";
			this->lstEmitters->Size = System::Drawing::Size(264, 40);
			this->lstEmitters->TabIndex = 1;
			this->lstEmitters->SelectedIndexChanged += gcnew System::EventHandler(this, &MainForm::lstEmitters_SelectedIndexChanged);
			// 
			// picCanvas
			// 
			this->picCanvas->Location = System::Drawing::Point(8, 32);
			this->picCanvas->Name = L"picCanvas";
			this->picCanvas->Size = System::Drawing::Size(800, 600);
			this->picCanvas->TabIndex = 11;
			this->picCanvas->TabStop = false;
			this->picCanvas->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &MainForm::picCanvas_MouseMove);
			this->picCanvas->MouseDoubleClick += gcnew System::Windows::Forms::MouseEventHandler(this, &MainForm::picCanvas_MouseDoubleClick);
			// 
			// 새로만들기NToolStripMenuItem
			// 
			this->새로만들기NToolStripMenuItem->Name = L"새로만들기NToolStripMenuItem";
			this->새로만들기NToolStripMenuItem->ShortcutKeyDisplayString = L"";
			this->새로만들기NToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::N));
			this->새로만들기NToolStripMenuItem->Size = System::Drawing::Size(256, 22);
			this->새로만들기NToolStripMenuItem->Text = L"새로 만들기(&N)";
			this->새로만들기NToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::새로만들기NToolStripMenuItem_Click);
			// 
			// 파일FToolStripMenuItem
			// 
			this->파일FToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(6) {this->새로만들기NToolStripMenuItem, 
				this->불러오기OToolStripMenuItem, this->저장하기SToolStripMenuItem, this->다른이름으로저장하기AToolStripMenuItem, this->toolStripMenuItem1, this->프로그램종료XToolStripMenuItem});
			this->파일FToolStripMenuItem->Name = L"파일FToolStripMenuItem";
			this->파일FToolStripMenuItem->Size = System::Drawing::Size(58, 20);
			this->파일FToolStripMenuItem->Text = L"파일(&F)";
			// 
			// mainMenu
			// 
			this->mainMenu->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->파일FToolStripMenuItem, 
				this->보기VToolStripMenuItem, this->정보AToolStripMenuItem});
			this->mainMenu->Location = System::Drawing::Point(0, 0);
			this->mainMenu->Name = L"mainMenu";
			this->mainMenu->Size = System::Drawing::Size(1086, 24);
			this->mainMenu->TabIndex = 14;
			this->mainMenu->Text = L"menuStrip1";
			// 
			// 보기VToolStripMenuItem
			// 
			this->보기VToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->테스트배경화면불러오기F5ToolStripMenuItem});
			this->보기VToolStripMenuItem->Name = L"보기VToolStripMenuItem";
			this->보기VToolStripMenuItem->Size = System::Drawing::Size(59, 20);
			this->보기VToolStripMenuItem->Text = L"보기(&V)";
			// 
			// 테스트배경화면불러오기F5ToolStripMenuItem
			// 
			this->테스트배경화면불러오기F5ToolStripMenuItem->Name = L"테스트배경화면불러오기F5ToolStripMenuItem";
			this->테스트배경화면불러오기F5ToolStripMenuItem->ShortcutKeys = System::Windows::Forms::Keys::F5;
			this->테스트배경화면불러오기F5ToolStripMenuItem->Size = System::Drawing::Size(234, 22);
			this->테스트배경화면불러오기F5ToolStripMenuItem->Text = L"테스트 배경화면 불러오기";
			this->테스트배경화면불러오기F5ToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::테스트배경화면불러오기F5ToolStripMenuItem_Click);
			// 
			// tabControl1
			// 
			this->tabControl1->Controls->Add(this->tabPage1);
			this->tabControl1->Controls->Add(this->tabPage2);
			this->tabControl1->Location = System::Drawing::Point(816, 136);
			this->tabControl1->Name = L"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(264, 496);
			this->tabControl1->TabIndex = 5;
			// 
			// tabPage1
			// 
			this->tabPage1->Controls->Add(this->txtFirstAppearMax);
			this->tabPage1->Controls->Add(this->txtFirstAppearMin);
			this->tabPage1->Controls->Add(this->label24);
			this->tabPage1->Controls->Add(this->cbRepeat);
			this->tabPage1->Controls->Add(this->txtRadiusY);
			this->tabPage1->Controls->Add(this->txtRadiusX);
			this->tabPage1->Controls->Add(this->label7);
			this->tabPage1->Controls->Add(this->txtColor4B);
			this->tabPage1->Controls->Add(this->txtColor4G);
			this->tabPage1->Controls->Add(this->txtColor4R);
			this->tabPage1->Controls->Add(this->txtColor2B);
			this->tabPage1->Controls->Add(this->txtColor2G);
			this->tabPage1->Controls->Add(this->txtColor2R);
			this->tabPage1->Controls->Add(this->btFind);
			this->tabPage1->Controls->Add(this->txtImageName);
			this->tabPage1->Controls->Add(this->label21);
			this->tabPage1->Controls->Add(this->txtColor3B);
			this->tabPage1->Controls->Add(this->txtColor4A);
			this->tabPage1->Controls->Add(this->txtColor3A);
			this->tabPage1->Controls->Add(this->txtColor2A);
			this->tabPage1->Controls->Add(this->txtColor1A);
			this->tabPage1->Controls->Add(this->txtColor1B);
			this->tabPage1->Controls->Add(this->txtColor3G);
			this->tabPage1->Controls->Add(this->txtColor3R);
			this->tabPage1->Controls->Add(this->txtColor1G);
			this->tabPage1->Controls->Add(this->txtColor1R);
			this->tabPage1->Controls->Add(this->txtScaleSpeed);
			this->tabPage1->Controls->Add(this->txtRotationSpeed);
			this->tabPage1->Controls->Add(this->txtMaxLifeTime);
			this->tabPage1->Controls->Add(this->txtMinLifeTime);
			this->tabPage1->Controls->Add(this->cmbDestBlend);
			this->tabPage1->Controls->Add(this->cmbSrcBlend);
			this->tabPage1->Controls->Add(this->txtSpawnPosY);
			this->tabPage1->Controls->Add(this->txtMaxDirY);
			this->tabPage1->Controls->Add(this->txtMinDirY);
			this->tabPage1->Controls->Add(this->txtGravityY);
			this->tabPage1->Controls->Add(this->txtSpawnPosX);
			this->tabPage1->Controls->Add(this->txtMaxDirX);
			this->tabPage1->Controls->Add(this->txtMinDirX);
			this->tabPage1->Controls->Add(this->txtGravityX);
			this->tabPage1->Controls->Add(this->txtMaxScale);
			this->tabPage1->Controls->Add(this->txtMinScale);
			this->tabPage1->Controls->Add(this->txtMaxCount);
			this->tabPage1->Controls->Add(this->txtRevival);
			this->tabPage1->Controls->Add(this->label14);
			this->tabPage1->Controls->Add(this->label13);
			this->tabPage1->Controls->Add(this->label12);
			this->tabPage1->Controls->Add(this->label11);
			this->tabPage1->Controls->Add(this->label10);
			this->tabPage1->Controls->Add(this->labHelpTip);
			this->tabPage1->Controls->Add(this->label9);
			this->tabPage1->Controls->Add(this->label8);
			this->tabPage1->Controls->Add(this->label6);
			this->tabPage1->Controls->Add(this->label5);
			this->tabPage1->Controls->Add(this->label4);
			this->tabPage1->Controls->Add(this->label3);
			this->tabPage1->Controls->Add(this->label2);
			this->tabPage1->Controls->Add(this->label1);
			this->tabPage1->Controls->Add(this->label22);
			this->tabPage1->Controls->Add(this->label23);
			this->tabPage1->Location = System::Drawing::Point(4, 21);
			this->tabPage1->Name = L"tabPage1";
			this->tabPage1->Padding = System::Windows::Forms::Padding(3);
			this->tabPage1->Size = System::Drawing::Size(256, 471);
			this->tabPage1->TabIndex = 0;
			this->tabPage1->Text = L"파티클 속성";
			this->tabPage1->UseVisualStyleBackColor = true;
			// 
			// txtFirstAppearMax
			// 
			this->txtFirstAppearMax->Location = System::Drawing::Point(192, 272);
			this->txtFirstAppearMax->MaxLength = 3;
			this->txtFirstAppearMax->Name = L"txtFirstAppearMax";
			this->txtFirstAppearMax->Size = System::Drawing::Size(32, 21);
			this->txtFirstAppearMax->TabIndex = 5002;
			this->txtFirstAppearMax->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->txtFirstAppearMax->Leave += gcnew System::EventHandler(this, &MainForm::txtFirstAppearMax_Leave);
			// 
			// txtFirstAppearMin
			// 
			this->txtFirstAppearMin->Location = System::Drawing::Point(120, 273);
			this->txtFirstAppearMin->MaxLength = 3;
			this->txtFirstAppearMin->Name = L"txtFirstAppearMin";
			this->txtFirstAppearMin->Size = System::Drawing::Size(32, 21);
			this->txtFirstAppearMin->TabIndex = 5001;
			this->txtFirstAppearMin->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->txtFirstAppearMin->Leave += gcnew System::EventHandler(this, &MainForm::txtFirstAppearMin_Leave);
			// 
			// label24
			// 
			this->label24->AutoSize = true;
			this->label24->Location = System::Drawing::Point(8, 277);
			this->label24->Name = L"label24";
			this->label24->Size = System::Drawing::Size(234, 12);
			this->label24->TabIndex = 5003;
			this->label24->Text = L"초기 등장 시간 :               초 ~           초";
			// 
			// cbRepeat
			// 
			this->cbRepeat->AutoSize = true;
			this->cbRepeat->Location = System::Drawing::Point(153, 400);
			this->cbRepeat->Name = L"cbRepeat";
			this->cbRepeat->Size = System::Drawing::Size(94, 16);
			this->cbRepeat->TabIndex = 40;
			this->cbRepeat->Text = L"반복 재생(&R)";
			this->cbRepeat->UseVisualStyleBackColor = true;
			this->cbRepeat->CheckedChanged += gcnew System::EventHandler(this, &MainForm::cbRepeat_CheckedChanged);
			// 
			// txtRadiusY
			// 
			this->txtRadiusY->Location = System::Drawing::Point(192, 128);
			this->txtRadiusY->Name = L"txtRadiusY";
			this->txtRadiusY->Size = System::Drawing::Size(40, 21);
			this->txtRadiusY->TabIndex = 11;
			this->txtRadiusY->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->txtRadiusY->Leave += gcnew System::EventHandler(this, &MainForm::txtRadiusY_Leave);
			// 
			// txtRadiusX
			// 
			this->txtRadiusX->Location = System::Drawing::Point(128, 128);
			this->txtRadiusX->Name = L"txtRadiusX";
			this->txtRadiusX->Size = System::Drawing::Size(40, 21);
			this->txtRadiusX->TabIndex = 10;
			this->txtRadiusX->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->txtRadiusX->Leave += gcnew System::EventHandler(this, &MainForm::txtRadiusX_Leave);
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(8, 133);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(185, 12);
			this->label7->TabIndex = 51;
			this->label7->Text = L"파티클 생성 범위  X :            Y :";
			// 
			// txtColor4B
			// 
			this->txtColor4B->Location = System::Drawing::Point(176, 372);
			this->txtColor4B->MaxLength = 3;
			this->txtColor4B->Name = L"txtColor4B";
			this->txtColor4B->Size = System::Drawing::Size(24, 21);
			this->txtColor4B->TabIndex = 37;
			this->txtColor4B->Tag = L"0";
			this->txtColor4B->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->txtColor4B->Leave += gcnew System::EventHandler(this, &MainForm::txtColor4_Leave);
			// 
			// txtColor4G
			// 
			this->txtColor4G->Location = System::Drawing::Point(128, 372);
			this->txtColor4G->MaxLength = 3;
			this->txtColor4G->Name = L"txtColor4G";
			this->txtColor4G->Size = System::Drawing::Size(24, 21);
			this->txtColor4G->TabIndex = 36;
			this->txtColor4G->Tag = L"8";
			this->txtColor4G->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->txtColor4G->Leave += gcnew System::EventHandler(this, &MainForm::txtColor4_Leave);
			// 
			// txtColor4R
			// 
			this->txtColor4R->Location = System::Drawing::Point(80, 372);
			this->txtColor4R->MaxLength = 3;
			this->txtColor4R->Name = L"txtColor4R";
			this->txtColor4R->Size = System::Drawing::Size(24, 21);
			this->txtColor4R->TabIndex = 35;
			this->txtColor4R->Tag = L"16";
			this->txtColor4R->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->txtColor4R->Leave += gcnew System::EventHandler(this, &MainForm::txtColor4_Leave);
			// 
			// txtColor2B
			// 
			this->txtColor2B->Location = System::Drawing::Point(184, 324);
			this->txtColor2B->MaxLength = 3;
			this->txtColor2B->Name = L"txtColor2B";
			this->txtColor2B->Size = System::Drawing::Size(24, 21);
			this->txtColor2B->TabIndex = 29;
			this->txtColor2B->Tag = L"0";
			this->txtColor2B->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->txtColor2B->Leave += gcnew System::EventHandler(this, &MainForm::txtColor2_Leave);
			// 
			// txtColor2G
			// 
			this->txtColor2G->Location = System::Drawing::Point(136, 324);
			this->txtColor2G->MaxLength = 3;
			this->txtColor2G->Name = L"txtColor2G";
			this->txtColor2G->Size = System::Drawing::Size(24, 21);
			this->txtColor2G->TabIndex = 28;
			this->txtColor2G->Tag = L"8";
			this->txtColor2G->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->txtColor2G->Leave += gcnew System::EventHandler(this, &MainForm::txtColor2_Leave);
			// 
			// txtColor2R
			// 
			this->txtColor2R->AcceptsTab = true;
			this->txtColor2R->Location = System::Drawing::Point(88, 324);
			this->txtColor2R->MaxLength = 3;
			this->txtColor2R->Name = L"txtColor2R";
			this->txtColor2R->Size = System::Drawing::Size(24, 21);
			this->txtColor2R->TabIndex = 27;
			this->txtColor2R->Tag = L"16";
			this->txtColor2R->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->txtColor2R->Leave += gcnew System::EventHandler(this, &MainForm::txtColor2_Leave);
			// 
			// btFind
			// 
			this->btFind->Location = System::Drawing::Point(192, 445);
			this->btFind->Name = L"btFind";
			this->btFind->Size = System::Drawing::Size(56, 24);
			this->btFind->TabIndex = 43;
			this->btFind->Text = L"찾기(&F)";
			this->btFind->UseVisualStyleBackColor = true;
			this->btFind->Click += gcnew System::EventHandler(this, &MainForm::btFind_Click);
			// 
			// txtImageName
			// 
			this->txtImageName->Location = System::Drawing::Point(91, 447);
			this->txtImageName->Name = L"txtImageName";
			this->txtImageName->ReadOnly = true;
			this->txtImageName->Size = System::Drawing::Size(96, 21);
			this->txtImageName->TabIndex = 42;
			// 
			// label21
			// 
			this->label21->AutoSize = true;
			this->label21->Location = System::Drawing::Point(8, 452);
			this->label21->Name = L"label21";
			this->label21->Size = System::Drawing::Size(89, 12);
			this->label21->TabIndex = 40;
			this->label21->Text = L"파티클 이미지 :";
			// 
			// txtColor3B
			// 
			this->txtColor3B->Location = System::Drawing::Point(176, 348);
			this->txtColor3B->MaxLength = 3;
			this->txtColor3B->Name = L"txtColor3B";
			this->txtColor3B->Size = System::Drawing::Size(24, 21);
			this->txtColor3B->TabIndex = 33;
			this->txtColor3B->Tag = L"0";
			this->txtColor3B->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->txtColor3B->Leave += gcnew System::EventHandler(this, &MainForm::txtColor3_Leave);
			// 
			// txtColor4A
			// 
			this->txtColor4A->Location = System::Drawing::Point(222, 372);
			this->txtColor4A->MaxLength = 3;
			this->txtColor4A->Name = L"txtColor4A";
			this->txtColor4A->Size = System::Drawing::Size(24, 21);
			this->txtColor4A->TabIndex = 38;
			this->txtColor4A->Tag = L"24";
			this->txtColor4A->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->txtColor4A->Leave += gcnew System::EventHandler(this, &MainForm::txtColor4_Leave);
			// 
			// txtColor3A
			// 
			this->txtColor3A->Location = System::Drawing::Point(222, 348);
			this->txtColor3A->MaxLength = 3;
			this->txtColor3A->Name = L"txtColor3A";
			this->txtColor3A->Size = System::Drawing::Size(24, 21);
			this->txtColor3A->TabIndex = 34;
			this->txtColor3A->Tag = L"24";
			this->txtColor3A->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->txtColor3A->Leave += gcnew System::EventHandler(this, &MainForm::txtColor3_Leave);
			// 
			// txtColor2A
			// 
			this->txtColor2A->Location = System::Drawing::Point(228, 324);
			this->txtColor2A->MaxLength = 3;
			this->txtColor2A->Name = L"txtColor2A";
			this->txtColor2A->Size = System::Drawing::Size(24, 21);
			this->txtColor2A->TabIndex = 30;
			this->txtColor2A->Tag = L"24";
			this->txtColor2A->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->txtColor2A->Leave += gcnew System::EventHandler(this, &MainForm::txtColor2_Leave);
			// 
			// txtColor1A
			// 
			this->txtColor1A->Location = System::Drawing::Point(228, 300);
			this->txtColor1A->MaxLength = 3;
			this->txtColor1A->Name = L"txtColor1A";
			this->txtColor1A->Size = System::Drawing::Size(24, 21);
			this->txtColor1A->TabIndex = 26;
			this->txtColor1A->Tag = L"24";
			this->txtColor1A->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->txtColor1A->Leave += gcnew System::EventHandler(this, &MainForm::txtColor1_Leave);
			// 
			// txtColor1B
			// 
			this->txtColor1B->Location = System::Drawing::Point(184, 300);
			this->txtColor1B->MaxLength = 3;
			this->txtColor1B->Name = L"txtColor1B";
			this->txtColor1B->Size = System::Drawing::Size(24, 21);
			this->txtColor1B->TabIndex = 25;
			this->txtColor1B->Tag = L"0";
			this->txtColor1B->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->txtColor1B->Leave += gcnew System::EventHandler(this, &MainForm::txtColor1_Leave);
			// 
			// txtColor3G
			// 
			this->txtColor3G->Location = System::Drawing::Point(128, 348);
			this->txtColor3G->MaxLength = 3;
			this->txtColor3G->Name = L"txtColor3G";
			this->txtColor3G->Size = System::Drawing::Size(24, 21);
			this->txtColor3G->TabIndex = 32;
			this->txtColor3G->Tag = L"8";
			this->txtColor3G->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->txtColor3G->Leave += gcnew System::EventHandler(this, &MainForm::txtColor3_Leave);
			// 
			// txtColor3R
			// 
			this->txtColor3R->Location = System::Drawing::Point(80, 348);
			this->txtColor3R->MaxLength = 3;
			this->txtColor3R->Name = L"txtColor3R";
			this->txtColor3R->Size = System::Drawing::Size(24, 21);
			this->txtColor3R->TabIndex = 31;
			this->txtColor3R->Tag = L"16";
			this->txtColor3R->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->txtColor3R->Leave += gcnew System::EventHandler(this, &MainForm::txtColor3_Leave);
			// 
			// txtColor1G
			// 
			this->txtColor1G->Location = System::Drawing::Point(136, 300);
			this->txtColor1G->MaxLength = 3;
			this->txtColor1G->Name = L"txtColor1G";
			this->txtColor1G->Size = System::Drawing::Size(24, 21);
			this->txtColor1G->TabIndex = 24;
			this->txtColor1G->Tag = L"8";
			this->txtColor1G->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->txtColor1G->Leave += gcnew System::EventHandler(this, &MainForm::txtColor1_Leave);
			// 
			// txtColor1R
			// 
			this->txtColor1R->Location = System::Drawing::Point(88, 300);
			this->txtColor1R->MaxLength = 3;
			this->txtColor1R->Name = L"txtColor1R";
			this->txtColor1R->Size = System::Drawing::Size(24, 21);
			this->txtColor1R->TabIndex = 23;
			this->txtColor1R->Tag = L"16";
			this->txtColor1R->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->txtColor1R->Leave += gcnew System::EventHandler(this, &MainForm::txtColor1_Leave);
			// 
			// txtScaleSpeed
			// 
			this->txtScaleSpeed->Location = System::Drawing::Point(88, 424);
			this->txtScaleSpeed->Name = L"txtScaleSpeed";
			this->txtScaleSpeed->Size = System::Drawing::Size(48, 21);
			this->txtScaleSpeed->TabIndex = 41;
			this->txtScaleSpeed->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->txtScaleSpeed->Leave += gcnew System::EventHandler(this, &MainForm::txtScaleSpeed_Leave);
			// 
			// txtRotationSpeed
			// 
			this->txtRotationSpeed->Location = System::Drawing::Point(88, 396);
			this->txtRotationSpeed->Name = L"txtRotationSpeed";
			this->txtRotationSpeed->Size = System::Drawing::Size(48, 21);
			this->txtRotationSpeed->TabIndex = 39;
			this->txtRotationSpeed->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->txtRotationSpeed->Leave += gcnew System::EventHandler(this, &MainForm::txtRotationSpeed_Leave);
			// 
			// txtMaxLifeTime
			// 
			this->txtMaxLifeTime->Location = System::Drawing::Point(192, 248);
			this->txtMaxLifeTime->MaxLength = 3;
			this->txtMaxLifeTime->Name = L"txtMaxLifeTime";
			this->txtMaxLifeTime->Size = System::Drawing::Size(32, 21);
			this->txtMaxLifeTime->TabIndex = 17;
			this->txtMaxLifeTime->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->txtMaxLifeTime->Leave += gcnew System::EventHandler(this, &MainForm::txtMaxLifeTime_Leave);
			// 
			// txtMinLifeTime
			// 
			this->txtMinLifeTime->Location = System::Drawing::Point(120, 249);
			this->txtMinLifeTime->MaxLength = 3;
			this->txtMinLifeTime->Name = L"txtMinLifeTime";
			this->txtMinLifeTime->Size = System::Drawing::Size(32, 21);
			this->txtMinLifeTime->TabIndex = 16;
			this->txtMinLifeTime->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->txtMinLifeTime->Leave += gcnew System::EventHandler(this, &MainForm::txtMinLifeTime_Leave);
			// 
			// cmbDestBlend
			// 
			this->cmbDestBlend->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cmbDestBlend->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->cmbDestBlend->FormattingEnabled = true;
			this->cmbDestBlend->Items->AddRange(gcnew cli::array< System::Object^  >(11) {L"D3DBLEND_ZERO", L"D3DBLEND_ONE", L"D3DBLEND_SRCCOLOR", 
				L"D3DBLEND_INVSRCCOLOR", L"D3DBLEND_SRCALPHA", L"D3DBLEND_INVSRCALPHA", L"D3DBLEND_DESTALPHA ", L"D3DBLEND_INVDESTALPHA", L"D3DBLEND_DESTCOLOR", 
				L"D3DBLEND_INVDESTCOLOR", L"D3DBLEND_SRCALPHASAT"});
			this->cmbDestBlend->Location = System::Drawing::Point(96, 201);
			this->cmbDestBlend->Name = L"cmbDestBlend";
			this->cmbDestBlend->Size = System::Drawing::Size(152, 21);
			this->cmbDestBlend->TabIndex = 15;
			this->cmbDestBlend->SelectedIndexChanged += gcnew System::EventHandler(this, &MainForm::cmbDestBlend_SelectedIndexChanged);
			// 
			// cmbSrcBlend
			// 
			this->cmbSrcBlend->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cmbSrcBlend->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->cmbSrcBlend->FormattingEnabled = true;
			this->cmbSrcBlend->Items->AddRange(gcnew cli::array< System::Object^  >(11) {L"D3DBLEND_ZERO", L"D3DBLEND_ONE", L"D3DBLEND_SRCCOLOR", 
				L"D3DBLEND_INVSRCCOLOR", L"D3DBLEND_SRCALPHA", L"D3DBLEND_INVSRCALPHA", L"D3DBLEND_DESTALPHA ", L"D3DBLEND_INVDESTALPHA", L"D3DBLEND_DESTCOLOR", 
				L"D3DBLEND_INVDESTCOLOR", L"D3DBLEND_SRCALPHASAT"});
			this->cmbSrcBlend->Location = System::Drawing::Point(88, 177);
			this->cmbSrcBlend->Name = L"cmbSrcBlend";
			this->cmbSrcBlend->Size = System::Drawing::Size(152, 21);
			this->cmbSrcBlend->TabIndex = 14;
			this->cmbSrcBlend->SelectedIndexChanged += gcnew System::EventHandler(this, &MainForm::cmbSrcBlend_SelectedIndexChanged);
			// 
			// txtSpawnPosY
			// 
			this->txtSpawnPosY->Location = System::Drawing::Point(192, 104);
			this->txtSpawnPosY->Name = L"txtSpawnPosY";
			this->txtSpawnPosY->Size = System::Drawing::Size(40, 21);
			this->txtSpawnPosY->TabIndex = 9;
			this->txtSpawnPosY->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->txtSpawnPosY->Leave += gcnew System::EventHandler(this, &MainForm::txtSpawnPosY_Leave);
			// 
			// txtMaxDirY
			// 
			this->txtMaxDirY->Location = System::Drawing::Point(208, 56);
			this->txtMaxDirY->Name = L"txtMaxDirY";
			this->txtMaxDirY->Size = System::Drawing::Size(32, 21);
			this->txtMaxDirY->TabIndex = 5;
			this->txtMaxDirY->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->txtMaxDirY->Leave += gcnew System::EventHandler(this, &MainForm::txtMaxDirY_Leave);
			// 
			// txtMinDirY
			// 
			this->txtMinDirY->Location = System::Drawing::Point(120, 56);
			this->txtMinDirY->Name = L"txtMinDirY";
			this->txtMinDirY->Size = System::Drawing::Size(32, 21);
			this->txtMinDirY->TabIndex = 3;
			this->txtMinDirY->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->txtMinDirY->Leave += gcnew System::EventHandler(this, &MainForm::txtMinDirY_Leave);
			// 
			// txtGravityY
			// 
			this->txtGravityY->Location = System::Drawing::Point(176, 80);
			this->txtGravityY->Name = L"txtGravityY";
			this->txtGravityY->Size = System::Drawing::Size(55, 21);
			this->txtGravityY->TabIndex = 7;
			this->txtGravityY->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->txtGravityY->Leave += gcnew System::EventHandler(this, &MainForm::txtGravityY_Leave);
			// 
			// txtSpawnPosX
			// 
			this->txtSpawnPosX->Location = System::Drawing::Point(128, 104);
			this->txtSpawnPosX->Name = L"txtSpawnPosX";
			this->txtSpawnPosX->Size = System::Drawing::Size(40, 21);
			this->txtSpawnPosX->TabIndex = 8;
			this->txtSpawnPosX->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->txtSpawnPosX->Leave += gcnew System::EventHandler(this, &MainForm::txtSpawnPosX_Leave);
			// 
			// txtMaxDirX
			// 
			this->txtMaxDirX->Location = System::Drawing::Point(168, 56);
			this->txtMaxDirX->Name = L"txtMaxDirX";
			this->txtMaxDirX->Size = System::Drawing::Size(32, 21);
			this->txtMaxDirX->TabIndex = 4;
			this->txtMaxDirX->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->txtMaxDirX->Leave += gcnew System::EventHandler(this, &MainForm::txtMaxDirX_Leave);
			// 
			// txtMinDirX
			// 
			this->txtMinDirX->Location = System::Drawing::Point(80, 56);
			this->txtMinDirX->Name = L"txtMinDirX";
			this->txtMinDirX->Size = System::Drawing::Size(32, 21);
			this->txtMinDirX->TabIndex = 2;
			this->txtMinDirX->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->txtMinDirX->Leave += gcnew System::EventHandler(this, &MainForm::txtMinDirX_Leave);
			// 
			// txtGravityX
			// 
			this->txtGravityX->Location = System::Drawing::Point(89, 80);
			this->txtGravityX->Name = L"txtGravityX";
			this->txtGravityX->Size = System::Drawing::Size(55, 21);
			this->txtGravityX->TabIndex = 6;
			this->txtGravityX->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->txtGravityX->Leave += gcnew System::EventHandler(this, &MainForm::txtGravityX_Leave);
			// 
			// txtMaxScale
			// 
			this->txtMaxScale->Location = System::Drawing::Point(160, 152);
			this->txtMaxScale->Name = L"txtMaxScale";
			this->txtMaxScale->Size = System::Drawing::Size(48, 21);
			this->txtMaxScale->TabIndex = 13;
			this->txtMaxScale->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->txtMaxScale->Leave += gcnew System::EventHandler(this, &MainForm::txtMaxScale_Leave);
			// 
			// txtMinScale
			// 
			this->txtMinScale->Location = System::Drawing::Point(88, 152);
			this->txtMinScale->Name = L"txtMinScale";
			this->txtMinScale->Size = System::Drawing::Size(48, 21);
			this->txtMinScale->TabIndex = 12;
			this->txtMinScale->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->txtMinScale->Leave += gcnew System::EventHandler(this, &MainForm::txtMinScale_Leave);
			// 
			// txtMaxCount
			// 
			this->txtMaxCount->Location = System::Drawing::Point(80, 32);
			this->txtMaxCount->Name = L"txtMaxCount";
			this->txtMaxCount->Size = System::Drawing::Size(104, 21);
			this->txtMaxCount->TabIndex = 1;
			this->txtMaxCount->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->txtMaxCount->Leave += gcnew System::EventHandler(this, &MainForm::txtMaxCount_Leave);
			// 
			// txtRevival
			// 
			this->txtRevival->Location = System::Drawing::Point(96, 8);
			this->txtRevival->MaxLength = 3;
			this->txtRevival->Name = L"txtRevival";
			this->txtRevival->Size = System::Drawing::Size(64, 21);
			this->txtRevival->TabIndex = 0;
			this->txtRevival->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->txtRevival->Leave += gcnew System::EventHandler(this, &MainForm::txtRevival_Leave);
			// 
			// label14
			// 
			this->label14->AutoSize = true;
			this->label14->Location = System::Drawing::Point(8, 428);
			this->label14->Name = L"label14";
			this->label14->Size = System::Drawing::Size(77, 12);
			this->label14->TabIndex = 29;
			this->label14->Text = L"크기 변화량 :";
			// 
			// label13
			// 
			this->label13->AutoSize = true;
			this->label13->Location = System::Drawing::Point(8, 401);
			this->label13->Name = L"label13";
			this->label13->Size = System::Drawing::Size(77, 12);
			this->label13->TabIndex = 28;
			this->label13->Text = L"회전 변화량 :";
			// 
			// label12
			// 
			this->label12->AutoSize = true;
			this->label12->Location = System::Drawing::Point(8, 353);
			this->label12->Name = L"label12";
			this->label12->Size = System::Drawing::Size(214, 12);
			this->label12->TabIndex = 27;
			this->label12->Text = L"끝 색깔  R :        G :        B :        A :";
			// 
			// label11
			// 
			this->label11->AutoSize = true;
			this->label11->Location = System::Drawing::Point(8, 305);
			this->label11->Name = L"label11";
			this->label11->Size = System::Drawing::Size(222, 12);
			this->label11->TabIndex = 22;
			this->label11->Text = L"시작 색깔 R :        G :        B :        A :";
			// 
			// label10
			// 
			this->label10->AutoSize = true;
			this->label10->Location = System::Drawing::Point(8, 253);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(234, 12);
			this->label10->TabIndex = 25;
			this->label10->Text = L"파티클 유지 시간 :            초 ~           초";
			// 
			// labHelpTip
			// 
			this->labHelpTip->Location = System::Drawing::Point(8, 224);
			this->labHelpTip->Name = L"labHelpTip";
			this->labHelpTip->Size = System::Drawing::Size(248, 24);
			this->labHelpTip->TabIndex = 5000;
			this->labHelpTip->TabStop = true;
			this->labHelpTip->Text = L"참고 : 최종색깔 = SRCBLEND * (파티클 픽셀) + DESTBLEND * (다른 픽셀)";
			// 
			// label9
			// 
			this->label9->AutoSize = true;
			this->label9->Location = System::Drawing::Point(8, 205);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(85, 12);
			this->label9->TabIndex = 23;
			this->label9->Text = L"DESTBLEND :";
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Location = System::Drawing::Point(8, 181);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(78, 12);
			this->label8->TabIndex = 22;
			this->label8->Text = L"SRCBLEND :";
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(8, 157);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(146, 12);
			this->label6->TabIndex = 20;
			this->label6->Text = L"생성시 크기 :               ~";
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(8, 109);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(185, 12);
			this->label5->TabIndex = 19;
			this->label5->Text = L"파티클 생성 위치  X :            Y :";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(8, 85);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(165, 12);
			this->label4->TabIndex = 18;
			this->label4->Text = L"중력 방향  X :                 Y :";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(8, 61);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(202, 12);
			this->label3->TabIndex = 17;
			this->label3->Text = L"생성시 방향          ,         ~          ,";
			this->label3->Click += gcnew System::EventHandler(this, &MainForm::label3_Click);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(8, 37);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(65, 12);
			this->label2->TabIndex = 16;
			this->label2->Text = L"최대 개수 :";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(8, 13);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(173, 12);
			this->label1->TabIndex = 15;
			this->label1->Text = L"재생성 딜레이 :                  초";
			// 
			// label22
			// 
			this->label22->AutoSize = true;
			this->label22->Location = System::Drawing::Point(8, 329);
			this->label22->Name = L"label22";
			this->label22->Size = System::Drawing::Size(223, 12);
			this->label22->TabIndex = 43;
			this->label22->Text = L"      ~      R :        G :        B :        A :";
			// 
			// label23
			// 
			this->label23->AutoSize = true;
			this->label23->Location = System::Drawing::Point(8, 377);
			this->label23->Name = L"label23";
			this->label23->Size = System::Drawing::Size(215, 12);
			this->label23->TabIndex = 47;
			this->label23->Text = L"     ~     R :        G :        B :        A :";
			// 
			// tabPage2
			// 
			this->tabPage2->Controls->Add(this->txtEventColorA);
			this->tabPage2->Controls->Add(this->txtEventColorB);
			this->tabPage2->Controls->Add(this->txtEventColorG);
			this->tabPage2->Controls->Add(this->txtEventColorR);
			this->tabPage2->Controls->Add(this->txtEventGravityY);
			this->tabPage2->Controls->Add(this->txtEventGravityX);
			this->tabPage2->Controls->Add(this->txtEventMaxY);
			this->tabPage2->Controls->Add(this->txtEventMinY);
			this->tabPage2->Controls->Add(this->txtEventMaxX);
			this->tabPage2->Controls->Add(this->txtEventMinX);
			this->tabPage2->Controls->Add(this->txtEventScaleSpeed);
			this->tabPage2->Controls->Add(this->txtEventRotSpeed);
			this->tabPage2->Controls->Add(this->txtEventTime);
			this->tabPage2->Controls->Add(this->cbEventColor);
			this->tabPage2->Controls->Add(this->cbEventGravity);
			this->tabPage2->Controls->Add(this->cbEventChangeDir);
			this->tabPage2->Controls->Add(this->cbEventScaleSpeed);
			this->tabPage2->Controls->Add(this->cbEventRotSpeed);
			this->tabPage2->Controls->Add(this->label20);
			this->tabPage2->Controls->Add(this->label19);
			this->tabPage2->Controls->Add(this->label18);
			this->tabPage2->Controls->Add(this->label17);
			this->tabPage2->Controls->Add(this->label16);
			this->tabPage2->Controls->Add(this->label15);
			this->tabPage2->Controls->Add(this->lstEvents);
			this->tabPage2->Controls->Add(this->btDelEvent);
			this->tabPage2->Controls->Add(this->btAddEvent);
			this->tabPage2->Location = System::Drawing::Point(4, 21);
			this->tabPage2->Name = L"tabPage2";
			this->tabPage2->Padding = System::Windows::Forms::Padding(3);
			this->tabPage2->Size = System::Drawing::Size(256, 471);
			this->tabPage2->TabIndex = 1;
			this->tabPage2->Text = L"이벤트";
			this->tabPage2->UseVisualStyleBackColor = true;
			// 
			// txtEventColorA
			// 
			this->txtEventColorA->Location = System::Drawing::Point(216, 369);
			this->txtEventColorA->MaxLength = 3;
			this->txtEventColorA->Name = L"txtEventColorA";
			this->txtEventColorA->Size = System::Drawing::Size(24, 21);
			this->txtEventColorA->TabIndex = 20;
			this->txtEventColorA->Tag = L"24";
			this->txtEventColorA->Leave += gcnew System::EventHandler(this, &MainForm::txtEventColorA_Leave);
			// 
			// txtEventColorB
			// 
			this->txtEventColorB->Location = System::Drawing::Point(176, 369);
			this->txtEventColorB->MaxLength = 3;
			this->txtEventColorB->Name = L"txtEventColorB";
			this->txtEventColorB->Size = System::Drawing::Size(24, 21);
			this->txtEventColorB->TabIndex = 19;
			this->txtEventColorB->Tag = L"0";
			this->txtEventColorB->Leave += gcnew System::EventHandler(this, &MainForm::txtEventColorA_Leave);
			// 
			// txtEventColorG
			// 
			this->txtEventColorG->AllowDrop = true;
			this->txtEventColorG->Location = System::Drawing::Point(136, 369);
			this->txtEventColorG->MaxLength = 3;
			this->txtEventColorG->Name = L"txtEventColorG";
			this->txtEventColorG->Size = System::Drawing::Size(24, 21);
			this->txtEventColorG->TabIndex = 18;
			this->txtEventColorG->Tag = L"8";
			this->txtEventColorG->Leave += gcnew System::EventHandler(this, &MainForm::txtEventColorA_Leave);
			// 
			// txtEventColorR
			// 
			this->txtEventColorR->Location = System::Drawing::Point(96, 369);
			this->txtEventColorR->MaxLength = 3;
			this->txtEventColorR->Name = L"txtEventColorR";
			this->txtEventColorR->Size = System::Drawing::Size(24, 21);
			this->txtEventColorR->TabIndex = 17;
			this->txtEventColorR->Tag = L"16";
			this->txtEventColorR->Leave += gcnew System::EventHandler(this, &MainForm::txtEventColorA_Leave);
			// 
			// txtEventGravityY
			// 
			this->txtEventGravityY->Location = System::Drawing::Point(192, 346);
			this->txtEventGravityY->Name = L"txtEventGravityY";
			this->txtEventGravityY->Size = System::Drawing::Size(55, 21);
			this->txtEventGravityY->TabIndex = 15;
			this->txtEventGravityY->Tag = L"1";
			this->txtEventGravityY->Leave += gcnew System::EventHandler(this, &MainForm::txtEventGravityX_Leave);
			// 
			// txtEventGravityX
			// 
			this->txtEventGravityX->Location = System::Drawing::Point(104, 346);
			this->txtEventGravityX->Name = L"txtEventGravityX";
			this->txtEventGravityX->Size = System::Drawing::Size(55, 21);
			this->txtEventGravityX->TabIndex = 14;
			this->txtEventGravityX->Tag = L"0";
			this->txtEventGravityX->Leave += gcnew System::EventHandler(this, &MainForm::txtEventGravityX_Leave);
			// 
			// txtEventMaxY
			// 
			this->txtEventMaxY->Location = System::Drawing::Point(168, 322);
			this->txtEventMaxY->Name = L"txtEventMaxY";
			this->txtEventMaxY->Size = System::Drawing::Size(40, 21);
			this->txtEventMaxY->TabIndex = 12;
			this->txtEventMaxY->Tag = L"3";
			this->txtEventMaxY->Leave += gcnew System::EventHandler(this, &MainForm::txtEventMinY_Leave);
			// 
			// txtEventMinY
			// 
			this->txtEventMinY->Location = System::Drawing::Point(168, 297);
			this->txtEventMinY->Name = L"txtEventMinY";
			this->txtEventMinY->Size = System::Drawing::Size(40, 21);
			this->txtEventMinY->TabIndex = 10;
			this->txtEventMinY->Tag = L"1";
			this->txtEventMinY->Leave += gcnew System::EventHandler(this, &MainForm::txtEventMinY_Leave);
			// 
			// txtEventMaxX
			// 
			this->txtEventMaxX->Location = System::Drawing::Point(104, 322);
			this->txtEventMaxX->Name = L"txtEventMaxX";
			this->txtEventMaxX->Size = System::Drawing::Size(40, 21);
			this->txtEventMaxX->TabIndex = 11;
			this->txtEventMaxX->Tag = L"2";
			this->txtEventMaxX->Leave += gcnew System::EventHandler(this, &MainForm::txtEventMinY_Leave);
			// 
			// txtEventMinX
			// 
			this->txtEventMinX->Location = System::Drawing::Point(104, 297);
			this->txtEventMinX->Name = L"txtEventMinX";
			this->txtEventMinX->Size = System::Drawing::Size(40, 21);
			this->txtEventMinX->TabIndex = 9;
			this->txtEventMinX->Tag = L"0";
			this->txtEventMinX->Leave += gcnew System::EventHandler(this, &MainForm::txtEventMinY_Leave);
			// 
			// txtEventScaleSpeed
			// 
			this->txtEventScaleSpeed->Location = System::Drawing::Point(104, 273);
			this->txtEventScaleSpeed->Name = L"txtEventScaleSpeed";
			this->txtEventScaleSpeed->Size = System::Drawing::Size(104, 21);
			this->txtEventScaleSpeed->TabIndex = 7;
			this->txtEventScaleSpeed->Leave += gcnew System::EventHandler(this, &MainForm::txtEventScaleSpeed_Leave);
			// 
			// txtEventRotSpeed
			// 
			this->txtEventRotSpeed->Location = System::Drawing::Point(104, 250);
			this->txtEventRotSpeed->Name = L"txtEventRotSpeed";
			this->txtEventRotSpeed->Size = System::Drawing::Size(104, 21);
			this->txtEventRotSpeed->TabIndex = 5;
			this->txtEventRotSpeed->Leave += gcnew System::EventHandler(this, &MainForm::txtEventRotSpeed_Leave);
			// 
			// txtEventTime
			// 
			this->txtEventTime->Location = System::Drawing::Point(136, 226);
			this->txtEventTime->MaxLength = 3;
			this->txtEventTime->Name = L"txtEventTime";
			this->txtEventTime->Size = System::Drawing::Size(40, 21);
			this->txtEventTime->TabIndex = 3;
			this->txtEventTime->Leave += gcnew System::EventHandler(this, &MainForm::txtEventTime_Leave);
			// 
			// cbEventColor
			// 
			this->cbEventColor->AutoSize = true;
			this->cbEventColor->Location = System::Drawing::Point(8, 374);
			this->cbEventColor->Name = L"cbEventColor";
			this->cbEventColor->Size = System::Drawing::Size(15, 14);
			this->cbEventColor->TabIndex = 16;
			this->cbEventColor->UseVisualStyleBackColor = true;
			this->cbEventColor->CheckedChanged += gcnew System::EventHandler(this, &MainForm::cbEventColor_CheckedChanged);
			// 
			// cbEventGravity
			// 
			this->cbEventGravity->AutoSize = true;
			this->cbEventGravity->Location = System::Drawing::Point(8, 350);
			this->cbEventGravity->Name = L"cbEventGravity";
			this->cbEventGravity->Size = System::Drawing::Size(15, 14);
			this->cbEventGravity->TabIndex = 13;
			this->cbEventGravity->UseVisualStyleBackColor = true;
			this->cbEventGravity->CheckedChanged += gcnew System::EventHandler(this, &MainForm::cbEventGravity_CheckedChanged);
			// 
			// cbEventChangeDir
			// 
			this->cbEventChangeDir->AutoSize = true;
			this->cbEventChangeDir->Location = System::Drawing::Point(8, 302);
			this->cbEventChangeDir->Name = L"cbEventChangeDir";
			this->cbEventChangeDir->Size = System::Drawing::Size(15, 14);
			this->cbEventChangeDir->TabIndex = 8;
			this->cbEventChangeDir->UseVisualStyleBackColor = true;
			this->cbEventChangeDir->CheckedChanged += gcnew System::EventHandler(this, &MainForm::cbEventChangeDir_CheckedChanged);
			// 
			// cbEventScaleSpeed
			// 
			this->cbEventScaleSpeed->AutoSize = true;
			this->cbEventScaleSpeed->Location = System::Drawing::Point(8, 277);
			this->cbEventScaleSpeed->Name = L"cbEventScaleSpeed";
			this->cbEventScaleSpeed->Size = System::Drawing::Size(15, 14);
			this->cbEventScaleSpeed->TabIndex = 6;
			this->cbEventScaleSpeed->UseVisualStyleBackColor = true;
			this->cbEventScaleSpeed->CheckedChanged += gcnew System::EventHandler(this, &MainForm::cbEventScaleSpeed_CheckedChanged);
			// 
			// cbEventRotSpeed
			// 
			this->cbEventRotSpeed->AutoSize = true;
			this->cbEventRotSpeed->Location = System::Drawing::Point(8, 254);
			this->cbEventRotSpeed->Name = L"cbEventRotSpeed";
			this->cbEventRotSpeed->Size = System::Drawing::Size(15, 14);
			this->cbEventRotSpeed->TabIndex = 4;
			this->cbEventRotSpeed->UseVisualStyleBackColor = true;
			this->cbEventRotSpeed->CheckedChanged += gcnew System::EventHandler(this, &MainForm::cbEventRotSpeed_CheckedChanged);
			// 
			// label20
			// 
			this->label20->AutoSize = true;
			this->label20->Location = System::Drawing::Point(24, 374);
			this->label20->Name = L"label20";
			this->label20->Size = System::Drawing::Size(198, 12);
			this->label20->TabIndex = 29;
			this->label20->Text = L"변경 색깔 R :      G :      B :      A :";
			// 
			// label19
			// 
			this->label19->AutoSize = true;
			this->label19->Location = System::Drawing::Point(24, 350);
			this->label19->Name = L"label19";
			this->label19->Size = System::Drawing::Size(165, 12);
			this->label19->TabIndex = 28;
			this->label19->Text = L"중력 변경  X :                 Y :";
			// 
			// label18
			// 
			this->label18->AutoSize = true;
			this->label18->Location = System::Drawing::Point(24, 302);
			this->label18->Name = L"label18";
			this->label18->Size = System::Drawing::Size(142, 36);
			this->label18->TabIndex = 27;
			this->label18->Text = L"방향 변경  X :           Y :\r\n\r\n~             X :           Y :";
			// 
			// label17
			// 
			this->label17->AutoSize = true;
			this->label17->Location = System::Drawing::Point(24, 277);
			this->label17->Name = L"label17";
			this->label17->Size = System::Drawing::Size(77, 12);
			this->label17->TabIndex = 25;
			this->label17->Text = L"크기 변화량 :";
			// 
			// label16
			// 
			this->label16->AutoSize = true;
			this->label16->Location = System::Drawing::Point(24, 254);
			this->label16->Name = L"label16";
			this->label16->Size = System::Drawing::Size(77, 12);
			this->label16->TabIndex = 25;
			this->label16->Text = L"회전 변화량 :";
			// 
			// label15
			// 
			this->label15->AutoSize = true;
			this->label15->Location = System::Drawing::Point(24, 230);
			this->label15->Name = L"label15";
			this->label15->Size = System::Drawing::Size(105, 12);
			this->label15->TabIndex = 5;
			this->label15->Text = L"이벤트 발생 시간 :";
			// 
			// lstEvents
			// 
			this->lstEvents->FormattingEnabled = true;
			this->lstEvents->ItemHeight = 12;
			this->lstEvents->Location = System::Drawing::Point(8, 8);
			this->lstEvents->Name = L"lstEvents";
			this->lstEvents->Size = System::Drawing::Size(240, 184);
			this->lstEvents->TabIndex = 0;
			this->lstEvents->SelectedIndexChanged += gcnew System::EventHandler(this, &MainForm::lstEvents_SelectedIndexChanged);
			// 
			// btDelEvent
			// 
			this->btDelEvent->Location = System::Drawing::Point(136, 192);
			this->btDelEvent->Name = L"btDelEvent";
			this->btDelEvent->Size = System::Drawing::Size(112, 24);
			this->btDelEvent->TabIndex = 2;
			this->btDelEvent->Text = L"삭제(&R)";
			this->btDelEvent->UseVisualStyleBackColor = true;
			this->btDelEvent->Click += gcnew System::EventHandler(this, &MainForm::btDelEvent_Click);
			// 
			// btAddEvent
			// 
			this->btAddEvent->Location = System::Drawing::Point(8, 192);
			this->btAddEvent->Name = L"btAddEvent";
			this->btAddEvent->Size = System::Drawing::Size(120, 24);
			this->btAddEvent->TabIndex = 1;
			this->btAddEvent->Text = L"추가(&A)";
			this->btAddEvent->UseVisualStyleBackColor = true;
			this->btAddEvent->Click += gcnew System::EventHandler(this, &MainForm::btAddEvent_Click);
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(816, 32);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(264, 24);
			this->button1->TabIndex = 0;
			this->button1->Text = L"처음부터 다시 실행(&R)";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &MainForm::button1_Click);
			// 
			// openFileDlg
			// 
			this->openFileDlg->Filter = L"이미지 파일(BMP, PNG, TGA, JPG, DDS)|*.BMP;*.PNG;*.TGA;*.JPG;*.DDS";
			this->openFileDlg->Title = L"파티클 이미지 파일 불러오기";
			// 
			// statusBar
			// 
			this->statusBar->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->toolStripStatusLabel1});
			this->statusBar->Location = System::Drawing::Point(0, 639);
			this->statusBar->Name = L"statusBar";
			this->statusBar->Size = System::Drawing::Size(1086, 22);
			this->statusBar->TabIndex = 15;
			this->statusBar->Text = L"statusStrip1";
			// 
			// toolStripStatusLabel1
			// 
			this->toolStripStatusLabel1->Name = L"toolStripStatusLabel1";
			this->toolStripStatusLabel1->Size = System::Drawing::Size(0, 17);
			// 
			// MainForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1086, 661);
			this->Controls->Add(this->statusBar);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->tabControl1);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->button3);
			this->Controls->Add(this->lstEmitters);
			this->Controls->Add(this->picCanvas);
			this->Controls->Add(this->mainMenu);
			this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"MainForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"네코노벨 파티클 에디터 Ver 1.00 ( 엔진 Ver 1.08 )";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &MainForm::MainForm_FormClosing);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->picCanvas))->EndInit();
			this->mainMenu->ResumeLayout(false);
			this->mainMenu->PerformLayout();
			this->tabControl1->ResumeLayout(false);
			this->tabPage1->ResumeLayout(false);
			this->tabPage1->PerformLayout();
			this->tabPage2->ResumeLayout(false);
			this->tabPage2->PerformLayout();
			this->statusBar->ResumeLayout(false);
			this->statusBar->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

		protected:

			// 파티클 새로 추가
			void AddParticle(String ^name)
			{
				mParticle->AddEmitter(ConvertToChar(name));
				lstEmitters->SelectedIndex = lstEmitters->Items->Add(name);
				/*lstEmitters->Items[lstEmitters->Items->Count - 1]*/
			}

			// 파티클이 선택되어졌을 때
			void OnParticleSelected()
			{
				if(lstEmitters->SelectedIndex < 0)
				{
					mEmitter = NULL;
					return;
				}

				mEmitter = mParticle->GetEmitter(lstEmitters->SelectedIndex);
				if(mEmitter == NULL)
					return;

				txtRevival->Text = String::Format("{0}", mEmitter->mEmitDelay);
				txtSpawnPosX->Text = String::Format("{0}", mEmitter->mSpawnPos.x);
				txtSpawnPosY->Text = String::Format("{0}", mEmitter->mSpawnPos.y);
				txtRadiusX->Text = String::Format("{0}", mEmitter->mSpawnRadius.x);
				txtRadiusY->Text = String::Format("{0}", mEmitter->mSpawnRadius.y);
				txtMaxCount->Text = String::Format("{0}", mEmitter->mMaxCount);
				txtMinDirX->Text = String::Format("{0}", mEmitter->mMinDir.x);
				txtMinDirY->Text = String::Format("{0}", mEmitter->mMinDir.y);
				txtMaxDirX->Text = String::Format("{0}", mEmitter->mMaxDir.x);
				txtMaxDirY->Text = String::Format("{0}", mEmitter->mMaxDir.y);

				txtGravityX->Text = String::Format("{0}", mEmitter->mGravity.x);
				txtGravityY->Text = String::Format("{0}", mEmitter->mGravity.y);

				txtMinScale->Text = String::Format("{0}", mEmitter->mMinScale);
				txtMaxScale->Text = String::Format("{0}", mEmitter->mMaxScale);
				cmbSrcBlend->SelectedIndex = mEmitter->mSrcBlend - 1;
				cmbDestBlend->SelectedIndex = mEmitter->mDestBlend - 1;
				txtMinLifeTime->Text = String::Format("{0}", mEmitter->mMinLifeTime);
				txtMaxLifeTime->Text = String::Format("{0}", mEmitter->mMaxLifeTime);
				txtFirstAppearMin->Text = String::Format("{0}", mEmitter->mMinFirstAppearTime);
				txtFirstAppearMax->Text = String::Format("{0}", mEmitter->mMaxFirstAppearTime);

				txtColor1R->Text = String::Format("{0}", (mEmitter->mMinStartColor >> 16) & 0xff);
				txtColor2R->Text = String::Format("{0}", (mEmitter->mMaxStartColor >> 16) & 0xff);
				txtColor3R->Text = String::Format("{0}", (mEmitter->mMinEndColor >> 16) & 0xff);
				txtColor4R->Text = String::Format("{0}", (mEmitter->mMaxEndColor >> 16) & 0xff);

				txtColor1G->Text = String::Format("{0}", (mEmitter->mMinStartColor >> 8) & 0xff);
				txtColor2G->Text = String::Format("{0}", (mEmitter->mMaxStartColor >> 8) & 0xff);
				txtColor3G->Text = String::Format("{0}", (mEmitter->mMinEndColor >> 8) & 0xff);
				txtColor4G->Text = String::Format("{0}", (mEmitter->mMaxEndColor >> 8) & 0xff);

				txtColor1B->Text = String::Format("{0}", (mEmitter->mMinStartColor) & 0xff);
				txtColor2B->Text = String::Format("{0}", (mEmitter->mMaxStartColor) & 0xff);
				txtColor3B->Text = String::Format("{0}", (mEmitter->mMinEndColor) & 0xff);
				txtColor4B->Text = String::Format("{0}", (mEmitter->mMaxEndColor) & 0xff);

				txtColor1A->Text = String::Format("{0}", (mEmitter->mMinStartColor >> 24) & 0xff);
				txtColor2A->Text = String::Format("{0}", (mEmitter->mMaxStartColor >> 24) & 0xff);
				txtColor3A->Text = String::Format("{0}", (mEmitter->mMinEndColor >> 24) & 0xff);
				txtColor4A->Text = String::Format("{0}", (mEmitter->mMaxEndColor >> 24) & 0xff);

				txtRotationSpeed->Text = String::Format("{0}", mEmitter->mRotationSpeed);
				txtScaleSpeed->Text = String::Format("{0}", mEmitter->mScaleSpeed);

				lstEvents->Items->Clear();
				nekoParticleEvent *event;
				for(int32 i = 0;event = mEmitter->GetEvent(i);++i)
				{
					lstEvents->Items->Add(String::Format("{0}", event->mTime) + " 초");
				}

				cbRepeat->Checked = mEmitter->mLoop;

				if(mEmitter->mTexture)
				{
					txtImageName->Text = ConvertToString(mEmitter->mTexture->GetName());
					txtImageName->Text = txtImageName->Text->Substring(txtImageName->Text->IndexOf("\\") + 1);
				}
				else
				{
					txtImageName->Text = "";
				}
			}

	private: System::Void label3_Click(System::Object^  sender, System::EventArgs^  e) {
			 }
private: System::Void textBox4_Leave(System::Object^  sender, System::EventArgs^  e) {
		 }
		 private: System::Void lstEmitters_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
		 {
			OnParticleSelected();
		 }
		 private: System::Void btFind_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			openFileDlg->InitialDirectory = mRunPath + "파티클\\";

			if(openFileDlg->ShowDialog(this) != System::Windows::Forms::DialogResult::OK)
			{
				mEmitter->SetTexture(NULL);
				txtImageName->Text = "";
				return;
			}

			txtImageName->Text = openFileDlg->FileName->Substring(openFileDlg->FileName->LastIndexOf("\\") + 1);
			mEmitter->SetTexture(ConvertToChar(txtImageName->Text));
		 }
		 private: System::Void cmbSrcBlend_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
		 {
			 mEmitter->mSrcBlend = cmbSrcBlend->SelectedIndex + 1;
		 }
		 private: System::Void cmbDestBlend_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
		 {
			 mEmitter->mDestBlend = cmbDestBlend->SelectedIndex + 1;
		 }
		 private: System::Void txtRevival_Leave(System::Object^  sender, System::EventArgs^  e) {
			 mEmitter->mEmitDelay = float::Parse(txtRevival->Text);
		 }
		 private: System::Void txtMaxCount_Leave(System::Object^  sender, System::EventArgs^  e)
		 {
			 int32 v;
			 
			 try
			 {
				v = int::Parse(txtMaxCount->Text);
			 } catch(Exception ^)
			 {
				v = 0;
			 }

			 if(v > 1000 || v < 0)
			 {
				v = mEmitter->mMaxCount;
			 }
			 else
			 {
				mEmitter->mMaxCount = v;
				mEmitter->FillNodes();
			}

			txtMaxCount->Text = String::Format("{0}", v);
		 }
		private: System::Void txtFirstRotation_Leave(System::Object^  sender, System::EventArgs^  e)
		{
			System::Windows::Forms::TextBox ^txtBox = (System::Windows::Forms::TextBox ^)sender;

			float32 v;

			try
			{
				v = float::Parse(txtBox->Text);
			} catch(Exception ^)
			{
				v = 0;
			}

			{
				mEmitter->mRotation = v;
				mEmitter->FillNodes();
			}

			txtBox->Text = String::Format("{0}", v);
		 }

		 // 파티클 Emitter 삭제
		 private: System::Void button3_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			if(lstEmitters->SelectedIndex < 0)
				return;

			if(lstEmitters->Items->Count == 1)
			{
				MessageBox::Show(this, "마지막 남은 잎새.. ㅠㅠ...", PAPA_TITLE, MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
				return;
			}

			if(MessageBox::Show(this, "정말 '" + lstEmitters->SelectedItem + "' (" + (lstEmitters->SelectedIndex + 1) + "번째) 파티클을 지우시겠습니까 ?", PAPA_TITLE, MessageBoxButtons::YesNo, MessageBoxIcon::Exclamation) == System::Windows::Forms::DialogResult::No)
				return;
				

			int32 oldIndex = lstEmitters->SelectedIndex;

			mParticle->RemoveEmitter(lstEmitters->SelectedIndex);
			lstEmitters->Items->RemoveAt(lstEmitters->SelectedIndex);

			lstEmitters->SelectedIndex = min(oldIndex, lstEmitters->Items->Count - 1);
		 }

		 // 파티클 Emitter 추가
		 private: System::Void button2_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 AddParticle("파티클");
		 }

private: System::Void txtMinLifeTime_Leave(System::Object^  sender, System::EventArgs^  e) {
			 try
			 {
				 mEmitter->mMinLifeTime = float::Parse(txtMinLifeTime->Text);
			 } catch(Exception ^)
			 {
				 txtMinLifeTime->Text = String::Format("{0}", mEmitter->mMinLifeTime);
			 }
		 }
private: System::Void txtMaxLifeTime_Leave(System::Object^  sender, System::EventArgs^  e) {
			 try
			 {
				 mEmitter->mMaxLifeTime = float::Parse(txtMaxLifeTime->Text);
			 } catch(Exception ^)
			 {
				 txtMaxLifeTime->Text = String::Format("{0}", mEmitter->mMaxLifeTime);
			 }
		 }

private: System::Void txtColor1_Leave(System::Object^  sender, System::EventArgs^  e) {
			 System::Windows::Forms::TextBox ^txtBox = (System::Windows::Forms::TextBox ^)sender;

			 int32 v;

			 try
			 {
				 v = int::Parse(txtBox->Text);
			 } catch(Exception ^)
			 {
				 v = 255;
			 }
			 
			 v = min(max(v, 0), 255);

			 {
				 int32 place = int::Parse((String ^)txtBox->Tag);
				 mEmitter->mMinStartColor = 
					 (mEmitter->mMinStartColor & (~(0xff << place))) | (v << place);
			 }

			 txtBox->Text = String::Format("{0}", v);
		 }
private: System::Void txtColor2_Leave(System::Object^  sender, System::EventArgs^  e) {
			 System::Windows::Forms::TextBox ^txtBox = (System::Windows::Forms::TextBox ^)sender;

			 int32 v;

			 try
			 {
				 v = int::Parse(txtBox->Text);
			 } catch(Exception ^)
			 {
				 v = 255;
			 }

			 v = min(max(v, 0), 255);

			 {
				 int32 place = int::Parse((String ^)txtBox->Tag);
				 mEmitter->mMaxStartColor = 
					 (mEmitter->mMaxStartColor & (~(0xff << place))) | (v << place);
			 }

			 txtBox->Text = String::Format("{0}", v);
		 }
private: System::Void txtColor3_Leave(System::Object^  sender, System::EventArgs^  e) {
			 System::Windows::Forms::TextBox ^txtBox = (System::Windows::Forms::TextBox ^)sender;

			 int32 v;

			 try
			 {
				 v = int::Parse(txtBox->Text);
			 } catch(Exception ^)
			 {
				 v = 255;
			 }

			 v = min(max(v, 0), 255);

			 {
				 int32 place = int::Parse((String ^)txtBox->Tag);
				 mEmitter->mMinEndColor = 
					 (mEmitter->mMinEndColor & (~(0xff << place))) | (v << place);
			 }

			 txtBox->Text = String::Format("{0}", v);
		 }
private: System::Void txtColor4_Leave(System::Object^  sender, System::EventArgs^  e) {
			 System::Windows::Forms::TextBox ^txtBox = (System::Windows::Forms::TextBox ^)sender;

			 int32 v;

			 try
			 {
				 v = int::Parse(txtBox->Text);
			 } catch(Exception ^)
			 {
				 v = 255;
			 }

			 v = min(max(v, 0), 255);

			 {
				 int32 place = int::Parse((String ^)txtBox->Tag);
				 mEmitter->mMaxEndColor = 
					 (mEmitter->mMaxEndColor & (~(0xff << place))) | (v << place);
			 }

			 txtBox->Text = String::Format("{0}", v);
		 }
private: System::Void txtRotationSpeed_Leave(System::Object^  sender, System::EventArgs^  e) {
			 try
			 {
				mEmitter->mRotationSpeed = float::Parse(txtRotationSpeed->Text);
			 } catch(Exception ^)
			 {
				txtRotationSpeed->Text = String::Format("{0}", mEmitter->mRotationSpeed);
			 }
		 }
private: System::Void txtScaleSpeed_Leave(System::Object^  sender, System::EventArgs^  e) {
			try
			{
				mEmitter->mScaleSpeed = float::Parse(txtScaleSpeed->Text);
			} catch(Exception ^)
			{
				txtScaleSpeed->Text = String::Format("{0}", mEmitter->mScaleSpeed);
			}
		 }
private: System::Void txtSpawnPosX_Leave(System::Object^  sender, System::EventArgs^  e) {
			 try
			 {
				 mEmitter->mSpawnPos.x = int::Parse(txtSpawnPosX->Text);
			 } catch(Exception ^)
			 {
				 txtSpawnPosX->Text = String::Format("{0}", mEmitter->mSpawnPos.x);
			 }
		 }
private: System::Void txtSpawnPosY_Leave(System::Object^  sender, System::EventArgs^  e) {
			try
			{
				mEmitter->mSpawnPos.y = int::Parse(txtSpawnPosY->Text);
			} catch(Exception ^)
			{
				txtSpawnPosY->Text = String::Format("{0}", mEmitter->mSpawnPos.y);
			}
		 }
private: System::Void txtGravityX_Leave(System::Object^  sender, System::EventArgs^  e) {
			 try
			 {
				mEmitter->mGravity.x = float::Parse(txtGravityX->Text);
			 } catch(Exception ^)
			 {
				 txtGravityX->Text = String::Format("{0}", mEmitter->mGravity.x);
			 }
		 }
private: System::Void txtGravityY_Leave(System::Object^  sender, System::EventArgs^  e) {
			 try
			 {
				 mEmitter->mGravity.y = float::Parse(txtGravityY->Text);
			 } catch(Exception ^)
			 {
				 txtGravityY->Text = String::Format("{0}", mEmitter->mGravity.y);
			 }
		 }

private: System::Void txtMinDirX_Leave(System::Object^  sender, System::EventArgs^  e) {
			 System::Windows::Forms::TextBox ^txtBox = (System::Windows::Forms::TextBox ^)sender;

			 try
			 {
				 mEmitter->mMinDir.x = float::Parse(txtBox->Text);
			 } catch(Exception ^)
			 {
				 txtBox->Text = String::Format("{0}", mEmitter->mMinDir.x);
			 }
		 }
private: System::Void txtMinDirY_Leave(System::Object^  sender, System::EventArgs^  e) {
			 System::Windows::Forms::TextBox ^txtBox = (System::Windows::Forms::TextBox ^)sender;
			 try
			 {
				 mEmitter->mMinDir.y = float::Parse(txtBox->Text);
			 } catch(Exception ^)
			 {
				 txtBox->Text = String::Format("{0}", mEmitter->mMinDir.y);
			 }
		 }
private: System::Void txtMaxDirX_Leave(System::Object^  sender, System::EventArgs^  e) {
			 System::Windows::Forms::TextBox ^txtBox = (System::Windows::Forms::TextBox ^)sender;
			 try
			 {
				 mEmitter->mMaxDir.x = float::Parse(txtBox->Text);
			 } catch(Exception ^)
			 {
				 txtBox->Text = String::Format("{0}", mEmitter->mMaxDir.x);
			 }
		 }
private: System::Void txtMaxDirY_Leave(System::Object^  sender, System::EventArgs^  e) {
			 System::Windows::Forms::TextBox ^txtBox = (System::Windows::Forms::TextBox ^)sender;
			 try
			 {
				 mEmitter->mMaxDir.y = float::Parse(txtBox->Text);
			 } catch(Exception ^)
			 {
				 txtBox->Text = String::Format("{0}", mEmitter->mMaxDir.y);
			 }
		 }
		 /// 리셋 버튼
		 private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {

			mParticle->AtFirst();
		 }
private: System::Void txtRadiusY_Leave(System::Object^  sender, System::EventArgs^  e) {
			 System::Windows::Forms::TextBox ^txtBox = (System::Windows::Forms::TextBox ^)sender;
			 try
			 {
				 mEmitter->mSpawnRadius.y = float::Parse(txtBox->Text);
			 } catch(Exception ^)
			 {
				 txtBox->Text = String::Format("{0}", mEmitter->mSpawnRadius.y);
			 }
		 }
private: System::Void txtRadiusX_Leave(System::Object^  sender, System::EventArgs^  e) {
			 System::Windows::Forms::TextBox ^txtBox = (System::Windows::Forms::TextBox ^)sender;
			 try
			 {
				 mEmitter->mSpawnRadius.x = float::Parse(txtBox->Text);
			 } catch(Exception ^)
			 {
				 txtBox->Text = String::Format("{0}", mEmitter->mSpawnRadius.x);
			 }
		 }
		 private: System::Void cbRepeat_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			mEmitter->mLoop = cbRepeat->Checked;
		 }
		private: System::Void txtMinScale_Leave(System::Object^  sender, System::EventArgs^  e) {
			 try
			 {
				 mEmitter->mMinScale = float::Parse(txtMinScale->Text);
			 } catch(Exception ^)
			 {
				 txtMinScale->Text = String::Format("{0}", mEmitter->mMinScale);
			 }
		 }
		private: System::Void txtMaxScale_Leave(System::Object^  sender, System::EventArgs^  e) {
			 try
			 {
				 mEmitter->mMaxScale = float::Parse(txtMaxScale->Text);
			 } catch(Exception ^)
			 {
				 txtMaxScale->Text = String::Format("{0}", mEmitter->mMaxScale);
			 }
		}

private: System::Void btAddEvent_Click(System::Object^  sender, System::EventArgs^  e) {
			 if(mEmitter)
			 {
				 if(lstEvents->Items->Count >= 32)
				 {
					 MessageBox::Show(this, "이벤트의 최대 개수는 32개 입니다.", PAPA_TITLE, MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
					 return;
				 }

				 lstEvents->SelectedIndex = lstEvents->Items->Add(String::Format("{0} 초", mEmitter->GetEvent(mEmitter->AddEvent())->mTime));
			 }
		 }
		 private: System::Void btDelEvent_Click(System::Object^  sender, System::EventArgs^  e) {
			 if(mEmitter && lstEvents->SelectedIndex >= 0)
			 {
				mEmitter->RemoveEvent(lstEvents->SelectedIndex);
				lstEvents->Items->RemoveAt(lstEvents->SelectedIndex);
			 }
		 }
		 private: System::Void lstEvents_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
			if(mEmitter && lstEvents->SelectedIndex >= 0)
			{
			  mEvent = mEmitter->GetEvent(lstEvents->SelectedIndex);
			  if(mEvent == NULL)
				  return;

			  txtEventTime->Text = String::Format("{0}", mEvent->mTime);
			  txtEventRotSpeed->Text = String::Format("{0}", mEvent->mRotationSpeed);
			  txtEventScaleSpeed->Text = String::Format("{0}", mEvent->mScaleSpeed);
			  txtEventMinX->Text = String::Format("{0}", mEvent->mMinDir.x);
			  txtEventMinY->Text = String::Format("{0}", mEvent->mMinDir.y);
			  txtEventMaxX->Text = String::Format("{0}", mEvent->mMaxDir.x);
			  txtEventMaxY->Text = String::Format("{0}", mEvent->mMaxDir.y);
			  txtEventColorA->Text = String::Format("{0}", (mEvent->mColor >> 24) & 0xff);
			  txtEventColorR->Text = String::Format("{0}", (mEvent->mColor >> 16) & 0xff);
			  txtEventColorG->Text = String::Format("{0}", (mEvent->mColor >> 8) & 0xff);
			  txtEventColorB->Text = String::Format("{0}", (mEvent->mColor) & 0xff);

			  txtEventGravityX->Text = String::Format("{0}", mEvent->mGravity.x);
			  txtEventGravityY->Text = String::Format("{0}", mEvent->mGravity.y);

			  cbEventRotSpeed->Checked = ((mEvent->mFlags & EPEF_ROTATION) > 0);
			  cbEventScaleSpeed->Checked = ((mEvent->mFlags & EPEF_SCALE) > 0);
			  //cbEventRepeat->Checked = ((mEvent->mFlags & EPEF_LOOP) > 0);
			  cbEventChangeDir->Checked = ((mEvent->mFlags & EPEF_DIR) > 0);
			  cbEventGravity->Checked = ((mEvent->mFlags & EPEF_GRAVITY) > 0);
			  cbEventColor->Checked = ((mEvent->mFlags & EPEF_COLOR) > 0);

			}
		 }
		 private: System::Void txtEventTime_Leave(System::Object^  sender, System::EventArgs^  e) {
			if(mEvent)
			{
				try
				{
					mEvent->mTime = float::Parse(txtEventTime->Text);
				} catch(Exception ^)
				{
					txtEventTime->Text = String::Format("{0}", mEvent->mTime);
				}

				lstEvents->Items[lstEvents->SelectedIndex] = txtEventTime->Text + " 초";
			}
		}
private: System::Void cbEventRotSpeed_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			 if(mEvent)
			 {
				 if(cbEventRotSpeed->Checked)
					 mEvent->mFlags |= EPEF_ROTATION;
				 else
					 mEvent->mFlags &= ~EPEF_ROTATION;
			 }
		 }
private: System::Void cbEventChangeDir_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			 if(mEvent)
			 {
				 if(cbEventChangeDir->Checked)
					 mEvent->mFlags |= EPEF_DIR;
				 else
					 mEvent->mFlags &= ~EPEF_DIR;
			 }
		 }
private: System::Void cbEventGravity_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			 if(mEvent)
			 {
				 if(cbEventGravity->Checked)
					 mEvent->mFlags |= EPEF_GRAVITY;
				 else
					 mEvent->mFlags &= ~EPEF_GRAVITY;
			 }
		 }
private: System::Void cbEventColor_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			 if(mEvent)
			 {
				 if(cbEventColor->Checked)
					 mEvent->mFlags |= EPEF_COLOR;
				 else
					 mEvent->mFlags &= ~EPEF_COLOR;
			 }
		 }
private: System::Void txtEventRotSpeed_Leave(System::Object^  sender, System::EventArgs^  e) {
			 if(!mEvent)
				 return;

			 System::Windows::Forms::TextBox ^txtBox = (System::Windows::Forms::TextBox ^)sender;

			 float32 v;

			 try
			 {
				 v = float::Parse(txtBox->Text);
			 } catch(Exception ^)
			 {
				 v = 0;
			 }

			 {
				 mEvent->mRotationSpeed = v;
			 }

			 txtBox->Text = String::Format("{0}", v);
		 }
private: System::Void txtEventMinY_Leave(System::Object^  sender, System::EventArgs^  e) {
			 if(!mEvent)
				 return;

			 System::Windows::Forms::TextBox ^txtBox = (System::Windows::Forms::TextBox ^)sender;

			 float32 v;

			 try
			 {
				 v = float32::Parse(txtBox->Text);
			 } catch(Exception ^)
			 {
				 v = 0.0f;
			 }

			 int32 place = int::Parse((String ^)txtBox->Tag);
			 switch(place)
			 {
			 case 0:
				 mEvent->mMinDir.x = v;
				 break;
			 case 1:
				 mEvent->mMinDir.y = v;
				 break;
			 case 2:
				 mEvent->mMaxDir.x = v;
				 break;
			 case 3:
				 mEvent->mMaxDir.y = v;
				 break;

			 }

			 txtBox->Text = String::Format("{0}", v);
		 }
private: System::Void txtEventGravityX_Leave(System::Object^  sender, System::EventArgs^  e) {
			 if(!mEvent)
				 return;

			 System::Windows::Forms::TextBox ^txtBox = (System::Windows::Forms::TextBox ^)sender;

			 float32 v;

			 try
			 {
				 v = float32::Parse(txtBox->Text);
			 } catch(Exception ^)
			 {
				 v = 0.0f;
			 }

			 int32 place = int::Parse((String ^)txtBox->Tag);
			 switch(place)
			 {
			 case 0:
				 mEvent->mGravity.x = v;
				 break;
			 case 1:
				 mEvent->mGravity.y = v;
				 break;

			 }

			 txtBox->Text = String::Format("{0}", v);
		 }
private: System::Void txtEventColorA_Leave(System::Object^  sender, System::EventArgs^  e) {
			 if(!mEvent)
				 return;

			 System::Windows::Forms::TextBox ^txtBox = (System::Windows::Forms::TextBox ^)sender;

			 int32 v;

			 try
			 {
				 v = int::Parse(txtBox->Text);
			 } catch(Exception ^)
			 {
				 v = 255;
			 }

			 v = min(max(v, 0), 255);

			 {
				 int32 place = int::Parse((String ^)txtBox->Tag);
				 mEvent->mColor = 
					 (mEvent->mColor & (~(0xff << place))) | (v << place);
			 }

			 txtBox->Text = String::Format("{0}", v);
		 }
		 private: System::Void txtEventScaleSpeed_Leave(System::Object^  sender, System::EventArgs^  e) {
			  if(!mEvent)
				  return;

			  System::Windows::Forms::TextBox ^txtBox = (System::Windows::Forms::TextBox ^)sender;

			  float v;

			  try
			  {
				  v = float::Parse(txtBox->Text);
			  } catch(Exception ^)
			  {
				  v = 0;
			  }

			  {
				  mEvent->mScaleSpeed = v;
			  }

			  txtBox->Text = String::Format("{0}", v);
		 }
		 private: System::Void cbEventScaleSpeed_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			  if(mEvent)
			  {
				  if(cbEventScaleSpeed->Checked)
					  mEvent->mFlags |= EPEF_SCALE;
				  else
					  mEvent->mFlags &= ~EPEF_SCALE;
			  }
		 }
		 private: System::Void 새로만들기NToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			System::Windows::Forms::DialogResult res = 
				MessageBox::Show(this, "현재 작업한 내용을 저장하시겠습니까 ?", PAPA_TITLE, MessageBoxButtons::YesNoCancel, MessageBoxIcon::Exclamation);

			if(res == System::Windows::Forms::DialogResult::Yes)
			{
				if(!SaveFile(false))
					return;
			}
			else if(res == System::Windows::Forms::DialogResult::Cancel)
				return;

			mFileName = "";
			UpdateCaption();

			lstEmitters->Items->Clear();

			// 새로 만드는 루틴 넣기..
			mParticle->Clear();
			AddParticle("시작");
		 }
		 private: System::Void 불러오기OToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
		  String ^fileName;
		  System::Windows::Forms::DialogResult res = 
			  MessageBox::Show(this, "현재 작업한 내용을 저장하시겠습니까 ?", PAPA_TITLE, MessageBoxButtons::YesNoCancel, MessageBoxIcon::Exclamation);

		  if(res == System::Windows::Forms::DialogResult::Yes)
		  {
				if(!SaveFile(false))
					return;
		  }
		  else if(res == System::Windows::Forms::DialogResult::Cancel)
			  return;

		  OpenFileDialog ^dlg = gcnew OpenFileDialog();
		  dlg->Filter = "nkNovel을 위한 파티클 파일(*.nkparticle)|*.nkparticle";
		  dlg->InitialDirectory = mRunPath + "파티클\\";

		  if(dlg->ShowDialog(this) == System::Windows::Forms::DialogResult::OK)
		  {
			  fileName = dlg->FileName;
		  }
		  else
			  return;

			// 불러오는 루틴 넣기..
		  if(FAILED(mParticle->LoadParticle(ConvertToChar(fileName))))
			  return;

		  mFileName = fileName;
		  lstEmitters->Items->Clear();

		  nekoParticleEmitter *emitter;
		  for(uint32 i = 0;emitter = mParticle->GetEmitter(i);++i)
		  {
			lstEmitters->Items->Add(ConvertToString(emitter->mName.c_str()));
		  }

		  if(mParticle->GetEmitterCount() > 0)
		  {
			  lstEmitters->SelectedIndex = 0;
		  }

		  UpdateCaption();
		 }
		 private: void UpdateCaption()
		 {
			 String ^fileName = "제목 없음";
			 if(mFileName->Length > 0)
				 fileName = mFileName->Substring(mFileName->LastIndexOf("\\") + 1);

			 this->Text = "네코노벨 파티클 에디터 <" + fileName + "> Ver " + String::Format("{0:##.00}", PAPA_VERSION * 0.01f) + 
				 " ( 엔진 Ver " +  String::Format("{0:##.00}", ENGINE_VERSION * 0.01f) + " )";
		 }
		 private: bool SaveFile(bool as)
		  {
			  if(as || mFileName->Length == 0)
			  {
				  SaveFileDialog ^dlg = gcnew SaveFileDialog();
				  dlg->Filter = "nkNovel을 위한 파티클 파일(*.nkparticle)|*.nkparticle";
				  dlg->InitialDirectory = mRunPath + "파티클\\";

				  if(dlg->ShowDialog(this) == System::Windows::Forms::DialogResult::OK)
				  {
					  mFileName = dlg->FileName;
					  UpdateCaption();
				  }
				  else
					  return false;
			  }

			  if(FAILED(mParticle->SaveParticle(ConvertToChar(mFileName))))
				  return false;

			  return true;
		  }
		 private: System::Void 저장하기SToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			  SaveFile(false);
		 }
		 private: System::Void 다른이름으로저장하기AToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			  SaveFile(true);
		 }
		 private: System::Void 프로그램종료XToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
				Application::Exit();
		 }
		 private: System::Void MainForm_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
			  System::Windows::Forms::DialogResult res = 
				  MessageBox::Show(this, "현재 작업한 내용을 저장하시겠습니까 ?", PAPA_TITLE, MessageBoxButtons::YesNoCancel, MessageBoxIcon::Exclamation);

			  if(res == System::Windows::Forms::DialogResult::Yes)
			  {
				if(!SaveFile(false))
					e->Cancel = true;
			  }
			  else if(res == System::Windows::Forms::DialogResult::Cancel)
			  {
				  e->Cancel = true;
			  }
		 }
		 private: System::Void picCanvas_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
			  statusBar->Items[0]->Text = "마우스 좌표 : (" + e->X + ", " + e->Y + ")";
		 }
private: System::Void picCanvas_MouseDoubleClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
			 OpenFileDialog ^dlg = gcnew OpenFileDialog();
			 dlg->Filter = L"이미지 파일(BMP, PNG, TGA, JPG, DDS)|*.BMP;*.PNG;*.TGA;*.JPG;*.DDS";
			 dlg->Title = L"테스트 배경화면 불러오기";
			 dlg->InitialDirectory = mRunPath + "이미지\\";

			 if(dlg->ShowDialog(this) == System::Windows::Forms::DialogResult::OK)
			 {
				 String ^str = dlg->FileName;
				 mBGImage->SetImage(ConvertToChar(str->Substring(str->LastIndexOf("\\") + 1)));
			 }
			 else
			 {
				 mBGImage->SetImage(NULL);
			 }
		 }
private: System::Void 테스트배경화면불러오기F5ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 picCanvas_MouseDoubleClick(sender, nullptr);
		 }
private: System::Void txtFirstAppearMin_Leave(System::Object^  sender, System::EventArgs^  e) {
			 try
			 {
				 mEmitter->mMinFirstAppearTime = float::Parse(txtFirstAppearMin->Text);
			 } catch(Exception ^)
			 {
				 txtFirstAppearMin->Text = String::Format("{0}", mEmitter->mMinFirstAppearTime);
			 }
		 }
private: System::Void txtFirstAppearMax_Leave(System::Object^  sender, System::EventArgs^  e) {
			 try
			 {
				 mEmitter->mMaxFirstAppearTime = float::Parse(txtFirstAppearMax->Text);
			 } catch(Exception ^)
			 {
				 txtFirstAppearMax->Text = String::Format("{0}", mEmitter->mMaxFirstAppearTime);
			 }
		 }
};
}

