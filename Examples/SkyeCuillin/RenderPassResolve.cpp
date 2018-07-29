#include "RenderPassResolve.h"
#include "Game/Loader/DevIL.h"

using namespace SkyeCuillin;

RenderPassResolve::RenderPassResolve(Engine::Log* log, Engine::D3DRenderer* renderer, unsigned int width, unsigned int height, unsigned int samples) :
	RenderPassFullscreen(renderer, width, height)
{
	mCurrentLuminanceTarget = 0;

	mResolveRS = new Engine::RootSignature(renderer->GetDevice(), 9, 1);
	(*mResolveRS)[0].InitAsDescriptorTable(1);
	(*mResolveRS)[0].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);
	(*mResolveRS)[1].InitAsConstantBuffer(0);
	(*mResolveRS)[2].InitAsDescriptorTable(1);
	(*mResolveRS)[2].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
	(*mResolveRS)[3].InitAsDescriptorTable(1);
	(*mResolveRS)[3].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 1);
	(*mResolveRS)[4].InitAsDescriptorTable(1);
	(*mResolveRS)[4].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 1);
	(*mResolveRS)[5].InitAsDescriptorTable(1);
	(*mResolveRS)[5].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 4, 1);
	(*mResolveRS)[6].InitAsDescriptorTable(1);
	(*mResolveRS)[6].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 5, 1);
	(*mResolveRS)[7].InitAsDescriptorTable(1);
	(*mResolveRS)[7].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 6, 1);
	(*mResolveRS)[8].InitAsDescriptorTable(1);
	(*mResolveRS)[8].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 7, 1);
	mResolveRS->InitStaticSampler(0, Engine::Sampler::CreateSamplerD3D(Engine::Sampler::Filter::MIN_LINEAR_MAG_LINEAR_MIP_LINEAR,
		Engine::Sampler::Address::CLAMP,
		Engine::Sampler::Address::CLAMP,
		Engine::Sampler::Address::CLAMP,
		0.0f,
		1,
		Engine::Sampler::ComparisonFunc::NEVER,
		1.0f, 1.0f, 1.0f, 1.0f,
		0.0f,
		32.0f));
	mResolveRS->Finalize(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	mResolve = new Engine::D3DShader("../Data/SkyeCuillin/Post.hlsl",
		std::vector<Engine::D3DShader::ShaderEntryPoint>
		{
			Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::VERTEX_SHADER, "VS"),
				Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::PIXEL_SHADER, "PS")
		},
		std::vector<Engine::D3DShader::ShaderDefine>
		{
			Engine::D3DShader::ShaderDefine("SamplesMSAA", std::to_string(samples))
		});

	Engine::InputLayout inputLayout = Engine::InputLayout();
	inputLayout.AddAttribute(Engine::InputLayout::Attribute("POSITION", 0, Engine::Graphics::Format::RGB32F, 0, 0, Engine::InputLayout::Classification::PER_VERTEX, 0));
	inputLayout.AddAttribute(Engine::InputLayout::Attribute("TEXCOORD", 0, Engine::Graphics::Format::RG32F, 0, 12, Engine::InputLayout::Classification::PER_VERTEX, 0));

	Engine::DepthStencilState dsState = Engine::DepthStencilState(false);

	std::vector<Engine::Graphics::Format> rtvFormats;
	rtvFormats.push_back(Engine::Graphics::Format::RGBA8);

	mResolvePS = new Engine::PipelineState(mRenderer->GetDevice(),
		mResolveRS,
		mResolve,
		Engine::BlendState::CreateStateD3D(),
		Engine::RasterizerState::CreateStateD3D(),
		&dsState,
		&inputLayout,
		Engine::PipelineState::PrimitiveType::TRIANGLE,
		rtvFormats,
		Engine::Graphics::Format::D32F,
		1,
		0);
	
	mConstBuffer = new Engine::GpuMappedBuffer();
	mConstBuffer->Init(mRenderer, 64, sizeof(float));
	mConstBufferPtr = mConstBuffer->Map();

	// Initial luminance buffer & pipeline state
	mInitialLuminanceRT = new Engine::ColorBuffer(mRenderer, 0.0f, 0.0f, 0.0f, 0.0f);
	mInitialLuminanceRT->Create(1024, 1024, 1, Engine::Graphics::Format::R32F);
	mInitialLuminance = new Engine::D3DShader("../Data/SkyeCuillin/Post.hlsl",
		std::vector<Engine::D3DShader::ShaderEntryPoint>
		{
			Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::VERTEX_SHADER, "VS"),
			Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::PIXEL_SHADER, "LuminanceMap")
		},
		std::vector<Engine::D3DShader::ShaderDefine>
		{
			Engine::D3DShader::ShaderDefine("SamplesMSAA", std::to_string(samples))
		});
	mInitialLuminancePS = new Engine::PipelineState(mRenderer->GetDevice(),
		mResolveRS,
		mInitialLuminance,
		Engine::BlendState::CreateStateD3D(),
		Engine::RasterizerState::CreateStateD3D(),
		&dsState,
		&inputLayout,
		Engine::PipelineState::PrimitiveType::TRIANGLE,
		std::vector<Engine::Graphics::Format> { Engine::Graphics::Format::R32F },
		Engine::Graphics::Format::D32F,
		1,
		0);

	// Adapt luminance buffers & pipeline state 
	mAdaptLuminanceRT[0] = new Engine::ColorBuffer(mRenderer, 0.0f, 0.0f, 0.0f, 0.0f);
	mAdaptLuminanceRT[0]->Create(1024, 1024, 11, Engine::Graphics::Format::R32F);
	mAdaptLuminanceRT[1] = new Engine::ColorBuffer(mRenderer, 0.0f, 0.0f, 0.0f, 0.0f);
	mAdaptLuminanceRT[1]->Create(1024, 1024, 11, Engine::Graphics::Format::R32F);
	mAdaptLuminance = new Engine::D3DShader("../Data/SkyeCuillin/Post.hlsl",
		std::vector<Engine::D3DShader::ShaderEntryPoint>
	{
		Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::VERTEX_SHADER, "VS"),
			Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::PIXEL_SHADER, "AdaptLuminance")
	},
		std::vector<Engine::D3DShader::ShaderDefine>
		{
			Engine::D3DShader::ShaderDefine("SamplesMSAA", std::to_string(samples))
		});
	mAdaptLuminancePS = new Engine::PipelineState(mRenderer->GetDevice(),
		mResolveRS,
		mAdaptLuminance,
		Engine::BlendState::CreateStateD3D(),
		Engine::RasterizerState::CreateStateD3D(),
		&dsState,
		&inputLayout,
		Engine::PipelineState::PrimitiveType::TRIANGLE,
		std::vector<Engine::Graphics::Format> { Engine::Graphics::Format::R32F },
		Engine::Graphics::Format::D32F,
		1,
		0);

	// Bloom buffers & pipeline state 
	mBloomRT[0] = new Engine::ColorBuffer(mRenderer, 0.0f, 0.0f, 0.0f, 0.0f);
	mBloomRT[0]->Create(mWidth/ 2, mHeight/ 2, 1, Engine::Graphics::Format::RGBA16F);
	mBloomRT[1] = new Engine::ColorBuffer(mRenderer, 0.0f, 0.0f, 0.0f, 0.0f);
	mBloomRT[1]->Create(mWidth/ 2, mHeight/ 2, 1, Engine::Graphics::Format::RGBA16F);
	mBloom[0] = new Engine::D3DShader("../Data/SkyeCuillin/Post.hlsl",
		std::vector<Engine::D3DShader::ShaderEntryPoint>
		{
			Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::VERTEX_SHADER, "VS"),
			Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::PIXEL_SHADER, "BloomFirstPass")
		},
		std::vector<Engine::D3DShader::ShaderDefine>
		{
			Engine::D3DShader::ShaderDefine("SamplesMSAA", std::to_string(samples))
		});
	mBloom[1] = new Engine::D3DShader("../Data/SkyeCuillin/Post.hlsl",
		std::vector<Engine::D3DShader::ShaderEntryPoint>
		{
			Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::VERTEX_SHADER, "VS"),
			Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::PIXEL_SHADER, "BloomSecondPass")
		},
		std::vector<Engine::D3DShader::ShaderDefine>
		{
			Engine::D3DShader::ShaderDefine("SamplesMSAA", std::to_string(samples))
		});
	mBloomPS[0] = new Engine::PipelineState(mRenderer->GetDevice(),
		mResolveRS,
		mBloom[0],
		Engine::BlendState::CreateStateD3D(),
		Engine::RasterizerState::CreateStateD3D(),
		&dsState,
		&inputLayout,
		Engine::PipelineState::PrimitiveType::TRIANGLE,
		std::vector<Engine::Graphics::Format> { Engine::Graphics::Format::RGBA16F },
		Engine::Graphics::Format::D32F,
		1,
		0);
	mBloomPS[1] = new Engine::PipelineState(mRenderer->GetDevice(),
		mResolveRS,
		mBloom[1],
		Engine::BlendState::CreateStateD3D(),
		Engine::RasterizerState::CreateStateD3D(),
		&dsState,
		&inputLayout,
		Engine::PipelineState::PrimitiveType::TRIANGLE,
		std::vector<Engine::Graphics::Format> { Engine::Graphics::Format::RGBA16F },
		Engine::Graphics::Format::D32F,
		1,
		0);

	mMipmapRS = new Engine::RootSignature(mRenderer->GetDevice(), 6, 1);
	(*mMipmapRS)[0].InitAsConstants(0, 4);
	(*mMipmapRS)[1].InitAsDescriptorTable(1);
	(*mMipmapRS)[1].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);
	(*mMipmapRS)[2].InitAsDescriptorTable(1);
	(*mMipmapRS)[2].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0, 1);
	(*mMipmapRS)[3].InitAsDescriptorTable(1);
	(*mMipmapRS)[3].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 1);
	(*mMipmapRS)[4].InitAsDescriptorTable(1);
	(*mMipmapRS)[4].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 2, 1);
	(*mMipmapRS)[5].InitAsDescriptorTable(1);
	(*mMipmapRS)[5].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 3, 1);
	mMipmapRS->InitStaticSampler(0, Engine::Sampler::CreateSamplerD3D(Engine::Sampler::Filter::MIN_LINEAR_MAG_LINEAR_MIP_LINEAR,
		Engine::Sampler::Address::WRAP,
		Engine::Sampler::Address::WRAP,
		Engine::Sampler::Address::WRAP,
		0.0f,
		1,
		Engine::Sampler::ComparisonFunc::NEVER,
		1.0f, 1.0f, 1.0f, 1.0f,
		0.0f,
		32.0f));
	mMipmapRS->Finalize(D3D12_ROOT_SIGNATURE_FLAG_NONE);

	mMipmap = new Engine::D3DShader("../Data/SkyeCuillin/Mipmap.hlsl",
		std::vector<Engine::D3DShader::ShaderEntryPoint>
	{
		Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::COMPUTE_SHADER, "GenerateMipmaps")
	});

	mMipmapPS = new Engine::PipelineState(renderer->GetDevice(),
		mMipmapRS,
		mMipmap);

	ShoulderStrength = 0.22f;
	LinearStrength = 0.3f;
	LinearAngle = 0.1f;
	ToeStrength = 0.2f;
	ToeNumerator = 0.01f;
	ToeDenominator = 0.3f;
	LinearWhite = 11.2f;
	BloomBlurSigma = 7.0f;
	//BloomThreshold = 2.0f;
	BloomThreshold = 4.0f;
	BloomMagnitude = 1.2f;

	Engine::LoaderDevIL loader(log);
	Engine::LoaderDevIL::Image* image = loader.Load("../Data/SkyeCuillin/lensdirt1.png");
	if (image != nullptr)
	{
		loader.GenerateMipmaps(image);
		mLensDirt = new Engine::Texture();
		mLensDirt->InitMipmaps(mRenderer, image->mWidth, image->mHeight, 1, image->mMipLevels, Engine::Graphics::RGBA8, (const void**)image->mMipmaps);
		mLensDirt->SetName("LensDirt");
	}
	else
	{
		log->Print("SkyeCuillin::RenderPassResolve", std::string("Error: Requested texture image '../Data/SkyeCuillin/lensdirt1.png' does not exists!"));
	}
}

RenderPassResolve::~RenderPassResolve()
{
	delete mLensDirt;

	delete mResolveRS;
	delete mResolvePS;
	delete mResolve;

	delete mInitialLuminanceRT;
	delete mInitialLuminancePS;
	delete mInitialLuminance;

	delete mAdaptLuminanceRT[0];
	delete mAdaptLuminanceRT[1];
	delete mAdaptLuminancePS;
	delete mAdaptLuminance;

	delete mBloomRT[0];
	delete mBloomRT[1];
	delete mBloomPS[0];
	delete mBloomPS[1];
	delete mBloom[0];
	delete mBloom[1];

	delete mMipmapRS;
	delete mMipmapPS;
	delete mMipmap;

	mConstBuffer->Unmap();
	delete mConstBuffer;
}

void RenderPassResolve::GenerateMipmaps(Engine::DescriptorHeap* heap, Engine::ComputeContext* context, Engine::ColorBuffer* image)
{
	context->SetPipelineState(mMipmapPS);
	context->SetRootSignature(mMipmapRS);
	context->SetDescriptorHeap(Engine::DescriptorHeap::CBV_SRV_UAV, heap);

	int todo = image->GetNumMipMaps() - 1;
	int base = 0;
	int dimension[2] = { 1024, 1024 };

	//context->TransitionResource(image, D3D12_RESOURCE_STATE_GENERIC_READ);

	while (todo != 0)
	{
		int mipLevels = 1;

		context->SetConstants(0, Engine::DWParam(base), Engine::DWParam(mipLevels), Engine::DWParam(1.0f / (float)dimension[0]), Engine::DWParam(1.0f / (float)dimension[1]));
		context->SetDescriptorTable(1, image->GetSRV());
		context->SetDescriptorTable(2, image->GetUAV(base + 1));
		context->SetDescriptorTable(3, image->GetUAV(base + 1));
		context->SetDescriptorTable(4, image->GetUAV(base + 1));
		context->SetDescriptorTable(5, image->GetUAV(base + 1));
		context->Dispatch(dimension[0] / 2, dimension[1] / 2, 1);

		todo -= 1;
		base += 1;
		dimension[0] /= 2;
		if (dimension[0] <= 2) { dimension[0] = 2; }
		dimension[1] /= 2;
		if (dimension[1] <= 2) { dimension[1] = 2; }
	}

	context->TransitionResource(image, D3D12_RESOURCE_STATE_GENERIC_READ);
}

void RenderPassResolve::Process(Engine::D3DSwapChain* swapChain, Engine::DescriptorHeap* heap, Engine::GraphicsContext* context)
{
	float offset[2] = { 1.0f / (float)mWidth, 1.0f / (float)mHeight };

	memcpy((unsigned int*)mConstBufferPtr, &mWidth, sizeof(unsigned int));
	memcpy((unsigned int*)mConstBufferPtr + 1, &mHeight, sizeof(unsigned int));
	memcpy((float*)mConstBufferPtr + 2, &(offset[0]), sizeof(float));
	memcpy((float*)mConstBufferPtr + 3, &(offset[1]), sizeof(float));
	((float*)mConstBufferPtr + 4)[0] = 1.0f / 60.0f;
	((float*)mConstBufferPtr + 5)[0] = 1.25f;
	((float*)mConstBufferPtr + 6)[0] = ShoulderStrength;
	((float*)mConstBufferPtr + 7)[0] = LinearStrength;
	((float*)mConstBufferPtr + 8)[0] = LinearAngle;
	((float*)mConstBufferPtr + 9)[0] = ToeStrength;
	((float*)mConstBufferPtr + 10)[0] = ToeNumerator;
	((float*)mConstBufferPtr + 11)[0] = ToeDenominator;
	((float*)mConstBufferPtr + 12)[0] = LinearWhite;
	((float*)mConstBufferPtr + 13)[0] = BloomBlurSigma;
	((float*)mConstBufferPtr + 14)[0] = BloomThreshold;
	((float*)mConstBufferPtr + 15)[0] = BloomMagnitude;
	
	context->TransitionResource(mInitialLuminanceRT, D3D12_RESOURCE_STATE_RENDER_TARGET, true);
	context->ClearColor(mInitialLuminanceRT, 0.0f, 0.0f, 0.0f, 0.0f);
	context->SetPipelineState(mInitialLuminancePS);
	context->SetRootSignature(mResolveRS);
	context->SetDescriptorHeap(Engine::DescriptorHeap::CBV_SRV_UAV, heap);
	context->SetViewport(0.0f, 0.0f, 1024.0f, 1024.0f);
	context->SetScissorRect(0.0f, 0.0f, 1024.0f, 1024.0f);
	context->SetRenderTargets(1, &mInitialLuminanceRT);
	context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(0, mInput->GetSRV().mGpuHandle);
	context->SetConstantBuffer(1, mConstBuffer->GetGpuVirtualAddress());
	context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(2, mInputDepth->GetDepthSRV().mGpuHandle);
	context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(3, mInitialLuminanceRT->GetSRV().mGpuHandle);
	context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(4, mAdaptLuminanceRT[!mCurrentLuminanceTarget]->GetSRV().mGpuHandle);
	context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(5, mAdaptLuminanceRT[mCurrentLuminanceTarget]->GetSRV().mGpuHandle);
	context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(6, mBloomRT[0]->GetSRV().mGpuHandle);
	context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(7, mBloomRT[1]->GetSRV().mGpuHandle);
	context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(8, mLensDirt->GetSRV().mGpuHandle);
	mMesh->Render(context);
	context->TransitionResource(mInitialLuminanceRT, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	
	context->TransitionResource(mAdaptLuminanceRT[mCurrentLuminanceTarget], D3D12_RESOURCE_STATE_RENDER_TARGET, true);
	context->ClearColor(mAdaptLuminanceRT[mCurrentLuminanceTarget], 0.0f, 0.0f, 0.0f, 0.0f);
	context->SetPipelineState(mAdaptLuminancePS);
	context->SetRootSignature(mResolveRS);
	context->SetDescriptorHeap(Engine::DescriptorHeap::CBV_SRV_UAV, heap);
	context->SetViewport(0.0f, 0.0f, 1024.0f, 1024.0f);
	context->SetScissorRect(0.0f, 0.0f, 1024.0f, 1024.0f);
	context->SetRenderTargets(1, &mAdaptLuminanceRT[mCurrentLuminanceTarget]);
	context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(0, mInput->GetSRV().mGpuHandle);
	context->SetConstantBuffer(1, mConstBuffer->GetGpuVirtualAddress());
	context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(2, mInputDepth->GetDepthSRV().mGpuHandle);
	context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(3, mInitialLuminanceRT->GetSRV().mGpuHandle);
	context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(4, mAdaptLuminanceRT[!mCurrentLuminanceTarget]->GetSRV().mGpuHandle);
	context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(5, mAdaptLuminanceRT[mCurrentLuminanceTarget]->GetSRV().mGpuHandle);
	context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(6, mBloomRT[0]->GetSRV().mGpuHandle);
	context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(7, mBloomRT[1]->GetSRV().mGpuHandle);
	context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(8, mLensDirt->GetSRV().mGpuHandle);
	mMesh->Render(context);
	context->TransitionResource(mAdaptLuminanceRT[mCurrentLuminanceTarget], D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	GenerateMipmaps(heap, reinterpret_cast<Engine::ComputeContext*>(context), mAdaptLuminanceRT[mCurrentLuminanceTarget]);

	context->TransitionResource(mBloomRT[0], D3D12_RESOURCE_STATE_RENDER_TARGET, true);
	context->ClearColor(mBloomRT[0], 0.0f, 0.0f, 0.0f, 0.0f);
	context->SetPipelineState(mBloomPS[0]);
	context->SetRootSignature(mResolveRS);
	context->SetDescriptorHeap(Engine::DescriptorHeap::CBV_SRV_UAV, heap);
	context->SetViewport(0.0f, 0.0f, (float)(mWidth/ 2), (float)(mHeight/ 2));
	context->SetScissorRect(0.0f, 0.0f, (float)(mWidth/ 2), (float)(mHeight/ 2));
	context->SetRenderTargets(1, &mBloomRT[0]);
	context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(0, mInput->GetSRV().mGpuHandle);
	context->SetConstantBuffer(1, mConstBuffer->GetGpuVirtualAddress());
	context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(2, mInputDepth->GetDepthSRV().mGpuHandle);
	context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(3, mInitialLuminanceRT->GetSRV().mGpuHandle);
	context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(4, mAdaptLuminanceRT[!mCurrentLuminanceTarget]->GetSRV().mGpuHandle);
	context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(5, mAdaptLuminanceRT[mCurrentLuminanceTarget]->GetSRV().mGpuHandle);
	context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(6, mBloomRT[0]->GetSRV().mGpuHandle);
	context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(7, mBloomRT[1]->GetSRV().mGpuHandle);
	context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(8, mLensDirt->GetSRV().mGpuHandle);
	mMesh->Render(context);
	context->TransitionResource(mBloomRT[0], D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	context->TransitionResource(mBloomRT[1], D3D12_RESOURCE_STATE_RENDER_TARGET, true);
	context->ClearColor(mBloomRT[1], 0.0f, 0.0f, 0.0f, 0.0f);
	context->SetPipelineState(mBloomPS[1]);
	context->SetRootSignature(mResolveRS);
	context->SetDescriptorHeap(Engine::DescriptorHeap::CBV_SRV_UAV, heap);
	context->SetViewport(0.0f, 0.0f, (float)(mWidth/ 2), (float)(mHeight/ 2));
	context->SetScissorRect(0.0f, 0.0f, (float)(mWidth/ 2), (float)(mHeight/ 2));
	context->SetRenderTargets(1, &mBloomRT[1]);
	context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(0, mInput->GetSRV().mGpuHandle);
	context->SetConstantBuffer(1, mConstBuffer->GetGpuVirtualAddress());
	context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(2, mInputDepth->GetDepthSRV().mGpuHandle);
	context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(3, mInitialLuminanceRT->GetSRV().mGpuHandle);
	context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(4, mAdaptLuminanceRT[!mCurrentLuminanceTarget]->GetSRV().mGpuHandle);
	context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(5, mAdaptLuminanceRT[mCurrentLuminanceTarget]->GetSRV().mGpuHandle);
	context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(6, mBloomRT[0]->GetSRV().mGpuHandle);
	context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(7, mBloomRT[1]->GetSRV().mGpuHandle);
	context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(8, mLensDirt->GetSRV().mGpuHandle);
	mMesh->Render(context);
	context->TransitionResource(mBloomRT[1], D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	context->TransitionResource(swapChain->GetBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, true);
	context->SetRenderTargets(swapChain->GetBackBuffer(), swapChain->GetDepthBuffer());
	context->ClearColor(swapChain->GetBackBuffer(), 0.0f, 0.0f, 0.0f, 1.0f);
	context->ClearDepth(swapChain->GetDepthBuffer(), 1.0f, 0);
	context->SetPipelineState(mResolvePS);
	context->SetRootSignature(mResolveRS);
	context->SetDescriptorHeap(Engine::DescriptorHeap::CBV_SRV_UAV, heap);
	context->SetViewport(0.0f, 0.0f, (float)mWidth, (float)mHeight);
	context->SetScissorRect(0.0f, 0.0f, (float)mWidth, (float)mHeight);
	context->SetPrimitiveTopology(Engine::Graphics::TRIANGLELIST);
	context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(0, mInput->GetSRV().mGpuHandle);
	context->SetConstantBuffer(1, mConstBuffer->GetGpuVirtualAddress());
	context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(2, mInputDepth->GetDepthSRV().mGpuHandle);
	context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(3, mInitialLuminanceRT->GetSRV().mGpuHandle);
	context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(4, mAdaptLuminanceRT[!mCurrentLuminanceTarget]->GetSRV().mGpuHandle);
	context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(5, mAdaptLuminanceRT[mCurrentLuminanceTarget]->GetSRV().mGpuHandle);
	context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(6, mBloomRT[0]->GetSRV().mGpuHandle);
	context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(7, mBloomRT[1]->GetSRV().mGpuHandle);
	context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(8, mLensDirt->GetSRV().mGpuHandle);
	mMesh->Render(context);

	mCurrentLuminanceTarget = (mCurrentLuminanceTarget + 1) % 2;
}

void RenderPassResolve::Resize(unsigned int width, unsigned int height)
{
	mWidth = width;
	mHeight = height;

	mBloomRT[0]->Release();
	mBloomRT[0]->Create(mWidth/ 2, mHeight/ 2, 1, Engine::Graphics::Format::RGBA16F);

	mBloomRT[1]->Release();
	mBloomRT[1]->Create(mWidth/ 2, mHeight/ 2, 1, Engine::Graphics::Format::RGBA16F);
}