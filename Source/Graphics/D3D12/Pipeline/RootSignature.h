///////////////////////////////////////////////////////////////////////////////////////////////////
//
// RootSignature.h
//
// Class holding a signature describing parameters going into shader
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __ROOT_SIGNATURE__H__
#define __ROOT_SIGNATURE__H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "Core/Memory/Memory.h"
#include <d3d12.h>
#include "../D3DDevice.h"

namespace Engine
{
	class RootSignature
	{
	private:
		ID3DBlob* mSignature;

	public:
		RootSignature(D3DDevice* device)
		{
			D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
			rootSignatureDesc.NumParameters = 0;
			rootSignatureDesc.pParameters = NULL;
			rootSignatureDesc.NumStaticSamplers = 0;
			rootSignatureDesc.pStaticSamplers = NULL;
			rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

			ID3DBlob* error;
			D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &mSignature, &error);
			device->Get()->CreateRootSignature(0, mSignature->GetBufferPointer(), mSignature->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&mSignature);
		}

		~RootSignature()
		{
			mSignature->Release();
		}

		ID3DBlob* Get()
		{
			return mSignature;
		}

		ALIGNED_NEW_DELETE("Engine::Graphics::RootSignature")
	};
}

#endif