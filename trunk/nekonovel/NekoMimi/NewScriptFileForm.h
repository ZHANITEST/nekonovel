#pragma once

using namespace System;
using namespace System::Globalization;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace NekoMimi {

	/// <summary>
	/// NewScriptFileForm에 대한 요약입니다.
	///
	/// 경고: 이 클래스의 이름을 변경하면 이 클래스가 의존하는
	///          모든 .resx 파일과 관련된 관리되는 리소스 컴파일러 도구의
	///          '리소스 파일 이름' 속성도 변경해야 합니다. 그렇지 않은 경우
	///          디자이너는 이 폼과 관련된 지역화된 리소스와
	///          올바르게 상호 작용할 수 없습니다.
	/// </summary>
	public ref class NewScriptFileForm : public System::Windows::Forms::Form
	{
	public:
		NewScriptFileForm(void)
		{
			InitializeComponent();
			
			this->DialogResult = System::Windows::Forms::DialogResult::Cancel;
		}

	protected:
		/// <summary>
		/// 사용 중인 모든 리소스를 정리합니다.
		/// </summary>
		~NewScriptFileForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TextBox^  txtName;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Button^  btAdd;
	private: System::Windows::Forms::Button^  btClose;
	private: System::Windows::Forms::Label^  label2;



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
			this->txtName = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->btAdd = (gcnew System::Windows::Forms::Button());
			this->btClose = (gcnew System::Windows::Forms::Button());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// txtName
			// 
			this->txtName->Location = System::Drawing::Point(80, 27);
			this->txtName->Name = L"txtName";
			this->txtName->Size = System::Drawing::Size(192, 21);
			this->txtName->TabIndex = 0;
			this->txtName->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &NewScriptFileForm::txtName_KeyUp);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(32, 32);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(37, 12);
			this->label1->TabIndex = 1;
			this->label1->Text = L"이름 :";
			// 
			// btAdd
			// 
			this->btAdd->Location = System::Drawing::Point(136, 88);
			this->btAdd->Name = L"btAdd";
			this->btAdd->Size = System::Drawing::Size(88, 24);
			this->btAdd->TabIndex = 2;
			this->btAdd->Text = L"추가(&A)";
			this->btAdd->UseVisualStyleBackColor = true;
			this->btAdd->Click += gcnew System::EventHandler(this, &NewScriptFileForm::btAdd_Click);
			// 
			// btClose
			// 
			this->btClose->Location = System::Drawing::Point(224, 88);
			this->btClose->Name = L"btClose";
			this->btClose->Size = System::Drawing::Size(88, 24);
			this->btClose->TabIndex = 3;
			this->btClose->Text = L"창 닫기(&C)";
			this->btClose->UseVisualStyleBackColor = true;
			this->btClose->Click += gcnew System::EventHandler(this, &NewScriptFileForm::btClose_Click);
			// 
			// label2
			// 
			this->label2->Location = System::Drawing::Point(8, 64);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(304, 16);
			this->label2->TabIndex = 4;
			this->label2->Text = L"※ 참고 : txt 확장자는 자동으로 붙으니 걱정마세요 ^^";
			// 
			// NewScriptFileForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(319, 124);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->btClose);
			this->Controls->Add(this->btAdd);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->txtName);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedToolWindow;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"NewScriptFileForm";
			this->ShowIcon = false;
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
			this->Text = L"새 스크립트 추가";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
		public:

			String ^mFileName;

		private: System::Void btAdd_Click(System::Object^  sender, System::EventArgs^  e)
		{
			if(txtName->Text->Length == 0)
			{
				MessageBox::Show(this, "파일 이름을 입력해주세요!", MIMI_TITLE, MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
				return;
			}

			String ^fileName = txtName->Text;

			if(fileName->Length < 5 || String::Compare(fileName->Substring(fileName->Length - 4, 4), ".txt", StringComparison::CurrentCultureIgnoreCase) != 0)
				fileName += ".txt";

			// 파일 생성하기
			String ^outpath = "스크립트\\" + fileName;

			try
			{
				FileStream ^fileStream = gcnew FileStream(outpath, System::IO::FileMode::Create);

				fileStream->Close();
			} catch(Exception ^)
			{
				MessageBox::Show(this, "'" + outpath + "' 파일을 생성할 수 없어요!", MIMI_TITLE, MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
				return;
			}

			mFileName = fileName;
			this->DialogResult = System::Windows::Forms::DialogResult::OK;
			this->Close();
		}

		private: System::Void btClose_Click(System::Object^  sender, System::EventArgs^  e)
		{
			this->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->Close();
		}
		private: System::Void txtName_KeyUp(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e)
		{
			if(e->KeyCode == Keys::Enter)
			{
				btAdd_Click(sender, e);
			}
		}
};
}
