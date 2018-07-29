///////////////////////////////////////////////////////////////////////////////////////////////////
//
// EventDispatcher.h
//
// Following class allows us to dispatch event structures to handler objects (and handle them there)
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __EVENT_DISPATCHER_H__
#define __EVENT_DISPATCHER_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include <list>
#include "EventBridge.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	/// <summary>Forward declaration of EventChannel</summary>
	class EventChannel;

	template <typename tEvent>
	class EventDispatcher
	{
	private:
		friend class EventChannel;

		// List of handlers
		std::list<EventHandler<tEvent>*> mHandlerList;
	
		/// <summary>Destructor, removes all handlers</summary>
		~EventDispatcher()
		{
			for(auto it = mHandlerList.begin(); it != mHandlerList.end(); it++)
			{
				delete (*it);
			}
		}

		/// <summary>Singleton pattern to get instance of single dispatcher</summary>
		static EventDispatcher& Instance()
		{
			static EventDispatcher mInstance;
			return mInstance;
		}

		/// <summary>Register handler into event dispatcher</summary>
		/// <param name="handler">Handler to add</param>
		template <class tHandler>
		void Add(tHandler& handler)
		{
			mHandlerList.push_back(new EventBridge<tEvent, tHandler>(handler));
		}

		/// <summary>Broadcast event object to all registered handlers</summary>
		/// <param name="object">Object to broadcast</param>
		void Broadcast(const tEvent& object)
		{
			typename std::list<EventHandler<tEvent>*>::const_iterator next;
		
			if (mHandlerList.size() > 0)
			{
				for (auto it = mHandlerList.begin(); it != mHandlerList.end(); it++)
				{
					next = it;
					(*next)->Handle(object);
				}
			}
		}
	
		/// <summary>Helper class for handler removal</summary>
		template<class tHandler>
		class PointsToSame
		{
		public:
			const tHandler& mHandler;
		
			PointsToSame(const tHandler& h) :
				mHandler(h)
			{
			}
		
			bool operator()(EventHandler<tEvent>* ptr)
			{
				return ((*(EventBridge<tEvent, tHandler>*)(ptr)) == mHandler);
			}
		};

		/// <summary>Removes handler</summary>
		/// <param name="handler">Object to remove</param>
		template <class tHandler>
		void Remove(const tHandler& handler)
		{
			PointsToSame<tHandler> pts(handler);
			mHandlerList.remove_if(pts);
		}
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOH

#endif
