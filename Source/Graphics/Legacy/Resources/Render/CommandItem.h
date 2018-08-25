///////////////////////////////////////////////////////////////////////////////////////////////////
//
// CommandItem.h
//
// A description of single drawable item that is passed into command queue.
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __COMMAND_ITEM__H__
#define __COMMAND_ITEM__H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "../Material/Material.h"
#include "../Texture/Texture.h"
#include "../Mesh/Mesh.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	/// <summary>
	/// A single item to be rendered by graphics pipeline
	/// </summary>
	class CommandItem
	{
	public:
		enum Command
		{
		};

	protected:
		struct
		{
			uint64_t mCommand;
			void* mArguments;
		};

	public:
		ALIGNED_NEW_DELETE("Engine::Graphics::CommandItem")
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOH

#endif