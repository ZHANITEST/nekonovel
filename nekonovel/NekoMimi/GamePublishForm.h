#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace NekoMimi {

	/// <summary>
	/// GamePublishForm에 대한 요약입니다.
	///
	/// 경고: 이 클래스의 이름을 변경하면 이 클래스가 의존하는
	///          모든 .resx 파일과 관련된 관리되는 리소스 컴파일러 도구의
	///          '리소스 파일 이름' 속성도 변경해야 합니다. 그렇지 않은 경우
	///          디자이너는 이 폼과 관련된 지역화된 리소스와
	///          올바르게 상호 작용할 수 없습니다.
	/// </summary>
	public ref class GamePublishForm : public System::Windows::Forms::Form
	{
	public:
		bool mPublishing;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::CheckBox^  cbAutoUpdate;


	public: 
		static GamePublishForm ^self;

	public:
		GamePublishForm(void)
			: mPublishing(false)
		{
			InitializeComponent();

			GamePublishForm::self = this;

			nekoGame *game = GetNekoNovel()->GetCurrentGame();
			nekoGameEnvironment &env = game->GetEnvironment();

			txtLastPublished->Text = ConvertToString(env.mLastPublishFolder.c_str());
			txtVersion->Text = String::Format("{0}", env.mLastPublishVersion);

			cbCompressBGM->Checked = env.mCompressBGM;
		}

	protected:
		/// <summary>
		/// 사용 중인 모든 리소스를 정리합니다.
		/// </summary>
		~GamePublishForm()
		{
			delete self;
			if (components)
			{
				delete components;
			}
		}

	private: System::Windows::Forms::Button^  btPublish;
	private: System::Windows::Forms::Button^  btClose;
	protected: 


	private: System::Windows::Forms::TextBox^  txtLastPublished;

	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Button^  btBrowse;

	private: System::Windows::Forms::CheckBox^  cbCompressBGM;
	private: System::Windows::Forms::ProgressBar^  pbar;


	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::TextBox^  txtVersion;

	private: System::Windows::Forms::Label^  label3;


	protected: 

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
			this->btPublish = (gcnew System::Windows::Forms::Button());
			this->btClose = (gcnew System::Windows::Forms::Button());
			this->txtLastPublished = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->btBrowse = (gcnew System::Windows::Forms::Button());
			this->cbCompressBGM = (gcnew System::Windows::Forms::CheckBox());
			this->pbar = (gcnew System::Windows::Forms::ProgressBar());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->txtVersion = (gcnew System::Windows::Forms::TextBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->cbAutoUpdate = (gcnew System::Windows::Forms::CheckBox());
			this->groupBox1->SuspendLayout();
			this->SuspendLayout();
			// 
			// btPublish
			// 
			this->btPublish->Location = System::Drawing::Point(264, 216);
			this->btPublish->Name = L"btPublish";
			this->btPublish->Size = System::Drawing::Size(136, 32);
			this->btPublish->TabIndex = 0;
			this->btPublish->Text = L"배포 시작하기(&S)";
			this->btPublish->UseVisualStyleBackColor = true;
			this->btPublish->Click += gcnew System::EventHandler(this, &GamePublishForm::btPublish_Click);
			// 
			// btClose
			// 
			this->btClose->Location = System::Drawing::Point(400, 216);
			this->btClose->Name = L"btClose";
			this->btClose->Size = System::Drawing::Size(136, 32);
			this->btClose->TabIndex = 1;
			this->btClose->Text = L"닫기(&C)";
			this->btClose->UseVisualStyleBackColor = true;
			this->btClose->Click += gcnew System::EventHandler(this, &GamePublishForm::btClose_Click);
			// 
			// txtLastPublished
			// 
			this->txtLastPublished->Location = System::Drawing::Point(146, 18);
			this->txtLastPublished->Name = L"txtLastPublished";
			this->txtLastPublished->Size = System::Drawing::Size(310, 21);
			this->txtLastPublished->TabIndex = 2;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(8, 22);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(137, 12);
			this->label1->TabIndex = 3;
			this->label1->Text = L"배포 게임을 만들 폴더 : ";
			// 
			// btBrowse
			// 
			this->btBrowse->Location = System::Drawing::Point(456, 16);
			this->btBrowse->Name = L"btBrowse";
			this->btBrowse->Size = System::Drawing::Size(80, 24);
			this->btBrowse->TabIndex = 4;
			this->btBrowse->Text = L"찾아보기(&B)";
			this->btBrowse->UseVisualStyleBackColor = true;
			this->btBrowse->Click += gcnew System::EventHandler(this, &GamePublishForm::btBrowse_Click);
			// 
			// cbCompressBGM
			// 
			this->cbCompressBGM->AutoSize = true;
			this->cbCompressBGM->Location = System::Drawing::Point(160, 56);
			this->cbCompressBGM->Name = L"cbCompressBGM";
			this->cbCompressBGM->Size = System::Drawing::Size(144, 16);
			this->cbCompressBGM->TabIndex = 5;
			this->cbCompressBGM->Text = L"배경음악 파일 압축(&1)";
			this->cbCompressBGM->UseVisualStyleBackColor = true;
			// 
			// pbar
			// 
			this->pbar->Location = System::Drawing::Point(8, 192);
			this->pbar->Name = L"pbar";
			this->pbar->Size = System::Drawing::Size(528, 16);
			this->pbar->TabIndex = 6;
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(8, 56);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(65, 12);
			this->label2->TabIndex = 7;
			this->label2->Text = L"배포 버전 :";
			// 
			// txtVersion
			// 
			this->txtVersion->Location = System::Drawing::Point(80, 51);
			this->txtVersion->MaxLength = 5;
			this->txtVersion->Name = L"txtVersion";
			this->txtVersion->Size = System::Drawing::Size(72, 21);
			this->txtVersion->TabIndex = 8;
			this->txtVersion->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &GamePublishForm::txtVersion_KeyPress);
			// 
			// label3
			// 
			this->label3->Location = System::Drawing::Point(8, 88);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(528, 24);
			this->label3->TabIndex = 9;
			this->label3->Text = L"※ 참고 : 배포를 시작하시기 전에 배포판을 만들 폴더를 비워주시는 것을 강력 추천합니다!!";
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->textBox1);
			this->groupBox1->Controls->Add(this->label4);
			this->groupBox1->Controls->Add(this->cbAutoUpdate);
			this->groupBox1->Location = System::Drawing::Point(8, 120);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(528, 64);
			this->groupBox1->TabIndex = 10;
			this->groupBox1->TabStop = false;
			// 
			// textBox1
			// 
			this->textBox1->Enabled = false;
			this->textBox1->Location = System::Drawing::Point(152, 19);
			this->textBox1->Name = L"textBox1";
			this->textBox1->Size = System::Drawing::Size(352, 21);
			this->textBox1->TabIndex = 3;
			this->textBox1->Text = L"http://localhost/patch/";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Enabled = false;
			this->label4->Location = System::Drawing::Point(8, 24);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(139, 12);
			this->label4->TabIndex = 1;
			this->label4->Text = L"version.ini 파일의 주소 :";
			// 
			// cbAutoUpdate
			// 
			this->cbAutoUpdate->AutoSize = true;
			this->cbAutoUpdate->Enabled = false;
			this->cbAutoUpdate->Location = System::Drawing::Point(0, 0);
			this->cbAutoUpdate->Name = L"cbAutoUpdate";
			this->cbAutoUpdate->Size = System::Drawing::Size(128, 16);
			this->cbAutoUpdate->TabIndex = 0;
			this->cbAutoUpdate->Text = L"자동 업데이트 기능";
			this->cbAutoUpdate->UseVisualStyleBackColor = true;
			// 
			// GamePublishForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(543, 259);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->txtVersion);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->pbar);
			this->Controls->Add(this->cbCompressBGM);
			this->Controls->Add(this->btBrowse);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->txtLastPublished);
			this->Controls->Add(this->btClose);
			this->Controls->Add(this->btPublish);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"GamePublishForm";
			this->ShowIcon = false;
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
			this->Text = L"게임을 배포해요!";
			this->Load += gcnew System::EventHandler(this, &GamePublishForm::GamePublishForm_Load);
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &GamePublishForm::GamePublishForm_FormClosing);
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
		private: System::Void GamePublishForm_Load(System::Object^  sender, System::EventArgs^  e)
		{


		}
		private: System::Void btClose_Click(System::Object^  sender, System::EventArgs^  e)
		{
			nekoGame *game = GetNekoNovel()->GetCurrentGame();
			nekoGameEnvironment &env = game->GetEnvironment();

			env.mLastPublishFolder = ConvertToChar(txtLastPublished->Text);
			env.mLastPublishVersion = Int32::Parse(txtVersion->Text);
			env.mCompressBGM = cbCompressBGM->Checked;

			this->Close();
		}
		private: System::Void btPublish_Click(System::Object^  sender, System::EventArgs^  e)
		{
			/// 일단 설정 저장..
			nekoGame *game = GetNekoNovel()->GetCurrentGame();
			nekoGameEnvironment &env = game->GetEnvironment();

			if(!mPublishing)
			{
				env.mLastPublishFolder = ConvertToChar(txtLastPublished->Text);
				env.mLastPublishVersion = Int32::Parse(txtVersion->Text);
				env.mCompressBGM = cbCompressBGM->Checked;

				if(FAILED(game->Build()))
				{
					MessageBox::Show(this, "Game.nkpublished 파일 작성에 오류가 생겼습니다.",
						MIMI_TITLE, MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
					return;
				}

				if(FAILED(game->StartPublishing()))
				{
					MessageBox::Show(this, "배포 작업을 시작할 수 없습니다!",
						MIMI_TITLE, MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
					return;
				}

				mPublishing = true;
				btPublish->Text = "작업 중단하기(&X)";
				btClose->Enabled = false;
			}
			else
			{
				game->StopPublishing();
			}
		}
		private: System::Void btBrowse_Click(System::Object^  sender, System::EventArgs^  e)
		{
			FolderBrowserDialog ^dlg = gcnew FolderBrowserDialog();

			String ^mRunPath = Environment::CurrentDirectory;
			dlg->SelectedPath = mRunPath;
			if(dlg->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			{
				txtLastPublished->Text = dlg->SelectedPath;
			}
			Directory::SetCurrentDirectory(mRunPath);
		}
		private: System::Void txtVersion_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e)
		{
			if (!(Char::IsDigit(e->KeyChar) || e->KeyChar == Convert::ToChar(Keys::Back)))
			{
				e->Handled = true;
			}
		}

		private:
			delegate void TestDelegate();
			int32 mLastStatusValue;

		void OnStatusUpdatedReal()
		{
			// 작업 종료
			if(mLastStatusValue < 0)
			{
				self->btClose->Enabled = true;
				self->mPublishing = false;
				self->btPublish->Text = "배포 시작하기(&S)";
				self->pbar->Value = 0;

				if(mLastStatusValue == -2)
				{
					MessageBox::Show(this, "배포 작업을 실행하는 도중 오류가 발생하였습니다.",
						MIMI_TITLE, MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
					this->Close();
				}
				else if(mLastStatusValue == -1)
				{
					MessageBox::Show(this, "성공적으로 배포 작업을 마쳤습니다!",
						MIMI_TITLE, MessageBoxButtons::OK, MessageBoxIcon::Information);
					this->Close();
				}
				else if(mLastStatusValue == -3)
				{
					MessageBox::Show(this, "배포 작업을 중단했습니다!",
						MIMI_TITLE, MessageBoxButtons::OK, MessageBoxIcon::Asterisk);
					this->Close();
				}
				return;
			}

			self->pbar->Value = mLastStatusValue;
		}

		public:
		static void OnStatusUpdated(const nekoEvent &e)
		{
			try
			{
				self->mLastStatusValue = e.value[0];

				TestDelegate ^realPut = gcnew TestDelegate(self , &GamePublishForm::OnStatusUpdatedReal);
				//self->Invoke(realPut, gcnew array<Object ^>{str});
				self->Invoke(realPut);
			} catch(Exception ^)
			{

			}
		}
		private: System::Void GamePublishForm_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e)
		{
			if(mPublishing)
				e->Cancel = true;
		}
};
}
