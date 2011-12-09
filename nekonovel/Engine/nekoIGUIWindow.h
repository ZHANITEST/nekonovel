// ===============================================================
//  nekoIGUIWindow - 2009.7.6
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoIGUIWindow_h__
#define __nekoIGUIWindow_h__

#include <string>
#include <list>
using namespace std;
#include "nekoDefined.h"
#include "nekoIEventListener.h"

namespace neko
{
	class nekoIGUIWindow : public nekoIEventListener
	{
	protected:

		string mName;
		string mCaption;
		nekoRectangle mBoundingBox;

		/// Children list
		list<nekoIGUIWindow *> mChildrenList;

	public:
		
		/// Constructor
		nekoIGUIWindow()
			: mBoundingBox(0, 0, 128, 128)
		{

		}
		
		/// Destructor
		virtual ~nekoIGUIWindow()
		{
			list<nekoIGUIWindow *>::iterator iter = mChildrenList.begin();
			for(;iter != mChildrenList.end();++iter)
				(*iter)->Release();
		}

		/// Update
		virtual void Update(float32 frameTime)
		{
			list<nekoIGUIWindow *>::iterator iter = mChildrenList.begin();
			for(;iter != mChildrenList.end();++iter)
				(*iter)->Update(frameTime);
		}

		/// Draw
		virtual void Draw()
		{
			list<nekoIGUIWindow *>::iterator iter = mChildrenList.begin();
			for(;iter != mChildrenList.end();++iter)
				(*iter)->Draw();
		}

		/// Set Caption
		virtual void SetCaption(const char8 *caption) { mCaption = caption; }

		/// Add a child
		inline void AddChild(nekoIGUIWindow *wnd)
		{
			nekoAssert(wnd, "");
			mChildrenList.push_back(wnd);
		}

		/// Remove a child
		bool RemoveChild(nekoIGUIWindow *wnd)
		{
			list<nekoIGUIWindow *>::iterator iter = mChildrenList.begin();
			for(;iter != mChildrenList.end();++iter)
			{
				if((*iter) == wnd)
				{
					wnd->Release();
					return true;
				}
			}

			return false;
		}
		
		virtual bool OnEvent(const nekoEvent &event)
		{
			list<nekoIGUIWindow *>::iterator iter = mChildrenList.begin();
			for(;iter != mChildrenList.end();++iter)
			{
				if((*iter)->OnEvent(event))
					return true;
			}

			return false;
		}

		inline nekoRectangle &GetBoundingBox() { return mBoundingBox; }
	};
}


#endif // __nekoIGUIWindow_h__