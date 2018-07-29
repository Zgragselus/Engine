#include "RenderPassDeferredShading.h"

using namespace SkyeCuillin;

RenderPassDeferredShading::RenderPassDeferredShading(Engine::D3DRenderer* renderer, LightingSystem* lightingSystem, unsigned int width, unsigned int height, unsigned int samples, unsigned int samplesGI) :
	RenderPassFullscreen(renderer, width, height)
{
	mLightingSystem = lightingSystem;

	mSamples = samples;

	mDeferredRS = new Engine::RootSignature(renderer->GetDevice(), 11, 1);
	(*mDeferredRS)[0].InitAsConstantBuffer(0);
	(*mDeferredRS)[1].InitAsDescriptorTable(1);
	(*mDeferredRS)[1].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);
	(*mDeferredRS)[2].InitAsDescriptorTable(1);
	(*mDeferredRS)[2].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
	(*mDeferredRS)[3].InitAsDescriptorTable(1);
	(*mDeferredRS)[3].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 1);
	(*mDeferredRS)[4].InitAsDescriptorTable(1);
	(*mDeferredRS)[4].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 1);
	(*mDeferredRS)[5].InitAsConstants(1, 1);
	(*mDeferredRS)[6].InitAsDescriptorTable(1);
	(*mDeferredRS)[6].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 4, 1);
	(*mDeferredRS)[7].InitAsDescriptorTable(1);
	(*mDeferredRS)[7].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 5, 1);
	(*mDeferredRS)[8].InitAsDescriptorTable(1);
	(*mDeferredRS)[8].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 6, 1);
	(*mDeferredRS)[9].InitAsDescriptorTable(1);
	(*mDeferredRS)[9].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 7, 1);
	(*mDeferredRS)[10].InitAsDescriptorTable(1);
	(*mDeferredRS)[10].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 8, 1);
	mDeferredRS->InitStaticSampler(0, Engine::Sampler::CreateSamplerD3D(Engine::Sampler::Filter::MIN_POINT_MAG_POINT_MIP_POINT,
		Engine::Sampler::Address::WRAP,
		Engine::Sampler::Address::WRAP,
		Engine::Sampler::Address::WRAP,
		0.0f,
		16,
		Engine::Sampler::ComparisonFunc::NEVER,
		1.0f, 1.0f, 1.0f, 1.0f,
		0.0f,
		32.0f));
	mDeferredRS->Finalize(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	
	mDeferred = new Engine::D3DShader("../Data/SkyeCuillin/DeferredResolve.hlsl",
		std::vector<Engine::D3DShader::ShaderEntryPoint>
		{
			Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::VERTEX_SHADER, "VS"),
			Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::PIXEL_SHADER, "PS")
		},
		std::vector<Engine::D3DShader::ShaderDefine>
		{
			Engine::D3DShader::ShaderDefine("SamplesMSAA", std::to_string(samples)),
			Engine::D3DShader::ShaderDefine("SamplesMSAAGI", std::to_string(samplesGI))
		});

	Engine::InputLayout inputLayout = Engine::InputLayout();
	inputLayout.AddAttribute(Engine::InputLayout::Attribute("POSITION", 0, Engine::Graphics::Format::RGB32F, 0, 0, Engine::InputLayout::Classification::PER_VERTEX, 0));
	inputLayout.AddAttribute(Engine::InputLayout::Attribute("TEXCOORD", 0, Engine::Graphics::Format::RG32F, 0, 12, Engine::InputLayout::Classification::PER_VERTEX, 0));

	Engine::DepthStencilState dsState = Engine::DepthStencilState(false);

	std::vector<Engine::Graphics::Format> rtvFormats;
	rtvFormats.push_back(Engine::Graphics::Format::RGBA16F);

	mDeferredPS = new Engine::PipelineState(mRenderer->GetDevice(),
		mDeferredRS,
		mDeferred,
		Engine::BlendState::CreateStateD3D(false, false, Engine::BlendState::BlendDesc(true, false, Engine::BlendState::ONE, Engine::BlendState::ONE, Engine::BlendState::OP_ADD)),
		Engine::RasterizerState::CreateStateD3D(),
		&dsState,
		&inputLayout,
		Engine::PipelineState::PrimitiveType::TRIANGLE,
		rtvFormats,
		Engine::Graphics::Format::D32F,
		mSamples,
		0);

	mColorBuffer = new Engine::ColorBuffer(renderer, 0.0f, 0.0f, 0.0f, 1.0f);
	mColorBuffer->Create(mWidth, mHeight, 1, Engine::Graphics::Format::RGBA16F, false, mSamples, 0);

	mDepthBuffer = new Engine::DepthBuffer(mRenderer, 1.0f, 0);
	mDepthBuffer->Create(mWidth, mHeight, Engine::Graphics::Format::D32F, false, mSamples, 0);

	mConstBuffer = new Engine::GpuMappedBuffer();
	mConstBuffer->Init(mRenderer, 64, sizeof(float));
	mConstBufferPtr = mConstBuffer->Map();
}

RenderPassDeferredShading::~RenderPassDeferredShading()
{
	delete mDeferredRS;
	delete mDeferredPS;
	delete mDeferred;

	delete mColorBuffer;
	delete mDepthBuffer;

	mConstBuffer->Unmap();
	delete mConstBuffer;
}

void RenderPassDeferredShading::Process(Engine::Entity* camera, Engine::DescriptorHeap* heap, Engine::GraphicsContext* context, std::vector<Engine::RenderNode>& nodes)
{
	if (camera->GameObject().Has<Engine::CameraComponent>())
	{
		Engine::Camera* cam = camera->GameObject().Get<Engine::CameraComponent>()->Get();

		Engine::mat4 invProjectionMatrix = inverse(cam->GetProjectionMatrix());
		Engine::mat4 invViewMatrix = inverse(cam->GetViewMatrix());

		memcpy(mConstBufferPtr, &invProjectionMatrix, sizeof(float) * 16);
		memcpy((float*)mConstBufferPtr + 16, &invViewMatrix, sizeof(float) * 16);

		memcpy((unsigned int*)mConstBufferPtr + 32, &mWidth, sizeof(unsigned int));
		memcpy((unsigned int*)mConstBufferPtr + 33, &mHeight, sizeof(unsigned int));

		context->SetPipelineState(mDeferredPS);
		context->SetRootSignature(mDeferredRS);
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

		context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(1, mSourceAlbedo->GetSRV().mGpuHandle);
		context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(2, mSourceNormals->GetSRV().mGpuHandle);
		context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(3, mSourceDepth->GetDepthSRV().mGpuHandle);
		context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(6, mSourceReflection->GetSRV().mGpuHandle);
		context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(7, mSourceAO->GetSRV().mGpuHandle);
		context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(8, mSourceGI->GetSRV().mGpuHandle);
		context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(9, mLightingSystem->GetLightBuffer()->GetSRV().mGpuHandle);
		context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(10, mLightingSystem->GetShadowAtlasBuffer()->GetSRV().mGpuHandle);
		context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(4, mLightingSystem->GetShadowMap()->GetSRV().mGpuHandle);
		
		context->SetConstants(5, Engine::DWParam(-1));
		mMesh->Render(context);

		for (int i = 0; i < mLightingSystem->GetLights(); i++)
		{
			context->SetConstants(5, Engine::DWParam(i));
			
			mMesh->Render(context);
		}
	}
}

void RenderPassDeferredShading::Resize(unsigned int width, unsigned int height)
{
	mWidth = width;
	mHeight = height;

	mColorBuffer->Release();
	mColorBuffer->Create(mWidth, mHeight, 1, Engine::Graphics::Format::RGBA16F, false, mSamples, 0);

	mDepthBuffer->Release();
	mDepthBuffer->Create(mWidth, mHeight, Engine::Graphics::Format::D32F, false, mSamples, 0);
}