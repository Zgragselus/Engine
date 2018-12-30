#include "RenderPassShadowMap.h"

using namespace SkyeCuillin;

RenderPassShadowMap::RenderPassShadowMap(Engine::D3DRenderer* renderer, unsigned int width, unsigned int height, float nearClip, float farClip) :
	RenderPass(renderer, width, height)
{
	mNear = nearClip;
	mFar = farClip;

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

	mDepthRS = new Engine::RootSignature(renderer->GetDevice(), 4, 1);
	(*mDepthRS)[0].InitAsConstantBuffer(0);
	(*mDepthRS)[1].InitAsDescriptorTable(1);
	(*mDepthRS)[1].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);
	(*mDepthRS)[2].InitAsConstantBuffer(1);
	(*mDepthRS)[3].InitAsConstants(2, 1);
	mDepthRS->InitStaticSampler(0, Engine::Sampler::CreateSamplerD3D(Engine::Sampler::Filter::ANISOTROPIC,
		Engine::Sampler::Address::WRAP,
		Engine::Sampler::Address::WRAP,
		Engine::Sampler::Address::WRAP,
		0.0f,
		16,
		Engine::Sampler::ComparisonFunc::NEVER,
		1.0f, 1.0f, 1.0f, 1.0f,
		0.0f,
		32.0f));
	mDepthRS->Finalize(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	mDepth = new Engine::D3DShader("../Data/SkyeCuillin/ShadowMap.hlsl",
		std::vector<Engine::D3DShader::ShaderEntryPoint>
		{
			Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::VERTEX_SHADER, "VS"),
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
	rtvFormats.push_back(Engine::Graphics::Format::RG32F);

	mDepthPS = new Engine::PipelineState(mRenderer->GetDevice(),
		mDepthRS,
		mDepth,
		Engine::BlendState::CreateStateD3D(true),
		//Engine::RasterizerState::CreateStateD3D(Engine::RasterizerState::SOLID, Engine::RasterizerState::FRONT),
		Engine::RasterizerState::CreateStateD3D(),
		&dsState,
		&inputLayout,
		Engine::PipelineState::PrimitiveType::TRIANGLE,
		rtvFormats,
		Engine::Graphics::Format::D32F,
		1,
		0);

	mShadowMap = new Engine::ColorBuffer(renderer, 0.0f, 0.0f, 0.0f, 1.0f);
	mShadowMap->Create(mWidth, mHeight, 0, Engine::Graphics::Format::RG32F, false);
	mMiplevels = mShadowMap->GetNumMipMaps();

	mDepthMap = new Engine::DepthBuffer(mRenderer, 1.0f, 0);
	mDepthMap->Create(mWidth, mHeight, Engine::Graphics::Format::D32F);

	mConstBuffer = new Engine::GpuMappedBuffer();
	mConstBuffer->Init(mRenderer, 64, sizeof(float));
	mConstBufferPtr = mConstBuffer->Map();

	mProjection = new Engine::mat4();
	mView = new Engine::mat4();
}

RenderPassShadowMap::~RenderPassShadowMap()
{
	delete mProjection;
	delete mView;

	delete mShadowMap;
	delete mDepthMap;

	delete mDepthRS;
	delete mDepthPS;
	delete mDepth;

	delete mMipmapRS;
	delete mMipmapPS;
	delete mMipmap;

	mConstBuffer->Unmap();
	delete mConstBuffer;
}

void RenderPassShadowMap::Process(Engine::Entity* light, Engine::DescriptorHeap* heap, Engine::GraphicsContext* context, std::vector<Engine::RenderNode>& nodes, Engine::GpuMappedBuffer* matrices)
{
	if (light->GameObject().Has<Engine::LightComponent>())
	{
		Engine::Light* l = light->GameObject().Get<Engine::LightComponent>()->GetLight();

		if (l->GetType() == Engine::Light::Type::SPOT)
		{
			Engine::SpotLight* spot = (Engine::SpotLight*)l;

			Engine::float4 position = Engine::float4(0.0f, 0.0f, 0.0f, 1.0f) * light->GetWorldMatrix();
			Engine::float4 up = Engine::float4(0.0f, 1.0f, 0.0f, 0.0f);
			Engine::float4 right = Engine::cross(spot->GetDirection(), up);
			up = Engine::cross(right, spot->GetDirection());

			Engine::mat4 viewMatrix = Engine::lookat(position, position + spot->GetDirection(), up);
			Engine::mat4 projectionMatrix = Engine::perspective(spot->GetSpotAngleRad() * 2.0f, 1.0f, mNear, mFar);
			(*mProjection) = projectionMatrix;
			(*mView) = viewMatrix;

			memcpy(mConstBufferPtr, &viewMatrix, sizeof(float) * 16);
			memcpy((float*)mConstBufferPtr + 16, &projectionMatrix, sizeof(float) * 16);

			context->SetPipelineState(mDepthPS);
			context->SetRootSignature(mDepthRS);
			context->SetDescriptorHeap(Engine::DescriptorHeap::CBV_SRV_UAV, heap);
			context->SetViewport(0.0f, 0.0f, (float)mWidth, (float)mHeight);
			context->SetScissorRect(0.0f, 0.0f, (float)mWidth, (float)mHeight);
			context->TransitionResource(mShadowMap, D3D12_RESOURCE_STATE_RENDER_TARGET, true);
			context->SetRenderTargets(mShadowMap, mDepthMap);
			context->ClearColor(mShadowMap, 0.0f, 0.0f, 0.0f, 1.0f);
			context->ClearDepth(mDepthMap, 1.0f, 0);
			context->SetPrimitiveTopology(Engine::Graphics::TRIANGLELIST);
			context->SetConstantBuffer(0, mConstBuffer->GetGpuVirtualAddress());

			int batchBound = -1;
			for (const Engine::RenderNode& node : nodes)
			{
				bool hasMaterial = false;
				int batch = node.mID / 16;

				if (node.mObject->Has<Engine::MaterialComponent>())
				{
					context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(1, node.mObject->Get<Engine::MaterialComponent>()->GetDiffuseMap()->GetSRV().mGpuHandle);
					hasMaterial = true;
				}

				if (node.mObject->Has<Engine::MeshComponent>() && hasMaterial)
				{
					if (batchBound != batch)
					{
						batchBound = batch;
						context->SetConstantBuffer(2, matrices->GetGpuVirtualAddress(batch * 16 * sizeof(float) * 16 * 2));
					}
					context->SetConstants(3, Engine::DWParam(node.mID % 16));
					node.mObject->Get<Engine::MeshComponent>()->GetMesh()->Render(context);
				}
			}

			context->TransitionResource(mShadowMap, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, true);
		}
	}
}

void RenderPassShadowMap::GenerateMipmaps(Engine::DescriptorHeap* heap, Engine::ComputeContext* context)
{
	context->SetPipelineState(mMipmapPS);
	context->SetRootSignature(mMipmapRS);
	context->SetDescriptorHeap(Engine::DescriptorHeap::CBV_SRV_UAV, heap);
	int todo = mMiplevels - 1;
	int base = 0;
	int dimension = mWidth;

	while (todo != 0)
	{
		int mipLevels = 1;
		
		context->SetConstants(0, Engine::DWParam(base), Engine::DWParam(mipLevels), Engine::DWParam(1.0f / (float)dimension), Engine::DWParam(1.0f / (float)dimension));
		context->SetDescriptorTable(1, mShadowMap->GetSRV());
		context->SetDescriptorTable(2, mShadowMap->GetUAV(base + 1));
		context->SetDescriptorTable(3, mShadowMap->GetUAV(base + 1));
		context->SetDescriptorTable(4, mShadowMap->GetUAV(base + 1));
		context->SetDescriptorTable(5, mShadowMap->GetUAV(base + 1));
		context->Dispatch(dimension / 2, dimension / 2, 1);

		todo -= 1;
		base += 1;
		dimension /= 2;
	}
}

void RenderPassShadowMap::Resize(unsigned int width, unsigned int height)
{
	mWidth = width;
	mHeight = height;

	mShadowMap->Release();
	mShadowMap->Create(mWidth, mHeight, 1, Engine::Graphics::Format::RG32F, false);

	mDepthMap->Release();
	mDepthMap->Create(mWidth, mHeight, Engine::Graphics::Format::D32F);
}