#include "RenderPassPicking.h"

using namespace SkyeCuillin;

RenderPassPicking::RenderPassPicking(Engine::D3DRenderer* renderer, unsigned int width, unsigned int height, SkyeCuillin::RenderGizmo* gizmoRenderer) : RenderPass(renderer, width, height)
{
	mGizmoRenderer = gizmoRenderer;

	mPickingRS = new Engine::RootSignature(renderer->GetDevice(), 3, 0);
	(*mPickingRS)[0].InitAsConstantBuffer(0);
	(*mPickingRS)[1].InitAsConstants(1, 2);
	(*mPickingRS)[2].InitAsConstantBuffer(2);
	mPickingRS->Finalize(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	mPicking = new Engine::D3DShader("../Data/SkyeCuillin/ColorPicker.hlsl",
		std::vector<Engine::D3DShader::ShaderEntryPoint>
	{
		Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::VERTEX_SHADER, "VS"),
		Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::PIXEL_SHADER, "PS")
	});

	Engine::InputLayout inputLayout = Engine::InputLayout();
	inputLayout.AddAttribute(Engine::InputLayout::Attribute("POSITION", 0, Engine::Graphics::Format::RGB32F, 0, 0, Engine::InputLayout::Classification::PER_VERTEX, 0));
	inputLayout.AddAttribute(Engine::InputLayout::Attribute("NORMAL", 0, Engine::Graphics::Format::RGB32F, 0, 12, Engine::InputLayout::Classification::PER_VERTEX, 0));
	inputLayout.AddAttribute(Engine::InputLayout::Attribute("TEXCOORD", 2, Engine::Graphics::Format::RGB32F, 0, 24, Engine::InputLayout::Classification::PER_VERTEX, 0));
	inputLayout.AddAttribute(Engine::InputLayout::Attribute("TEXCOORD", 3, Engine::Graphics::Format::RGB32F, 0, 36, Engine::InputLayout::Classification::PER_VERTEX, 0));
	inputLayout.AddAttribute(Engine::InputLayout::Attribute("TEXCOORD", 0, Engine::Graphics::Format::RG32F, 0, 48, Engine::InputLayout::Classification::PER_VERTEX, 0));
	inputLayout.AddAttribute(Engine::InputLayout::Attribute("TEXCOORD", 1, Engine::Graphics::Format::RG32F, 0, 56, Engine::InputLayout::Classification::PER_VERTEX, 0));

	Engine::DepthStencilState dsState = Engine::DepthStencilState(true);

	std::vector<Engine::Graphics::Format> rtvFormats;
	rtvFormats.push_back(Engine::Graphics::Format::RGBA8);

	mPickingPS = new Engine::PipelineState(mRenderer->GetDevice(),
		mPickingRS,
		mPicking,
		Engine::BlendState::CreateStateD3D(),
		Engine::RasterizerState::CreateStateD3D(),
		&dsState,
		&inputLayout,
		Engine::PipelineState::PrimitiveType::TRIANGLE,
		rtvFormats,
		Engine::Graphics::Format::D32F,
		1,
		0);

	mColorBuffer = new Engine::ColorBuffer(renderer, 1.0f, 1.0f, 1.0f, 1.0f);
	mColorBuffer->Create(mWidth, mHeight, 1, Engine::Graphics::Format::RGBA8);
	mDepthBuffer = new Engine::DepthBuffer(mRenderer, 1.0f, 0);
	mDepthBuffer->Create(mWidth, mHeight, Engine::Graphics::Format::D32F);

	D3D12_PLACED_SUBRESOURCE_FOOTPRINT placedFootprint;
	mRenderer->GetDevice()->GetCopyableFootprints(&mColorBuffer->Get()->GetDesc(), 0, 1, 0, &placedFootprint, nullptr, nullptr, nullptr);
	mRowPitch = placedFootprint.Footprint.RowPitch;
	mReadbackBuffer = new Engine::GpuMappedBuffer();
	mReadbackBuffer->Init(mRenderer, placedFootprint.Footprint.RowPitch * placedFootprint.Footprint.Height, sizeof(unsigned int), Engine::GpuMappedBuffer::READBACK);

	mCameraBuffer = new Engine::GpuMappedBuffer();
	mCameraBuffer->Init(mRenderer, 64, sizeof(float));
	mCameraBufferPtr = mCameraBuffer->Map();
}

RenderPassPicking::~RenderPassPicking()
{
	delete mCameraBuffer;
	delete mDepthBuffer;
	delete mColorBuffer;
	delete mPickingPS;
	delete mPicking;
	delete mPickingRS;
}

void RenderPassPicking::Process(Engine::Entity* camera, Engine::DescriptorHeap* heap, Engine::GraphicsContext* context, std::vector<Engine::RenderNode>& nodes, Engine::GpuMappedBuffer* matrices)
{
	if (camera->GameObject().Has<Engine::CameraComponent>())
	{
		Engine::Camera* cam = camera->GameObject().Get<Engine::CameraComponent>()->Get();

		Engine::mat4 viewMatrix = cam->GetViewMatrix();
		Engine::mat4 projectionMatrix = cam->GetProjectionMatrix();

		memcpy(mCameraBufferPtr, &viewMatrix, sizeof(float) * 16);
		memcpy((float*)mCameraBufferPtr + 16, &projectionMatrix, sizeof(float) * 16);

		context->SetPipelineState(mPickingPS);
		context->SetRootSignature(mPickingRS);
		context->SetDescriptorHeap(Engine::DescriptorHeap::CBV_SRV_UAV, heap);
		context->SetViewport(0.0f, 0.0f, (float)mWidth, (float)mHeight);
		context->SetScissorRect(0.0f, 0.0f, (float)mWidth, (float)mHeight);
		context->TransitionResource(mColorBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, false);
		context->TransitionResource(mDepthBuffer, D3D12_RESOURCE_STATE_DEPTH_WRITE, true);
		context->SetRenderTargets(mColorBuffer, mDepthBuffer);
		context->ClearColor(mColorBuffer, 1.0f, 1.0f, 1.0f, 1.0f);
		context->ClearDepth(mDepthBuffer, 1.0f, 0);
		context->SetPrimitiveTopology(Engine::Graphics::TRIANGLELIST);
		context->SetConstantBuffer(0, mCameraBuffer->GetGpuVirtualAddress());

		int batchBound = -1;
		for (const Engine::RenderNode& node : nodes)
		{
			bool hasMaterial = false;
			int batch = node.mID / 16;

			if (node.mObject->Has<Engine::MaterialComponent>())
			{
				hasMaterial = true;
			}

			if (node.mObject->Has<Engine::MeshComponent>() && hasMaterial)
			{
				if (batchBound != batch)
				{
					batchBound = batch;
					context->SetConstantBuffer(2, matrices->GetGpuVirtualAddress(batch * 16 * sizeof(float) * 16 * 2));
				}
				context->SetConstants(1, Engine::DWParam(node.mID % 16), Engine::DWParam(node.mID));

				node.mObject->Get<Engine::MeshComponent>()->GetMesh()->Render(context);
			}
		}

		mGizmoRenderer->RenderPicking(camera, heap, context);

		context->TransitionResource(mColorBuffer, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, false);
		context->TransitionResource(mDepthBuffer, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, true);
	}
}

void RenderPassPicking::Readback(Engine::GraphicsContext* context)
{
	context->ReadbackTexture2D(mReadbackBuffer, mColorBuffer);
}

void RenderPassPicking::Resize(unsigned int width, unsigned int height)
{
	mWidth = width;
	mHeight = height;

	mColorBuffer->Release();
	mColorBuffer->Create(mWidth, mHeight, 1, Engine::Graphics::Format::RGBA8);

	mDepthBuffer->Release();
	mDepthBuffer->Create(mWidth, mHeight, Engine::Graphics::Format::D32F);

	D3D12_PLACED_SUBRESOURCE_FOOTPRINT placedFootprint;
	mRenderer->GetDevice()->GetCopyableFootprints(&mColorBuffer->Get()->GetDesc(), 0, 1, 0, &placedFootprint, nullptr, nullptr, nullptr);
	mRowPitch = placedFootprint.Footprint.RowPitch;
	delete mReadbackBuffer;
	mReadbackBuffer = new Engine::GpuMappedBuffer();
	mReadbackBuffer->Init(mRenderer, placedFootprint.Footprint.RowPitch * placedFootprint.Footprint.Height, sizeof(unsigned int), Engine::GpuMappedBuffer::READBACK);
}