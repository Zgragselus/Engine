#include "RenderPassCubeShadowMap.h"

using namespace SkyeCuillin;

RenderPassCubeShadowMap::RenderPassCubeShadowMap(Engine::D3DRenderer* renderer, unsigned int width, unsigned int height) :
	RenderPass(renderer, width, height)
{
	mDepthRS = new Engine::RootSignature(renderer->GetDevice(), 5, 1);
	(*mDepthRS)[0].InitAsConstantBuffer(0);
	(*mDepthRS)[1].InitAsDescriptorTable(1);
	(*mDepthRS)[1].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);
	(*mDepthRS)[2].InitAsConstants(1, 1);
	(*mDepthRS)[3].InitAsConstantBuffer(2);
	(*mDepthRS)[4].InitAsConstants(3, 1);
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

	mDepth = new Engine::D3DShader("../Data/SkyeCuillin/ShadowMapCube.hlsl",
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
	rtvFormats.push_back(Engine::Graphics::Format::RGBA32F);

	mDepthPS = new Engine::PipelineState(mRenderer->GetDevice(),
		mDepthRS,
		mDepth,
		Engine::BlendState::CreateStateD3D(true),
		Engine::RasterizerState::CreateStateD3D(),
		&dsState,
		&inputLayout,
		Engine::PipelineState::PrimitiveType::TRIANGLE,
		rtvFormats,
		Engine::Graphics::Format::D32F,
		1,
		0);

	mShadowMap = new Engine::ColorBuffer(renderer, 0.0f, 0.0f, 0.0f, 1.0f);
	mShadowMap->Create(mWidth, mHeight, 1, Engine::Graphics::Format::RGBA32F, true);
	mDepthMap = new Engine::DepthBuffer(mRenderer, 1.0f, 0);
	mDepthMap->Create(mWidth, mHeight, Engine::Graphics::Format::D32F, true);

	mConstBuffer = new Engine::GpuMappedBuffer();
	mConstBuffer->Init(mRenderer, 128, sizeof(float));
	mConstBufferPtr = mConstBuffer->Map();

	mProjection = new Engine::mat4();
	mView = new Engine::mat4[6];
}

RenderPassCubeShadowMap::~RenderPassCubeShadowMap()
{
	delete mProjection;
	delete mView;

	delete mShadowMap;
	delete mDepthMap;

	delete mDepthRS;
	delete mDepthPS;
	delete mDepth;

	mConstBuffer->Unmap();
	delete mConstBuffer;
}

void RenderPassCubeShadowMap::Process(Engine::Entity* light, Engine::DescriptorHeap* heap, Engine::GraphicsContext* context, std::vector<Engine::RenderNode>& nodes, Engine::GpuMappedBuffer* matrices)
{
	if (light->GameObject().Has<Engine::LightComponent>())
	{
		Engine::Light* l = light->GameObject().Get<Engine::LightComponent>()->GetLight();

		if (l->GetType() == Engine::Light::Type::POINT)
		{
			Engine::float4 position = Engine::float4(0.0f, 0.0f, 0.0f, 1.0f) * light->GetWorldMatrix();
			
			(*mProjection) = Engine::perspective(Engine::Math::PI * 0.5f, 1.0f, 20.0f, 1500.0f);
			mView[0] = Engine::lookat(position, position + Engine::float4(-1.0f, 0.0f, 0.0f, 0.0f), Engine::float4(0.0f, 1.0f, 0.0f, 0.0f));
			mView[1] = Engine::lookat(position, position + Engine::float4(1.0f, 0.0f, 0.0f, 0.0f), Engine::float4(0.0f, 1.0f, 0.0f, 0.0f));
			mView[2] = Engine::lookat(position, position + Engine::float4(0.0f, 1.0f, 0.0f, 0.0f), Engine::float4(0.0f, 0.0f, -1.0f, 0.0f));
			mView[3] = Engine::lookat(position, position + Engine::float4(0.0f, -1.0f, 0.0f, 0.0f), Engine::float4(0.0f, 0.0f, 1.0f, 0.0f));
			mView[4] = Engine::lookat(position, position + Engine::float4(0.0f, 0.0f, 1.0f, 0.0f), Engine::float4(0.0f, 1.0f, 0.0f, 0.0f));
			mView[5] = Engine::lookat(position, position + Engine::float4(0.0f, 0.0f, -1.0f, 0.0f), Engine::float4(0.0f, 1.0f, 0.0f, 0.0f));

			memcpy((float*)mConstBufferPtr, mProjection, sizeof(float) * 16);
			memcpy((float*)mConstBufferPtr + 16, &(mView[0]), sizeof(float) * 16);
			memcpy((float*)mConstBufferPtr + 32, &(mView[1]), sizeof(float) * 16);
			memcpy((float*)mConstBufferPtr + 48, &(mView[2]), sizeof(float) * 16);
			memcpy((float*)mConstBufferPtr + 64, &(mView[3]), sizeof(float) * 16);
			memcpy((float*)mConstBufferPtr + 80, &(mView[4]), sizeof(float) * 16);
			memcpy((float*)mConstBufferPtr + 96, &(mView[5]), sizeof(float) * 16);
			((float*)mConstBufferPtr)[112] = position.x;
			((float*)mConstBufferPtr)[113] = position.y;
			((float*)mConstBufferPtr)[114] = position.z;
			((float*)mConstBufferPtr)[115] = position.w;

			context->SetPipelineState(mDepthPS);
			context->SetRootSignature(mDepthRS);
			context->SetDescriptorHeap(Engine::DescriptorHeap::CBV_SRV_UAV, heap);
			context->SetViewport(0.0f, 0.0f, (float)mWidth, (float)mHeight);
			context->SetScissorRect(0.0f, 0.0f, (float)mWidth, (float)mHeight);
			context->SetPrimitiveTopology(Engine::Graphics::TRIANGLELIST);
			context->SetConstantBuffer(0, mConstBuffer->GetGpuVirtualAddress());
			context->TransitionResource(mShadowMap, D3D12_RESOURCE_STATE_RENDER_TARGET, true);

			for (int i = 0; i < 6; i++)
			{
				context->ClearColor(mShadowMap->GetRTVCube(i), 0.0f, 0.0f, 0.0f, 1.0f);
				context->ClearDepth(mDepthMap->GetDSVCube(i), 1.0f, 0);
				context->SetRenderTargets(mShadowMap->GetRTVCube(i), mDepthMap->GetDSVCube(i));
				context->SetConstants(2, Engine::DWParam(i));

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
							context->SetConstantBuffer(3, matrices->GetGpuVirtualAddress(batch * 16 * sizeof(float) * 16 * 2));
						}
						context->SetConstants(4, Engine::DWParam(node.mID % 16));
						node.mObject->Get<Engine::MeshComponent>()->GetMesh()->Render(context);
					}
				}
			}

			context->TransitionResource(mShadowMap, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, true);
		}
	}
}

void RenderPassCubeShadowMap::Resize(unsigned int width, unsigned int height)
{
	mWidth = width;
	mHeight = height;

	mShadowMap->Release();
	mShadowMap->Create(mWidth, mHeight, 1, Engine::Graphics::Format::RGBA32F, true);

	mDepthMap->Release();
	mDepthMap->Create(mWidth, mHeight, Engine::Graphics::Format::D32F, true);
}