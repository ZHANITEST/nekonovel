using namespace System;
using namespace System::Collections;
using namespace System::ComponentModel;
using namespace System::Drawing;
using namespace System::Data;
using namespace System::Windows::Forms;

namespace NekoMimi
{
	/// <summary>
	/// Summary description for DraggableTabPage.
	/// </summary>
	public ref class DraggableTabControl : System::Windows::Forms::TabControl
	{
		/*
		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System::ComponentModel.Container components = null;
		*/

		public:

			bool IsUpdating;

		private:
			int32 mPreviousOne;

		public:
		DraggableTabControl()
			: mPreviousOne(-1)
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();
			AllowDrop = true;
			// TODO: Add any initialization after the InitForm call

		}

		/// <summary>
		/// 사용 중인 모든 리소스를 정리합니다.
		/// </summary>
		~DraggableTabControl()
		{
			/*
			if (components)
			{
				delete components;
			}
			*/
		}

#pragma region Windows Form Designer generated code
		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private:
		void InitializeComponent()
		{

		}
#pragma endregion

		/*

		protected:
		virtual void OnDragOver(System::Windows::Forms::DragEventArgs ^e) override
		{
			TabControl::OnDragOver(e);

			Point pt;
			//We need client coordinates.
			pt = PointToClient(Point(e->X, e->Y));

			//Get the tab we are hovering over.
			TabPage ^hover_tab = GetTabPageByTab(pt);

			//Make sure we are on a tab.
			if(hover_tab != nullptr)
			{
				//Make sure there is a TabPage being dragged.
				if(e->Data->GetDataPresent(hover_tab->GetType()))
				{
					e->Effect = DragDropEffects::All;
					TabPage ^drag_tab = (TabPage ^)e->Data->GetData(hover_tab->GetType());

					int item_drag_index = FindIndex(drag_tab);
					int drop_location_index = FindIndex(hover_tab);

					//Don't do anything if we are hovering over ourself.
					if(item_drag_index != drop_location_index)
					{
						if(drop_location_index < item_drag_index)
							mPreviousOne = this->SelectedIndex;
						else
							mPreviousOne = this->SelectedIndex;

						ArrayList ^pages = gcnew ArrayList();

						IsUpdating = true;

						TabPages->Insert(drop_location_index, drag_tab);
						TabPages->Remove(drag_tab);

						IsUpdating = false;
						SelectedTab = drag_tab;
					}
				}
			}
			else
			{
				e->Effect = DragDropEffects::All;
			}
		}

		protected:
		virtual void OnMouseDown(MouseEventArgs ^e) override
		{
			TabControl::OnMouseDown(e);

			Point ^pt = gcnew Point(e->X, e->Y);
			TabPage ^tp = GetTabPageByTab(pt);

			if(tp != nullptr)
			{
				mPreviousOne = -1;
				DoDragDrop(tp, DragDropEffects::All);
			}
		}

		/// <summary>
		/// Finds the TabPage whose tab is contains the given point.
		/// </summary>
		/// <param name="pt">The point (given in client coordinates) to look for a TabPage.</param>
		/// <returns>The TabPage whose tab is at the given point (null if there isn't one).</returns>
		private:
		TabPage ^GetTabPageByTab(Point ^pt)
		{
			TabPage ^tp = nullptr;

			System::Drawing::Rectangle rect;
			for(int i = 0; i < TabPages->Count; i++)
			{
				rect = GetTabRect(i);

				if(mPreviousOne == i)
				{
					if(i > this->SelectedIndex)
					{
						rect = System::Drawing::Rectangle(rect.Left + rect.Width / 2, rect.Top, rect.Width / 2, rect.Height);
					}
					else if(i < this->SelectedIndex)
						rect = System::Drawing::Rectangle(rect.Left, rect.Top, rect.Width / 2, rect.Height);
				}

				if(rect.Contains(Point(pt->X, pt->Y)))
				{
					tp = TabPages[i];
					break;
				}
			}

			return tp;
		}

		/// <summary>
		/// Loops over all the TabPages to find the index of the given TabPage.
		/// </summary>
		/// <param name="page">The TabPage we want the index for.</param>
		/// <returns>The index of the given TabPage(-1 if it isn't found.)</returns>
		private:
		int FindIndex(TabPage ^page)
		{
			for(int i = 0; i < TabPages->Count; i++)
			{
				if(TabPages[i] == page)
					return i;
			}

			return -1;
		}

		*/
	};
}
