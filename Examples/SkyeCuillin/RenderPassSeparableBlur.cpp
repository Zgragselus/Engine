#include "RenderPassSeparableBlur.h"

using namespace SkyeCuillin;

RenderPassSeparableBlur::RenderPassSeparableBlur(Engine::D3DRenderer* renderer, unsigned int width, unsigned int height, Engine::Graphics::Format format) :
	RenderPassFullscreen(renderer, width, height)
{
	mFormat = format;

	mBlurRS = new Engine::RootSignature(renderer->GetDevice(), 3, 1);
	(*mBlurRS)[0].InitAsConstantBuffer(0);
	(*mBlurRS)[1].InitAsDescriptorTable(1);
	(*mBlurRS)[1].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);
	(*mBlurRS)[2].InitAsConstants(1, 1);
	mBlurRS->InitStaticSampler(0, Engine::Sampler::CreateSamplerD3D(Engine::Sampler::Filter::MIN_LINEAR_MAG_LINEAR_MIP_LINEAR,
		Engine::Sampler::Address::WRAP,
		Engine::Sampler::Address::WRAP,
		Engine::Sampler::Address::WRAP,
		0.0f,
		16,
		Engine::Sampler::ComparisonFunc::NEVER,
		1.0f, 1.0f, 1.0f, 1.0f,
		0.0f,
		32.0f));
	mBlurRS->Finalize(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	mBlur = new Engine::D3DShader("../Data/SkyeCuillin/Blur.hlsl",
		std::vector<Engine::D3DShader::ShaderEntryPoint>
	{
		Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::VERTEX_SHADER, "VS"),
		Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::PIXEL_SHADER, "PS")
	});

	Engine::InputLayout inputLayout = Engine::InputLayout();
	inputLayout.AddAttribute(Engine::InputLayout::Attribute("POSITION", 0, Engine::Graphics::Format::RGB32F, 0, 0, Engine::InputLayout::Classification::PER_VERTEX, 0));
	inputLayout.AddAttribute(Engine::InputLayout::Attribute("TEXCOORD", 0, Engine::Graphics::Format::RG32F, 0, 12, Engine::InputLayout::Classification::PER_VERTEX, 0));

	Engine::DepthStencilState dsState = Engine::DepthStencilState(false);

	std::vector<Engine::Graphics::Format> rtvFormats;
	rtvFormats.push_back(mFormat);

	mBlurPS = new Engine::PipelineState(mRenderer->GetDevice(),
		mBlurRS,
		mBlur,
		Engine::BlendState::CreateStateD3D(),
		Engine::RasterizerState::CreateStateD3D(),
		&dsState,
		&inputLayout,
		Engine::PipelineState::PrimitiveType::TRIANGLE,
		rtvFormats,
		Engine::Graphics::Format::D32F,
		1,
		0);

	mColorBuffer = new Engine::ColorBuffer(renderer, 0.0f, 0.0f, 0.0f, 1.0f);
	mColorBuffer->Create(mWidth, mHeight, 1, mFormat);

	mDepthBuffer = new Engine::DepthBuffer(mRenderer, 1.0f, 0);
	mDepthBuffer->Create(mWidth, mHeight, Engine::Graphics::Format::D32F);

	mConstBuffer = new Engine::GpuMappedBuffer();
	mConstBuffer->Init(mRenderer, 64, sizeof(float));
	mConstBufferPtr = mConstBuffer->Map();
}

RenderPassSeparableBlur::~RenderPassSeparableBlur()
{
	delete mBlurRS;
	delete mBlurPS;
	delete mBlur;

	delete mColorBuffer;
	delete mDepthBuffer;

	mConstBuffer->Unmap();
	delete mConstBuffer;
}

void RenderPassSeparableBlur::Process(Engine::DescriptorHeap* heap, Engine::GraphicsContext* context, unsigned int direction)
{
	float offset[2] = { 1.0f / (float)mWidth, 1.0f / (float)mHeight };

	memcpy((unsigned int*)mConstBufferPtr, &mWidth, sizeof(unsigned int));
	memcpy((unsigned int*)mConstBufferPtr + 1, &mHeight, sizeof(unsigned int));
	memcpy((float*)mConstBufferPtr + 2, &(offset[0]), sizeof(float));
	memcpy((float*)mConstBufferPtr + 3, &(offset[1]), sizeof(float));

	context->SetPipelineState(mBlurPS);
	context->SetRootSignature(mBlurRS);
	context->SetDescriptorHeap(Engine::DescriptorHeap::CBV_SRV_UAV, heap);
	context->SetViewport(0.0f, 0.0f, (float)mWidth, (float)mHeight);
	context->SetScissorRect(0.0f, 0.0f, (float)mWidth, (float)mHeight);
	if (mOutput == nullptr)
	{
		context->TransitionResource(mColorBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, false);
		context->TransitionResource(mDepthBuffer, D3D12_RESOURCE_STATE_DEPTH_WRITE, true);
		context->SetRenderTargets(mColorBuffer, mDepthBuffer);
		context->ClearColor(mColorBuffer, 0.0f, 0.0f, 0.0f, 1.0f);
		context->ClearDepth(mDepthBuffer, 1.0f, 0);
	}
	else
	{
		context->TransitionResource(mOutput, D3D12_RESOURCE_STATE_RENDER_TARGET, false);
		context->TransitionResource(mDepthBuffer, D3D12_RESOURCE_STATE_DEPTH_WRITE, true);
		context->SetRenderTargets(mOutput, mDepthBuffer);
		context->ClearColor(mOutput, 0.0f, 0.0f, 0.0f, 1.0f);
		context->ClearDepth(mDepthBuffer, 1.0f, 0);
	}
	context->SetPrimitiveTopology(Engine::Graphics::TRIANGLELIST);
	context->SetConstantBuffer(0, mConstBuffer->GetGpuVirtualAddress());

	if (mInput == nullptr)
	{
		context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(1, mColorBuffer->GetSRV().mGpuHandle);
	}
	else
	{
		context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(1, mInput->GetSRV().mGpuHandle);
	}
	
	context->SetConstants(2, Engine::DWParam(direction));

	mMesh->Render(context);

	if (mOutput == nullptr)
	{
		context->TransitionResource(mColorBuffer, D3D12_RESOURCE_STATE_GENERIC_READ, true);
	}
	else
	{
		context->TransitionResource(mOutput, D3D12_RESOURCE_STATE_GENERIC_READ, true);
	}
}

void RenderPassSeparableBlur::Resize(unsigned int width, unsigned int height)
{
	mWidth = width;
	mHeight = height;

	mColorBuffer->Release();
	mColorBuffer->Create(mWidth, mHeight, 1, mFormat);

	mDepthBuffer->Release();
	mDepthBuffer->Create(mWidth, mHeight, Engine::Graphics::Format::D32F);
}