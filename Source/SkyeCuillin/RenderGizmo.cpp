#include "RenderGizmo.h"
#include "Game/Entity/Component/CollisionComponent.h"

using namespace SkyeCuillin;

RenderGizmo::RenderGizmo(Engine::Log* log, Engine::D3DRenderer* renderer, unsigned int gizmoImageSize)
{
	mIconsAtlasSize = gizmoImageSize;

	mGizmoRS = new Engine::RootSignature(renderer->GetDevice(), 1, 0);
	(*mGizmoRS)[0].InitAsConstantBuffer(0);
	mGizmoRS->Finalize(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	mGizmo = new Engine::D3DShader("../Data/SkyeCuillin/Gizmo.hlsl",
		std::vector<Engine::D3DShader::ShaderEntryPoint>
		{
			Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::VERTEX_SHADER, "VS"),
			Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::PIXEL_SHADER, "PS")
		},
		std::vector<Engine::D3DShader::ShaderDefine>());

	Engine::InputLayout guiLayout = Engine::InputLayout();
	guiLayout.AddAttribute(Engine::InputLayout::Attribute("POSITION", 0, Engine::Graphics::Format::RGBA32F, 0, 0, Engine::InputLayout::Classification::PER_VERTEX, 0));
	guiLayout.AddAttribute(Engine::InputLayout::Attribute("COLOR", 0, Engine::Graphics::Format::RGBA8, 0, 16, Engine::InputLayout::Classification::PER_VERTEX, 0));

	Engine::DepthStencilState guiDsState = Engine::DepthStencilState(false);
	Engine::DepthStencilState guiDsState2 = Engine::DepthStencilState(true);

	std::vector<Engine::Graphics::Format> guiRtvFormats;
	guiRtvFormats.push_back(Engine::Graphics::Format::RGBA8);
	
	mGizmoPS = new Engine::PipelineState(renderer->GetDevice(),
		mGizmoRS,
		mGizmo,
		Engine::BlendState::CreateStateD3D(),
		Engine::RasterizerState::CreateStateD3D(Engine::RasterizerState::SOLID, Engine::RasterizerState::NONE),
		&guiDsState,
		&guiLayout,
		Engine::PipelineState::PrimitiveType::LINE,
		guiRtvFormats,
		Engine::Graphics::Format::D32F,
		1,
		0);
	
	mGizmoFlatPS = new Engine::PipelineState(renderer->GetDevice(),
		mGizmoRS,
		mGizmo,
		Engine::BlendState::CreateStateD3D(false, false, 
			Engine::BlendState::BlendDesc(true, false, Engine::BlendState::SRC_ALPHA, Engine::BlendState::INV_SRC_ALPHA, Engine::BlendState::OP_ADD,
				Engine::BlendState::INV_SRC_ALPHA, Engine::BlendState::ZERO, Engine::BlendState::OP_ADD)),
		Engine::RasterizerState::CreateStateD3D(Engine::RasterizerState::SOLID, Engine::RasterizerState::NONE),
		&guiDsState,
		&guiLayout,
		Engine::PipelineState::PrimitiveType::TRIANGLE,
		guiRtvFormats,
		Engine::Graphics::Format::D32F,
		1,
		0);

	Engine::InputLayout guiLayoutTexCoord = Engine::InputLayout();
	guiLayoutTexCoord.AddAttribute(Engine::InputLayout::Attribute("POSITION", 0, Engine::Graphics::Format::RGBA32F, 0, 0, Engine::InputLayout::Classification::PER_VERTEX, 0));
	guiLayoutTexCoord.AddAttribute(Engine::InputLayout::Attribute("TEXCOORD", 0, Engine::Graphics::Format::RGBA32F, 0, 16, Engine::InputLayout::Classification::PER_VERTEX, 0));
	guiLayoutTexCoord.AddAttribute(Engine::InputLayout::Attribute("TEXCOORD", 1, Engine::Graphics::Format::R32, 0, 32, Engine::InputLayout::Classification::PER_VERTEX, 0));

	mGizmoIconsRS = new Engine::RootSignature(renderer->GetDevice(), 2, 1);
	(*mGizmoIconsRS)[0].InitAsConstantBuffer(0);
	(*mGizmoIconsRS)[1].InitAsDescriptorTable(1);
	(*mGizmoIconsRS)[1].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);
	mGizmoIconsRS->InitStaticSampler(0, Engine::Sampler::CreateSamplerD3D(Engine::Sampler::Filter::MIN_LINEAR_MAG_LINEAR_MIP_LINEAR,
		Engine::Sampler::Address::WRAP,
		Engine::Sampler::Address::WRAP,
		Engine::Sampler::Address::WRAP,
		0.0f,
		1,
		Engine::Sampler::ComparisonFunc::NEVER,
		1.0f, 1.0f, 1.0f, 1.0f,
		0.0f,
		32.0f));
	mGizmoIconsRS->Finalize(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	mGizmoIcons = new Engine::D3DShader("../Data/SkyeCuillin/GizmoIcons.hlsl",
		std::vector<Engine::D3DShader::ShaderEntryPoint>
		{
			Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::VERTEX_SHADER, "VS"),
			Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::GEOMETRY_SHADER, "GS"),
			Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::PIXEL_SHADER, "PS_RenderImage")
		},
		std::vector<Engine::D3DShader::ShaderDefine>());

	mGizmoIconsPS = new Engine::PipelineState(renderer->GetDevice(),
		mGizmoIconsRS,
		mGizmoIcons,
		Engine::BlendState::CreateStateD3D(false, false,
			Engine::BlendState::BlendDesc(true, false, Engine::BlendState::SRC_ALPHA, Engine::BlendState::INV_SRC_ALPHA, Engine::BlendState::OP_ADD,
				Engine::BlendState::INV_SRC_ALPHA, Engine::BlendState::ZERO, Engine::BlendState::OP_ADD)),
		Engine::RasterizerState::CreateStateD3D(Engine::RasterizerState::SOLID, Engine::RasterizerState::NONE),
		&guiDsState,
		&guiLayoutTexCoord,
		Engine::PipelineState::PrimitiveType::POINT,
		guiRtvFormats,
		Engine::Graphics::Format::D32F,
		1,
		0);
	
	mGizmoPicking = new Engine::D3DShader("../Data/SkyeCuillin/GizmoIcons.hlsl",
		std::vector<Engine::D3DShader::ShaderEntryPoint>
		{
			Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::VERTEX_SHADER, "VS"),
			Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::GEOMETRY_SHADER, "GS"),
			Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::PIXEL_SHADER, "PS_RenderID")
		},
		std::vector<Engine::D3DShader::ShaderDefine>());

	mGizmoPickingPS = new Engine::PipelineState(renderer->GetDevice(),
		mGizmoIconsRS,
		mGizmoPicking,
		Engine::BlendState::CreateStateD3D(),
		Engine::RasterizerState::CreateStateD3D(Engine::RasterizerState::SOLID, Engine::RasterizerState::NONE),
		&guiDsState2,
		&guiLayoutTexCoord,
		Engine::PipelineState::PrimitiveType::POINT,
		guiRtvFormats,
		Engine::Graphics::Format::D32F,
		1,
		0);

	mLinesVBOSize = 65536;
	mLinesVBOUsage = 0;
	mLinesVBOMem = new float[65536];
	mLinesVBO = new Engine::TypedBuffer(Engine::Graphics::R32F);
	mLinesVBO->Init(renderer, mLinesVBOSize, sizeof(float), nullptr);

	mTrianglesVBOSize = 65536;
	mTrianglesVBOUsage = 0;
	mTrianglesVBOMem = new float[65536];
	mTrianglesVBO = new Engine::TypedBuffer(Engine::Graphics::R32F);
	mTrianglesVBO->Init(renderer, mTrianglesVBOSize, sizeof(float), nullptr);

	mIconsVBOSize = 65536;
	mIconsVBOUsage = 0;
	mIconsVBOMem = new float[65536];
	mIconsVBO = new Engine::TypedBuffer(Engine::Graphics::R32F);
	mIconsVBO->Init(renderer, mTrianglesVBOSize, sizeof(float), nullptr);

	Engine::LoaderDevIL loader(log);
	Engine::LoaderDevIL::Image* image = loader.Load("../Data/SkyeCuillin/Gui.png");
	if (image != nullptr)
	{
		loader.GenerateMipmaps(image);
		mIconsTexture = new Engine::Texture();
		mIconsTexture->InitMipmaps(renderer, image->mWidth, image->mHeight, 1, image->mMipLevels, Engine::Graphics::RGBA8, (const void**)image->mMipmaps);
		mIconsTexture->SetName("GUI");

		mIconsAtlasCount[0] = (unsigned int)image->mWidth / mIconsAtlasSize;
		mIconsAtlasCount[1] = (unsigned int)image->mHeight / mIconsAtlasSize;
		mIconsAtlasResolution[0] = (unsigned int)image->mWidth;
		mIconsAtlasResolution[1] = (unsigned int)image->mHeight;
		mIconsAtlasStep[0] = (float)mIconsAtlasSize / (float)image->mWidth;
		mIconsAtlasStep[1] = (float)mIconsAtlasSize / (float)image->mHeight;
	}
	else
	{
		log->Print("SkyeCuillin::RenderPassResolve", std::string("Error: Requested texture image '../Data/SkyeCuillin/lensdirt1.png' does not exists!"));
	}

	mCameraBuffer = new Engine::GpuMappedBuffer();
	mCameraBuffer->Init(renderer, 64, sizeof(float));
	mCameraBufferPtr = mCameraBuffer->Map();
}

RenderGizmo::~RenderGizmo()
{
	delete mGizmoIconsPS;
	delete mGizmoIconsRS;
	delete mGizmoIcons;

	delete mGizmoFlatPS;

	delete mGizmoPS;
	delete mGizmoRS;
	delete mGizmo;

	delete mIconsTexture;

	delete mLinesVBO;
	delete mTrianglesVBO;
	delete mIconsVBO;

	delete mLinesVBOMem;
	delete mTrianglesVBOMem;
	delete mIconsVBOMem;

	delete mCameraBuffer;
}

void RenderGizmo::ProcessRenderNode(Engine::RenderNode& node)
{
	AddAABB(node.mBounds, Engine::float4(1.0f, 1.0f, 1.0f, 1.0f));

	if (node.mObject->Has<Engine::CollisionComponent>() && node.mObject->Has<Engine::RigidBodyComponent>())
	{
		AddCollider(node.mObject->GetEntity(), node.mObject->Get<Engine::CollisionComponent>(), node.mObject->Get<Engine::RigidBodyComponent>());
	}
}

/*void RenderGizmo::Process(std::vector<Engine::RenderNode>& nodes, Engine::GpuMappedBuffer* matrices)
{
	for (Engine::RenderNode& n : nodes)
	{
		ProcessRenderNode(n);
		//AddAABB(n.mBounds, Engine::float4(1.0f, 1.0f, 1.0f, 1.0f));
	}
}*/

void RenderGizmo::Process(std::vector<Engine::RenderNode>& nodes, Engine::Scene* scene, Engine::GpuMappedBuffer* matrices, unsigned int id)
{
	std::set<size_t> entityIDs;
	std::vector<Engine::Entity*> entities;
	std::stack<Engine::Entity*> tmp;
	std::stack<Engine::Entity*> tmp2;

	if (scene->GetScenegraph() != NULL)
	{
		tmp.push(scene->GetScenegraph());

		while (!tmp.empty())
		{
			Engine::Entity* e = tmp.top();
			tmp.pop();

			for (Engine::Entity* child : e->Children())
			{
				tmp.push(child);
			}

			if (e->mSceneID == id)
			{
				entities.push_back(e);
				break;
			}
		}
	}

	if (entities.size() > 0)
	{
		tmp2.push(entities[0]);

		while (!tmp2.empty())
		{
			Engine::Entity* e = tmp2.top();
			tmp2.pop();

			for (Engine::Entity* child : e->Children())
			{
				tmp2.push(child);
			}

			entityIDs.emplace(e->mSceneID);
		}
	}

	for (size_t i = 0; i < nodes.size(); i++)
	{
		if (entityIDs.find(nodes[i].mID) != entityIDs.end())
		{
			ProcessRenderNode(nodes[i]);
		}
	}
	
	/*for (Engine::RenderNode& n : nodes)
	{
		if (n.mID == id)
		{
			ProcessRenderNode(n);

			for (int i = 0; i < n.mObject->GetEntity()->Children().size(); i++)
			{
				for (Engine::RenderNode& childNode : nodes)
				{
					if (n.mObject->GetEntity()->Children()[i]->mSceneID == childNode.mID)
					{
						ProcessRenderNode(childNode);
					}
				}
			}
		}
	}*/
}

void RenderGizmo::Process(std::vector<Engine::RenderNode>& nodes, Engine::Scene* scene, Engine::GpuMappedBuffer* matrices, std::set<int>& ids)
{
	std::set<size_t> entityIDs;					// set of all IDs for selected entities and their whole subtree' entities
	std::vector<Engine::Entity*> entities;		// list of selected entities
	std::stack<Engine::Entity*> tmp;			// 1st traversal stack
	std::stack<Engine::Entity*> tmp2;			// 2nd traversal stack

	// 1st pass - find all entities that are selected
	if (scene->GetScenegraph() != NULL)
	{
		tmp.push(scene->GetScenegraph());

		while (!tmp.empty())
		{
			Engine::Entity* e = tmp.top();
			tmp.pop();

			for (Engine::Entity* child : e->Children())
			{
				tmp.push(child);
			}

			if (ids.find(e->mSceneID) != ids.end())
			{
				entities.push_back(e);
			}
		}
	}

	// 2nd pass for each selected entity, go through its whole subtree, and collect all entity IDs that are there (incl. root)
	if (entities.size() > 0)
	{
		for (size_t i = 0; i < entities.size(); i++)
		{
			tmp2.push(entities[i]);
		}

		while (!tmp2.empty())
		{
			Engine::Entity* e = tmp2.top();
			tmp2.pop();

			for (Engine::Entity* child : e->Children())
			{
				tmp2.push(child);
			}

			entityIDs.emplace(e->mSceneID);
		}
	}

	// 3rd pass, render gizmo for each render node that was picked in 2nd pass
	for (size_t i = 0; i < nodes.size(); i++)
	{
		if (entityIDs.find(nodes[i].mID) != entityIDs.end())
		{
			ProcessRenderNode(nodes[i]);
		}
	}

	/*for (Engine::RenderNode& n : nodes)
	{
		if (ids.find(n.mID) != ids.end())
		{
			ProcessRenderNode(n);

			for (int i = 0; i < n.mObject->GetEntity()->Children().size(); i++)
			{
				for (Engine::RenderNode& childNode : nodes)
				{
					if (n.mObject->GetEntity()->Children()[i]->mSceneID == childNode.mID)
					{
						ProcessRenderNode(childNode);
					}
				}
			}
		}
	}*/
}

void RenderGizmo::ProcessIcons(std::vector<Engine::RenderNode>& nodes)
{
	for (Engine::RenderNode& n : nodes)
	{
		if (n.mObject->Has<Engine::LightComponent>())
		{
			AddWidget(n.mCentroid, n.mID, Gizmo::LIGHT);
		}
	}
}

void RenderGizmo::Render(Engine::Entity* camera, Engine::DescriptorHeap* heap, Engine::GraphicsContext* context)
{
	if (camera->GameObject().Has<Engine::CameraComponent>())
	{
		Engine::Camera* cam = camera->GameObject().Get<Engine::CameraComponent>()->Get();

		Engine::mat4 viewMatrix = cam->GetViewMatrix();
		Engine::mat4 projectionMatrix = cam->GetProjectionMatrix();

		memcpy(mCameraBufferPtr, &viewMatrix, sizeof(float) * 16);
		memcpy((float*)mCameraBufferPtr + 16, &projectionMatrix, sizeof(float) * 16);
		memcpy((float*)mCameraBufferPtr + 32, &(cam->GetPosition()), sizeof(float) * 4);
		((float*)mCameraBufferPtr)[36] = cam->GetType() == Engine::Camera::Type::PERSPECTIVE ? ((Engine::PerspectiveCamera*)cam)->GetFov() : 1.0f;
		((float*)mCameraBufferPtr)[37] = 0.02f;

		context->TransitionResource(mLinesVBO, D3D12_RESOURCE_STATE_COPY_DEST, true);
		context->WriteBuffer(mLinesVBO, 0, mLinesVBOMem, mLinesVBOSize * sizeof(float));
		context->TransitionResource(mLinesVBO, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, true);

		context->TransitionResource(mTrianglesVBO, D3D12_RESOURCE_STATE_COPY_DEST, true);
		context->WriteBuffer(mTrianglesVBO, 0, mTrianglesVBOMem, mTrianglesVBOSize * sizeof(float));
		context->TransitionResource(mTrianglesVBO, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, true);

		context->TransitionResource(mIconsVBO, D3D12_RESOURCE_STATE_COPY_DEST, true);
		context->WriteBuffer(mIconsVBO, 0, mIconsVBOMem, mIconsVBOSize * sizeof(float));
		context->TransitionResource(mIconsVBO, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, true);

		context->SetPipelineState(mGizmoPS);
		context->SetRootSignature(mGizmoRS);
		context->SetPrimitiveTopology(Engine::Graphics::LINELIST);
		context->SetConstantBuffer(0, mCameraBuffer->GetGpuVirtualAddress());

		context->SetVertexBuffer(0, mLinesVBO->VertexBufferView(0, (unsigned int)(mLinesVBOUsage * sizeof(float)), sizeof(float) * 5));
		context->Draw((unsigned int)mLinesVBOUsage / 5, 0);

		context->SetPipelineState(mGizmoFlatPS);
		context->SetPrimitiveTopology(Engine::Graphics::TRIANGLELIST);

		context->SetVertexBuffer(0, mTrianglesVBO->VertexBufferView(0, (unsigned int)(mTrianglesVBOUsage * sizeof(float)), sizeof(float) * 5));
		context->Draw((unsigned int)mTrianglesVBOUsage / 5, 0);

		context->SetPipelineState(mGizmoIconsPS);
		context->SetRootSignature(mGizmoIconsRS);
		context->SetPrimitiveTopology(Engine::Graphics::POINTLIST);

		context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(1, mIconsTexture->GetSRV().mGpuHandle);

		context->SetVertexBuffer(0, mIconsVBO->VertexBufferView(0, (unsigned int)(mIconsVBOUsage * sizeof(float)), sizeof(float) * 9));
		context->Draw((unsigned int)mIconsVBOUsage / 9, 0);
	}
}

void RenderGizmo::RenderPicking(Engine::Entity* camera, Engine::DescriptorHeap* heap, Engine::GraphicsContext* context)
{
	if (camera->GameObject().Has<Engine::CameraComponent>())
	{
		context->SetPipelineState(mGizmoPickingPS);
		context->SetRootSignature(mGizmoIconsRS);
		context->SetPrimitiveTopology(Engine::Graphics::POINTLIST);

		context->SetConstantBuffer(0, mCameraBuffer->GetGpuVirtualAddress());
		context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(1, mIconsTexture->GetSRV().mGpuHandle);

		context->SetVertexBuffer(0, mIconsVBO->VertexBufferView(0, (unsigned int)(mIconsVBOUsage * sizeof(float)), sizeof(float) * 9));
		context->Draw((unsigned int)mIconsVBOUsage / 9, 0);
	}
}