#include "RenderPassGBuffer.h"

using namespace SkyeCuillin;

RenderPassGBuffer::RenderPassGBuffer(Engine::D3DRenderer* renderer, unsigned int width, unsigned int height, unsigned int samples) : RenderPass(renderer, width, height)
{
	mSamples = samples;

	mGBufferRS = new Engine::RootSignature(renderer->GetDevice(), 8, 1);
	(*mGBufferRS)[0].InitAsConstantBuffer(0);
	(*mGBufferRS)[1].InitAsDescriptorTable(1);
	(*mGBufferRS)[1].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);
	(*mGBufferRS)[2].InitAsDescriptorTable(1);
	(*mGBufferRS)[2].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
	(*mGBufferRS)[3].InitAsDescriptorTable(1);
	(*mGBufferRS)[3].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 1);
	(*mGBufferRS)[4].InitAsDescriptorTable(1);
	(*mGBufferRS)[4].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 1);
	(*mGBufferRS)[5].InitAsDescriptorTable(1);
	(*mGBufferRS)[5].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 4, 1);
	(*mGBufferRS)[6].InitAsConstantBuffer(2);
	(*mGBufferRS)[7].InitAsConstants(1, 1);
	mGBufferRS->InitStaticSampler(0, Engine::Sampler::CreateSamplerD3D(Engine::Sampler::Filter::ANISOTROPIC,
		Engine::Sampler::Address::WRAP,
		Engine::Sampler::Address::WRAP,
		Engine::Sampler::Address::WRAP,
		0.0f,
		16,
		Engine::Sampler::ComparisonFunc::NEVER,
		1.0f, 1.0f, 1.0f, 1.0f,
		0.0f,
		32.0f));
	mGBufferRS->Finalize(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	mGBuffer = new Engine::D3DShader("../Data/SkyeCuillin/DeferredGBuffer.hlsl",
		std::vector<Engine::D3DShader::ShaderEntryPoint>
		{
			Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::VERTEX_SHADER, "VS"),
			Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::HULL_SHADER, "HS"),
			Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::DOMAIN_SHADER, "DS"),
			Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::PIXEL_SHADER, "PS")
		},
		std::vector<Engine::D3DShader::ShaderDefine>());

	Engine::InputLayout inputLayout = Engine::InputLayout();
	inputLayout.AddAttribute(Engine::InputLayout::Attribute("POSITION", 0, Engine::Graphics::Format::RGB32F, 0, 0, Engine::InputLayout::Classification::PER_VERTEX, 0));
	inputLayout.AddAttribute(Engine::InputLayout::Attribute("NORMAL", 0, Engine::Graphics::Format::RGB32F, 0, 12, Engine::InputLayout::Classification::PER_VERTEX, 0));
	inputLayout.AddAttribute(Engine::InputLayout::Attribute("TEXCOORD", 2, Engine::Graphics::Format::RGB32F, 0, 24, Engine::InputLayout::Classification::PER_VERTEX, 0));
	inputLayout.AddAttribute(Engine::InputLayout::Attribute("TEXCOORD", 3, Engine::Graphics::Format::RGB32F, 0, 36, Engine::InputLayout::Classification::PER_VERTEX, 0));
	inputLayout.AddAttribute(Engine::InputLayout::Attribute("TEXCOORD", 0, Engine::Graphics::Format::RG32F, 0, 48, Engine::InputLayout::Classification::PER_VERTEX, 0));
	inputLayout.AddAttribute(Engine::InputLayout::Attribute("TEXCOORD", 1, Engine::Graphics::Format::RG32F, 0, 56, Engine::InputLayout::Classification::PER_VERTEX, 0));

	Engine::DepthStencilState dsState = Engine::DepthStencilState(true);

	std::vector<Engine::Graphics::Format> rtvFormats;
	rtvFormats.push_back(Engine::Graphics::Format::RGBA16F);
	rtvFormats.push_back(Engine::Graphics::Format::RGBA16F);
	//rtvFormats.push_back(Engine::Graphics::Format::RGBA8);

	mGBufferPS = new Engine::PipelineState(mRenderer->GetDevice(),
		mGBufferRS,
		mGBuffer,
		Engine::BlendState::CreateStateD3D(true),
		Engine::RasterizerState::CreateStateD3D(),
		//Engine::RasterizerState::CreateStateD3D(Engine::RasterizerState::WIREFRAME),
		&dsState,
		&inputLayout,
		Engine::PipelineState::PrimitiveType::PATCH,
		rtvFormats,
		Engine::Graphics::Format::D32F,
		mSamples,
		0);

	mGBufferTerrainRS = new Engine::RootSignature(renderer->GetDevice(), 8, 1);
	(*mGBufferTerrainRS)[0].InitAsConstantBuffer(0);
	(*mGBufferTerrainRS)[1].InitAsDescriptorTable(1);
	(*mGBufferTerrainRS)[1].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);
	(*mGBufferTerrainRS)[2].InitAsDescriptorTable(1);
	(*mGBufferTerrainRS)[2].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
	(*mGBufferTerrainRS)[3].InitAsDescriptorTable(1);
	(*mGBufferTerrainRS)[3].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 1);
	(*mGBufferTerrainRS)[4].InitAsDescriptorTable(1);
	(*mGBufferTerrainRS)[4].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 1);
	(*mGBufferTerrainRS)[5].InitAsDescriptorTable(1);
	(*mGBufferTerrainRS)[5].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 4, 1);
	(*mGBufferTerrainRS)[6].InitAsConstantBuffer(2);
	(*mGBufferTerrainRS)[7].InitAsConstants(1, 1);
	mGBufferTerrainRS->InitStaticSampler(0, Engine::Sampler::CreateSamplerD3D(Engine::Sampler::Filter::ANISOTROPIC,
		Engine::Sampler::Address::WRAP,
		Engine::Sampler::Address::WRAP,
		Engine::Sampler::Address::WRAP,
		0.0f,
		16,
		Engine::Sampler::ComparisonFunc::NEVER,
		1.0f, 1.0f, 1.0f, 1.0f,
		0.0f,
		32.0f));
	mGBufferTerrainRS->Finalize(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	mGBufferTerrain = new Engine::D3DShader("../Data/SkyeCuillin/GBufferTerrain.hlsl",
		std::vector<Engine::D3DShader::ShaderEntryPoint>
		{
			Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::VERTEX_SHADER, "VS"),
				Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::HULL_SHADER, "HS"),
				Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::DOMAIN_SHADER, "DS"),
				Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::PIXEL_SHADER, "PS")
		},
		std::vector<Engine::D3DShader::ShaderDefine>());

	mGBufferTerrainPS = new Engine::PipelineState(mRenderer->GetDevice(),
		mGBufferTerrainRS,
		mGBufferTerrain,
		Engine::BlendState::CreateStateD3D(true),
		Engine::RasterizerState::CreateStateD3D(),
		//Engine::RasterizerState::CreateStateD3D(Engine::RasterizerState::WIREFRAME),
		&dsState,
		&inputLayout,
		Engine::PipelineState::PrimitiveType::PATCH,
		rtvFormats,
		Engine::Graphics::Format::D32F,
		mSamples,
		0);

	mColorBuffers = new Engine::ColorBuffer*[2];
	mColorBuffers[0] = new Engine::ColorBuffer(renderer, 0.0f, 0.0f, 0.0f, 1.0f);
	mColorBuffers[0]->Create(mWidth, mHeight, 1, Engine::Graphics::Format::RGBA16F, false, mSamples, 0);
	mColorBuffers[1] = new Engine::ColorBuffer(renderer, 0.0f, 0.0f, 0.0f, 1.0f);
	mColorBuffers[1]->Create(mWidth, mHeight, 1, Engine::Graphics::Format::RGBA16F, false, mSamples, 0);
	mDepthBuffer = new Engine::DepthBuffer(mRenderer, 1.0f, 0);
	mDepthBuffer->Create(mWidth, mHeight, Engine::Graphics::Format::D32F, false, mSamples, 0);

	mCameraBuffer = new Engine::GpuMappedBuffer();
	mCameraBuffer->Init(mRenderer, 64, sizeof(float));
	mCameraBufferPtr = mCameraBuffer->Map();
}

RenderPassGBuffer::~RenderPassGBuffer()
{
	delete mGBufferTerrainRS;
	delete mGBufferTerrainPS;
	delete mGBufferTerrain;

	delete mGBufferRS;
	delete mGBufferPS;
	delete mGBuffer;

	delete mDepthBuffer;
	delete mColorBuffers[0];
	delete mColorBuffers[1];
	delete[] mColorBuffers;

	mCameraBuffer->Unmap();
	delete mCameraBuffer;
}

void RenderPassGBuffer::Process(Engine::Entity* camera, Engine::DescriptorHeap* heap, Engine::GraphicsContext* context, std::vector<Engine::RenderNode>& nodes, Engine::GpuMappedBuffer* matrices)
{
	if (camera->GameObject().Has<Engine::CameraComponent>())
	{
		Engine::Camera* cam = camera->GameObject().Get<Engine::CameraComponent>()->Get();

		Engine::mat4 viewMatrix = cam->GetViewMatrix();
		Engine::mat4 projectionMatrix = cam->GetProjectionMatrix();

		memcpy(mCameraBufferPtr, &viewMatrix, sizeof(float) * 16);
		memcpy((float*)mCameraBufferPtr + 16, &projectionMatrix, sizeof(float) * 16);

		context->SetPipelineState(mGBufferPS);
		context->SetRootSignature(mGBufferRS);
		context->SetDescriptorHeap(Engine::DescriptorHeap::CBV_SRV_UAV, heap);
		context->SetViewport(0.0f, 0.0f, (float)mWidth, (float)mHeight);
		context->SetScissorRect(0.0f, 0.0f, (float)mWidth, (float)mHeight);
		context->TransitionResource(mColorBuffers[0], D3D12_RESOURCE_STATE_RENDER_TARGET, false);
		context->TransitionResource(mColorBuffers[1], D3D12_RESOURCE_STATE_RENDER_TARGET, false);
		context->TransitionResource(mDepthBuffer, D3D12_RESOURCE_STATE_DEPTH_WRITE, true);
		context->SetRenderTargets(2, mColorBuffers, mDepthBuffer);
		context->ClearColor(mColorBuffers[0], 0.0f, 0.0f, 0.0f, 1.0f);
		context->ClearColor(mColorBuffers[1], 0.0f, 0.0f, 0.0f, 1.0f);
		context->ClearDepth(mDepthBuffer, 1.0f, 0);
		//context->SetPrimitiveTopology(Engine::Graphics::TRIANGLELIST);
		context->SetPrimitiveTopology(Engine::Graphics::PATCHLIST_3);
		context->SetConstantBuffer(0, mCameraBuffer->GetGpuVirtualAddress());

		bool matused = false;
		int batchBound = -1;
		for (const Engine::RenderNode& node : nodes)
		{
			bool hasMaterial = false;
			int batch = node.mID / 16;

			if (node.mObject->Has<Engine::MaterialComponent>())
			{
				context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(1, node.mObject->Get<Engine::MaterialComponent>()->GetDiffuseMap()->GetSRV().mGpuHandle);
				context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(2, node.mObject->Get<Engine::MaterialComponent>()->GetNormalsMap()->GetSRV().mGpuHandle);
				context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(3, node.mObject->Get<Engine::MaterialComponent>()->GetMetallicMap()->GetSRV().mGpuHandle);
				context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(4, node.mObject->Get<Engine::MaterialComponent>()->GetRoughnessMap()->GetSRV().mGpuHandle);
				context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(5, node.mObject->Get<Engine::MaterialComponent>()->GetHeightMap()->GetSRV().mGpuHandle);
				hasMaterial = true;
				matused = true;
			}

			if (node.mObject->Has<Engine::MeshComponent>() && hasMaterial)
			{
				if (batchBound != batch)
				{
					batchBound = batch;
					context->SetConstantBuffer(6, matrices->GetGpuVirtualAddress(batch * 16 * sizeof(float) * 16 * 2));
				}
				context->SetConstants(7, Engine::DWParam(node.mID % 16));

				node.mObject->Get<Engine::MeshComponent>()->GetMesh()->Render(context);
			}
		}

		context->SetPipelineState(mGBufferTerrainPS);
		context->SetRootSignature(mGBufferTerrainRS);

		for (const Engine::RenderNode& node : nodes)
		{
			int batch = node.mID / 16;

			if (node.mObject->Has<Engine::TerrainComponent>() && matused)
			{
				if (batchBound != batch)
				{
					batchBound = batch;
					context->SetConstantBuffer(6, matrices->GetGpuVirtualAddress(batch * 16 * sizeof(float) * 16 * 2));
				}
				context->SetConstants(7, Engine::DWParam(node.mID % 16));

				node.mObject->Get<Engine::TerrainComponent>()->GetTerrain()->Render(context);
			}
		}

		context->TransitionResource(mColorBuffers[0], D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, false);
		context->TransitionResource(mColorBuffers[1], D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, false);
		context->TransitionResource(mDepthBuffer, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, true);
	}
}

void RenderPassGBuffer::Resize(unsigned int width, unsigned int height)
{
	mWidth = width;
	mHeight = height;

	mColorBuffers[0]->Release();
	mColorBuffers[0]->Create(mWidth, mHeight, 1, Engine::Graphics::Format::RGBA16F, false, mSamples, 0);
	mColorBuffers[1]->Release();
	mColorBuffers[1]->Create(mWidth, mHeight, 1, Engine::Graphics::Format::RGBA16F, false, mSamples, 0);

	mDepthBuffer->Release();
	mDepthBuffer->Create(mWidth, mHeight, Engine::Graphics::Format::D32F, false, mSamples, 0);
}