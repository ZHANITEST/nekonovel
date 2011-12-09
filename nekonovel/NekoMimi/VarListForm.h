#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace NekoMimi {

	/// <summary>
	/// VarListForm에 대한 요약입니다.
	///
	/// 경고: 이 클래스의 이름을 변경하면 이 클래스가 의존하는
	///          모든 .resx 파일과 관련된 관리되는 리소스 컴파일러 도구의
	///          '리소스 파일 이름' 속성도 변경해야 합니다. 그렇지 않은 경우
	///          디자이너는 이 폼과 관련된 지역화된 리소스와
	///          올바르게 상호 작용할 수 없습니다.
	/// </summary>
	public ref class VarListForm : public System::Windows::Forms::Form
	{
	public:
		static VarListForm ^singletonForm = nullptr;

		VarListForm(void)
		{
			InitializeComponent();
			//
			//TODO: 생성자 코드를 여기에 추가합니다.
			//
		}

	protected:
		/// <summary>
		/// 사용 중인 모든 리소스를 정리합니다.
		/// </summary>
		~VarListForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::ListView^  lstVariables;
	protected: 

	protected: 
	private: System::Windows::Forms::ColumnHeader^  columnHeader1;
	private: System::Windows::Forms::ColumnHeader^  columnHeader2;
	private: System::Windows::Forms::ColumnHeader^  columnHeader3;

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
			this->lstVariables = (gcnew System::Windows::Forms::ListView());
			this->columnHeader1 = (gcnew System::Windows::Forms::ColumnHeader());
			this->columnHeader2 = (gcnew System::Windows::Forms::ColumnHeader());
			this->columnHeader3 = (gcnew System::Windows::Forms::ColumnHeader());
			this->SuspendLayout();
			// 
			// lstVariables
			// 
			this->lstVariables->Columns->AddRange(gcnew cli::array< System::Windows::Forms::ColumnHeader^  >(3) {this->columnHeader1, 
				this->columnHeader2, this->columnHeader3});
			this->lstVariables->Dock = System::Windows::Forms::DockStyle::Fill;
			this->lstVariables->Location = System::Drawing::Point(0, 0);
			this->lstVariables->Name = L"lstVariables";
			this->lstVariables->Size = System::Drawing::Size(378, 188);
			this->lstVariables->TabIndex = 0;
			this->lstVariables->UseCompatibleStateImageBehavior = false;
			this->lstVariables->View = System::Windows::Forms::View::Details;
			// 
			// columnHeader1
			// 
			this->columnHeader1->Text = L"변수 이름";
			this->columnHeader1->Width = 104;
			// 
			// columnHeader2
			// 
			this->columnHeader2->Text = L"타입";
			this->columnHeader2->Width = 67;
			// 
			// columnHeader3
			// 
			this->columnHeader3->Text = L"내용";
			this->columnHeader3->Width = 201;
			// 
			// VarListForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(378, 188);
			this->Controls->Add(this->lstVariables);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::SizableToolWindow;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"VarListForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"사용중인 변수 리스트";
			this->Load += gcnew System::EventHandler(this, &VarListForm::VarListForm_Load);
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &VarListForm::VarListForm_FormClosing);
			this->ResumeLayout(false);

		}
#pragma endregion

	private: System::Void VarListForm_Load(System::Object^  sender, System::EventArgs^  e);
	private: System::Void VarListForm_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e);

	public:
		void OnVariableChanged(const nekoEvent &event);
		void UpdateAllVariables();
};
}
