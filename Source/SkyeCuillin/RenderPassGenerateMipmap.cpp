#include "RenderPassGenerateMipmap.h"

using namespace SkyeCuillin;

RenderPassGenerateMipmap::RenderPassGenerateMipmap(Engine::D3DRenderer* renderer, unsigned int width, unsigned int height) :
	RenderPass(renderer, width, height)
{
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
		},
		std::vector<Engine::D3DShader::ShaderDefine>());

	mMipmapPS = new Engine::PipelineState(renderer->GetDevice(),
		mMipmapRS,
		mMipmap);

	mTexture = new Engine::ColorBuffer(renderer, 0.0f, 0.0f, 0.0f, 1.0f);
	mTexture->Create(mWidth, mHeight, 0, Engine::Graphics::Format::RGBA16F, false);
	mMiplevels = mTexture->GetNumMipMaps();

	D3D12_PLACED_SUBRESOURCE_FOOTPRINT placedFootprint;
	mRenderer->GetDevice()->GetCopyableFootprints(&mTexture->Get()->GetDesc(), mMiplevels - 1, 1, 0, &placedFootprint, nullptr, nullptr, nullptr);
	mRowPitch = placedFootprint.Footprint.RowPitch;
	mReadbackBuffer = new Engine::GpuMappedBuffer();
	mReadbackBuffer->Init(mRenderer, placedFootprint.Footprint.RowPitch * placedFootprint.Footprint.Height, sizeof(unsigned short) * 4, Engine::GpuMappedBuffer::READBACK);
}

RenderPassGenerateMipmap::~RenderPassGenerateMipmap()
{
	delete mTexture;

	delete mMipmapRS;
	delete mMipmapPS;
	delete mMipmap;
}

void RenderPassGenerateMipmap::Process(Engine::DescriptorHeap* heap, Engine::ComputeContext* context, Engine::ColorBuffer* source)
{
	context->SetPipelineState(mMipmapPS);
	context->SetRootSignature(mMipmapRS);
	context->SetDescriptorHeap(Engine::DescriptorHeap::CBV_SRV_UAV, heap);
	
	int todo = mMiplevels - 1;
	int base = 0;
	int dimension[2] = { (int)mWidth, (int)mHeight };

	context->TransitionResource(source, D3D12_RESOURCE_STATE_GENERIC_READ);

	while (todo != 0)
	{
		int mipLevels = 1;

		context->SetConstants(0, Engine::DWParam(base), Engine::DWParam(mipLevels), Engine::DWParam(1.0f / (float)dimension[0]), Engine::DWParam(1.0f / (float)dimension[1]));
		if (dimension[0] == mWidth && dimension[1] == mHeight)
		{
			context->SetDescriptorTable(1, source->GetSRV());
		}
		else
		{
			context->SetDescriptorTable(1, mTexture->GetSRV());
		}
		context->SetDescriptorTable(2, mTexture->GetUAV(base + 1));
		context->SetDescriptorTable(3, mTexture->GetUAV(base + 1));
		context->SetDescriptorTable(4, mTexture->GetUAV(base + 1));
		context->SetDescriptorTable(5, mTexture->GetUAV(base + 1));
		context->Dispatch(dimension[0] / 2, dimension[1] / 2, 1);

		todo -= 1;
		base += 1;
		dimension[0] /= 2;
		if (dimension[0] <= 2) { dimension[0] = 2; }
		dimension[1] /= 2;
		if (dimension[1] <= 2) { dimension[1] = 2; }
	}

	context->TransitionResource(mTexture, D3D12_RESOURCE_STATE_GENERIC_READ);
}

void RenderPassGenerateMipmap::Readback(Engine::GraphicsContext* context)
{
	context->ReadbackTexture2D(mReadbackBuffer, mTexture, mMiplevels - 1);
}

void RenderPassGenerateMipmap::Resize(unsigned int width, unsigned int height)
{
	mWidth = width;
	mHeight = height;

	mTexture->Release();
	mTexture->Create(mWidth, mHeight, 0, Engine::Graphics::Format::RGBA8, false);
	mMiplevels = mTexture->GetNumMipMaps();

	D3D12_PLACED_SUBRESOURCE_FOOTPRINT placedFootprint;
	mRenderer->GetDevice()->GetCopyableFootprints(&mTexture->Get()->GetDesc(), mMiplevels - 1, 1, 0, &placedFootprint, nullptr, nullptr, nullptr);
	mRowPitch = placedFootprint.Footprint.RowPitch;
	delete mReadbackBuffer;
	mReadbackBuffer = new Engine::GpuMappedBuffer();
	mReadbackBuffer->Init(mRenderer, placedFootprint.Footprint.RowPitch * placedFootprint.Footprint.Height, sizeof(unsigned short) * 4, Engine::GpuMappedBuffer::READBACK);
}