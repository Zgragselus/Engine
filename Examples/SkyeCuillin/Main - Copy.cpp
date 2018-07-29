#include "Main.h"
#include "Graphics/Graphics.h"
#include "Game/Game.h"
#include "Voxelize.h"
#include "RenderPassGBuffer.h"
#include "RenderPassDeferredShading.h"
#include "RenderPassShadowMap.h"
#include "RenderPassCubeShadowMap.h"
#include "RenderPassSeparableBlur.h"
#include "RenderPassConeTracingAO.h"
#include "RenderPassPicking.h"
#include "Game/Scene/BVH.h"
#include "ImguiSubsystem.h"
#include "RenderGizmo.h"
#include "Core/Math/Primitive/Ray.h"

class Main : public Engine::System
{
private:
	Engine::mat4 clickView;
	Engine::mat4 clickProj;

	SkyeCuillin::Voxelize* mVoxelize;
	SkyeCuillin::RenderPassGBuffer* mDeferredRenderer;
	SkyeCuillin::RenderPassDeferredShading* mDeferredPass;
	SkyeCuillin::RenderPassShadowMap* mShadowMap;
	SkyeCuillin::RenderPassCubeShadowMap* mCubeShadowMap;
	SkyeCuillin::RenderPassSeparableBlur* mBlurPass;
	SkyeCuillin::RenderPassConeTracingAO* mAOPass;
	SkyeCuillin::ImguiSubsystem* mImgui;
	SkyeCuillin::RenderGizmo* mGizmo;
	SkyeCuillin::RenderPassPicking* mPicking;

	Engine::Log* mLog;
	Engine::Window* mWindow;
	Engine::Constants* mOptions;
	Engine::D3DRenderer* mRenderer;
	Engine::D3DSwapChain* mSwapChain;
	
	Engine::Manager<Engine::Texture> mTextureManager;

	Engine::RootSignature* mRootSignaturePost;
	Engine::PipelineState* mPipelineStatePost;
	Engine::GpuMappedBuffer* mConstantBufferPost;
	void* mCbufferPost;
	Engine::D3DShader* mShaderPost;

	Engine::Scene* mScene;
	Engine::Mesh* mFullscreenQuad;

	Engine::QueryHeap* mQueryHeap;
	Engine::GpuMappedBuffer* mTimestampResults;
	void* mTimestampResultsPtr;
	Engine::QueryHandle* mTimestamps;
	size_t mTimestampCount = 64;

	bool mCamForward = false;
	bool mCamBackward = false;
	bool mCamLeft = false;
	bool mCamRight = false;
	bool mCenterMouse = false;
	bool mSwitchPressed = false;
	void* mCbuffer;

	Engine::mat4* mMatrices;
	Engine::GpuMappedBuffer* mMatricesBuffer;
	void* mMatricesBufferPtr;

public:
	Main(Engine::Log* log, Engine::Constants* options) : Engine::System("Main")
	{
		EnableUpdater();

		mLog = log;
		mOptions = options;

		mWindow = new Engine::Window(log, options);
		mWindow->Create();

		mRenderer = new Engine::D3DRenderer(mWindow, log, options);
		mSwapChain = new Engine::D3DSwapChain(mRenderer, mWindow, log, options);

		mScene = new Engine::Scene();

		static Engine::EventChannel chan;
		chan.Add<Engine::Keyboard::KeyPressed>(*this);
		chan.Add<Engine::Keyboard::KeyReleased>(*this);
		chan.Add<Engine::Keyboard::KeyChar>(*this);
		chan.Add<Engine::Mouse::MouseMove>(*this);
		chan.Add<Engine::Mouse::ButtonPressed>(*this);
		chan.Add<Engine::Mouse::ButtonReleased>(*this);
		chan.Add<Engine::Window::Resize>(*this);
	}

	virtual ~Main()
	{
		delete mScene;
		delete mSwapChain;
		delete mRenderer;
		delete mWindow;
	}

	virtual bool Init()
	{
		int w;
		int h;
		this->mWindow->GetSize(&w, &h);

		mImgui = new SkyeCuillin::ImguiSubsystem(mRenderer, w, h);
		mGizmo = new SkyeCuillin::RenderGizmo(mRenderer);
		
		mVoxelize = new SkyeCuillin::Voxelize(mRenderer, 256, SkyeCuillin::Voxelize::MipmapCalculation::MIPMAP_3);

		mDeferredRenderer = new SkyeCuillin::RenderPassGBuffer(mRenderer, w, h, 8);
		mDeferredPass = new SkyeCuillin::RenderPassDeferredShading(mRenderer, w, h, 8, 1024);
		mShadowMap = new SkyeCuillin::RenderPassShadowMap(mRenderer, 2048, 2048, 50.0f, 1500.0f);
		mCubeShadowMap = new SkyeCuillin::RenderPassCubeShadowMap(mRenderer, 2048, 2048);
		mBlurPass = new SkyeCuillin::RenderPassSeparableBlur(mRenderer, 1024, 1024, Engine::Graphics::Format::RG32F);
		//mAOPass = new SkyeCuillin::RenderPassConeTracingAO(mRenderer, w / 2, h / 2);
		mAOPass = new SkyeCuillin::RenderPassConeTracingAO(mRenderer, w, h);
		mPicking = new SkyeCuillin::RenderPassPicking(mRenderer, w, h);
		
		mRootSignaturePost = new Engine::RootSignature(mRenderer->GetDevice(), 5, 1);
		(*mRootSignaturePost)[0].InitAsDescriptorTable(1);
		(*mRootSignaturePost)[0].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);
		(*mRootSignaturePost)[1].InitAsConstants(0, 2);
		(*mRootSignaturePost)[2].InitAsConstants(1, 1);
		(*mRootSignaturePost)[3].InitAsConstantBuffer(2);
		(*mRootSignaturePost)[4].InitAsDescriptorTable(1);
		(*mRootSignaturePost)[4].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
		mRootSignaturePost->InitStaticSampler(0, Engine::Sampler::CreateSamplerD3D(Engine::Sampler::Filter::MIN_LINEAR_MAG_LINEAR_MIP_LINEAR,
			Engine::Sampler::Address::WRAP,
			Engine::Sampler::Address::WRAP,
			Engine::Sampler::Address::WRAP,
			0.0f,
			1,
			Engine::Sampler::ComparisonFunc::NEVER,
			1.0f, 1.0f, 1.0f, 1.0f,
			0.0f,
			32.0f));
		mRootSignaturePost->Finalize(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
						
		// Compile HLSL shaders
		mShaderPost = new Engine::D3DShader("../Data/SkyeCuillin/Post.hlsl",
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
		Engine::DepthStencilState dsState = Engine::DepthStencilState();
		std::vector<Engine::Graphics::Format> rtvFormats;
		rtvFormats.push_back(Engine::Graphics::Format::RGBA8);
			
		Engine::InputLayout inputLayoutPost = Engine::InputLayout();
		inputLayoutPost.AddAttribute(Engine::InputLayout::Attribute("POSITION", 0, Engine::Graphics::Format::RGB32F, 0, 0, Engine::InputLayout::Classification::PER_VERTEX, 0));
		inputLayoutPost.AddAttribute(Engine::InputLayout::Attribute("TEXCOORD", 0, Engine::Graphics::Format::RG32F, 0, 12, Engine::InputLayout::Classification::PER_VERTEX, 0));
		mPipelineStatePost = new Engine::PipelineState(mRenderer->GetDevice(),
			mRootSignaturePost,
			mShaderPost,
			Engine::BlendState::CreateStateD3D(),
			Engine::RasterizerState::CreateStateD3D(),
			&dsState,
			&inputLayoutPost,
			Engine::PipelineState::PrimitiveType::TRIANGLE,
			rtvFormats,
			Engine::Graphics::Format::D32F,
			1,
			0);

		// Create vertex buffer
		float triangleVertices[] =
		{
			-1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
			-1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 1.0f
		};

		unsigned int triangleIndices[] =
		{
			0, 1, 2,
			3, 4, 5
		};

		mMatrices = new Engine::mat4[mOptions->Get<int>("Scene.MaxObjects")];
		mMatricesBuffer = new Engine::GpuMappedBuffer();
		mMatricesBuffer->Init(mRenderer, mOptions->Get<int>("Scene.MaxObjects"), sizeof(float) * 16);
		mMatricesBufferPtr = mMatricesBuffer->Map();

		Engine::LoaderAssimp* loader = new Engine::LoaderAssimp(mLog, mRenderer);
		loader->SetTextureManager(&mTextureManager);
		Engine::Entity* ent = loader->Load("../Data/Example05/Sponza/sponza.obj");
		Engine::Entity* ent2 = loader->Load("../Data/Shared/Models/roundcube.obj");
		//ent2->Transformation().SetScale(Engine::float4(2.0f, 2.0f, 2.0f, 1.0f));
		ent2->Update();
		delete loader;

		mScene->AddEntity(ent, -1, "Sponza");
		mScene->AddEntity(ent2, -1, "Roundcube");
		
		ent = new Engine::Entity();
		ent->Transformation().SetTranslation(Engine::float4(40.0f, 80.0f, 60.0f, 1.0f));
		ent->Transformation().Update();
		Engine::PointLight* light = new Engine::PointLight(Engine::float4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f, 0.0f);
		ent->GameObject().Add<Engine::LightComponent>(light);
		Engine::ColorBuffer* cubeMap = mCubeShadowMap->GetBuffer();
		Engine::mat4* cubeView = mCubeShadowMap->GetView();
		Engine::mat4* cubeProj = mCubeShadowMap->GetView();
		ent->GameObject().Add<Engine::ShadowComponent>(cubeMap, cubeView, cubeProj);
		mScene->AddEntity(ent, -1, "Light1");

		ent = new Engine::Entity();
		ent->Transformation().SetTranslation(Engine::float4(-450.0f, 90.0f, -40.0f, 1.0f));
		//ent->Transformation().SetTranslation(Engine::float4(-100.0f, 1200.0f, -40.0f, 1.0f));
		ent->Transformation().Update();
		Engine::SpotLight* spotlight = new Engine::SpotLight(Engine::float4(1.0f, 1.0f, 1.0f, 1.0f), Engine::float4(-0.7f, -0.2f, 1.0f, 0.0f), Engine::Math::PI / 4.0f, 0.0f, 0.0f);
		//Engine::SpotLight* spotlight = new Engine::SpotLight(Engine::float4(1.0f, 1.0f, 1.0f, 1.0f), Engine::float4(-0.01f, -1.0f, 0.0f, 0.0f), Engine::Math::PI / 4.0f, 0.0f, 0.0f);
		ent->GameObject().Add<Engine::LightComponent>(spotlight);
		Engine::ColorBuffer* shadowMap = mShadowMap->GetBuffer();
		Engine::mat4* shadowView = mShadowMap->GetView();
		Engine::mat4* shadowProj = mShadowMap->GetProjection();
		ent->GameObject().Add<Engine::ShadowComponent>(shadowMap, shadowView, shadowProj);
		mScene->AddEntity(ent, -1, "SpotLight");
		
		mFullscreenQuad = new Engine::Mesh(mRenderer, triangleVertices, sizeof(float) * 5, 6, triangleIndices, 6);

		ent = new Engine::Entity();
		Engine::Camera* cam = new Engine::PerspectiveCamera(Engine::float4(0.0f, 100.0f, 100.0f, 1.0f),
			Engine::float4(0.0f, 100.0f, 0.0f, 1.0f),
			Engine::float4(0.0f, 1.0f, 0.0f, 0.0f),
			60.0f * 3.141592f / 180.0f,
			1.0f,
			1.0f,
			5000.0f);
		ent->GameObject().Add<Engine::CameraComponent>(cam);
		mScene->AddEntity(ent, -1, "Camera");
				
		mQueryHeap = new Engine::QueryHeap(mRenderer->GetDevice(), Engine::QueryHeapType::TIMESTAMP, mTimestampCount);
		mTimestampResults = new Engine::GpuMappedBuffer();
		mTimestampResults->Init(mRenderer, mTimestampCount, sizeof(uint64_t) * 2, Engine::GpuMappedBuffer::Mode::READBACK);
		mTimestamps = new Engine::QueryHandle[mTimestampCount];
		for (size_t i = 0; i < mTimestampCount; i++)
		{
			mTimestamps[i] = mQueryHeap->Allocate();
		}

		Engine::ComputeContext* computeCtx = mRenderer->GetComputeContext();
		computeCtx->Begin();
		mVoxelize->Clear(mRenderer->Heap(), computeCtx);
		computeCtx->Finish();

		mRenderer->Flush();

		return true;
	}

	virtual void Shutdown()
	{
		mRenderer->Flush();

		delete mVoxelize;
		delete mDeferredRenderer;
		delete mDeferredPass;
		delete mShadowMap;
		delete mCubeShadowMap;
		delete mBlurPass;
		delete mAOPass;
		delete mImgui;
		delete mGizmo;
		delete mPicking;

		delete[] mMatrices;
		delete mMatricesBuffer;

		mTextureManager.RemoveAll();

		delete mFullscreenQuad;
		delete mRootSignaturePost;
		delete mPipelineStatePost;
		
		mWindow->Destroy();
	}

	float time = 0.0f;
	float position = 0.0f;
	float direction = 1.0f;
	bool refresh = true;

	float scale = 1.0f;
	char guiBuffer[255];

	int mousePos[2];
	int mouseClick[2];
	bool click = false;
	float pos[4];
	unsigned int selected = 0;

	virtual void Update()
	{
		/*position += 0.0005f * direction;
		if (position > 1.0f)
		{
			direction = -1.0f;
		}
		else if (position < -1.0f)
		{
			direction = 1.0f;
		}

		mScene->GetEntity("SpotLight")->Transformation().SetTranslation(Engine::float4(-100.0f + position * 400.0f, 1200.0f, -40.0f, 1.0f));
		mScene->GetEntity("SpotLight")->Transformation().Update();*/

		mScene->GetEntity("Roundcube")->Transformation().SetTranslation(Engine::float4(scale * 150.0f, 0.0f, -50.0f, 1.0f));
		mScene->GetEntity("Roundcube")->Transformation().SetRotation(Engine::quat(0.0f, scale * 0.1f, 0.0f));
		mScene->GetEntity("Roundcube")->Update();

		int w;
		int h;
		this->mWindow->GetSize(&w, &h);

		mWindow->Process();
		mScene->Update();

		Engine::Camera* cam = mScene->GetEntity("Camera")->GameObject().Get<Engine::CameraComponent>()->Get();
		((Engine::PerspectiveCamera*)cam)->SetAspectRatio((float)h / (float)w);

		if (mCamForward == true) cam->Move(1.0f);
		if (mCamBackward == true) cam->Move(-1.0f);
		if (mCamLeft == true) cam->Strafe(1.0f);
		if (mCamRight == true) cam->Strafe(-1.0f);

		if (mCenterMouse)
		{
			int x, y;
			mWindow->GetCursorPosition(&x, &y);

			cam->Rotate((w / 2 - x) * 0.001f, (h / 2 - y) * 0.001f);

			mWindow->SetCursorPosition(w / 2, h / 2);
		}

		cam->Process(5.0f);

		std::vector<Engine::RenderNode> nodes;
		mScene->Render(mScene->GetEntity("Camera"), nodes, mMatrices);

		memcpy(mMatricesBufferPtr, mMatrices, sizeof(Engine::mat4) * mOptions->Get<int>("Scene.MaxObjects"));

		Engine::BVH bvh(Engine::BVH::SplitMethod::SPLIT_SAH);
		bvh.Build(nodes);

		Engine::float4 cameraPos = cam->GetPosition();
		std::vector<Engine::RenderNode> cameraNodes = bvh.Render(cam);
		std::sort(cameraNodes.begin(), cameraNodes.end(),
			[cameraPos](const Engine::RenderNode& a, const Engine::RenderNode& b) -> bool
		{
			return Engine::lengthsq(a.mCentroid - cameraPos) < Engine::lengthsq(b.mCentroid - cameraPos);
		});

		Engine::AABB bounds = bvh.GetBounds();

		float maxSize = max(max(bounds.mMax.x - bounds.mMin.x, bounds.mMax.y - bounds.mMin.y), bounds.mMax.y - bounds.mMin.y);
		float maxOffset = max(max(-bounds.mMin.x, -bounds.mMin.y), -bounds.mMin.z);
		float aabb[2];
		aabb[0] = min(min(bounds.mMin.x, bounds.mMin.y), bounds.mMin.z);
		aabb[1] = max(max(bounds.mMax.x, bounds.mMax.y), bounds.mMax.z);

		time += 0.01f;

		/*Engine::ComputeContext* computeCtx = mRenderer->GetComputeContext();
		computeCtx->Begin();
		mVoxelize->GenerateMipmaps(mRenderer->Heap(), computeCtx);
		computeCtx->Finish();*/

		Engine::float4 rayDir;
		{
			float x = (2.0f * mouseClick[0]) / (float)w - 1.0f;
			float y = 1.0f - (2.0f * mouseClick[1]) / (float)h;
			float z = 1.0f;
			Engine::float4 ray_ndc(x, y, z, 0.0f);
			Engine::float4 ray_clip(ray_ndc.x, ray_ndc.y, -1.0f, 1.0f);
			Engine::float4 ray_eye = (clickProj * ray_clip) * Engine::float4(1.0f, 1.0f, 0.0f, 0.0f) + Engine::float4(0.0f, 0.0f, -1.0f, 0.0f);
			Engine::float4 ray_world = clickView * ray_eye;
			rayDir = ray_world;
		}
		if (click)
		{
			pos[0] = cam->GetPosition().x;
			pos[1] = cam->GetPosition().y;
			pos[2] = cam->GetPosition().z;
			pos[3] = cam->GetPosition().w;
			clickView = inverse(cam->GetViewMatrix());
			clickProj = inverse(cam->GetProjectionMatrix());
		}
		Engine::Ray r(Engine::float4(pos[0], pos[1], pos[2], 1.0f), rayDir);
		
		Engine::GraphicsContext* context = mRenderer->GetGraphicsContext();

		uint64_t freq = context->GetTimestampFrequency();
		context->Begin();
		context->EndQuery(mQueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, mTimestamps[0]);

		if (refresh)
		{
			//refresh = false;
			mShadowMap->Process(mScene->GetEntity("SpotLight"), mRenderer->Heap(), context, nodes, mMatricesBuffer);
			/*mBlurPass->SetSources(mShadowMap->GetBuffer(), nullptr);
			mBlurPass->Process(mRenderer->Heap(), context, 0);
			mBlurPass->SetSources(nullptr, mShadowMap->GetBuffer());
			mBlurPass->Process(mRenderer->Heap(), context, 1);*/
			//mShadowMap->GenerateMipmaps(mRenderer->Heap(), reinterpret_cast<Engine::ComputeContext*>(context));
			mCubeShadowMap->Process(mScene->GetEntity("Light1"), mRenderer->Heap(), context, nodes, mMatricesBuffer);
		}

		mPicking->Process(mScene->GetEntity("Camera"), mRenderer->Heap(), context, cameraNodes, mMatricesBuffer);

		mDeferredRenderer->Process(mScene->GetEntity("Camera"), mRenderer->Heap(), context, cameraNodes, mMatricesBuffer);

		mDeferredPass->SetSources(mDeferredRenderer->GetBuffer(0), mDeferredRenderer->GetBuffer(1), mDeferredRenderer->GetDepthBuffer());
		mDeferredPass->Process(mScene->GetEntity("Camera"), mRenderer->Heap(), context, nodes);

		mAOPass->SetSources(mDeferredRenderer->GetBuffer(1), mDeferredRenderer->GetDepthBuffer(), mVoxelize);
		mAOPass->Process(mScene->GetEntity("Camera"), mRenderer->Heap(), context, nodes);

		context->TransitionResource(mSwapChain->GetBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, true);
		context->SetRenderTargets(mSwapChain->GetBackBuffer(), mSwapChain->GetDepthBuffer());
		context->ClearColor(mSwapChain->GetBackBuffer(), 0.0f, 0.0f, 0.0f, 1.0f);
		context->ClearDepth(mSwapChain->GetDepthBuffer(), 1.0f, 0);

		mVoxelize->Generate(mRenderer->Heap(), context, nodes);
		mVoxelize->GenerateMipmaps(mRenderer->Heap(), reinterpret_cast<Engine::ComputeContext*>(context));

		context->SetPipelineState(mPipelineStatePost);
		context->SetRootSignature(mRootSignaturePost);
		context->SetDescriptorHeap(Engine::DescriptorHeap::CBV_SRV_UAV, mRenderer->Heap());
		context->TransitionResource(mSwapChain->GetBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, true);
		context->SetRenderTargets(mSwapChain->GetBackBuffer(), mSwapChain->GetDepthBuffer());
		context->SetViewport(0.0f, 0.0f, (float)w, (float)h);
		context->SetScissorRect(0.0f, 0.0f, (float)w, (float)h);
		context->ClearColor(mSwapChain->GetBackBuffer(), 0.0f, 0.0f, 0.0f, 1.0f);
		context->ClearDepth(mSwapChain->GetDepthBuffer(), 1.0f, 0);
		context->SetPrimitiveTopology(Engine::Graphics::TRIANGLELIST);
		context->SetConstants(1, Engine::DWParam(w), Engine::DWParam(h));
		context->SetConstants(2, 8);
		//context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(0, mRTColor->GetSRV().mGpuHandle);
		//context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(0, mDeferredRenderer->GetBuffer(0)->GetSRV().mGpuHandle);
		context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(0, mDeferredPass->GetBuffer()->GetSRV().mGpuHandle);
		//context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(0, mAOPass->GetBuffer()->GetSRV().mGpuHandle);
		//context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(0, guiFont->GetSRV().mGpuHandle);		
		//context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(0, mPicking->GetBuffer()->GetSRV().mGpuHandle);
		context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(4, mDeferredRenderer->GetDepthBuffer()->GetDepthSRV().mGpuHandle);
		mFullscreenQuad->Render(context);

		float d;
		bool hit = r.TestPlane(Engine::Plane(0.0f, 1.0f, 0.0f, 0.0f), d);

		mGizmo->Clear();
		mGizmo->Process(cameraNodes, mMatricesBuffer, mScene->GetState()->GetSelection());
		mGizmo->AddAxis(Engine::float4(0.0f, 0.0f, 0.0f, 1.0f), 30.0f);
		mGizmo->AddRay(r.origin, r.direction);
		if (hit)
		{
			mGizmo->AddPoint(r.origin + r.direction * d, 15.0f);
		}
		mGizmo->Render(mScene->GetEntity("Camera"), mRenderer->Heap(), context);

		mImgui->Update(context);
		mImgui->NewFrame();
		ImGui::Text("Hello World!");
		ImGui::InputText("Input", guiBuffer, sizeof(guiBuffer)); 
		ImGui::SliderFloat("float", &scale, -10.0f, 10.0f);
		mImgui->Render();

		context->TransitionResource(mSwapChain->GetBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, true);
		context->EndQuery(mQueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, mTimestamps[1]);
		context->ResolveQueryData(mQueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, mTimestamps[0], 64, mTimestampResults, 0);
		context->Finish();

		if (click)
		{
			click = false;
			mPicking->Readback(mRenderer->GetGraphicsContext());
			selected = mPicking->GetValue(mouseClick[0], mouseClick[1]);

			if (!(selected == *mScene->GetState()->GetSelection().begin() &&
				mScene->GetState()->GetSelection().size() == 1))
			{
				Engine::Command* cmd = new Engine::Command(Engine::Command::Deselect);
				for (int i : mScene->GetState()->GetSelection())
				{
					cmd->AddArg<int>(i);
				}
				mScene->Apply(cmd);

				if (selected != (unsigned int)(-1))
				{
					Engine::Command* cmd = new Engine::Command(Engine::Command::Select);
					cmd->AddArg<int>(selected);
					mScene->Apply(cmd);
				}
			}
		}


		Engine::Command* cmd = new Engine::Command(Engine::Command::Translate);
		cmd->AddArg<float>(0.0f);
		cmd->AddArg<float>(0.0f);
		cmd->AddArg<float>(0.5f);
		mScene->Apply(cmd);

		mTimestampResultsPtr = mTimestampResults->Map(0, sizeof(uint64_t) * mTimestampCount * 2);
		uint64_t* times = (uint64_t*)mTimestampResultsPtr;
		printf("Frame Time: %lfms; FPS: %lf\n", 1000.0 / freq * (times[1] - times[0]), 1000.0 / (1000.0 / freq * (times[1] - times[0])));
		mTimestampResults->Unmap();
		mSwapChain->SwapBuffers();
	}

	void Handle(const Engine::Keyboard::KeyChar& kc)
	{
		ImGuiIO& io = ImGui::GetIO();

		if (kc.mKey < 256)
		{
			io.AddInputCharacter((ImWchar)kc.mKey);
		}
	}

	void Handle(const Engine::Keyboard::KeyPressed& kp)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[kp.mKey] = 1;

		switch (kp.mKey)
		{
		case Engine::Keyboard::KEY_CONTROL_L: 
			io.KeyCtrl = true;
			break;

		case Engine::Keyboard::KEY_ALT_L:
			io.KeyAlt = true;
			break;

		case Engine::Keyboard::KEY_SHIFT_L:
			io.KeyShift = true;
			break;
		}
		
		static Engine::EventChannel chan;

		if (kp.mKey == Engine::Keyboard::KEY_ESCAPE)
		{
			chan.Broadcast(Engine::Scheduler::StopEvent());
		}

		if (kp.mKey == Engine::Keyboard::KEY_UP_ARROW ||
			kp.mKey == 'W')
		{
			mCamForward = true;
		}

		if (kp.mKey == Engine::Keyboard::KEY_DOWN_ARROW ||
			kp.mKey == 'S')
		{
			mCamBackward = true;
		}

		if (kp.mKey == Engine::Keyboard::KEY_LEFT_ARROW ||
			kp.mKey == 'A')
		{
			mCamLeft = true;
		}

		if (kp.mKey == Engine::Keyboard::KEY_RIGHT_ARROW ||
			kp.mKey == 'D')
		{
			mCamRight = true;
		}

		if (kp.mKey == Engine::Keyboard::KEY_SPACE && !mSwitchPressed)
		{
			mSwitchPressed = true;
			mCenterMouse = !mCenterMouse;
		}
	}

	void Handle(const Engine::Keyboard::KeyReleased& kp)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[kp.mKey] = 0;

		switch (kp.mKey)
		{
		case Engine::Keyboard::KEY_CONTROL_L:
			io.KeyCtrl = false;
			break;

		case Engine::Keyboard::KEY_ALT_L:
			io.KeyAlt = false;
			break;

		case Engine::Keyboard::KEY_SHIFT_L:
			io.KeyShift = false;
			break;
		}

		static Engine::EventChannel chan;

		if (kp.mKey == Engine::Keyboard::KEY_UP_ARROW ||
			kp.mKey == 'W')
		{
			mCamForward = false;
		}

		if (kp.mKey == Engine::Keyboard::KEY_DOWN_ARROW ||
			kp.mKey == 'S')
		{
			mCamBackward = false;
		}

		if (kp.mKey == Engine::Keyboard::KEY_LEFT_ARROW ||
			kp.mKey == 'A')
		{
			mCamLeft = false;
		}

		if (kp.mKey == Engine::Keyboard::KEY_RIGHT_ARROW ||
			kp.mKey == 'D')
		{
			mCamRight = false;
		}

		if (kp.mKey == Engine::Keyboard::KEY_SPACE)
		{
			mSwitchPressed = false;
		}
	}

	void Handle(const Engine::Mouse::MouseMove& m)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos.x = m.mX;
		io.MousePos.y = m.mY;

		mousePos[0] = m.mX;
		mousePos[1] = m.mY;
	}

	void Handle(const Engine::Mouse::ButtonPressed& bp)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[bp.mButton] = true;

		if (bp.mButton == 0)
		{
			mouseClick[0] = mousePos[0];
			mouseClick[1] = mousePos[1];
			click = true;
		}
	}

	void Handle(const Engine::Mouse::ButtonReleased& br)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[br.mButton] = false;
	}

	void Handle(const Engine::Window::Resize& r)
	{
		mImgui->Resize(r.mWidth, r.mHeight);
		mSwapChain->Resize(r.mWidth, r.mHeight);
		mDeferredRenderer->Resize(r.mWidth, r.mHeight);
		mDeferredPass->Resize(r.mWidth, r.mHeight);
		mAOPass->Resize(r.mWidth, r.mHeight);
		mPicking->Resize(r.mWidth, r.mHeight);
	}
};

int main()
{
	Engine::Log* log = new Engine::Log();
	log->AddOutput(new std::ofstream("Output.log"), Engine::Log::LOG_DEFAULT);

	Engine::Constants* options = new Engine::Constants(log, "../Data/SkyeCuillin/Config.conf");

	Engine::Input* input = new Engine::Input();

	Main* m = new Main(log, options);
	
	Engine::Core::Instance()->Add(input);
	Engine::Core::Instance()->Add(m);
	Engine::Core::Instance()->SetLog(log);
	Engine::Core::Instance()->SetOptions(options);
	Engine::Core::Instance()->Run();
	Engine::Core::Instance()->Dispose();

	delete m;
	delete input;
	delete options;
	delete log;

	Engine::MemoryDump(std::cout);

	//std::cin.get();

	return 0;
}