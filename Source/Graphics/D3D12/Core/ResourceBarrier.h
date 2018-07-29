///////////////////////////////////////////////////////////////////////////////////////////////////
//
// D3DBarrier.h
//
// 
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __D3D_BARRIER__H__
#define __D3D_BARRIER__H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include <d3d12.h>
#include "Core/Memory/Memory.h"
#include "CommandList.h"
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	/// <summary>
	/// Barrier class
	/// </summary>
	class ResourceBarrier
	{
	private:
		std::vector<D3D12_RESOURCE_BARRIER> mDesc;

	public:
		void AddTransition(ID3D12Resource* resource,
			D3D12_RESOURCE_STATES stateBefore,
			D3D12_RESOURCE_STATES stateAfter,
			unsigned int subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES)
		{
			D3D12_RESOURCE_BARRIER desc = {};
			desc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			desc.Transition.pResource = resource;
			desc.Transition.StateBefore = stateBefore;
			desc.Transition.StateAfter = stateAfter;
			desc.Transition.Subresource = subresource;
			mDesc.push_back(desc);
		}

		void Reverse()
		{
			for (D3D12_RESOURCE_BARRIER& b : mDesc)
			{
				if (b.Type == D3D12_RESOURCE_BARRIER_TYPE_TRANSITION)
				{
					std::swap(b.Transition.StateBefore, b.Transition.StateAfter);
				}
			}
		}

		void Submit(CommandList* list)
		{
			for (unsigned int i = 0; i < mDesc.size(); i++)
			{
				list->Get()->ResourceBarrier(1, &mDesc[i]);
			}
		}

		ALIGNED_NEW_DELETE("Engine::Graphics::D3DBarrier")
	};
}

#endif