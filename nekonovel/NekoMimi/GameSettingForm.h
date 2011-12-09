#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace NekoMimi {

	/// <summary>
	/// GameSettingForm에 대한 요약입니다.
	///
	/// 경고: 이 클래스의 이름을 변경하면 이 클래스가 의존하는
	///          모든 .resx 파일과 관련된 관리되는 리소스 컴파일러 도구의
	///          '리소스 파일 이름' 속성도 변경해야 합니다. 그렇지 않은 경우
	///          디자이너는 이 폼과 관련된 지역화된 리소스와
	///          올바르게 상호 작용할 수 없습니다.
	/// </summary>
	public ref class GameSettingForm : public System::Windows::Forms::Form
	{
	private:

		bool extFontsChanged;
	private: System::Windows::Forms::Label^  label11;
	private: System::Windows::Forms::TextBox^  txtMaxPage;
	private: System::Windows::Forms::CheckBox^  cbChatBoxThumbnail;
	private: System::Windows::Forms::TrackBar^  tbWinTransparency;

	private: System::Windows::Forms::Label^  label12;
	private: System::Windows::Forms::CheckBox^  cbNewTypeCursor;


			 bool initialized;
		
	public:
		GameSettingForm(void)
			: extFontsChanged(false)
			, initialized(false)
		{
			InitializeComponent();

			nekoGame *game = GetNekoNovel()->GetCurrentGame();

			txtTitle->Text = ConvertToString(game->GetName());
			txtMadeBy->Text = ConvertToString(game->GetMadeBy());

			nekoGameEnvironment &env = game->GetEnvironment();
			tbFXVolume->Value = env.mDefFXVolume;
			tbBGMVolume->Value = env.mDefBGMVolume;
			tbVoiceVolume->Value = env.mDefVoiceVolume;
			tbWinTransparency->Value = env.mTextWindowAlpha;
			cbFullScreen->Checked = env.mDefFullScreen;

			txtFirstFileName->Text = ConvertToString(env.mFirstScriptName.c_str());
			if(env.mCursorType == ECCT_TWINKLE)
				rbCursor1->Checked = true;
			else if(env.mCursorType == ECCT_ANIMATION)
			{
				rbCursor2->Checked = true;
				txtCursorWidth->Text = String::Format("{0}", env.mCursorAniWidth);
				txtCursorHeight->Text = String::Format("{0}", env.mCursorAniHeight);
			}

			txtScreenWidth->Text = String::Format("{0}", env.mWidth);
			txtScreenHeight->Text = String::Format("{0}", env.mHeight);

			txtExternalFonts->Text = ConvertToString(env.mExternalFonts.c_str());

			txtMaxPage->Text = String::Format("{0}", env.mMaxPage);
			cbChatBoxThumbnail->Checked = ((env.mProjectFlags & EPFT_CHATBOX_IN_THUMBNAIL) > 0);
			cbNewTypeCursor->Checked = ((env.mProjectFlags & EPFT_NEWTYPE_CURSOR) > 0);

			initialized = true;
		}

	protected:
		/// <summary>
		/// 사용 중인 모든 리소스를 정리합니다.
		/// </summary>
		~GameSettingForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  btClose;
	protected: 

	private: System::Windows::Forms::Button^  btSave;
	protected: 

	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::TextBox^  txtTitle;
	private: System::Windows::Forms::CheckBox^  cbFullScreen;




	private: System::Windows::Forms::TrackBar^  tbFXVolume;

	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::TrackBar^  tbBGMVolume;
	private: System::Windows::Forms::TrackBar^  tbVoiceVolume;







	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::TextBox^  txtMadeBy;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::TextBox^  txtFirstFileName;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::RadioButton^  rbCursor1;
	private: System::Windows::Forms::RadioButton^  rbCursor2;


	private: System::Windows::Forms::TextBox^  txtCursorHeight;

	private: System::Windows::Forms::TextBox^  txtCursorWidth;

	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::Label^  label8;
	private: System::Windows::Forms::TextBox^  txtScreenHeight;
	private: System::Windows::Forms::TextBox^  txtScreenWidth;
	private: System::Windows::Forms::Label^  label9;
	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::TextBox^  txtExternalFonts;
	private: System::Windows::Forms::Label^  label10;



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
			this->btClose = (gcnew System::Windows::Forms::Button());
			this->btSave = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->txtTitle = (gcnew System::Windows::Forms::TextBox());
			this->cbFullScreen = (gcnew System::Windows::Forms::CheckBox());
			this->tbFXVolume = (gcnew System::Windows::Forms::TrackBar());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->tbBGMVolume = (gcnew System::Windows::Forms::TrackBar());
			this->tbVoiceVolume = (gcnew System::Windows::Forms::TrackBar());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->txtMadeBy = (gcnew System::Windows::Forms::TextBox());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->txtFirstFileName = (gcnew System::Windows::Forms::TextBox());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->cbNewTypeCursor = (gcnew System::Windows::Forms::CheckBox());
			this->txtCursorHeight = (gcnew System::Windows::Forms::TextBox());
			this->txtCursorWidth = (gcnew System::Windows::Forms::TextBox());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->rbCursor1 = (gcnew System::Windows::Forms::RadioButton());
			this->rbCursor2 = (gcnew System::Windows::Forms::RadioButton());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->txtScreenHeight = (gcnew System::Windows::Forms::TextBox());
			this->txtScreenWidth = (gcnew System::Windows::Forms::TextBox());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->txtExternalFonts = (gcnew System::Windows::Forms::TextBox());
			this->label11 = (gcnew System::Windows::Forms::Label());
			this->txtMaxPage = (gcnew System::Windows::Forms::TextBox());
			this->cbChatBoxThumbnail = (gcnew System::Windows::Forms::CheckBox());
			this->tbWinTransparency = (gcnew System::Windows::Forms::TrackBar());
			this->label12 = (gcnew System::Windows::Forms::Label());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->tbFXVolume))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->tbBGMVolume))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->tbVoiceVolume))->BeginInit();
			this->groupBox1->SuspendLayout();
			this->groupBox2->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->tbWinTransparency))->BeginInit();
			this->SuspendLayout();
			// 
			// btClose
			// 
			this->btClose->Location = System::Drawing::Point(509, 355);
			this->btClose->Name = L"btClose";
			this->btClose->Size = System::Drawing::Size(136, 32);
			this->btClose->TabIndex = 1;
			this->btClose->Text = L"창 닫기(&C)";
			this->btClose->UseVisualStyleBackColor = true;
			this->btClose->Click += gcnew System::EventHandler(this, &GameSettingForm::btClose_Click);
			// 
			// btSave
			// 
			this->btSave->Location = System::Drawing::Point(373, 355);
			this->btSave->Name = L"btSave";
			this->btSave->Size = System::Drawing::Size(136, 32);
			this->btSave->TabIndex = 0;
			this->btSave->Text = L"저장(&S)";
			this->btSave->UseVisualStyleBackColor = true;
			this->btSave->Click += gcnew System::EventHandler(this, &GameSettingForm::btSave_Click);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(8, 24);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(49, 12);
			this->label1->TabIndex = 2;
			this->label1->Text = L"타이틀 :";
			// 
			// txtTitle
			// 
			this->txtTitle->Location = System::Drawing::Point(88, 19);
			this->txtTitle->MaxLength = 128;
			this->txtTitle->Name = L"txtTitle";
			this->txtTitle->Size = System::Drawing::Size(400, 21);
			this->txtTitle->TabIndex = 3;
			// 
			// cbFullScreen
			// 
			this->cbFullScreen->AutoSize = true;
			this->cbFullScreen->Location = System::Drawing::Point(16, 80);
			this->cbFullScreen->Name = L"cbFullScreen";
			this->cbFullScreen->Size = System::Drawing::Size(117, 16);
			this->cbFullScreen->TabIndex = 4;
			this->cbFullScreen->Text = L"기본 풀스크린(&F)";
			this->cbFullScreen->UseVisualStyleBackColor = true;
			// 
			// tbFXVolume
			// 
			this->tbFXVolume->Location = System::Drawing::Point(288, 80);
			this->tbFXVolume->Maximum = 100;
			this->tbFXVolume->Name = L"tbFXVolume";
			this->tbFXVolume->Size = System::Drawing::Size(200, 45);
			this->tbFXVolume->TabIndex = 5;
			this->tbFXVolume->Value = 50;
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(176, 88);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(105, 12);
			this->label2->TabIndex = 6;
			this->label2->Text = L"기본 효과음 볼륨 :";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(176, 120);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(105, 12);
			this->label3->TabIndex = 7;
			this->label3->Text = L"기본 배경음 볼륨 :";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(176, 152);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(105, 12);
			this->label4->TabIndex = 8;
			this->label4->Text = L"기본 보이스 볼륨 :";
			// 
			// tbBGMVolume
			// 
			this->tbBGMVolume->Location = System::Drawing::Point(288, 112);
			this->tbBGMVolume->Maximum = 100;
			this->tbBGMVolume->Name = L"tbBGMVolume";
			this->tbBGMVolume->Size = System::Drawing::Size(200, 45);
			this->tbBGMVolume->TabIndex = 9;
			this->tbBGMVolume->Value = 50;
			// 
			// tbVoiceVolume
			// 
			this->tbVoiceVolume->Location = System::Drawing::Point(288, 144);
			this->tbVoiceVolume->Maximum = 100;
			this->tbVoiceVolume->Name = L"tbVoiceVolume";
			this->tbVoiceVolume->Size = System::Drawing::Size(200, 45);
			this->tbVoiceVolume->TabIndex = 10;
			this->tbVoiceVolume->Value = 50;
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(8, 48);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(77, 12);
			this->label6->TabIndex = 15;
			this->label6->Text = L"제작자 이름 :";
			// 
			// txtMadeBy
			// 
			this->txtMadeBy->Location = System::Drawing::Point(88, 45);
			this->txtMadeBy->MaxLength = 128;
			this->txtMadeBy->Name = L"txtMadeBy";
			this->txtMadeBy->Size = System::Drawing::Size(400, 21);
			this->txtMadeBy->TabIndex = 16;
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(8, 217);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(133, 12);
			this->label5->TabIndex = 17;
			this->label5->Text = L"첫 실행 스크립트 이름 :";
			// 
			// txtFirstFileName
			// 
			this->txtFirstFileName->Location = System::Drawing::Point(144, 213);
			this->txtFirstFileName->Name = L"txtFirstFileName";
			this->txtFirstFileName->Size = System::Drawing::Size(144, 21);
			this->txtFirstFileName->TabIndex = 18;
			this->txtFirstFileName->Text = L"타이틀.txt";
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->cbNewTypeCursor);
			this->groupBox1->Controls->Add(this->txtCursorHeight);
			this->groupBox1->Controls->Add(this->txtCursorWidth);
			this->groupBox1->Controls->Add(this->label7);
			this->groupBox1->Controls->Add(this->rbCursor1);
			this->groupBox1->Controls->Add(this->rbCursor2);
			this->groupBox1->Location = System::Drawing::Point(10, 265);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(408, 84);
			this->groupBox1->TabIndex = 19;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"대사 커서 설정";
			// 
			// cbNewTypeCursor
			// 
			this->cbNewTypeCursor->AutoSize = true;
			this->cbNewTypeCursor->Location = System::Drawing::Point(8, 62);
			this->cbNewTypeCursor->Name = L"cbNewTypeCursor";
			this->cbNewTypeCursor->Size = System::Drawing::Size(274, 16);
			this->cbNewTypeCursor->TabIndex = 4;
			this->cbNewTypeCursor->Text = L"커서를 대사출력영역 오른쪽 아래에 붙이기(&P)\r\n";
			this->cbNewTypeCursor->UseVisualStyleBackColor = true;
			// 
			// txtCursorHeight
			// 
			this->txtCursorHeight->Location = System::Drawing::Point(354, 37);
			this->txtCursorHeight->MaxLength = 2;
			this->txtCursorHeight->Name = L"txtCursorHeight";
			this->txtCursorHeight->ReadOnly = true;
			this->txtCursorHeight->Size = System::Drawing::Size(32, 21);
			this->txtCursorHeight->TabIndex = 2;
			this->txtCursorHeight->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &GameSettingForm::txtCursorWidth_KeyPress);
			// 
			// txtCursorWidth
			// 
			this->txtCursorWidth->Location = System::Drawing::Point(286, 37);
			this->txtCursorWidth->MaxLength = 2;
			this->txtCursorWidth->Name = L"txtCursorWidth";
			this->txtCursorWidth->ReadOnly = true;
			this->txtCursorWidth->Size = System::Drawing::Size(32, 21);
			this->txtCursorWidth->TabIndex = 2;
			this->txtCursorWidth->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &GameSettingForm::txtCursorWidth_KeyPress);
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(176, 42);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(179, 12);
			this->label7->TabIndex = 3;
			this->label7->Text = L"조각 개수  -  가로 :         세로 :";
			// 
			// rbCursor1
			// 
			this->rbCursor1->AutoSize = true;
			this->rbCursor1->Checked = true;
			this->rbCursor1->Location = System::Drawing::Point(8, 20);
			this->rbCursor1->Name = L"rbCursor1";
			this->rbCursor1->Size = System::Drawing::Size(159, 16);
			this->rbCursor1->TabIndex = 1;
			this->rbCursor1->TabStop = true;
			this->rbCursor1->Text = L"깜빡 거리는 커서 사용(&1)";
			this->rbCursor1->UseVisualStyleBackColor = true;
			this->rbCursor1->CheckedChanged += gcnew System::EventHandler(this, &GameSettingForm::radioButton2_CheckedChanged);
			// 
			// rbCursor2
			// 
			this->rbCursor2->AutoSize = true;
			this->rbCursor2->Location = System::Drawing::Point(8, 40);
			this->rbCursor2->Name = L"rbCursor2";
			this->rbCursor2->Size = System::Drawing::Size(155, 16);
			this->rbCursor2->TabIndex = 0;
			this->rbCursor2->Text = L"커서 애니메이션 사용(&2)";
			this->rbCursor2->UseVisualStyleBackColor = true;
			this->rbCursor2->CheckedChanged += gcnew System::EventHandler(this, &GameSettingForm::rbCursor2_CheckedChanged);
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Location = System::Drawing::Point(16, 104);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(107, 12);
			this->label8->TabIndex = 20;
			this->label8->Text = L"해상도 :            ×";
			// 
			// txtScreenHeight
			// 
			this->txtScreenHeight->Location = System::Drawing::Point(128, 100);
			this->txtScreenHeight->MaxLength = 4;
			this->txtScreenHeight->Name = L"txtScreenHeight";
			this->txtScreenHeight->Size = System::Drawing::Size(32, 21);
			this->txtScreenHeight->TabIndex = 22;
			// 
			// txtScreenWidth
			// 
			this->txtScreenWidth->Location = System::Drawing::Point(71, 100);
			this->txtScreenWidth->MaxLength = 4;
			this->txtScreenWidth->Name = L"txtScreenWidth";
			this->txtScreenWidth->Size = System::Drawing::Size(32, 21);
			this->txtScreenWidth->TabIndex = 21;
			// 
			// label9
			// 
			this->label9->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)), 
				static_cast<System::Int32>(static_cast<System::Byte>(64)));
			this->label9->Location = System::Drawing::Point(16, 128);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(144, 40);
			this->label9->TabIndex = 23;
			this->label9->Text = L"* 특정 해상도의 경우에는 \'전체화면\'이 지원되지 않을 수 있습니다.";
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->label10);
			this->groupBox2->Controls->Add(this->txtExternalFonts);
			this->groupBox2->Location = System::Drawing::Point(494, 12);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(151, 254);
			this->groupBox2->TabIndex = 24;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"외부 폰트 등록";
			// 
			// label10
			// 
			this->label10->Location = System::Drawing::Point(8, 175);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(136, 66);
			this->label10->TabIndex = 1;
			this->label10->Text = L"* 각 폰트의 파일명을 차례로 써주세요.\r\n예)\r\n나눔고딕.ttf\r\n나눔고딕코딩.ttf";
			// 
			// txtExternalFonts
			// 
			this->txtExternalFonts->Location = System::Drawing::Point(8, 18);
			this->txtExternalFonts->Multiline = true;
			this->txtExternalFonts->Name = L"txtExternalFonts";
			this->txtExternalFonts->ScrollBars = System::Windows::Forms::ScrollBars::Horizontal;
			this->txtExternalFonts->Size = System::Drawing::Size(137, 151);
			this->txtExternalFonts->TabIndex = 0;
			this->txtExternalFonts->TextChanged += gcnew System::EventHandler(this, &GameSettingForm::txtExternalFonts_TextChanged);
			// 
			// label11
			// 
			this->label11->AutoSize = true;
			this->label11->Location = System::Drawing::Point(8, 243);
			this->label11->Name = L"label11";
			this->label11->Size = System::Drawing::Size(265, 12);
			this->label11->TabIndex = 25;
			this->label11->Text = L"총 로드/세이브 페이지 수 :           ( 최대치 20 )";
			// 
			// txtMaxPage
			// 
			this->txtMaxPage->Location = System::Drawing::Point(165, 238);
			this->txtMaxPage->MaxLength = 2;
			this->txtMaxPage->Name = L"txtMaxPage";
			this->txtMaxPage->Size = System::Drawing::Size(32, 21);
			this->txtMaxPage->TabIndex = 26;
			this->txtMaxPage->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &GameSettingForm::txtCursorWidth_KeyPress);
			// 
			// cbChatBoxThumbnail
			// 
			this->cbChatBoxThumbnail->AutoSize = true;
			this->cbChatBoxThumbnail->Location = System::Drawing::Point(10, 355);
			this->cbChatBoxThumbnail->Name = L"cbChatBoxThumbnail";
			this->cbChatBoxThumbnail->Size = System::Drawing::Size(235, 16);
			this->cbChatBoxThumbnail->TabIndex = 27;
			this->cbChatBoxThumbnail->Text = L"세이브 썸네일 화면에 \'대사창\' 표시(&C)";
			this->cbChatBoxThumbnail->UseVisualStyleBackColor = true;
			// 
			// tbWinTransparency
			// 
			this->tbWinTransparency->Location = System::Drawing::Point(288, 176);
			this->tbWinTransparency->Maximum = 255;
			this->tbWinTransparency->Name = L"tbWinTransparency";
			this->tbWinTransparency->Size = System::Drawing::Size(200, 45);
			this->tbWinTransparency->TabIndex = 29;
			this->tbWinTransparency->Value = 128;
			// 
			// label12
			// 
			this->label12->AutoSize = true;
			this->label12->Location = System::Drawing::Point(176, 184);
			this->label12->Name = L"label12";
			this->label12->Size = System::Drawing::Size(105, 12);
			this->label12->TabIndex = 28;
			this->label12->Text = L"    대사창 투명도 :";
			// 
			// GameSettingForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(653, 397);
			this->Controls->Add(this->tbWinTransparency);
			this->Controls->Add(this->label12);
			this->Controls->Add(this->cbChatBoxThumbnail);
			this->Controls->Add(this->txtMaxPage);
			this->Controls->Add(this->label11);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->label9);
			this->Controls->Add(this->txtScreenHeight);
			this->Controls->Add(this->txtScreenWidth);
			this->Controls->Add(this->label8);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->txtFirstFileName);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->txtMadeBy);
			this->Controls->Add(this->label6);
			this->Controls->Add(this->tbVoiceVolume);
			this->Controls->Add(this->tbBGMVolume);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->tbFXVolume);
			this->Controls->Add(this->cbFullScreen);
			this->Controls->Add(this->txtTitle);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->btSave);
			this->Controls->Add(this->btClose);
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"GameSettingForm";
			this->ShowIcon = false;
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
			this->Text = L"게임 설정";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->tbFXVolume))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->tbBGMVolume))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->tbVoiceVolume))->EndInit();
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->groupBox2->ResumeLayout(false);
			this->groupBox2->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->tbWinTransparency))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
		private: System::Void btSave_Click(System::Object^  sender, System::EventArgs^  e)
		{
			nekoGame *game = GetNekoNovel()->GetCurrentGame();

			game->SetName(ConvertToChar(txtTitle->Text));
			game->SetMadeBy(ConvertToChar(txtMadeBy->Text));

			nekoGameEnvironment &env = game->GetEnvironment();
			env.mDefFXVolume = tbFXVolume->Value;
			env.mDefBGMVolume = tbBGMVolume->Value;
			env.mDefVoiceVolume = tbVoiceVolume->Value;
			env.mDefFullScreen = cbFullScreen->Checked;
			env.mTextWindowAlpha = tbWinTransparency->Value;
			env.mFirstScriptName = ConvertToChar(txtFirstFileName->Text);

			if(rbCursor1->Checked)
				env.mCursorType = ECCT_TWINKLE;
			else if(rbCursor2->Checked)
			{
				env.mCursorType = ECCT_ANIMATION;
			}

			try
			{
				env.mCursorAniWidth = int::Parse(txtCursorWidth->Text);
				if(env.mCursorAniWidth <= 0)
					env.mCursorAniWidth = 1;

			} catch(Exception ^)
			{
				env.mCursorAniWidth = 1;
			}

			try
			{
				env.mCursorAniHeight = int::Parse(txtCursorHeight->Text);
				if(env.mCursorAniHeight <= 0)
					env.mCursorAniHeight = 1;

			} catch(Exception ^)
			{
				env.mCursorAniHeight = 1;
			}

			try
			{
				env.mWidth = int::Parse(txtScreenWidth->Text);
				if(env.mWidth <= 0)
					env.mWidth = 800;

			} catch(Exception ^)
			{
				env.mWidth = 800;
			}

			try
			{
				env.mHeight = int::Parse(txtScreenHeight->Text);
				if(env.mHeight <= 0)
					env.mHeight = 600;

			} catch(Exception ^)
			{
				env.mHeight = 600;
			}

			if(extFontsChanged)
			{
				env.mExternalFonts = ConvertToChar(txtExternalFonts->Text);
				MessageBox::Show(this, "외부 폰트 목록은 프로그램을 재실행해야 적용됩니다.", MIMI_TITLE, MessageBoxButtons::OK, MessageBoxIcon::Information);
			}

			try
			{
				env.mMaxPage = int::Parse(txtMaxPage->Text);
				env.mMaxPage = min(max(env.mMaxPage, 1), 20);

			} catch(Exception ^)
			{
				env.mHeight = 6;
			}

			if(cbChatBoxThumbnail->Checked)
			{
				env.mProjectFlags |= EPFT_CHATBOX_IN_THUMBNAIL;
			}
			else
			{
				env.mProjectFlags &= ~EPFT_CHATBOX_IN_THUMBNAIL;
			}

			if(cbNewTypeCursor->Checked)
			{
				env.mProjectFlags |= EPFT_NEWTYPE_CURSOR;
			}
			else
			{
				env.mProjectFlags &= ~EPFT_NEWTYPE_CURSOR;
			}


			this->Close();
		}
		private: System::Void btClose_Click(System::Object^  sender, System::EventArgs^  e)
		{
			this->Close();
		}
private: System::Void radioButton2_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void rbCursor2_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
				txtCursorWidth->ReadOnly = !rbCursor2->Checked;
				txtCursorHeight->ReadOnly = !rbCursor2->Checked;
		 }
private: System::Void txtCursorWidth_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) {
			 if (!(Char::IsDigit(e->KeyChar)) && e->KeyChar != 8)
			 {
				 e->Handled = true;
			 }
		 }
		 private: System::Void txtExternalFonts_TextChanged(System::Object^  sender, System::EventArgs^  e) {
				if(initialized)
				{
					extFontsChanged = true;
				}
		 }
};
}
