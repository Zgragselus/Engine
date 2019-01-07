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
#include "RenderPassConeTracingReflection.h"
#include "RenderPassConeTracingGI.h"
#include "RenderPassPicking.h"
#include "RenderPassResolve.h"
#include "Game/Scene/BVH.h"
#include "ImguiSubsystem.h"
#include "RenderGizmo.h"
#include "Core/Math/Primitive/Ray.h"
#include "DirectoryWatch.h"
#include "Editor.h"
#include "GraphicsProfiler.h"
#include "RenderPassGenerateMipmap.h"

class Main : public Engine::System
{
private:
	SkyeCuillin::Voxelize* mVoxelize;
	SkyeCuillin::RenderPassGBuffer* mDeferredRenderer;
	SkyeCuillin::RenderPassDeferredShading* mDeferredPass;
	SkyeCuillin::RenderPassConeTracingAO* mAOPass;
	SkyeCuillin::RenderPassConeTracingReflection* mReflectionPass;
	SkyeCuillin::RenderPassConeTracingGI* mGIPass;
	SkyeCuillin::ImguiSubsystem* mImgui;
	SkyeCuillin::RenderGizmo* mGizmo;
	SkyeCuillin::RenderPassPicking* mPicking;
	SkyeCuillin::RenderPassResolve* mResolve;
	SkyeCuillin::LightingSystem* mLightingSystem;

	SkyeCuillin::GraphicsProfiler* mGfxProfiler;

	Engine::Log* mLog;
	Engine::Window* mWindow;
	Engine::Constants* mOptions;
	Engine::D3DRenderer* mRenderer;
	Engine::D3DSwapChain* mSwapChain;

	Engine::Manager<Engine::Texture>* mTextureManager;
	Engine::Manager<Engine::Mesh>* mMeshManager;
	Engine::Manager<Engine::Model>* mModelManager;

	Engine::Scene* mScene;

	bool mCamForward = false;
	bool mCamBackward = false;
	bool mCamLeft = false;
	bool mCamRight = false;
	bool mCenterMouse = false;
	bool mSwitchPressed = false;

	Engine::mat4* mMatrices;
	Engine::GpuMappedBuffer* mMatricesBuffer;
	void* mMatricesBufferPtr;

	SkyeCuillin::Editor* mEditor;
	SkyeCuillin::DirectoryTree* mDirTree;

	double mFrameTimeGPU;
	double mFramesPerSecondGPU;
	double mDeltaTime;
	double mDeltaTimeAccumulator;

	Engine::Timer* mTimer;

	bool mShowProfiler;

	Engine::Entity* FromModel(const std::string& name, Engine::Model* mdl)
	{
		Engine::Entity* ent = new Engine::Entity(name);

		for (size_t i = 0; i < mdl->GetMeshesCount(); i++)
		{
			Engine::Mesh* m = mdl->GetMesh(i);
			Engine::Texture* diffuseMap = mdl->GetTexture(i, 0);
			Engine::Texture* normalsMap = mdl->GetTexture(i, 1);
			Engine::Texture* heightMap = mdl->GetTexture(i, 2);
			Engine::Texture* roughnessMap = mdl->GetTexture(i, 3);
			Engine::Texture* metallicMap = mdl->GetTexture(i, 4);

			Engine::Entity* child = new Engine::Entity(mdl->GetMesh(i)->GetName(), ent);
			child->GameObject().Add<Engine::MeshComponent>(m, mMeshManager);
			child->GameObject().Add<Engine::MaterialComponent>(diffuseMap, normalsMap, metallicMap, roughnessMap, heightMap, mTextureManager);
			child->Transformation().SetTranslation(mdl->GetTransformation(i));
		}

		return ent;
	}

public:
	Main(Engine::Log* log, Engine::Constants* options) : Engine::System("Main")
	{
		EnableUpdater();

		mLog = log;
		mOptions = options;

		mTimer = new Engine::Timer();
		mTimer->Reset();
		mDeltaTime = 0.0;
		mDeltaTimeAccumulator = 0.0;

		mWindow = new Engine::Window(log, options);
		mWindow->Create();

		mRenderer = new Engine::D3DRenderer(mWindow, log, options);
		mSwapChain = new Engine::D3DSwapChain(mRenderer, mWindow, log, options);
		mEditor = new SkyeCuillin::Editor();

		static Engine::EventChannel chan;
		chan.Add<Engine::Keyboard::KeyPressed>(*this);
		chan.Add<Engine::Keyboard::KeyReleased>(*this);
		chan.Add<Engine::Keyboard::KeyChar>(*this);
		chan.Add<Engine::Mouse::MouseMove>(*this);
		chan.Add<Engine::Mouse::ButtonPressed>(*this);
		chan.Add<Engine::Mouse::ButtonReleased>(*this);
		chan.Add<Engine::Mouse::MouseWheel>(*this);
		chan.Add<Engine::Window::Resize>(*this);

		mDirTree = new SkyeCuillin::DirectoryTree(mOptions->Get<std::string>("Editor.Resources"), options, log, mRenderer);

		mShowProfiler = false;
	}

	virtual ~Main()
	{
		delete mScene;

		mModelManager->RemoveAll();
		delete mModelManager;

		mMeshManager->RemoveAll();
		delete mMeshManager;

		mTextureManager->RemoveAll();
		delete mTextureManager;

		delete mEditor;
		delete mSwapChain;
		delete mRenderer;
		delete mWindow;

		delete mTimer;
	}

	virtual bool Init()
	{
		mMeshManager = new Engine::Manager<Engine::Mesh>();
		mModelManager = new Engine::Manager<Engine::Model>();
		mTextureManager = new Engine::Manager<Engine::Texture>();

		mScene = new Engine::Scene(mMeshManager, mTextureManager);

		SkyeCuillin::DirectoryWatch* watch = (SkyeCuillin::DirectoryWatch*)(Engine::Core::Instance()->Get("DirectoryWatch"));
		if (watch != nullptr)
		{
			watch->SetDirectoryTree(mDirTree);
		}
		mDirTree->SetManagers(mMeshManager, mModelManager, mTextureManager);
		mDirTree->Initialize();

		int w;
		int h;
		this->mWindow->GetSize(&w, &h);

		mImgui = new SkyeCuillin::ImguiSubsystem(mRenderer, w, h);
		ImGui::GetStyle().WindowBorderSize = 0;
		mGizmo = new SkyeCuillin::RenderGizmo(mLog, mRenderer, 64);

		mLightingSystem = new SkyeCuillin::LightingSystem(mRenderer, mOptions->Get<int>("Renderer.Lighting.MaxLights"), mOptions->Get<int>("Renderer.Lighting.VirtualShadowMapResolution"));
		mVoxelize = new SkyeCuillin::Voxelize(mRenderer, mLightingSystem, mOptions->Get<int>("Renderer.GlobalIllumination.VoxelResolution"), SkyeCuillin::Voxelize::MipmapCalculation::MIPMAP_3);
		mDeferredRenderer = new SkyeCuillin::RenderPassGBuffer(mRenderer, w, h, mOptions->Get<int>("Renderer.Antialiasing.SamplesMSAA"));
		mDeferredPass = new SkyeCuillin::RenderPassDeferredShading(mLog, mRenderer, mLightingSystem, w, h, mOptions->Get<int>("Renderer.Antialiasing.SamplesMSAA"), mOptions->Get<int>("Renderer.Antialiasing.SamplesMSAAGI"));
		mAOPass = new SkyeCuillin::RenderPassConeTracingAO(mRenderer, w, h, mOptions->Get<int>("Renderer.Antialiasing.SamplesMSAAGI"));
		mReflectionPass = new SkyeCuillin::RenderPassConeTracingReflection(mRenderer, w, h, mOptions->Get<int>("Renderer.Antialiasing.SamplesMSAAGI"));
		mGIPass = new SkyeCuillin::RenderPassConeTracingGI(mRenderer, w, h, mOptions->Get<int>("Renderer.Antialiasing.SamplesMSAAGI"));
		mPicking = new SkyeCuillin::RenderPassPicking(mRenderer, w, h, mGizmo);
		mResolve = new SkyeCuillin::RenderPassResolve(mLog, mRenderer, w, h, mOptions->Get<int>("Renderer.Antialiasing.SamplesMSAA"));

		mEditor->Resize(w, h);
		mEditor->SetScene(mScene);
		mEditor->SetPickingPass(mPicking);

		mMatrices = new Engine::mat4[mOptions->Get<int>("Scene.MaxObjects")];
		mMatricesBuffer = new Engine::GpuMappedBuffer();
		mMatricesBuffer->Init(mRenderer, mOptions->Get<int>("Scene.MaxObjects"), sizeof(float) * 16);
		mMatricesBufferPtr = mMatricesBuffer->Map();

		Engine::LoaderAssimp* loader = new Engine::LoaderAssimp(mLog, mRenderer);
		loader->SetManagers(mMeshManager, mModelManager, mTextureManager);
		Engine::Entity* ent = FromModel("sponza", loader->Load("../Data/Example05/Sponza/sponza.obj"));
		//Engine::Entity* ent = FromModel("env", loader->Load("../Data/Shared/Models/environment.obj"));
		Engine::Entity* ent2 = FromModel("sphere", loader->Load("../Data/Shared/Models/sphere.obj"));
		//ent2->Transformation().SetTranslation(Engine::float4(310.0f, 210.0f, -180.0f, 1.0f));
		ent2->Transformation().SetTranslation(Engine::float4(-1096.0f, 27.0f, 10.0f, 1.0f));
		ent2->Transformation().Update();
		Engine::Shape* shp = new Engine::Box(Engine::float4(25.0f, 25.0f, 25.0f, 1.0f));
		ent2->Children()[0]->GameObject().Add<Engine::CollisionComponent>(shp);
		Engine::RigidBody* rb = new Engine::RigidBody(0.0f);
		ent2->Children()[0]->GameObject().Add<Engine::RigidBodyComponent>(rb);
		ent2->Update();

		Engine::TerrainGrid* grid = new Engine::TerrainGrid(mRenderer, mLog, 4, 100.0f, 8, 8, 0, 0, 1.0f);
		Engine::Entity* terrain = new Engine::Entity("Terrain");
		terrain->GameObject().Add<Engine::TerrainComponent>(grid);
		mScene->AddEntity(terrain, -1);

		/*Engine::Entity* tmp = FromModel("cube", loader->Load("../Data/Shared/Models/cube.obj"));
		Engine::Mesh* m = tmp->Children()[0]->GameObject().Get<Engine::MeshComponent>()->GetMesh();
		Engine::MaterialComponent* mat = tmp->Children()[0]->GameObject().Get<Engine::MaterialComponent>();

		for (int i = 0; i < 100; i++)
		{
			Engine::Entity* cube = new Engine::Entity("Cube" + std::to_string(i));
			cube->Transformation().SetTranslation(Engine::float4((float)(rand() % 600 - 300), (float)(rand() % 300 - 150) + 500, (float)(rand() % 300 - 150), 1.0f));
			cube->Transformation().Update();
			cube->GameObject().Add<Engine::MeshComponent>(m, mMeshManager);
			cube->GameObject().Add<Engine::MaterialComponent>(*mat);
			Engine::Shape* shp2 = new Engine::Box(Engine::float4(25.0f, 25.0f, 25.0f, 1.0f));
			cube->GameObject().Add<Engine::CollisionComponent>(shp2);
			Engine::RigidBody* rb2 = new Engine::RigidBody(2.0f);
			cube->GameObject().Add<Engine::RigidBodyComponent>(rb2);
			mScene->AddEntity(cube, -1);
		}*/

		delete loader;

		mScene->AddEntity(ent, -1);
		mScene->AddEntity(ent2, -1);
		//mScene->AddEntity(tmp, -1);

		/*ent = new Engine::Entity("Light1");
		ent->Transformation().SetTranslation(Engine::float4(-440.0f, 380.0f, 60.0f, 1.0f));
		//ent->Transformation().SetTranslation(Engine::float4(540.0f, 300.0f, 60.0f, 1.0f));
		ent->Transformation().Update();
		Engine::PointLight* light = new Engine::PointLight(Engine::float4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f, 0.0f);
		ent->GameObject().Add<Engine::LightComponent>(light);
		mScene->AddEntity(ent, -1);*/

		/*ent = new Engine::Entity("Light2");
		ent->Transformation().SetTranslation(Engine::float4(-440.0f, 380.0f, 20.0f, 1.0f));
		//ent->Transformation().SetTranslation(Engine::float4(540.0f, 300.0f, 60.0f, 1.0f));
		ent->Transformation().Update();
		light = new Engine::PointLight(Engine::float4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f, 0.0f);
		ent->GameObject().Add<Engine::LightComponent>(light);
		mScene->AddEntity(ent, -1);

		ent = new Engine::Entity("Light3");
		ent->Transformation().SetTranslation(Engine::float4(-440.0f, 320.0f, 60.0f, 1.0f));
		//ent->Transformation().SetTranslation(Engine::float4(540.0f, 300.0f, 60.0f, 1.0f));
		ent->Transformation().Update();
		light = new Engine::PointLight(Engine::float4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f, 0.0f);
		ent->GameObject().Add<Engine::LightComponent>(light);
		mScene->AddEntity(ent, -1);

		ent = new Engine::Entity("Light4");
		ent->Transformation().SetTranslation(Engine::float4(-440.0f, 320.0f, 20.0f, 1.0f));
		//ent->Transformation().SetTranslation(Engine::float4(540.0f, 300.0f, 60.0f, 1.0f));
		ent->Transformation().Update();
		light = new Engine::PointLight(Engine::float4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f, 0.0f);
		ent->GameObject().Add<Engine::LightComponent>(light);
		mScene->AddEntity(ent, -1);*/

		ent = new Engine::Entity("SpotLight");
		ent->Transformation().SetTranslation(Engine::float4(-450.0f, 90.0f, -40.0f, 1.0f));
		//ent->Transformation().SetTranslation(Engine::float4(-100.0f, 1200.0f, -40.0f, 1.0f));
		ent->Transformation().Update();
		Engine::SpotLight* spotlight = new Engine::SpotLight(Engine::float4(1.0f, 1.0f, 1.0f, 1.0f), Engine::float4(-0.7f, -0.2f, 1.0f, 0.0f), 45.0f, 0.0f, 0.0f);
		//Engine::SpotLight* spotlight = new Engine::SpotLight(Engine::float4(1.0f, 1.0f, 1.0f, 1.0f), Engine::float4(-0.01f, -1.0f, 0.0f, 0.0f), 45.0f, 0.0f, 0.0f);
		ent->GameObject().Add<Engine::LightComponent>(spotlight);
		mScene->AddEntity(ent, -1);

		/*for (int i = 0; i < 2; i++)
		{
			ent = new Engine::Entity("SpotLight" + std::to_string(i));
			ent->Transformation().SetTranslation(Engine::float4((float)(rand() % 900 - 450), (float)(rand() % 100 + 50), (float)(rand() % 160 - 80), 1.0f));
			ent->Transformation().Update();
			Engine::SpotLight* spotlight = new Engine::SpotLight(Engine::float4(1.0f, 1.0f, 1.0f, 1.0f),
				Engine::normalize(Engine::float4(rand() % 100 - 50, rand() % 100 - 50, rand() % 100 - 50, 0.0f)),
				Engine::Math::PI / 4.0f, 0.0f, 0.0f);
			ent->GameObject().Add<Engine::LightComponent>(spotlight);
			mScene->AddEntity(ent, -1);
		}*/

		ent = new Engine::Entity("Camera");
		/*Engine::Camera* cam = new Engine::PerspectiveCamera(Engine::float4(100.0f, 100.0f, 0.0f, 1.0f),
			Engine::float4(0.0f, 100.0f, 0.0f, 1.0f),
			Engine::float4(0.0f, 1.0f, 0.0f, 0.0f),
			60.0f * 3.141592f / 180.0f,
			1.0f,
			1.0f,
			5000.0f,
			500.0f);*/
		Engine::Camera* cam = new Engine::PerspectiveCamera(Engine::float4(-1202.0f, 166.0f, -443.0f, 1.0f),
			Engine::float4(-1202.0f, 66.0f, 403.0f, 1.0f),
			Engine::float4(0.0f, 1.0f, 0.0f, 0.0f),
			60.0f * 3.141592f / 180.0f,
			1.0f,
			1.0f,
			5000.0f,
			500.0f);
		cam->Process(1.0f / 60.0f);
		ent->GameObject().Add<Engine::CameraComponent>(cam);
		mScene->AddEntity(ent, -1);

		mGfxProfiler = new SkyeCuillin::GraphicsProfiler(mRenderer, mLog, 64);
		mGfxProfiler->AddChannel("LightingSystem");
		mGfxProfiler->AddChannel("Picking");
		mGfxProfiler->AddChannel("GBuffer");
		mGfxProfiler->AddChannel("Reflection");
		mGfxProfiler->AddChannel("AmbientOcclusion");
		mGfxProfiler->AddChannel("GlobalIllumination");
		mGfxProfiler->AddChannel("Deferred");
		mGfxProfiler->AddChannel("Voxelization");
		mGfxProfiler->AddChannel("Resolve");
		mGfxProfiler->AddChannel("Gizmo");
		mGfxProfiler->AddChannel("UI");

		/*Engine::ComputeContext* computeCtx = mRenderer->GetComputeContext();
		computeCtx->Begin();
		mVoxelize->Clear(mRenderer->Heap(), computeCtx);
		computeCtx->Finish();*/

		mRenderer->Flush();

		return true;
	}

	virtual void Shutdown()
	{
		mRenderer->Flush();

		SkyeCuillin::DirectoryWatch* watch = (SkyeCuillin::DirectoryWatch*)(Engine::Core::Instance()->Get("DirectoryWatch"));
		if (watch != nullptr)
		{
			watch->SetDirectoryTree(nullptr);
		}
		delete mDirTree;

		delete mGfxProfiler;

		delete mVoxelize;
		delete mDeferredRenderer;
		delete mDeferredPass;
		delete mAOPass;
		delete mReflectionPass;
		delete mGIPass;
		delete mImgui;
		delete mGizmo;
		delete mPicking;
		delete mResolve;
		delete mLightingSystem;

		delete[] mMatrices;
		delete mMatricesBuffer;

		mWindow->Destroy();
	}

	float time = 0.0f;
	bool refresh = true;

	virtual void Update()
	{
		if (mDeltaTime >= 0.25)
		{
			mDeltaTime = 0.25;
		}
		mDeltaTimeAccumulator += mDeltaTime;
		long long frameBegin = mTimer->Elapsed();

		int w;
		int h;
		this->mWindow->GetSize(&w, &h);

		mEditor->SetCamera(mScene->GetEntity("Camera"));
		refresh = mEditor->mChange;
		mEditor->mChange = false;

		Engine::Camera* cam = mScene->GetEntity("Camera")->GameObject().Get<Engine::CameraComponent>()->Get();
		//printf("%f %f %f\n", cam->GetForward().x, cam->GetForward().y, cam->GetForward().z);
		if (cam->GetType() == Engine::Camera::Type::PERSPECTIVE)
		{
			((Engine::PerspectiveCamera*)cam)->SetAspectRatio((float)h / (float)w);
		}

		if (mCenterMouse)
		{
			int x, y;
			mWindow->GetCursorPosition(&x, &y);

			cam->Rotate((w / 2 - x) * 0.001f, (h / 2 - y) * 0.001f);

			mWindow->SetCursorPosition(w / 2, h / 2);
		}

		mWindow->Process();
		while (mDeltaTimeAccumulator >= 1.0 / 60.0)
		{
			if (mCamForward == true) cam->Move(1.0f);
			if (mCamBackward == true) cam->Move(-1.0f);
			if (mCamLeft == true) cam->Strafe(1.0f);
			if (mCamRight == true) cam->Strafe(-1.0f);

			cam->Process(1.0f / 60.0f);

			mScene->Update();
			mDeltaTimeAccumulator -= 1.0 / 60.0;

			if (mScene->GetSimulationState() == Engine::Scene::PhysicsState::RUNNING)
			{
				refresh = true;
			}
		}

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

		//printf("Rendering %d nodes out of total %d nodes\n", cameraNodes.size(), nodes.size());

		Engine::AABB bounds = bvh.GetBounds();

		float maxSize = std::max(std::max(bounds.mMax.x - bounds.mMin.x, bounds.mMax.y - bounds.mMin.y), bounds.mMax.y - bounds.mMin.y);
		float maxOffset = std::max(std::max(-bounds.mMin.x, -bounds.mMin.y), -bounds.mMin.z);
		float aabb[2];
		aabb[0] = std::min(std::min(bounds.mMin.x, bounds.mMin.y), bounds.mMin.z);
		aabb[1] = std::max(std::max(bounds.mMax.x, bounds.mMax.y), bounds.mMax.z);

		time += 0.01f;

		/*Engine::ComputeContext* computeCtx = mRenderer->GetComputeContext();
		computeCtx->Begin();
		mVoxelize->GenerateMipmaps(mRenderer->Heap(), computeCtx);
		computeCtx->Finish();*/

		Engine::GraphicsContext* context = mRenderer->GetGraphicsContext();

		context->Begin();
		mGfxProfiler->Reset(context);

		mGfxProfiler->BeginProfile(context, "LightingSystem");
		mLightingSystem->Process(mScene->GetEntity("Camera"), nodes, context, mRenderer->Heap(), mMatricesBuffer, &bvh);
		mGfxProfiler->EndProfile(context, "LightingSystem");

		mGfxProfiler->BeginProfile(context, "Picking");
		mPicking->Process(mScene->GetEntity("Camera"), mRenderer->Heap(), context, cameraNodes, mMatricesBuffer);
		mGfxProfiler->EndProfile(context, "Picking");

		mGfxProfiler->BeginProfile(context, "GBuffer");
		mDeferredRenderer->Process(mScene->GetEntity("Camera"), mRenderer->Heap(), context, cameraNodes, mMatricesBuffer);
		mGfxProfiler->EndProfile(context, "GBuffer");

		mGfxProfiler->BeginProfile(context, "Reflection");
		mReflectionPass->SetSources(mDeferredRenderer->GetBuffer(1), mDeferredRenderer->GetDepthBuffer(), mVoxelize);
		mReflectionPass->Process(mScene->GetEntity("Camera"), mRenderer->Heap(), context, nodes);
		mGfxProfiler->EndProfile(context, "Reflection");

		mGfxProfiler->BeginProfile(context, "AmbientOcclusion");
		mAOPass->SetSources(mDeferredRenderer->GetBuffer(1), mDeferredRenderer->GetDepthBuffer(), mVoxelize);
		mAOPass->Process(mScene->GetEntity("Camera"), mRenderer->Heap(), context, nodes);
		mGfxProfiler->EndProfile(context, "AmbientOcclusion");

		mGfxProfiler->BeginProfile(context, "GlobalIllumination");
		mGIPass->SetSources(mDeferredRenderer->GetBuffer(1), mDeferredRenderer->GetDepthBuffer(), mVoxelize);
		mGIPass->Process(mScene->GetEntity("Camera"), mRenderer->Heap(), context, nodes);
		mGfxProfiler->EndProfile(context, "GlobalIllumination");

		mGfxProfiler->BeginProfile(context, "Deferred");
		mDeferredPass->SetSources(mDeferredRenderer->GetBuffer(0), mDeferredRenderer->GetBuffer(1), mReflectionPass->GetBuffer(), mAOPass->GetBuffer(), mGIPass->GetBuffer(), mDeferredRenderer->GetDepthBuffer());
		mDeferredPass->Process(mScene->GetEntity("Camera"), mRenderer->Heap(), context, nodes);
		mGfxProfiler->EndProfile(context, "Deferred");

		context->TransitionResource(mSwapChain->GetBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, true);
		context->SetRenderTargets(mSwapChain->GetBackBuffer(), mSwapChain->GetDepthBuffer());
		context->ClearColor(mSwapChain->GetBackBuffer(), 0.0f, 0.0f, 0.0f, 1.0f);
		context->ClearDepth(mSwapChain->GetDepthBuffer(), 1.0f, 0);

		mGfxProfiler->BeginProfile(context, "Voxelization");
		//if (refresh)
		{
			mVoxelize->Clear(mRenderer->Heap(), reinterpret_cast<Engine::ComputeContext*>(context));
			mVoxelize->Generate(mRenderer->Heap(), context, nodes, mMatricesBuffer);
			mVoxelize->GenerateMipmaps(mRenderer->Heap(), reinterpret_cast<Engine::ComputeContext*>(context));
		}
		mGfxProfiler->EndProfile(context, "Voxelization");

		context->SetDescriptorHeap(Engine::DescriptorHeap::CBV_SRV_UAV, mRenderer->Heap());
		context->TransitionResource(mSwapChain->GetBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, true);
		context->SetRenderTargets(mSwapChain->GetBackBuffer(), mSwapChain->GetDepthBuffer());
		context->SetViewport(0.0f, 0.0f, (float)w, (float)h);
		context->SetScissorRect(0.0f, 0.0f, (float)w, (float)h);
		context->ClearColor(mSwapChain->GetBackBuffer(), 0.0f, 0.0f, 0.0f, 1.0f);
		context->ClearDepth(mSwapChain->GetDepthBuffer(), 1.0f, 0);

		mGfxProfiler->BeginProfile(context, "Resolve");
		mResolve->SetInput(mDeferredPass->GetBuffer());
		mResolve->SetInputDepth(mDeferredRenderer->GetDepthBuffer());
		mResolve->Process(mSwapChain, mRenderer->Heap(), context);
		mGfxProfiler->EndProfile(context, "Resolve");

		mGfxProfiler->BeginProfile(context, "Gizmo");
		mGizmo->Clear();
		mGizmo->Process(cameraNodes, mScene, mMatricesBuffer, mScene->GetState()->GetSelection());
		if (mEditor->mEditMode == SkyeCuillin::Editor::Mode::TRANSLATE ||
			mEditor->mEditMode == SkyeCuillin::Editor::Mode::SCALE)
		{
			mGizmo->AddTranslationAxis(mScene->GetGizmoPosition(), mEditor->mAxis, mEditor->mAxisScale, mEditor->mActiveAxis, mEditor->mActivePlane);
		}
		else if (mEditor->mEditMode == SkyeCuillin::Editor::Mode::ROTATE)
		{
			mGizmo->AddRotationAxis(mScene->GetGizmoPosition(), mEditor->mAxis, mEditor->mAxisScale, mEditor->mActiveAxis);
		}
		mGizmo->ProcessIcons(nodes);

		mGizmo->Render(mScene->GetEntity("Camera"), mRenderer->Heap(), context);
		mGfxProfiler->EndProfile(context, "Gizmo");

		mGfxProfiler->BeginProfile(context, "UI");
		mImgui->Update(context);
		mImgui->NewFrame();
		ImGui::SetNextWindowSize(ImVec2((float)w, 48.0f), ImGuiCond_Always);
		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		if (ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar))
		{
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("New", "CTRL+N", nullptr))
					{

					}

					if (ImGui::MenuItem("Open", "CTRL+O", nullptr))
					{
						std::string filename;
						bool open = Engine::FileDialog::Show(mLog, "Open Scene", "Skye Cuillin Scene\0*.scene\0All Files\0*.*\0", Engine::FileDialog::Type::OPEN_FILE_DIALOG, filename);
						if (open)
						{
							// TODO

							mScene->LoadScene(filename);

							//LoadScene
						}
					}

					if (ImGui::MenuItem("Save", "CTRL+S", nullptr))
					{
						std::string filename;
						bool save = Engine::FileDialog::Show(mLog, "Save Scene", "Skye Cuillin Scene\0*.scene\0All Files\0*.*\0", Engine::FileDialog::Type::SAVE_FILE_DIALOG, filename);
						if (save)
						{
							mScene->SaveScene(filename);
						}
					}

					if (ImGui::MenuItem("Export", "CTRL+E", nullptr))
					{
						/*std::string filename;
						bool save = Engine::FileDialog::Show(mLog, "Save Scene", "Skye Cuillin Scene\0*.bin\0All Files\0*.*\0", Engine::FileDialog::Type::SAVE_FILE_DIALOG, filename);
						if (save)
						{
							mScene->ExportScene(filename);
						}*/
					}

					if (ImGui::MenuItem("Exit"))
					{

					}

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Edit"))
				{
					if (ImGui::MenuItem("Undo", "CTRL+Z", nullptr, mScene->CanUndo()))
					{
						mScene->PerformUndo();
						mEditor->mChange = true;
					}

					if (ImGui::MenuItem("Redo", "CTRL+Y", nullptr, mScene->CanRedo()))
					{
						mScene->PerformRedo();
						mEditor->mChange = true;
					}

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("View"))
				{
					ImGui::MenuItem("Show Profiler", "", &mShowProfiler);

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Create"))
				{
					static bool gameObject;
					static bool deleteGameObject;

					if (ImGui::MenuItem("Game Object", "", &gameObject))
					{
						mScene->AddEntity(new Engine::Entity("GameObject"), -1);
					}

					if (ImGui::MenuItem("Delete Game Object", "", &deleteGameObject))
					{
						Engine::Command* cmd = new Engine::Command(Engine::Command::Delete);

						for (int id : mScene->GetState()->GetSelection())
						{
							cmd->AddArg<std::string>(mScene->GetEntity(id)->Serialize());
						}

						mScene->Apply(cmd);

						mScene->GetState()->GetSelection().clear();
					}

					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}

			static int mode = 0;
			ImGui::RadioButton("Select", &mode, 0); ImGui::SameLine();
			ImGui::RadioButton("Translate", &mode, 1); ImGui::SameLine();
			ImGui::RadioButton("Rotate", &mode, 2); ImGui::SameLine();
			ImGui::RadioButton("Scale", &mode, 3); ImGui::SameLine();

			static int basis = 0;
			ImGui::RadioButton("Global", &basis, 0); ImGui::SameLine();
			ImGui::RadioButton("Local", &basis, 1); ImGui::SameLine();

			static int simulation = 0;
			ImGui::RadioButton("Stop", &simulation, 0); ImGui::SameLine();
			ImGui::RadioButton("Play", &simulation, 1); ImGui::SameLine();
			ImGui::RadioButton("Pause", &simulation, 2); ImGui::SameLine();

			static int snap = 0;
			ImGui::RadioButton("NoSnap", &snap, 0); ImGui::SameLine();
			ImGui::RadioButton("Snap", &snap, 1); ImGui::SameLine();
			ImGui::RadioButton("SnapGrid", &snap, 2); ImGui::SameLine(ImGui::GetWindowWidth() - 160);

			char fps[255] = { 0 };
			snprintf(fps, sizeof(fps), "%.2f fps (%.2f ms)", mFramesPerSecondGPU, mDeltaTime);
			ImGui::LabelText("", fps);

			mEditor->mEditMode = SkyeCuillin::Editor::Mode(mode);
			mEditor->mSnapMode = SkyeCuillin::Editor::SnapMode(snap);
			if ((basis == 0 ? false : true) != mEditor->mLocalSpace)
			{
				mEditor->SetLocalSpace(basis == 0 ? false : true);
			}

			if (simulation == 1 && mScene->GetSimulationState() == Engine::Scene::PhysicsState::STOPPED)
			{
				mScene->StartSimulation();
			}
			else if (simulation == 0 && mScene->GetSimulationState() == Engine::Scene::PhysicsState::RUNNING)
			{
				mScene->StopSimulation();
			}
		}
		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::SetNextWindowSize(ImVec2(300.0f, (float)h - 48.0f), ImGuiCond_Always);
		ImGui::SetNextWindowPos(ImVec2((float)w - 300.0f, 48.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		if (ImGui::Begin("Edit", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus))
		{
			mEditor->ImguiSelectionEditor();
		}
		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::SetNextWindowSize(ImVec2(300.0f, ((float)h - 48.0f) / 2.0f), ImGuiCond_Always);
		ImGui::SetNextWindowPos(ImVec2(0.0f, 48.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		if (ImGui::Begin("Scenegraph", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus))
		{
			mEditor->ImguiScenegraphEditor();
		}
		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::SetNextWindowSize(ImVec2(300.0f, ((float)h - 48.0f) / 2.0f), ImGuiCond_Always);
		ImGui::SetNextWindowPos(ImVec2(0.0f, 48.0f + ((float)h - 48.0f) / 2.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		if (ImGui::Begin("Directory View", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus))
		{
			mDirTree->Imgui();
		}
		ImGui::End();
		ImGui::PopStyleVar();

		if (ImGui::Begin("Debug"))
		{
			//ImGui::Image((ImTextureID)(mReflectionPass->GetBuffer()->GetSRV().mGpuHandle.ptr), ImGui::GetContentRegionAvail());
			//ImGui::Image((ImTextureID)(mGIPass->GetBuffer()->GetSRV().mGpuHandle.ptr), ImGui::GetContentRegionAvail());
			ImGui::Image((ImTextureID)(mLightingSystem->GetShadowMap()->GetSRV().mGpuHandle.ptr), ImGui::GetContentRegionAvail());
			//ImGui::Image((ImTextureID)(mPicking->GetBuffer()->GetSRV().mGpuHandle.ptr), ImGui::GetContentRegionAvail());
		}
		ImGui::End();

		if (ImGui::Begin("Manager"))
		{
			ImGui::BeginChild("Records", ImVec2(0, -32), false);
			if (ImGui::TreeNodeEx("Mesh Manager:", ImGuiTreeNodeFlags_DefaultOpen))
			{
				std::vector<std::string> records;
				mMeshManager->FilterKeys("", records);

				for (const std::string& r : records)
				{
					if (ImGui::TreeNodeEx(r.c_str(), ImGuiTreeNodeFlags_Leaf))
					{
						ImGui::TreePop();
					}
				}

				ImGui::TreePop();
			}
			ImGui::EndChild();
		}
		ImGui::End();

		if (Engine::ComponentStatic::mEditedComponent != nullptr)
		{
			mEditor->ImguiManagerList<Engine::Texture>("Select Texture", mTextureManager);
		}

		if (Engine::ComponentStatic::mEditedMeshComponent != nullptr)
		{
			mEditor->ImguiManagerList<Engine::Mesh>("Select Mesh", mMeshManager);
		}

		if (mShowProfiler)
		{
			if (ImGui::Begin("Profile", &mShowProfiler))
			{
				char tmp[255] = { 0 };
				snprintf(tmp, sizeof(tmp), "%.2f ms", mGfxProfiler->GetTime("LightingSystem")); ImGui::LabelText("LightingSystem", tmp);
				snprintf(tmp, sizeof(tmp), "%.2f ms", mGfxProfiler->GetTime("Picking")); ImGui::LabelText("Picking", tmp);
				snprintf(tmp, sizeof(tmp), "%.2f ms", mGfxProfiler->GetTime("GBuffer")); ImGui::LabelText("GBuffer", tmp);
				snprintf(tmp, sizeof(tmp), "%.2f ms", mGfxProfiler->GetTime("Reflection")); ImGui::LabelText("Reflection", tmp);
				snprintf(tmp, sizeof(tmp), "%.2f ms", mGfxProfiler->GetTime("AmbientOcclusion")); ImGui::LabelText("AmbientOcclusion", tmp);
				snprintf(tmp, sizeof(tmp), "%.2f ms", mGfxProfiler->GetTime("GlobalIllumination")); ImGui::LabelText("GlobalIllumination", tmp);
				snprintf(tmp, sizeof(tmp), "%.2f ms", mGfxProfiler->GetTime("Deferred")); ImGui::LabelText("Deferred", tmp);
				snprintf(tmp, sizeof(tmp), "%.2f ms", mGfxProfiler->GetTime("Voxelization")); ImGui::LabelText("Voxelization", tmp);
				snprintf(tmp, sizeof(tmp), "%.2f ms", mGfxProfiler->GetTime("Resolve")); ImGui::LabelText("Resolve", tmp);
				snprintf(tmp, sizeof(tmp), "%.2f ms", mGfxProfiler->GetTime("Gizmo")); ImGui::LabelText("Gizmo", tmp);
				snprintf(tmp, sizeof(tmp), "%.2f ms", mGfxProfiler->GetTime("UI")); ImGui::LabelText("UI", tmp);
			}
			ImGui::End();
		}

		mImgui->Render();
		mGfxProfiler->EndProfile(context, "UI");

		context->TransitionResource(mSwapChain->GetBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, true);
		mGfxProfiler->Resolve(context);
		context->Finish();

		if (mEditor->mAction == SkyeCuillin::Editor::Action::SELECT)
		{
			mPicking->Readback(mRenderer->GetGraphicsContext());
		}

		mEditor->Process();

		mGfxProfiler->Process();

		mFrameTimeGPU = mGfxProfiler->GetTime("Default");
		mFramesPerSecondGPU = 1000.0 / mFrameTimeGPU;

		mSwapChain->SwapBuffers();

		mScene->ProcessChanges();

		mDirTree->ProcessChanges();

		long long frameEnd = mTimer->Elapsed();
		mDeltaTime = ((double)frameEnd - (double)frameBegin) * 0.001;
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

		Engine::Input* input = (Engine::Input*)Engine::Core::Instance()->Get("Input");
		if (input->GetKeyboard()->GetKeyState(Engine::Keyboard::KEY_CONTROL_L))
		{
			io.AddInputCharacter((ImWchar)kp.mKey);
		}

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

		if (!io.WantCaptureKeyboard)
		{
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

			mEditor->KeyPress(kp.mKey);
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

		if (!io.WantCaptureKeyboard)
		{
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

			mEditor->KeyRelease(kp.mKey);
		}
	}

	void Handle(const Engine::Mouse::MouseMove& m)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos.x = (float)m.mX;
		io.MousePos.y = (float)m.mY;

		/*mousePos[0] = m.mX;
		mousePos[1] = m.mY;*/

		if (!io.WantCaptureMouse)
		{
			mEditor->MouseMove(m.mX, m.mY);
		}
	}

	void Handle(const Engine::Mouse::MouseWheel& m)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheel = (float)m.mDelta / 120.0f;
	}

	void Handle(const Engine::Mouse::ButtonPressed& bp)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[bp.mButton] = true;
		//mEditor->TreeViewPress(bp.mButton);

		/*if (bp.mButton == 0)
		{
			mouseClick[0] = mousePos[0];
			mouseClick[1] = mousePos[1];
			click = true;
		}*/

		if (!io.WantCaptureMouse)
		{
			int mouseX, mouseY;
			Engine::Input* input = (Engine::Input*)Engine::Core::Instance()->Get("Input");
			input->GetMouse()->GetPosition(mouseX, mouseY);
			mEditor->MousePress(bp.mButton, mouseX, mouseY);
		}
	}

	void Handle(const Engine::Mouse::ButtonReleased& br)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[br.mButton] = false;
		//mEditor->TreeViewRelease(br.mButton);

		if (!io.WantCaptureMouse)
		{
			int mouseX, mouseY;
			Engine::Input* input = (Engine::Input*)Engine::Core::Instance()->Get("Input");
			input->GetMouse()->GetPosition(mouseX, mouseY);
			mEditor->MouseRelease(br.mButton, mouseX, mouseY);
		}

		ImGuiContext* ctx = ImGui::GetCurrentContext();
		if (ctx->DragDropPayload.Data != nullptr && (std::string(ctx->DragDropPayload.DataType) == "RESOURCE_MODEL"))
		{
			Engine::Manager<Engine::Model>::Node* node = *((Engine::Manager<Engine::Model>::Node**)ctx->DragDropPayload.Data);

			Engine::Entity* ent = new Engine::Entity("Temp");

			for (size_t i = 0; i < node->Get()->GetMeshesCount(); i++)
			{
				Engine::Mesh* m = node->Get()->GetMesh(i);
				Engine::Texture* diffuseMap = node->Get()->GetTexture(i, 0);
				Engine::Texture* normalsMap = node->Get()->GetTexture(i, 1);
				Engine::Texture* metallicMap = node->Get()->GetTexture(i, 2);
				Engine::Texture* roughnessMap = node->Get()->GetTexture(i, 3);
				Engine::Texture* heightMap = node->Get()->GetTexture(i, 4);

				Engine::Entity* child = new Engine::Entity(node->Get()->GetMesh(i)->GetName(), ent);
				child->GameObject().Add<Engine::MeshComponent>(m, mMeshManager);
				child->GameObject().Add<Engine::MaterialComponent>(diffuseMap, normalsMap, metallicMap, roughnessMap, heightMap, mTextureManager);
				child->Transformation().SetTranslation(node->Get()->GetTransformation(i));
			}

			Engine::Command* cmd = new Engine::Command(Engine::Command::Create);
			cmd->AddArg<std::string>(ent->Serialize());

			mScene->Apply(cmd);

			//mScene->AddEntity(ent, -1);
		}
		//printf("S %lx\n", ctx->DragDropPayload.Data);
	}

	void Handle(const Engine::Window::Resize& r)
	{
		mImgui->Resize(r.mWidth, r.mHeight);
		mSwapChain->Resize(r.mWidth, r.mHeight);
		mDeferredRenderer->Resize(r.mWidth, r.mHeight);
		mDeferredPass->Resize(r.mWidth, r.mHeight);
		mAOPass->Resize(r.mWidth, r.mHeight);
		mGIPass->Resize(r.mWidth, r.mHeight);
		mReflectionPass->Resize(r.mWidth, r.mHeight);
		mPicking->Resize(r.mWidth, r.mHeight);
		mEditor->Resize(r.mWidth, r.mHeight);
		mResolve->Resize(r.mWidth, r.mHeight);
	}
};

int main()
{
	Engine::Log* log = new Engine::Log();
	log->AddOutput(new std::ofstream("Output.log"), Engine::Log::LOG_DEFAULT);

	Engine::Constants* options = new Engine::Constants(log, "../Data/SkyeCuillin/Config.conf");

	Engine::Input* input = new Engine::Input();

	Main* m = new Main(log, options);
	SkyeCuillin::DirectoryWatch* dw = new SkyeCuillin::DirectoryWatch(log, options);

	Engine::Core::Instance()->Add(input);
	Engine::Core::Instance()->Add(m);
	Engine::Core::Instance()->Add(dw);
	Engine::Core::Instance()->SetLog(log);
	Engine::Core::Instance()->SetOptions(options);
	Engine::Core::Instance()->Run();
	Engine::Core::Instance()->Dispose();

	delete dw;
	delete m;
	delete input;
	delete options;
	delete log;

	Engine::MemoryDump(std::cout);

	//std::cin.get();

	return 0;
}