///////////////////////////////////////////////////////////////////////////////////////////////////
//
// EventBridge.h
//
// Following file defines base templated class EventBridge, which connects EventDispatcher to 
// EventHandler
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __EVENT_BRIDGE_H__
#define __EVENT_BRIDGE_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "EventHandler.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	/// <summary>Forward definition of EventDispatcher</summary>
	template <class> class EventDispatcher;

	/// <summary>EventBridge class, connect EventHandler of given type to EventDispatcher of same type</summary>
	template <typename tEventType, class tHandler>
	class EventBridge : public EventHandler<tEventType>
	{
	private:
		friend class EventDispatcher<tEventType>;

		tHandler& mHandler;
	
		/// <summary>Constructor</summary>
		/// <param name="handler">Handler for given event</param>
		EventBridge(tHandler& handler);

		/// <summary>Handle method</summary>
		/// <param name="type">Given event for which handle function is called</param>
		void Handle(const tEventType& type);

	public:
		/// <summary>Comparsion operator</summary>
		/// <param name="handler">Argument to compare against mHandler</param>
		bool operator == (const tHandler& handler) const;
	};

	/// <summary>Constructor</summary>
	/// <param name="handler">Handler for given event</param>
	template <typename tEventType, class tHandler>
	EventBridge<tEventType, tHandler>::EventBridge(tHandler& handler) :
		mHandler(handler)
	{
	}

	/// <summary>Handle method</summary>
	/// <param name="type">Given event for which handle function is called</param>
	template <typename tEventType, class tHandler>
	void EventBridge<tEventType, tHandler>::Handle(const tEventType& object)
	{
		mHandler.Handle(object);
	}

	/// <summary>Comparsion operator</summary>
	/// <param name="handler">Argument to compare against mHandler</param>
	template <typename tEventType, class tHandler>
	bool EventBridge<tEventType, tHandler>::operator == (const tHandler& handler) const
	{
		return ((&mHandler) == (&handler));
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOH

#endif
