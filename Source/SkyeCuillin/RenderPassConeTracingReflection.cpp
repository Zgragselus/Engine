#include "RenderPassConeTracingReflection.h"

using namespace SkyeCuillin;

RenderPassConeTracingReflection::RenderPassConeTracingReflection(Engine::D3DRenderer* renderer, unsigned int width, unsigned int height, unsigned int samples) :
	RenderPassFullscreen(renderer, width, height)
{
	mSamples = samples;

	mReflectionRS = new Engine::RootSignature(renderer->GetDevice(), 4, 1);
	(*mReflectionRS)[0].InitAsConstantBuffer(0);
	(*mReflectionRS)[1].InitAsDescriptorTable(1);
	(*mReflectionRS)[1].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);
	(*mReflectionRS)[2].InitAsDescriptorTable(1);
	(*mReflectionRS)[2].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
	(*mReflectionRS)[3].InitAsDescriptorTable(1);
	(*mReflectionRS)[3].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 1);
	mReflectionRS->InitStaticSampler(0, Engine::Sampler::CreateSamplerD3D(Engine::Sampler::Filter::MIN_LINEAR_MAG_LINEAR_MIP_LINEAR,
		Engine::Sampler::Address::WRAP,
		Engine::Sampler::Address::WRAP,
		Engine::Sampler::Address::WRAP,
		0.0f,
		16,
		Engine::Sampler::ComparisonFunc::NEVER,
		1.0f, 1.0f, 1.0f, 1.0f,
		0.0f,
		32.0f));
	mReflectionRS->Finalize(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	mReflection = new Engine::D3DShader("../Data/SkyeCuillin/ConeTracingReflection.hlsl",
		std::vector<Engine::D3DShader::ShaderEntryPoint>
		{
			Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::VERTEX_SHADER, "VS"),
			Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::PIXEL_SHADER, "PS")
		},
		std::vector<Engine::D3DShader::ShaderDefine>());

	Engine::InputLayout inputLayout = Engine::InputLayout();
	inputLayout.AddAttribute(Engine::InputLayout::Attribute("POSITION", 0, Engine::Graphics::Format::RGB32F, 0, 0, Engine::InputLayout::Classification::PER_VERTEX, 0));
	inputLayout.AddAttribute(Engine::InputLayout::Attribute("TEXCOORD", 0, Engine::Graphics::Format::RG32F, 0, 12, Engine::InputLayout::Classification::PER_VERTEX, 0));

	Engine::DepthStencilState dsState = Engine::DepthStencilState(false);

	std::vector<Engine::Graphics::Format> rtvFormats;
	rtvFormats.push_back(Engine::Graphics::Format::RGBA16F);

	mReflectionPS = new Engine::PipelineState(mRenderer->GetDevice(),
		mReflectionRS,
		mReflection,
		Engine::BlendState::CreateStateD3D(),
		Engine::RasterizerState::CreateStateD3D(),
		&dsState,
		&inputLayout,
		Engine::PipelineState::PrimitiveType::TRIANGLE,
		rtvFormats,
		Engine::Graphics::Format::D32F,
		mSamples,
		0);

	mColorBuffer = new Engine::ColorBuffer(renderer, 0.0f, 0.0f, 0.0f, 1.0f);
	mColorBuffer->Create(mWidth, mHeight, 1, Engine::Graphics::Format::RGBA16F, false, mSamples);

	mDepthBuffer = new Engine::DepthBuffer(mRenderer, 1.0f, 0);
	mDepthBuffer->Create(mWidth, mHeight, Engine::Graphics::Format::D32F, false, mSamples);

	mConstBuffer = new Engine::GpuMappedBuffer();
	mConstBuffer->Init(mRenderer, 64, sizeof(float));
	mConstBufferPtr = mConstBuffer->Map();
}

RenderPassConeTracingReflection::~RenderPassConeTracingReflection()
{
	delete mReflectionRS;
	delete mReflectionPS;
	delete mReflection;

	delete mColorBuffer;
	delete mDepthBuffer;

	mConstBuffer->Unmap();
	delete mConstBuffer;
}

void RenderPassConeTracingReflection::Process(Engine::Entity* camera, Engine::DescriptorHeap* heap, Engine::GraphicsContext* context, std::vector<Engine::RenderNode>& nodes)
{
	if (camera->GameObject().Has<Engine::CameraComponent>())
	{
		Engine::Camera* cam = camera->GameObject().Get<Engine::CameraComponent>()->Get();

		Engine::mat4 invProjectionMatrix = inverse(cam->GetProjectionMatrix());
		Engine::mat4 invViewMatrix = inverse(cam->GetViewMatrix());

		float resolution = (float)mVoxelData->GetResolution();
		float invResolution = 1.0f / resolution;

		memcpy(mConstBufferPtr, &invProjectionMatrix, sizeof(float) * 16);
		memcpy((float*)mConstBufferPtr + 16, &invViewMatrix, sizeof(float) * 16);

		memcpy((unsigned int*)mConstBufferPtr + 40, &mWidth, sizeof(unsigned int));
		memcpy((unsigned int*)mConstBufferPtr + 41, &mHeight, sizeof(unsigned int));
		memcpy((float*)mConstBufferPtr + 42, &resolution, sizeof(float));
		memcpy((float*)mConstBufferPtr + 43, &invResolution, sizeof(float));
		memcpy((float*)mConstBufferPtr + 44, &cam->GetPosition(), sizeof(float) * 4);

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

		((float*)mConstBufferPtr)[32] = maxOffset;
		((float*)mConstBufferPtr)[33] = maxOffset;
		((float*)mConstBufferPtr)[34] = maxOffset;
		((float*)mConstBufferPtr)[35] = 0.0f;
		((float*)mConstBufferPtr)[36] = 1.0f / maxSize;
		((float*)mConstBufferPtr)[37] = 1.0f / maxSize;
		((float*)mConstBufferPtr)[38] = 1.0f / maxSize;
		((float*)mConstBufferPtr)[39] = 0.0f;

		context->SetPipelineState(mReflectionPS);
		context->SetRootSignature(mReflectionRS);
		context->SetDescriptorHeap(Engine::DescriptorHeap::CBV_SRV_UAV, heap);
		context->SetViewport(0.0f, 0.0f, (float)mWidth, (float)mHeight);
		context->SetScissorRect(0.0f, 0.0f, (float)mWidth, (float)mHeight);
		context->TransitionResource(mColorBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, false);
		context->TransitionResource(mDepthBuffer, D3D12_RESOURCE_STATE_DEPTH_WRITE, true);
		context->SetRenderTargets(mColorBuffer, mDepthBuffer);
		context->ClearColor(mColorBuffer, 0.0f, 0.0f, 0.0f, 1.0f);
		context->ClearDepth(mDepthBuffer, 1.0f, 0);
		context->SetPrimitiveTopology(Engine::Graphics::TRIANGLELIST);
		context->SetConstantBuffer(0, mConstBuffer->GetGpuVirtualAddress());

		context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(1, mSourceDepth->GetDepthSRV().mGpuHandle);
		context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(2, mSourceNormals->GetSRV().mGpuHandle);
		context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(3, mVoxelData->GetColorTexture()->GetSRV().mGpuHandle);

		mMesh->Render(context);

		context->TransitionResource(mColorBuffer, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, true);
	}
}

void RenderPassConeTracingReflection::Resize(unsigned int width, unsigned int height)
{
	mWidth = width;
	mHeight = height;

	mColorBuffer->Release();
	mColorBuffer->Create(mWidth, mHeight, 1, Engine::Graphics::Format::RGBA8, false, mSamples);

	mDepthBuffer->Release();
	mDepthBuffer->Create(mWidth, mHeight, Engine::Graphics::Format::D32F, false, mSamples);
}