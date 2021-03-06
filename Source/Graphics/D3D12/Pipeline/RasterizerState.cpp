///////////////////////////////////////////////////////////////////////////////////////////////////
//
// RasterizerState.h
//
// Generic rasterizer state descriptor
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "RasterizerState.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Declaration section

using namespace Engine;

///////////////////////////////////////////////////////////////////////////////////////////////////
// Definition section

D3D12_FILL_MODE RasterizerState::FillModeD3D[2] =
{
	D3D12_FILL_MODE_WIREFRAME,
	D3D12_FILL_MODE_SOLID
};

D3D12_CULL_MODE RasterizerState::CullModeD3D[3] = 
{
	D3D12_CULL_MODE_NONE,
	D3D12_CULL_MODE_FRONT,
	D3D12_CULL_MODE_BACK
};

D3D12_CONSERVATIVE_RASTERIZATION_MODE RasterizerState::ConservativeRasterizationD3D[2] = 
{
	D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF,
	D3D12_CONSERVATIVE_RASTERIZATION_MODE_ON
};