#include "Voxelize.h"

using namespace SkyeCuillin;

Voxelize::Voxelize(Engine::D3DRenderer* renderer, LightingSystem* lightingSystem, unsigned int resolution, MipmapCalculation method)
{
	mLightingSystem = lightingSystem;

	mVoxelizeRS = new Engine::RootSignature(renderer->GetDevice(), 9, 1);
	(*mVoxelizeRS)[0].InitAsConstantBuffer(0);
	(*mVoxelizeRS)[1].InitAsDescriptorTable(1);
	(*mVoxelizeRS)[1].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);
	(*mVoxelizeRS)[2].InitAsDescriptorTable(1);
	(*mVoxelizeRS)[2].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 1);
	(*mVoxelizeRS)[3].InitAsConstantBuffer(2);
	(*mVoxelizeRS)[4].InitAsConstants(1, 2);
	(*mVoxelizeRS)[5].InitAsDescriptorTable(1);
	(*mVoxelizeRS)[5].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
	(*mVoxelizeRS)[6].InitAsDescriptorTable(1);
	(*mVoxelizeRS)[6].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 1);
	(*mVoxelizeRS)[7].InitAsDescriptorTable(1);
	(*mVoxelizeRS)[7].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 1);
	(*mVoxelizeRS)[8].InitAsDescriptorTable(1);
	(*mVoxelizeRS)[8].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 2, 1);
	mVoxelizeRS->InitStaticSampler(0, Engine::Sampler::CreateSamplerD3D(Engine::Sampler::Filter::ANISOTROPIC,
		Engine::Sampler::Address::WRAP,
		Engine::Sampler::Address::WRAP,
		Engine::Sampler::Address::WRAP,
		0.0f,
		16,
		Engine::Sampler::ComparisonFunc::NEVER,
		1.0f, 1.0f, 1.0f, 1.0f,
		0.0f,
		32.0f));
	mVoxelizeRS->Finalize(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	mVoxelize = new Engine::D3DShader("../Data/SkyeCuillin/Voxelize.hlsl",
		std::vector<Engine::D3DShader::ShaderEntryPoint>
	{
		Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::VERTEX_SHADER, "VS"),
			Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::GEOMETRY_SHADER, "GS"),
			Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::PIXEL_SHADER, "PS")
	});	// Flags??? D3DCOMPILE_ALL_RESOURCES_BOUND | D3DCOMPILE_ENABLE_STRICTNESS)

	Engine::InputLayout inputLayout = Engine::InputLayout();
	inputLayout.AddAttribute(Engine::InputLayout::Attribute("POSITION", 0, Engine::Graphics::Format::RGB32F, 0, 0, Engine::InputLayout::Classification::PER_VERTEX, 0));
	inputLayout.AddAttribute(Engine::InputLayout::Attribute("NORMAL", 0, Engine::Graphics::Format::RGB32F, 0, 12, Engine::InputLayout::Classification::PER_VERTEX, 0));
	inputLayout.AddAttribute(Engine::InputLayout::Attribute("TEXCOORD", 2, Engine::Graphics::Format::RGB32F, 0, 24, Engine::InputLayout::Classification::PER_VERTEX, 0));
	inputLayout.AddAttribute(Engine::InputLayout::Attribute("TEXCOORD", 3, Engine::Graphics::Format::RGB32F, 0, 36, Engine::InputLayout::Classification::PER_VERTEX, 0));
	inputLayout.AddAttribute(Engine::InputLayout::Attribute("TEXCOORD", 0, Engine::Graphics::Format::RG32F, 0, 48, Engine::InputLayout::Classification::PER_VERTEX, 0));
	inputLayout.AddAttribute(Engine::InputLayout::Attribute("TEXCOORD", 1, Engine::Graphics::Format::RG32F, 0, 56, Engine::InputLayout::Classification::PER_VERTEX, 0));
	
	Engine::DepthStencilState dsState = Engine::DepthStencilState(false);
	
	std::vector<Engine::Graphics::Format> rtvFormats;
	rtvFormats.push_back(Engine::Graphics::Format::RGBA8);
	//rtvFormats.push_back(Engine::Graphics::Format::RGBA16F);
	
	mVoxelizePS = new Engine::PipelineState(renderer->GetDevice(),
		mVoxelizeRS,
		mVoxelize,
		Engine::BlendState::CreateStateD3D(),
		Engine::RasterizerState::CreateStateD3D(Engine::RasterizerState::SOLID, Engine::RasterizerState::NONE, true, 0, 0.0f, 0.0f, true, false, false, 0, Engine::RasterizerState::OFF),
		&dsState,
		&inputLayout,
		Engine::PipelineState::PrimitiveType::TRIANGLE,
		rtvFormats,
		Engine::Graphics::Format::D32F,
		1, 0);

	mClearRS = new Engine::RootSignature(renderer->GetDevice(), 2, 0);
	(*mClearRS)[0].InitAsConstants(0, 3);
	(*mClearRS)[1].InitAsDescriptorTable(1);
	(*mClearRS)[1].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0, 1);
	mClearRS->Finalize(D3D12_ROOT_SIGNATURE_FLAG_NONE);

	mClear = new Engine::D3DShader("../Data/SkyeCuillin/VoxelClear.hlsl",
		std::vector<Engine::D3DShader::ShaderEntryPoint>
	{
		Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::COMPUTE_SHADER, "CS")
	});

	mClearPS = new Engine::PipelineState(renderer->GetDevice(),
		mClearRS,
		mClear);

	mMipMethod = method;
	switch (mMipMethod)
	{
	case MipmapCalculation::MIPMAP_1:
		mMipmapRS = new Engine::RootSignature(renderer->GetDevice(), 3, 1);
		(*mMipmapRS)[0].InitAsConstants(1, 3);
		(*mMipmapRS)[1].InitAsDescriptorTable(1);
		(*mMipmapRS)[1].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);
		(*mMipmapRS)[2].InitAsDescriptorTable(1);
		(*mMipmapRS)[2].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0, 1);
		mMipmapRS->InitStaticSampler(0, Engine::Sampler::CreateSamplerD3D(Engine::Sampler::Filter::MIN_LINEAR_MAG_LINEAR_MIP_LINEAR,
			Engine::Sampler::Address::CLAMP,
			Engine::Sampler::Address::CLAMP,
			Engine::Sampler::Address::CLAMP,
			0.0f,
			1,
			Engine::Sampler::ComparisonFunc::NEVER,
			1.0f, 1.0f, 1.0f, 1.0f,
			0.0f,
			32.0f));
		mMipmapRS->Finalize(D3D12_ROOT_SIGNATURE_FLAG_NONE);

		mMipmap = new Engine::D3DShader("../Data/SkyeCuillin/VoxelMipmap.hlsl",
			std::vector<Engine::D3DShader::ShaderEntryPoint>
		{
			Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::COMPUTE_SHADER, "GenerateMipmaps")
		});
		break;

	case MipmapCalculation::MIPMAP_2:
		mMipmapRS = new Engine::RootSignature(renderer->GetDevice(), 4, 1);
		(*mMipmapRS)[0].InitAsConstants(1, 3);
		(*mMipmapRS)[1].InitAsDescriptorTable(1);
		(*mMipmapRS)[1].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);
		(*mMipmapRS)[2].InitAsDescriptorTable(1);
		(*mMipmapRS)[2].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0, 1);
		(*mMipmapRS)[3].InitAsDescriptorTable(1);
		(*mMipmapRS)[3].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 1);
		mMipmapRS->InitStaticSampler(0, Engine::Sampler::CreateSamplerD3D(Engine::Sampler::Filter::MIN_LINEAR_MAG_LINEAR_MIP_LINEAR,
			Engine::Sampler::Address::CLAMP,
			Engine::Sampler::Address::CLAMP,
			Engine::Sampler::Address::CLAMP,
			0.0f,
			1,
			Engine::Sampler::ComparisonFunc::NEVER,
			1.0f, 1.0f, 1.0f, 1.0f,
			0.0f,
			32.0f));
		mMipmapRS->Finalize(D3D12_ROOT_SIGNATURE_FLAG_NONE);

		mMipmap = new Engine::D3DShader("../Data/SkyeCuillin/VoxelMipmap2.hlsl",
			std::vector<Engine::D3DShader::ShaderEntryPoint>
		{
			Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::COMPUTE_SHADER, "GenerateMipmaps")
		});
		break;

	case MipmapCalculation::MIPMAP_3:
		mMipmapRS = new Engine::RootSignature(renderer->GetDevice(), 5, 1);
		(*mMipmapRS)[0].InitAsConstants(1, 3);
		(*mMipmapRS)[1].InitAsDescriptorTable(1);
		(*mMipmapRS)[1].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);
		(*mMipmapRS)[2].InitAsDescriptorTable(1);
		(*mMipmapRS)[2].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0, 1);
		(*mMipmapRS)[3].InitAsDescriptorTable(1);
		(*mMipmapRS)[3].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 1);
		(*mMipmapRS)[4].InitAsDescriptorTable(1);
		(*mMipmapRS)[4].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 2, 1);
		mMipmapRS->InitStaticSampler(0, Engine::Sampler::CreateSamplerD3D(Engine::Sampler::Filter::MIN_LINEAR_MAG_LINEAR_MIP_LINEAR,
			Engine::Sampler::Address::CLAMP,
			Engine::Sampler::Address::CLAMP,
			Engine::Sampler::Address::CLAMP,
			0.0f,
			1,
			Engine::Sampler::ComparisonFunc::NEVER,
			1.0f, 1.0f, 1.0f, 1.0f,
			0.0f,
			32.0f));
		mMipmapRS->Finalize(D3D12_ROOT_SIGNATURE_FLAG_NONE);

		mMipmap = new Engine::D3DShader("../Data/SkyeCuillin/VoxelMipmap3.hlsl",
			std::vector<Engine::D3DShader::ShaderEntryPoint>
		{
			Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::COMPUTE_SHADER, "GenerateMipmaps")
		});
		break;
	}

	mMipmapPS = new Engine::PipelineState(renderer->GetDevice(),
		mMipmapRS,
		mMipmap);

	mVoxelizeParams = new Engine::GpuMappedBuffer();
	mVoxelizeParams->Init(renderer, 64, sizeof(float));
	mVoxelizeParamsPtr = mVoxelizeParams->Map();

	Engine::mat4 ortho = Engine::OrthoLH(2.0f, 2.0f, -1.0f, 1.0f);
	mProjections = new Engine::mat4[3];
	mProjections[0] = Engine::LookAtLH(Engine::float4(0.0f, 0.0f, 0.0f, 1.0f), Engine::float4(1.0f, 0.0f, 0.0f, 1.0f), Engine::float4(0.0f, 1.0f, 0.0f, 0.0f)) * ortho;
	mProjections[1] = Engine::LookAtLH(Engine::float4(0.0f, 0.0f, 0.0f, 1.0f), Engine::float4(0.0f, 1.0f, 0.0f, 1.0f), Engine::float4(0.0f, 0.0f, 1.0f, 0.0f)) * ortho;
	mProjections[2] = Engine::LookAtLH(Engine::float4(0.0f, 0.0f, 0.0f, 1.0f), Engine::float4(0.0f, 0.0f, 1.0f, 1.0f), Engine::float4(0.0f, 1.0f, 0.0f, 0.0f)) * ortho;
	mScale = new Engine::mat4();

	mDimensions = resolution;

	mColorTexture[0] = new Engine::Texture();
	mColorTexture[1] = new Engine::Texture();

	mMiplevels = Engine::Math::log2(mDimensions); // 1 + LOG2(mDimensionVoxel)

	//mColorTexture[0]->InitMipmaps(renderer, mDimensions, mDimensions, mDimensions, mMiplevels, Engine::Graphics::Format::RGBA8, nullptr, nullptr, true);
	//mColorTexture[1]->InitMipmaps(renderer, mDimensions, mDimensions, mDimensions, mMiplevels, Engine::Graphics::Format::RGBA8, nullptr, nullptr, true);
	mColorTexture[0]->InitMipmaps(renderer, mDimensions, mDimensions, mDimensions, mMiplevels, Engine::Graphics::Format::RGBA16F, nullptr, nullptr, true);
	mColorTexture[1]->InitMipmaps(renderer, mDimensions, mDimensions, mDimensions, mMiplevels, Engine::Graphics::Format::RGBA16F, nullptr, nullptr, true);
}

Voxelize::~Voxelize()
{
	delete mColorTexture[0];
	delete mColorTexture[1];

	delete mScale;
	delete[] mProjections;

	delete mVoxelizeParams;
	
	delete mMipmapPS;
	delete mMipmap;
	delete mMipmapRS;

	delete mClearPS;
	delete mClear;
	delete mClearRS;
	
	delete mVoxelizePS;
	delete mVoxelize;
	delete mVoxelizeRS;
}

void Voxelize::Clear(Engine::DescriptorHeap* heap, Engine::ComputeContext* context)
{
	mInput = (mInput + 1) & 0x1;
	mOutput = (mOutput + 1) & 0x1;

	context->SetPipelineState(mClearPS);
	context->SetRootSignature(mClearRS);
	context->SetDescriptorHeap(Engine::DescriptorHeap::CBV_SRV_UAV, heap);
	context->SetConstants(0, Engine::DWParam(0.001f), Engine::DWParam(0.0f), Engine::DWParam(0.0f));
	context->SetDescriptorTable(1, mColorTexture[mOutput]->GetUAV());
	context->Dispatch(mDimensions / 8, mDimensions / 8, mDimensions / 8);
	context->TransitionResource(mColorTexture[mOutput], D3D12_RESOURCE_STATE_UNORDERED_ACCESS, true);
}

//int temp = 0;

void Voxelize::GenerateMipmaps(Engine::DescriptorHeap* heap, Engine::ComputeContext* context)
{
	/*if (temp > 3)
	{
		return;
	}*/

	context->SetPipelineState(mMipmapPS);
	context->SetRootSignature(mMipmapRS);
	context->SetDescriptorHeap(Engine::DescriptorHeap::CBV_SRV_UAV, heap);
	int todo = mMiplevels - 1;
	int base = 0;
	int dimension = mDimensions;

	switch (mMipMethod)
	{
	case MipmapCalculation::MIPMAP_1:
		while (todo != 0)
		{
			int mipLevels = 1;

			context->SetConstants(0, Engine::DWParam(base), Engine::DWParam(mipLevels), Engine::DWParam(1.0f / (float)dimension));
			context->SetDescriptorTable(1, mColorTexture[mOutput]->GetSRV());
			context->SetDescriptorTable(2, mColorTexture[mOutput]->GetUAV(base + 1));
			context->Dispatch(dimension / 2, dimension / 2, dimension / 2);
			
			todo -= mipLevels;
			base += mipLevels;
			dimension /= 2 << (mipLevels - 1);
		}
		break;

	case MipmapCalculation::MIPMAP_2:
		while (todo != 0)
		{
			int mipLevels = 2;

			if (todo == 1)
			{
				todo++;
				base--;
				dimension *= 2;
			}

			context->SetConstants(0, Engine::DWParam(base), Engine::DWParam(mipLevels), Engine::DWParam(1.0f / (float)dimension));
			context->SetDescriptorTable(1, mColorTexture[mOutput]->GetSRV());
			context->SetDescriptorTable(2, mColorTexture[mOutput]->GetUAV(base + 1));
			context->SetDescriptorTable(3, mColorTexture[mOutput]->GetUAV(base + 2));
			context->Dispatch(dimension / 4, dimension / 4, dimension / 4);
			
			todo -= mipLevels;
			base += mipLevels;
			dimension /= 2 << (mipLevels - 1);
		}
		break;

	case MipmapCalculation::MIPMAP_3:
		while (todo != 0)
		{
			int mipLevels = 3;

			if (todo == 1)
			{
				todo += 2;
				base -= 2;
				dimension *= 4;
			}
			else if (todo == 2)
			{
				todo++;
				base--;
				dimension *= 2;
			}

			context->SetConstants(0, Engine::DWParam(base), Engine::DWParam(mipLevels), Engine::DWParam(1.0f / (float)dimension));
			context->SetDescriptorTable(1, mColorTexture[mOutput]->GetSRV());
			context->SetDescriptorTable(2, mColorTexture[mOutput]->GetUAV(base + 1));
			context->SetDescriptorTable(3, mColorTexture[mOutput]->GetUAV(base + 2));
			context->SetDescriptorTable(4, mColorTexture[mOutput]->GetUAV(base + 3));
			context->Dispatch(dimension / 8, dimension / 8, dimension / 8);

			todo -= mipLevels;
			base += mipLevels;
			dimension /= 2 << (mipLevels - 1);
		}
		break;
	}
}

void Voxelize::Generate(Engine::DescriptorHeap* heap, Engine::GraphicsContext* context, std::vector<Engine::RenderNode>& nodes, Engine::GpuMappedBuffer* matrices)
{
	Engine::AABB bounds;
	for (const Engine::RenderNode& node : nodes)
	{
		if (node.mObject->Has<Engine::MeshComponent>())
		{
			bounds.Union(node.mObject->Get<Engine::MeshComponent>()->GetMesh()->Bounds());
		}
	}

	float maxSize = max(max(bounds.mMax.x - bounds.mMin.x, bounds.mMax.y - bounds.mMin.y), bounds.mMax.y - bounds.mMin.y);
	float maxOffset = max(max(-bounds.mMin.x, -bounds.mMin.y), -bounds.mMin.z);

	memcpy(mVoxelizeParamsPtr, &mProjections[0], sizeof(float) * 16);
	memcpy((float*)mVoxelizeParamsPtr + 16, &mProjections[1], sizeof(float) * 16);
	memcpy((float*)mVoxelizeParamsPtr + 32, &mProjections[2], sizeof(float) * 16);
	memcpy((float*)mVoxelizeParamsPtr + 48, mScale, sizeof(float) * 16);

	float* ptr = (float*)mVoxelizeParamsPtr;
	ptr[48] = maxOffset;
	ptr[49] = maxOffset;
	ptr[50] = maxOffset;
	ptr[51] = 0.0f;
	ptr[52] = 1.0f / maxSize;
	ptr[53] = 1.0f / maxSize;
	ptr[54] = 1.0f / maxSize;
	ptr[55] = 0.0f;

	unsigned int* uptr = (unsigned int*)mVoxelizeParamsPtr;
	uptr[56] = mDimensions;
	uptr[57] = mDimensions;
	uptr[58] = mDimensions;

	context->SetPipelineState(mVoxelizePS);
	context->SetRootSignature(mVoxelizeRS);
	context->SetDescriptorHeap(Engine::DescriptorHeap::CBV_SRV_UAV, heap);
	context->SetViewport(0.0f, 0.0f, (float)mDimensions, (float)mDimensions);
	context->SetScissorRect(0.0f, 0.0f, (float)mDimensions, (float)mDimensions);
	context->SetPrimitiveTopology(Engine::Graphics::TRIANGLELIST);
	context->SetConstantBuffer(0, mVoxelizeParams->GetGpuVirtualAddress());

	context->SetDescriptorTable(2, mColorTexture[mOutput]->GetUAV());

	context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(5, mLightingSystem->GetShadowMap()->GetSRV().mGpuHandle);
	context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(6, mLightingSystem->GetLightBuffer()->GetSRV().mGpuHandle);
	context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(7, mLightingSystem->GetShadowAtlasBuffer()->GetSRV().mGpuHandle);

	context->SetDescriptorTable(8, mColorTexture[mInput]->GetUAV());

	int batchBound = -1;
	for (const Engine::RenderNode& node : nodes)
	{
		bool hasTexture = false;
		int batch = node.mID / 16;

		if (node.mObject->Has<Engine::MaterialComponent>())
		{
			context->SetDescriptorTable(1, node.mObject->Get<Engine::MaterialComponent>()->GetDiffuseMap()->GetSRV());
			hasTexture = true;
		}

		if (node.mObject->Has<Engine::MeshComponent>() && hasTexture)
		{
			if (batchBound != batch)
			{
				batchBound = batch;
				context->SetConstantBuffer(3, matrices->GetGpuVirtualAddress(batch * 16 * sizeof(float) * 16 * 2));
			}
			context->SetConstants(4, Engine::DWParam(node.mID % 16), Engine::DWParam(mLightingSystem->GetLights()));

			node.mObject->Get<Engine::MeshComponent>()->GetMesh()->Render(context);
		}
	}

	context->TransitionResource(mColorTexture[mOutput], D3D12_RESOURCE_STATE_UNORDERED_ACCESS, true);
}

VoxelizeOctree::VoxelizeOctree(Engine::D3DRenderer* renderer, unsigned int resolution)
{
	mVoxelizeRS = new Engine::RootSignature(renderer->GetDevice(), 4, 1);
	(*mVoxelizeRS)[0].InitAsConstantBuffer(0);
	(*mVoxelizeRS)[1].InitAsDescriptorTable(1);
	(*mVoxelizeRS)[1].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);
	(*mVoxelizeRS)[2].InitAsDescriptorTable(1);
	(*mVoxelizeRS)[2].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0, 1);
	(*mVoxelizeRS)[3].InitAsDescriptorTable(1);
	(*mVoxelizeRS)[3].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 1);
	mVoxelizeRS->InitStaticSampler(0, Engine::Sampler::CreateSamplerD3D(Engine::Sampler::Filter::ANISOTROPIC,
		Engine::Sampler::Address::WRAP,
		Engine::Sampler::Address::WRAP,
		Engine::Sampler::Address::WRAP,
		0.0f,
		16,
		Engine::Sampler::ComparisonFunc::NEVER,
		1.0f, 1.0f, 1.0f, 1.0f,
		0.0f,
		32.0f));
	mVoxelizeRS->Finalize(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	mVoxelize = new Engine::D3DShader("../Data/SkyeCuillin/VoxelizeOctree.hlsl",
		std::vector<Engine::D3DShader::ShaderEntryPoint>
	{
		Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::VERTEX_SHADER, "VS"),
		Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::GEOMETRY_SHADER, "GS"),
		Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::PIXEL_SHADER, "PS")
	});

	Engine::InputLayout inputLayout = Engine::InputLayout();
	inputLayout.AddAttribute(Engine::InputLayout::Attribute("POSITION", 0, Engine::Graphics::Format::RGB32F, 0, 0, Engine::InputLayout::Classification::PER_VERTEX, 0));
	inputLayout.AddAttribute(Engine::InputLayout::Attribute("NORMAL", 0, Engine::Graphics::Format::RGB32F, 0, 12, Engine::InputLayout::Classification::PER_VERTEX, 0));
	inputLayout.AddAttribute(Engine::InputLayout::Attribute("TEXCOORD", 2, Engine::Graphics::Format::RGB32F, 0, 24, Engine::InputLayout::Classification::PER_VERTEX, 0));
	inputLayout.AddAttribute(Engine::InputLayout::Attribute("TEXCOORD", 3, Engine::Graphics::Format::RGB32F, 0, 36, Engine::InputLayout::Classification::PER_VERTEX, 0));
	inputLayout.AddAttribute(Engine::InputLayout::Attribute("TEXCOORD", 0, Engine::Graphics::Format::RG32F, 0, 48, Engine::InputLayout::Classification::PER_VERTEX, 0));
	inputLayout.AddAttribute(Engine::InputLayout::Attribute("TEXCOORD", 1, Engine::Graphics::Format::RG32F, 0, 56, Engine::InputLayout::Classification::PER_VERTEX, 0));

	Engine::DepthStencilState dsState = Engine::DepthStencilState(false);

	std::vector<Engine::Graphics::Format> rtvFormats;
	rtvFormats.push_back(Engine::Graphics::Format::RGBA8);

	mVoxelizePS = new Engine::PipelineState(renderer->GetDevice(),
		mVoxelizeRS,
		mVoxelize,
		Engine::BlendState::CreateStateD3D(),
		Engine::RasterizerState::CreateStateD3D(Engine::RasterizerState::SOLID, Engine::RasterizerState::NONE, true, 0, 0.0f, 0.0f, true, false, false, 0, Engine::RasterizerState::OFF),
		&dsState,
		&inputLayout,
		Engine::PipelineState::PrimitiveType::TRIANGLE,
		rtvFormats,
		Engine::Graphics::Format::D32F,
		1, 0);

	mOctreeNodeFlagRS = new Engine::RootSignature(renderer->GetDevice(), 3, 0);
	(*mOctreeNodeFlagRS)[0].InitAsConstants(0, 3);
	(*mOctreeNodeFlagRS)[1].InitAsDescriptorTable(1);
	(*mOctreeNodeFlagRS)[1].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0, 1);
	(*mOctreeNodeFlagRS)[2].InitAsDescriptorTable(1);
	(*mOctreeNodeFlagRS)[2].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 1);
	mOctreeNodeFlagRS->Finalize(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	mOctreeNodeFlag = new Engine::D3DShader("../Data/SkyeCuillin/OctreeNodeFlag.hlsl",
		std::vector<Engine::D3DShader::ShaderEntryPoint>
	{
		Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::COMPUTE_SHADER, "OctreeNodeFlag")
	});
	mOctreeNodeFlagPS = new Engine::PipelineState(renderer->GetDevice(), mOctreeNodeFlagRS, mOctreeNodeFlag);

	mOctreeNodeAllocRS = new Engine::RootSignature(renderer->GetDevice(), 3, 0);
	(*mOctreeNodeAllocRS)[0].InitAsConstants(0, 3);
	(*mOctreeNodeAllocRS)[1].InitAsDescriptorTable(1);
	(*mOctreeNodeAllocRS)[1].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0, 1);
	(*mOctreeNodeAllocRS)[2].InitAsDescriptorTable(1);
	(*mOctreeNodeAllocRS)[2].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 1);
	mOctreeNodeAllocRS->Finalize(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	mOctreeNodeAlloc = new Engine::D3DShader("../Data/SkyeCuillin/OctreeNodeAlloc.hlsl",
		std::vector<Engine::D3DShader::ShaderEntryPoint>
	{
		Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::COMPUTE_SHADER, "OctreeNodeAlloc")
	});
	mOctreeNodeAllocPS = new Engine::PipelineState(renderer->GetDevice(), mOctreeNodeAllocRS, mOctreeNodeAlloc);

	mOctreeNodeInitRS = new Engine::RootSignature(renderer->GetDevice(), 2, 0);
	(*mOctreeNodeInitRS)[0].InitAsConstants(0, 2);
	(*mOctreeNodeInitRS)[1].InitAsDescriptorTable(1);
	(*mOctreeNodeInitRS)[1].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0, 1);
	mOctreeNodeInitRS->Finalize(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	mOctreeNodeInit = new Engine::D3DShader("../Data/SkyeCuillin/OctreeNodeInit.hlsl",
		std::vector<Engine::D3DShader::ShaderEntryPoint>
	{
		Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::COMPUTE_SHADER, "OctreeNodeInit")
	});
	mOctreeNodeInitPS = new Engine::PipelineState(renderer->GetDevice(), mOctreeNodeInitRS, mOctreeNodeInit);

	mOctreeDataFillRS = new Engine::RootSignature(renderer->GetDevice(), 5, 0);
	(*mOctreeDataFillRS)[0].InitAsConstants(0, 3);
	(*mOctreeDataFillRS)[1].InitAsDescriptorTable(1);
	(*mOctreeDataFillRS)[1].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);
	(*mOctreeDataFillRS)[2].InitAsDescriptorTable(1);
	(*mOctreeDataFillRS)[2].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
	(*mOctreeDataFillRS)[3].InitAsDescriptorTable(1);
	(*mOctreeDataFillRS)[3].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 1);
	(*mOctreeDataFillRS)[4].InitAsDescriptorTable(1);
	(*mOctreeDataFillRS)[4].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0, 1);
	mOctreeDataFillRS->Finalize(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	mOctreeDataFill = new Engine::D3DShader("../Data/SkyeCuillin/OctreeDataFill.hlsl",
		std::vector<Engine::D3DShader::ShaderEntryPoint>
	{
		Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::COMPUTE_SHADER, "OctreeDataFill")
	});
	mOctreeDataFillPS = new Engine::PipelineState(renderer->GetDevice(), mOctreeDataFillRS, mOctreeDataFill);

	mOctreeDataClearRS = new Engine::RootSignature(renderer->GetDevice(), 2, 0);
	(*mOctreeDataClearRS)[0].InitAsConstants(0, 1);
	(*mOctreeDataClearRS)[1].InitAsDescriptorTable(1);
	(*mOctreeDataClearRS)[1].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0, 1);
	mOctreeDataClearRS->Finalize(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	mOctreeDataClear = new Engine::D3DShader("../Data/SkyeCuillin/OctreeDataClear.hlsl",
		std::vector<Engine::D3DShader::ShaderEntryPoint>
	{
		Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::COMPUTE_SHADER, "OctreeDataClear")
	});
	mOctreeDataClearPS = new Engine::PipelineState(renderer->GetDevice(), mOctreeDataClearRS, mOctreeDataClear);

	mOctreeDataFilterRS = new Engine::RootSignature(renderer->GetDevice(), 4, 0);
	(*mOctreeDataFilterRS)[0].InitAsConstants(0, 4);
	(*mOctreeDataFilterRS)[1].InitAsDescriptorTable(1);
	(*mOctreeDataFilterRS)[1].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);
	(*mOctreeDataFilterRS)[2].InitAsDescriptorTable(1);
	(*mOctreeDataFilterRS)[2].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
	(*mOctreeDataFilterRS)[3].InitAsDescriptorTable(1);
	(*mOctreeDataFilterRS)[3].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0, 1); 
	mOctreeDataFilterRS->Finalize(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	mOctreeDataFilter = new Engine::D3DShader("../Data/SkyeCuillin/OctreeDataFilter.hlsl",
		std::vector<Engine::D3DShader::ShaderEntryPoint>
	{
		Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::COMPUTE_SHADER, "OctreeDataFilter")
	});
	mOctreeDataFilterPS = new Engine::PipelineState(renderer->GetDevice(), mOctreeDataFilterRS, mOctreeDataFilter);
	
	mOctreeDataInteriorRS = new Engine::RootSignature(renderer->GetDevice(), 4, 0);
	(*mOctreeDataInteriorRS)[0].InitAsConstants(0, 3);
	(*mOctreeDataInteriorRS)[1].InitAsDescriptorTable(1);
	(*mOctreeDataInteriorRS)[1].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);
	(*mOctreeDataInteriorRS)[2].InitAsDescriptorTable(1);
	(*mOctreeDataInteriorRS)[2].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
	(*mOctreeDataInteriorRS)[3].InitAsDescriptorTable(1);
	(*mOctreeDataInteriorRS)[3].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0, 1);
	mOctreeDataInteriorRS->Finalize(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	mOctreeDataInterior = new Engine::D3DShader("../Data/SkyeCuillin/OctreeDataInterior.hlsl",
		std::vector<Engine::D3DShader::ShaderEntryPoint>
	{
		Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::COMPUTE_SHADER, "OctreeDataInterior")
	});
	mOctreeDataInteriorPS = new Engine::PipelineState(renderer->GetDevice(), mOctreeDataInteriorRS, mOctreeDataInterior);

	mDimensions = resolution;
	mVoxelArraySize = 10000;

	mVoxelBuffer = new Engine::StructuredBuffer();
	mVoxelBuffer->Init(renderer, mVoxelArraySize, sizeof(unsigned int), NULL);
	mVoxelColorBuffer = new Engine::StructuredBuffer();
	mVoxelColorBuffer->Init(renderer, mVoxelArraySize, sizeof(unsigned int), NULL);

	mOctreeLevels = Engine::Math::log2(mDimensions) - 1;
	mOctreeNodes = CalculateOctreeNodes(mOctreeLevels, mVoxelArraySize);
	mOctreeNodeBuffer = new Engine::StructuredBuffer();
	mOctreeNodeBuffer->Init(renderer, mOctreeNodes, sizeof(unsigned int), NULL);
	mOctreeCounterBuffer = new Engine::ByteAddressBuffer();
	mOctreeCounterBuffer->Init(renderer, 64, sizeof(unsigned int), NULL);
	mOctreeDataBuffer = new Engine::Texture();
	mOctreeDataSize[0] = 1536;
	mOctreeDataSize[1] = 1536;
	mOctreeDataSize[2] = 3;
	mOctreeDataBuffer->Init(renderer, mOctreeDataSize[0], mOctreeDataSize[1], mOctreeDataSize[2], Engine::Graphics::Format::RGBA8, nullptr, nullptr, true);
	/*mDataReadback = new Engine::GpuMappedBuffer();
	mDataReadback->Init(renderer, mVoxelArraySize, sizeof(unsigned int), Engine::GpuMappedBuffer::READBACK);
	mTreeReadback = new Engine::GpuMappedBuffer();
	mTreeReadback->Init(renderer, mOctreeNodes * 8, sizeof(unsigned int), Engine::GpuMappedBuffer::READBACK);*/
	
	mVoxelizeParams = new Engine::GpuMappedBuffer();
	mVoxelizeParams->Init(renderer, 64, sizeof(float));
	mVoxelizeParamsPtr = mVoxelizeParams->Map();

	mVoxelizeReadback = new Engine::GpuMappedBuffer();
	mVoxelizeReadback->Init(renderer, 64, sizeof(unsigned int), Engine::GpuMappedBuffer::READBACK);

	mOctreeReadback = new Engine::GpuMappedBuffer();
	mOctreeReadback->Init(renderer, 64, sizeof(unsigned int), Engine::GpuMappedBuffer::READBACK);

	Engine::mat4 ortho = Engine::OrthoLH(2.0f, 2.0f, -1.0f, 1.0f);
	mProjections = new Engine::mat4[3];
	mProjections[0] = Engine::LookAtLH(Engine::float4(0.0f, 0.0f, 0.0f, 1.0f), Engine::float4(1.0f, 0.0f, 0.0f, 1.0f), Engine::float4(0.0f, 1.0f, 0.0f, 0.0f)) * ortho;
	mProjections[1] = Engine::LookAtLH(Engine::float4(0.0f, 0.0f, 0.0f, 1.0f), Engine::float4(0.0f, 1.0f, 0.0f, 1.0f), Engine::float4(0.0f, 0.0f, 1.0f, 0.0f)) * ortho;
	mProjections[2] = Engine::LookAtLH(Engine::float4(0.0f, 0.0f, 0.0f, 1.0f), Engine::float4(0.0f, 0.0f, 1.0f, 1.0f), Engine::float4(0.0f, 1.0f, 0.0f, 0.0f)) * ortho;
	mScale = new Engine::mat4();

	mDimensions = resolution;

	float vertices[] =
	{
		-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
	};
	mVertexBuffer = new Engine::TypedBuffer(Engine::Graphics::R32F);
	mVertexBuffer->Init(renderer, 8, sizeof(float) * 16, vertices);

	unsigned int indices[] = 
	{
		0, 1, 2,
		2, 3, 0,
		1, 5, 6,
		6, 2, 1,
		7, 6, 5,
		5, 4, 7,
		4, 0, 3,
		3, 7, 4,
		4, 5, 1,
		1, 0, 4,
		3, 2, 6,
		6, 7, 3,
	};
	mIndexBuffer = new Engine::TypedBuffer(Engine::Graphics::R32);
	mIndexBuffer->Init(renderer, 36, sizeof(unsigned int), indices);
}

void VoxelizeOctree::Prepare(Engine::D3DRenderer* renderer)
{
	if (mInstances > mVoxelArraySize)
	{
		renderer->Flush();

		mVoxelArraySize = mInstances * 2;

		delete mVoxelBuffer;
		mVoxelBuffer = new Engine::StructuredBuffer();
		mVoxelBuffer->Init(renderer, mVoxelArraySize, sizeof(unsigned int), NULL);

		delete mVoxelColorBuffer;
		mVoxelColorBuffer = new Engine::StructuredBuffer();
		mVoxelColorBuffer->Init(renderer, mVoxelArraySize, sizeof(unsigned int), NULL);

		mOctreeNodes = CalculateOctreeNodes(mOctreeLevels, mVoxelArraySize);

 		delete mOctreeNodeBuffer;
		mOctreeNodeBuffer = new Engine::StructuredBuffer();
		mOctreeNodeBuffer->Init(renderer, mOctreeNodes, sizeof(unsigned int), NULL);

		delete mOctreeDataBuffer;
		mOctreeDataBuffer = new Engine::Texture();
		mOctreeDataSize[0] = 1536;
		mOctreeDataSize[1] = 1536;
		mOctreeDataSize[2] = (mVoxelArraySize / (512 * 512) + ((mVoxelArraySize % (512 * 512)) > 0 ? 1 : 0)) * 3;
		mOctreeDataBuffer->Init(renderer, mOctreeDataSize[0], mOctreeDataSize[1], mOctreeDataSize[2], Engine::Graphics::Format::RGBA8, nullptr, nullptr, true);

		/*delete mDataReadback;
		mDataReadback = new Engine::GpuMappedBuffer();
		mDataReadback->Init(renderer, mVoxelArraySize, sizeof(unsigned int), Engine::GpuMappedBuffer::READBACK);

		delete mTreeReadback;
		mTreeReadback = new Engine::GpuMappedBuffer();
		mTreeReadback->Init(renderer, mOctreeNodes * 8 * 4, sizeof(unsigned int), Engine::GpuMappedBuffer::READBACK);*/

		renderer->Flush();
	}
}

int built = 0;

void VoxelizeOctree::BuildOctree(Engine::D3DRenderer* renderer, Engine::QueryHeap* heap, Engine::QueryHandle* timestamps)
{
	if (mInstances > mVoxelArraySize || mInstances == 0)
	{
		return;
	}
	
	if (built > 2)
	{
		return;
	}

	//printf("Building octree:\n");
	int nodesOffset = 0;	// Node to-be-processed
	int nodesAllocated = 1;	// Number of allocated nodes

	std::vector<int> nodesLevels;	// Number of nodes per level
	nodesLevels.push_back(1);
	int nodesCount = 0;
	int nodesCurrentCount = 0;

	uint64_t freq;

	int k = 4;

	for (unsigned int i = 0; i < mOctreeLevels; i++)
	{
		int items = min(mInstances, mVoxelArraySize);
		int group = (items + 1023) / 1024;

		assert(group > 0);

		Engine::ComputeContext* context = renderer->GetComputeContext();
		freq = context->GetTimestampFrequency();
		context->Begin(mOctreeNodeFlagPS);
		context->SetRootSignature(mOctreeNodeFlagRS);
		context->SetDescriptorHeap(Engine::DescriptorHeap::CBV_SRV_UAV, renderer->Heap());
		context->SetConstants(0, Engine::DWParam(items), Engine::DWParam(i), Engine::DWParam(mDimensions));
		context->SetDescriptorTable(1, mVoxelBuffer->GetUAV());
		context->SetDescriptorTable(2, mOctreeNodeBuffer->GetUAV());
		context->EndQuery(heap, D3D12_QUERY_TYPE_TIMESTAMP, timestamps[k++]);
		context->Dispatch(group, 1, 1);
		context->EndQuery(heap, D3D12_QUERY_TYPE_TIMESTAMP, timestamps[k++]);
		context->TransitionResource(mOctreeNodeBuffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, true);
		context->Finish();
		
		int nodesToProcess = nodesLevels[i];
		int groupX = (nodesToProcess + 1023) / 1024;

		assert(groupX > 0);

		context = renderer->GetComputeContext();
		context->Begin(mOctreeNodeAllocPS);
		context->ResetCounter(mOctreeCounterBuffer, 0);
		context->SetRootSignature(mOctreeNodeAllocRS);
		context->SetDescriptorHeap(Engine::DescriptorHeap::CBV_SRV_UAV, renderer->Heap());
		context->SetConstants(0, Engine::DWParam(nodesToProcess), Engine::DWParam(nodesOffset), Engine::DWParam(nodesAllocated));
		context->SetDescriptorTable(1, mOctreeNodeBuffer->GetUAV());
		context->SetDescriptorTable(2, mOctreeCounterBuffer->GetUAV());
		context->EndQuery(heap, D3D12_QUERY_TYPE_TIMESTAMP, timestamps[k++]);
		context->Dispatch(groupX, 1, 1);
		context->EndQuery(heap, D3D12_QUERY_TYPE_TIMESTAMP, timestamps[k++]);
		context->TransitionResource(mOctreeNodeBuffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, true);
		context->TransitionResource(mOctreeCounterBuffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, true);
		context->CopyCounter(mOctreeReadback, 0, mOctreeCounterBuffer);
		context->Finish();

		mOctreeReadbackPtr = mOctreeReadback->Map(0, sizeof(unsigned int));
		unsigned int totalNodes = ((unsigned int*)mOctreeReadbackPtr)[0];
		mOctreeReadback->Unmap();
			
		nodesCurrentCount = (int)totalNodes;
		nodesCount += totalNodes;

		int childrenCount = nodesCurrentCount * 8;
		group = (childrenCount + 1023) / 1024;

		assert(group > 0);

		context = renderer->GetComputeContext();
		context->Begin(mOctreeNodeInitPS);
		context->SetRootSignature(mOctreeNodeInitRS);
		context->SetDescriptorHeap(Engine::DescriptorHeap::CBV_SRV_UAV, renderer->Heap());
		context->SetConstants(0, Engine::DWParam(childrenCount), Engine::DWParam(nodesAllocated));
		context->SetDescriptorTable(1, mOctreeNodeBuffer->GetUAV());
		context->EndQuery(heap, D3D12_QUERY_TYPE_TIMESTAMP, timestamps[k++]);
		context->Dispatch(group, 1, 1);
		context->EndQuery(heap, D3D12_QUERY_TYPE_TIMESTAMP, timestamps[k++]);
		context->TransitionResource(mOctreeNodeBuffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, true);
		context->Finish();

		nodesAllocated += nodesCurrentCount * 8;
		nodesLevels.push_back(nodesCurrentCount * 8);
		nodesOffset += nodesLevels[i];

		//printf("\tOctree level (%d): %d nodes\n", i, nodesCurrentCount * 8);
	}

	int nodesTotal = 0;
	for (int i = 0; i < nodesLevels.size(); i++)
	{
		nodesTotal += nodesLevels[i];
	}
	nodesTotal *= 27;
	//printf("Total number of nodes %u\n", nodesTotal);

	Engine::ComputeContext* context = renderer->GetComputeContext();
	context->Begin(mOctreeDataClearPS);
	context->SetRootSignature(mOctreeDataClearRS);
	context->SetDescriptorHeap(Engine::DescriptorHeap::CBV_SRV_UAV, renderer->Heap());
	context->SetConstants(0, Engine::DWParam(nodesTotal));
	context->SetDescriptorTable(1, mOctreeDataBuffer->GetUAV());
	context->Dispatch((nodesTotal + 1023) / 1024, 1, 1);
	context->TransitionResource(mOctreeDataBuffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, true);
	context->Finish();

	int items = min(mInstances, mVoxelArraySize);
	int group = (items + 1023) / 1024;

	context = renderer->GetComputeContext();
	context->Begin(mOctreeDataFillPS);
	context->SetRootSignature(mOctreeDataFillRS);
	context->SetDescriptorHeap(Engine::DescriptorHeap::CBV_SRV_UAV, renderer->Heap());
	context->SetConstants(0, Engine::DWParam(items), Engine::DWParam(mDimensions), Engine::DWParam(mOctreeLevels));
	context->SetDescriptorTable(1, mVoxelBuffer->GetSRV());
	context->SetDescriptorTable(2, mVoxelColorBuffer->GetSRV());
	context->SetDescriptorTable(3, mOctreeNodeBuffer->GetSRV());
	context->SetDescriptorTable(4, mOctreeDataBuffer->GetUAV());
	context->Dispatch(group, 1, 1);
	context->TransitionResource(mOctreeDataBuffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, true);
	context->Finish();

	context = renderer->GetComputeContext();
	context->Begin(mOctreeDataFilterPS);
	context->SetRootSignature(mOctreeDataFilterRS);
	context->SetDescriptorHeap(Engine::DescriptorHeap::CBV_SRV_UAV, renderer->Heap());
	context->SetDescriptorTable(1, mVoxelBuffer->GetSRV());
	context->SetDescriptorTable(2, mOctreeNodeBuffer->GetSRV());
	context->SetDescriptorTable(3, mOctreeDataBuffer->GetUAV());
	context->SetConstants(0, Engine::DWParam(2), Engine::DWParam(mDimensions), Engine::DWParam(mOctreeLevels), Engine::DWParam(6));
	context->Dispatch(mDimensions / 2 / 8, mDimensions / 2 / 8, mDimensions / 2 / 8);
	context->TransitionResource(mOctreeDataBuffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, true);
	/*context->SetConstants(0, Engine::DWParam(2), Engine::DWParam(mDimensions), Engine::DWParam(mOctreeLevels), Engine::DWParam(7));
	context->Dispatch(mDimensions / 2 / 8, mDimensions / 2 / 8, mDimensions / 2 / 8);
	context->TransitionResource(mOctreeDataBuffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, true);*/
	context->SetConstants(0, Engine::DWParam(2), Engine::DWParam(mDimensions), Engine::DWParam(mOctreeLevels), Engine::DWParam(0));
	context->Dispatch(mDimensions / 2 / 8, mDimensions / 2 / 8, mDimensions / 2 / 8);
	context->TransitionResource(mOctreeDataBuffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, true);
	context->SetConstants(0, Engine::DWParam(2), Engine::DWParam(mDimensions), Engine::DWParam(mOctreeLevels), Engine::DWParam(1));
	context->Dispatch(mDimensions / 2 / 8, mDimensions / 2 / 8, mDimensions / 2 / 8);
	context->TransitionResource(mOctreeDataBuffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, true);
	context->SetConstants(0, Engine::DWParam(2), Engine::DWParam(mDimensions), Engine::DWParam(mOctreeLevels), Engine::DWParam(2));
	context->Dispatch(mDimensions / 2 / 8, mDimensions / 2 / 8, mDimensions / 2 / 8);
	context->TransitionResource(mOctreeDataBuffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, true);
	context->SetConstants(0, Engine::DWParam(2), Engine::DWParam(mDimensions), Engine::DWParam(mOctreeLevels), Engine::DWParam(3));
	context->Dispatch(mDimensions / 2 / 8, mDimensions / 2 / 8, mDimensions / 2 / 8);
	context->TransitionResource(mOctreeDataBuffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, true);
	context->SetConstants(0, Engine::DWParam(2), Engine::DWParam(mDimensions), Engine::DWParam(mOctreeLevels), Engine::DWParam(4));
	context->Dispatch(mDimensions / 2 / 8, mDimensions / 2 / 8, mDimensions / 2 / 8);
	context->TransitionResource(mOctreeDataBuffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, true);
	context->SetConstants(0, Engine::DWParam(2), Engine::DWParam(mDimensions), Engine::DWParam(mOctreeLevels), Engine::DWParam(5));
	context->Dispatch(mDimensions / 2 / 8, mDimensions / 2 / 8, mDimensions / 2 / 8);
	context->TransitionResource(mOctreeDataBuffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, true);
	context->Finish();

	unsigned int factor = 4;
	for (unsigned int i = 0; i < mOctreeLevels; i++)
	{
		int items = min(mInstances, mVoxelArraySize);
		int group = (items + 255) / 256;

		Engine::ComputeContext* context = renderer->GetComputeContext();
		freq = context->GetTimestampFrequency();
		context->Begin(mOctreeDataInteriorPS);
		context->SetRootSignature(mOctreeDataInteriorRS);
		context->SetDescriptorHeap(Engine::DescriptorHeap::CBV_SRV_UAV, renderer->Heap());
		context->SetConstants(0, Engine::DWParam(items), Engine::DWParam(mOctreeLevels - i - 1), Engine::DWParam(mDimensions));
		context->SetDescriptorTable(1, mVoxelBuffer->GetSRV());
		context->SetDescriptorTable(2, mOctreeNodeBuffer->GetSRV());
		context->SetDescriptorTable(3, mOctreeDataBuffer->GetUAV());
		context->Dispatch(group, 1, 1);
		context->TransitionResource(mOctreeNodeBuffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, true);
		/*context->SetPipelineState(mOctreeDataFilterPS);
		context->SetRootSignature(mOctreeDataFilterRS);
		context->SetConstants(0, Engine::DWParam(factor), Engine::DWParam(mDimensions), Engine::DWParam(mOctreeLevels - i - 1), Engine::DWParam(7));
		context->Dispatch(mDimensions / factor / 8, mDimensions / factor / 8, mDimensions / factor / 8);
		context->TransitionResource(mOctreeDataBuffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, true);
		context->SetConstants(0, Engine::DWParam(factor), Engine::DWParam(mDimensions), Engine::DWParam(mOctreeLevels - i - 1), Engine::DWParam(0));
		context->Dispatch(mDimensions / factor / 8, mDimensions / factor / 8, mDimensions / factor / 8);
		context->TransitionResource(mOctreeDataBuffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, true);
		context->SetConstants(0, Engine::DWParam(factor), Engine::DWParam(mDimensions), Engine::DWParam(mOctreeLevels - i - 1), Engine::DWParam(1));
		context->Dispatch(mDimensions / factor / 8, mDimensions / factor / 8, mDimensions / factor / 8);
		context->TransitionResource(mOctreeDataBuffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, true);
		context->SetConstants(0, Engine::DWParam(factor), Engine::DWParam(mDimensions), Engine::DWParam(mOctreeLevels - i - 1), Engine::DWParam(2));
		context->Dispatch(mDimensions / factor / 8, mDimensions / factor / 8, mDimensions / factor / 8);
		context->TransitionResource(mOctreeDataBuffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, true);
		context->SetConstants(0, Engine::DWParam(factor), Engine::DWParam(mDimensions), Engine::DWParam(mOctreeLevels - i - 1), Engine::DWParam(3));
		context->Dispatch(mDimensions / factor / 8, mDimensions / factor / 8, mDimensions / factor / 8);
		context->TransitionResource(mOctreeDataBuffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, true);
		context->SetConstants(0, Engine::DWParam(factor), Engine::DWParam(mDimensions), Engine::DWParam(mOctreeLevels - i - 1), Engine::DWParam(4));
		context->Dispatch(mDimensions / factor / 8, mDimensions / factor / 8, mDimensions / factor / 8);
		context->TransitionResource(mOctreeDataBuffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, true);
		context->SetConstants(0, Engine::DWParam(factor), Engine::DWParam(mDimensions), Engine::DWParam(mOctreeLevels - i - 1), Engine::DWParam(5));
		context->Dispatch(mDimensions / factor / 8, mDimensions / factor / 8, mDimensions / factor / 8);
		context->TransitionResource(mOctreeDataBuffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, true);*/
		context->Finish();

		factor *= 2;
	}

	built++;
}

void VoxelizeOctree::Generate(Engine::DescriptorHeap* heap, Engine::GraphicsContext* context, std::vector<Engine::RenderNode>& nodes)
{
	if (built > 2)
	{
		return;
	}

	Engine::AABB bounds;
	for (const Engine::RenderNode& node : nodes)
	{
		if (node.mObject->Has<Engine::MeshComponent>())
		{
			bounds.Union(node.mObject->Get<Engine::MeshComponent>()->GetMesh()->Bounds());
		}
	}

	float maxSize = max(max(bounds.mMax.x - bounds.mMin.x, bounds.mMax.y - bounds.mMin.y), bounds.mMax.y - bounds.mMin.y);
	float maxOffset = max(max(-bounds.mMin.x, -bounds.mMin.y), -bounds.mMin.z);

	memcpy(mVoxelizeParamsPtr, &mProjections[0], sizeof(float) * 16);
	memcpy((float*)mVoxelizeParamsPtr + 16, &mProjections[1], sizeof(float) * 16);
	memcpy((float*)mVoxelizeParamsPtr + 32, &mProjections[2], sizeof(float) * 16);
	memcpy((float*)mVoxelizeParamsPtr + 48, mScale, sizeof(float) * 16);

	float* ptr = (float*)mVoxelizeParamsPtr;
	ptr[48] = maxOffset;
	ptr[49] = maxOffset;
	ptr[50] = maxOffset;
	ptr[51] = 0.0f;
	ptr[52] = 1.0f / maxSize;
	ptr[53] = 1.0f / maxSize;
	ptr[54] = 1.0f / maxSize;
	ptr[55] = 0.0f;

	unsigned int* uptr = (unsigned int*)mVoxelizeParamsPtr;
	uptr[56] = mDimensions;
	uptr[57] = mDimensions;
	uptr[58] = mDimensions;
	uptr[59] = mVoxelArraySize;

	context->SetPipelineState(mVoxelizePS);
	context->ResetCounter(mVoxelBuffer, 0);
	context->SetRootSignature(mVoxelizeRS);
	context->SetDescriptorHeap(Engine::DescriptorHeap::CBV_SRV_UAV, heap);
	context->SetViewport(0.0f, 0.0f, (float)mDimensions, (float)mDimensions);
	context->SetScissorRect(0.0f, 0.0f, (float)mDimensions, (float)mDimensions);
	context->SetPrimitiveTopology(Engine::Graphics::TRIANGLELIST);
	context->SetConstantBuffer(0, mVoxelizeParams->GetGpuVirtualAddress());

	context->SetDescriptorTable(2, mVoxelBuffer->GetUAV());
	context->SetDescriptorTable(3, mVoxelColorBuffer->GetUAV());
	
	for (const Engine::RenderNode& node : nodes)
	{
		bool hasTexture = false;

		if (node.mObject->Has<Engine::MaterialComponent>())
		{
			context->SetDescriptorTable(1, node.mObject->Get<Engine::MaterialComponent>()->GetDiffuseMap()->GetSRV());
			hasTexture = true;
		}

		if (node.mObject->Has<Engine::MeshComponent>() && hasTexture)
		{
			node.mObject->Get<Engine::MeshComponent>()->GetMesh()->Render(context);
		}
	}

	context->CopyCounter(mVoxelizeReadback, 0, mVoxelBuffer);
}

void VoxelizeOctree::DrawVoxels(Engine::GraphicsContext* context)
{
	if (mInstances > 0)
	{
		context->SetDescriptorTable(1, mVoxelBuffer->GetSRV());
		context->SetDescriptorTable(2, mOctreeDataBuffer->GetSRV());
		context->SetDescriptorTable(3, mOctreeNodeBuffer->GetSRV());
		context->SetConstants(4, Engine::DWParam(9));
		context->SetIndexBuffer(mIndexBuffer->IndexBufferView(0, (unsigned int)mIndexBuffer->Size()));
		context->SetVertexBuffer(0, mVertexBuffer->VertexBufferView(0, (unsigned int)mVertexBuffer->Size(), (unsigned int)mVertexBuffer->ElementSize()));
		context->DrawIndexedInstanced((unsigned int)mIndexBuffer->NumElements(), min(mInstances, mVoxelArraySize), 0, 0, 0);
	}
}

void VoxelizeOctree::ReadbackCounter(Engine::GraphicsContext* context)
{
	mVoxelizeReadbackPtr = mVoxelizeReadback->Map(0, sizeof(unsigned int));
	mInstances = ((unsigned int*)mVoxelizeReadbackPtr)[0];
	mVoxelizeReadback->Unmap();
}