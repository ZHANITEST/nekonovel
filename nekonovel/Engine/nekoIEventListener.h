// ===============================================================
//  nekoIEventListener - 2009.7.6
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoIEventListener_h__
#define __nekoIEventListener_h__

#include <list>
#include <algorithm>
using namespace std;
#include "nekoIUnknown.h"

namespace neko
{
	enum E_EVENT_TYPE
	{
		EVENT_HW_RESOURCE_LOADED = 0,
		EVENT_WINDOW_MESSAGE,
		EVENT_LOG,
		EVENT_PUBLISHING_STATUS, // 배포 작업 상황
		EVENT_SELECTION_FINISHED,
		EVENT_ENGINE_FREEZED,
		EVENT_GAME_LOADED,
		EVENT_GAME_SAVED,
		EVENT_WINDOW_SIZE_CHANGED,
		
		EVENT_VARIABLE_CHANGED,

		EVENT_COUNT,
	};

	struct NEKONOVEL_API nekoEvent
	{
		E_EVENT_TYPE type;

		union
		{
			int32 value[4];
			struct  
			{
				UINT uMsg;
				WPARAM wParam;
				LPARAM lParam;
			};
		};

		/// 생성자
		nekoEvent(E_EVENT_TYPE type)
			: type(type)
		{
			
		}

		inline void SetText(int32 n, const char8 *text)
		{
			value[n] = (int32)text;
		}

		inline const char8 *GetText(int32 n) const
		{
			return (const char8 *)(value[n]);
		}
	};

	class NEKONOVEL_API nekoIEventListener : public nekoIUnknown
	{
	protected:

		list<nekoIEventListener *> mListeners;

	public:
		
		/// Constructor
		nekoIEventListener()
		{

		}
		
		/// Destructor
		virtual ~nekoIEventListener()
		{
			mListeners.clear();
		}

		/// 이벤트 도착시
		virtual bool OnEvent(const nekoEvent &event)
		{
			list<nekoIEventListener *>::iterator iter = mListeners.begin();
			for(;iter != mListeners.end();++iter)
			{
				if((*iter)->OnEvent(event))
					return true;
			}

			return false;
		}

		/// 이벤트 리스너 등록
		virtual void AddEventListener(nekoIEventListener *listener)
		{
			mListeners.push_back(listener);
		}

		/// 이벤트 리스너 해제
		virtual bool RemoveEventListener(const nekoIEventListener *listener)
		{
			list<nekoIEventListener *>::iterator iter = find(mListeners.begin(), mListeners.end(), listener);
			if(iter == mListeners.end())
				return false;

			mListeners.erase(iter);
			return true;
		}
	};
}


#endif // __nekoIEventListener_h__