///////////////////////////////////////////////////////////////////////////////////////////////////
//
// EventChannel.h
//
// Following file defines class through which we can broadcast events and register/unregister
// their handlers
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __EVENT_CHANNEL_H__
#define __EVENT_CHANNEL_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "EventDispatcher.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	/// <summary>EventChannel class that allows us to register handlers for events and unregister them</summary>
	class EventChannel
	{
	public:
		/// <summary>Register handler into dispatcher for event type tHandler</summary>
		/// <param name="handler">Handler to add</param>
		template <typename tEvent, class tHandler>
		void Add(tHandler& handler)
		{
			EventDispatcher<tEvent>::Instance().Add<tHandler>(handler);
		}

		/// <summary>Unregister handler</summary>
		/// <param name="handler">Handler to remove</param>
		template <typename tEvent, class tHandler>
		void Remove(const tHandler& handler)
		{
			EventDispatcher<tEvent>::Instance().Remove<tHandler>(handler);
		}

		/// <summary>Broadcast event type tEvent through dispatcher</summary>
		/// <param name="object">Object to broadcast</param>
		template <typename tEvent>
		void Broadcast(const tEvent& object)
		{	
			EventDispatcher<tEvent>::Instance().Broadcast(object);
		}
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOH

#endif
