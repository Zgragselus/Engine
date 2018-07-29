#include "Main.h"
#include "Graphics/Graphics.h"
#include "Game/Game.h"

class Main : public Engine::System
{
private:
	Engine::Log* mLog;
	Engine::Window* mWindow;
	//Engine::Renderer* mRenderer;
	Engine::D3DRenderer* mRenderer;
	Engine::D3DSwapChain* mSwapChain;
	
	//Engine::Manager<Engine::Shader> mShaderManager;

	Engine::RootSignature* mRootSignature;
	Engine::PipelineState* mPipelineState;
	Engine::RootSignature* mRootSignaturePost;
	Engine::PipelineState* mPipelineStatePost;
	Engine::D3DShader* mShader;
	Engine::D3DShader* mShaderPost;
	Engine::GpuMappedBuffer* mConstantBuffer;

	Engine::ColorBuffer* mRTColor;
	Engine::DepthBuffer* mRTDepth;

	Engine::Scene* mScene;
	Engine::Mesh* mFullscreenQuad;

	float angle = 0.0f;
	bool mCamForward = false;
	bool mCamBackward = false;
	bool mCamLeft = false;
	bool mCamRight = false;
	bool mCenterMouse = false;
	bool mSwitchPressed = false;
	void* mCbuffer;

public:
	Main(Engine::Log* log, Engine::Constants* options) : Engine::System("Main")
	{
		EnableUpdater();

		mLog = log;

		mWindow = new Engine::Window(log, options);
		mWindow->Create();

		mRenderer = new Engine::D3DRenderer(mWindow, log, options);
		mSwapChain = new Engine::D3DSwapChain(mRenderer, mWindow, log, options);

		mScene = new Engine::Scene(nullptr, nullptr);

		static Engine::EventChannel chan;
		chan.Add<Engine::Keyboard::KeyPressed>(*this);
		chan.Add<Engine::Keyboard::KeyReleased>(*this);
		chan.Add<Engine::Mouse::MouseMove>(*this);
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
		mRootSignature = new Engine::RootSignature(mRenderer->GetDevice(), 2, 1);
		//(*mRootSignature)[0].InitAsConstants(0, 16);
		//(*mRootSignature)[1].InitAsConstants(1, 16);
		(*mRootSignature)[0].InitAsConstantBuffer(0);
		(*mRootSignature)[1].InitAsDescriptorTable(1);
		(*mRootSignature)[1].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);
		mRootSignature->InitStaticSampler(0, Engine::Sampler::CreateSamplerD3D(Engine::Sampler::Filter::ANISOTROPIC,
			Engine::Sampler::Address::WRAP,
			Engine::Sampler::Address::WRAP,
			Engine::Sampler::Address::WRAP,
			0.0f,
			16,
			Engine::Sampler::ComparisonFunc::NEVER,
			1.0f, 1.0f, 1.0f, 1.0f,
			0.0f,
			32.0f));
		mRootSignature->Finalize(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		mRootSignaturePost = new Engine::RootSignature(mRenderer->GetDevice(), 3, 1);
		(*mRootSignaturePost)[0].InitAsDescriptorTable(1);
		(*mRootSignaturePost)[0].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);
		(*mRootSignaturePost)[1].InitAsConstants(0, 2);
		(*mRootSignaturePost)[2].InitAsConstants(1, 1);
		mRootSignaturePost->InitStaticSampler(0, Engine::Sampler::CreateSamplerD3D(Engine::Sampler::Filter::MIN_POINT_MAG_POINT_MIP_POINT,
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
		mShader = new Engine::D3DShader("../Data/Example05/Color.hlsl",
			std::vector<Engine::D3DShader::ShaderEntryPoint>
		{
			Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::VERTEX_SHADER, "VS"),
			Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::PIXEL_SHADER, "PS")
		});

		mShaderPost = new Engine::D3DShader("../Data/Example05/Post.hlsl",
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
		mPipelineState = new Engine::PipelineState(mRenderer->GetDevice(),
			this->mRootSignature,
			this->mShader,
			Engine::BlendState::CreateStateD3D(true),
			Engine::RasterizerState::CreateStateD3D(),
			&dsState,
			&inputLayout,
			Engine::PipelineState::PrimitiveType::TRIANGLE,
			rtvFormats,
			Engine::Graphics::Format::D32F,
			8,
			0);

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

		Engine::LoaderAssimp* loader = new Engine::LoaderAssimp(mLog, mRenderer);
		//Engine::Entity* ent = loader->Load("../Data/Example05/sibenik/sibenik.obj");
		Engine::Entity* ent = loader->Load("../Data/Example05/Sponza/sponza.obj");
		//Engine::Entity* ent = loader->Load("../Data/Example05/sanMiguel/sanMiguel.obj");
		mScene->AddEntity(ent, -1);

		mFullscreenQuad = new Engine::Mesh(mRenderer, triangleVertices, sizeof(float) * 5, 6, triangleIndices, 6);
		/*ent = new Engine::Entity();
		ent->GameObject().Add<Engine::MeshComponent>(mesh);
		mScene->AddEntity(ent, -1, "Triangle");*/

		ent = new Engine::Entity("Camera");
		Engine::Camera* cam = new Engine::PerspectiveCamera(Engine::float4(0.0f, 100.0f, 100.0f, 1.0f),
			Engine::float4(0.0f, 100.0f, 0.0f, 1.0f),
			Engine::float4(0.0f, 1.0f, 0.0f, 0.0f),
			45.0f,
			1.0f,
			1.0f,
			5000.0f);
		ent->GameObject().Add<Engine::CameraComponent>(cam);
		mScene->AddEntity(ent, -1);

		Engine::mat4 view = cam->GetViewMatrix();
		Engine::mat4 proj = cam->GetProjectionMatrix();

		float cbvData[64] = {};
		memcpy(&cbvData[0], &view, sizeof(float) * 16);
		memcpy(&cbvData[16], &proj, sizeof(float) * 16);
		for (int i = 32; i < 64; i++)
		{
			cbvData[i] = (float)(rand() % 1000) * 0.001f;
		}

		mConstantBuffer = new Engine::GpuMappedBuffer();
		mConstantBuffer->Init(mRenderer, 64, sizeof(float));
		mCbuffer = mConstantBuffer->Map();

		int w;
		int h;
		this->mWindow->GetSize(&w, &h);

		mRTColor = new Engine::ColorBuffer(mRenderer, 0.0f, 0.0f, 0.0f, 1.0f);
		mRTColor->Create(w * 2, h * 2, 1, Engine::Graphics::Format::RGBA8, false, 8, 0);

		mRTDepth = new Engine::DepthBuffer(mRenderer, 1.0f, 0);
		mRTDepth->Create(w * 2, h * 2, Engine::Graphics::Format::D32F, false, 8, 0);

		mRenderer->Flush();
		
/*		InitializeBuffer(this, ptr, numElements * elementSize);
		float* ptr;
		mConstantBuffer->Map(0, 0, (void**)&ptr);
		for (int i = 0; i < 64; i++)
		{
			ptr[i] = (float)(rand() % 1000) * 0.001f;
		}
		mConstantBuffer->Unmap(0, sizeof(float) * 64);*/

		/*Engine::LoaderAssimp* loader = new Engine::LoaderAssimp(mLog);

		Engine::Model* mdl2 = loader->Load("../Data/Example05/Sponza/sponza.obj");
		Engine::ModelEntity* ent2 = new Engine::ModelEntity(mdl2);
		mScene->AddEntity(ent2, 0, "model2");

		float vbo[] =
		{
			-1.0f, -1.0f, 0.0f,
			1.0f, -1.0f, 0.0f,
			0.0f, 1.0f, 0.0f
		};

		unsigned int ibo[] = { 0, 1, 2 };

		std::vector<Engine::Mesh::Attribute> attribs;
		attribs.push_back(Engine::Mesh::Attribute(0, 3, Engine::Mesh::Attribute::FLOAT));

		Engine::Mesh* m = new Engine::Mesh((float*)vbo, sizeof(float) * 3, 3, (unsigned int*)ibo, 3, attribs);

		Engine::Shader* s = mShaderManager.Load<Engine::Shader>("../Data/Example05/Base.glsl");

		struct color_uniform
		{
			float world[16];
			float view[16];
			float projection[16];
		};

		color_uniform uniform_data = { 0.0f };

		Engine::Material* mat = new Engine::Material(s);
		mat->SetUniform("Sc_World", Engine::Material::UniformParamType::UNIFORM_PARAM_MAT4X4);
		mat->SetUniform("Sc_View", Engine::Material::UniformParamType::UNIFORM_PARAM_MAT4X4);
		mat->SetUniform("Sc_Projection", Engine::Material::UniformParamType::UNIFORM_PARAM_MAT4X4);
		mat->BuildUniformIndex();

		Engine::TextureUnitState state;

		Engine::Model* mdl = new Engine::Model();

		mdl->Add(m, mat, &uniform_data, sizeof(uniform_data), state);

		Engine::ModelEntity* ent = new Engine::ModelEntity(mdl);

		mScene->AddEntity(ent, 0, "model1");

		int w, h;
		mWindow->GetSize(&w, &h);
		Engine::CameraEntity* cam = new Engine::CameraEntity(new Engine::PerspectiveCamera(
			Engine::float4(0, 20, 1, 1),
			Engine::float4(0, 20, 0, 1),
			Engine::float4(0, 1, 0, 1),
			45.0f,
			(float)h / (float)w,
			1.0f,
			10000.0f));
		mScene->AddEntity(cam, 0, "camera1");

		delete loader;*/

		return true;
	}

	virtual void Shutdown()
	{
		mRenderer->Flush();

		delete mConstantBuffer;
		delete mRootSignature;
		delete mShader;
		delete mPipelineState;
		
		mWindow->Destroy();
	}

	virtual void Update()
	{
		/*Engine::CameraEntity* cam = (Engine::CameraEntity*)mScene->GetEntity("camera1");
		cam->Get()->Rotate(0.001f, 0.0f);

		mScene->Update();

		mRenderer->SetViewport();
		mRenderer->Clear(Engine::float4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0);

		mScene->Render((Engine::CameraEntity*)mScene->GetEntity("camera1"));

		mWindow->Process();
		mWindow->Paint();
		mRenderer->Swap();*/

		int w;
		int h;
		this->mWindow->GetSize(&w, &h);

		mWindow->Process();

		std::vector<Engine::RenderNode> nodes;
		Engine::mat4 tmp[255];
		mScene->Render(mScene->GetEntity("Camera"), nodes, tmp);

		/*Engine::PerspectiveCamera cam = Engine::PerspectiveCamera(Engine::float4(100.0f * sinf(angle), 100.0f, 100.0f * cosf(angle), 1.0f),
			Engine::float4(0.0f, 100.0f, 0.0f, 1.0f),
			Engine::float4(0.0f, 1.0f, 0.0f, 0.0f),
			45.0f,
			(float)h / (float)w,
			1.0f,
			2000.0f);
		Engine::mat4 view = cam.GetViewMatrix();
		Engine::mat4 proj = cam.GetProjectionMatrix();*/

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

		Engine::mat4 view = cam->GetViewMatrix();
		Engine::mat4 proj = cam->GetProjectionMatrix();

		memcpy(mCbuffer, &view, sizeof(float) * 16);
		memcpy((float*)mCbuffer + 16, &proj, sizeof(float) * 16);

/*		Engine::mat4 view = Engine::mat4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);
		Engine::mat4 proj = Engine::mat4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);*/

		/*float cbvData[64] = {};
		memcpy(&cbvData[0], &view, sizeof(float) * 16);
		memcpy(&cbvData[16], &proj, sizeof(float) * 16);
		for (int i = 32; i < 64; i++)
		{
			cbvData[i] = (float)(rand() % 1000) * 0.001f;
		}

		mRenderer->GetGraphicsContext()->InitializeBuffer(mConstantBuffer, cbvData, sizeof(float) * 64);
		mRenderer->Flush();*/
		
		Engine::GraphicsContext* context = mRenderer->GetGraphicsContext();

		context->Begin(mPipelineState);
		context->SetRootSignature(mRootSignature);
		context->SetDescriptorHeap(Engine::DescriptorHeap::CBV_SRV_UAV, mRenderer->Heap());
		context->SetViewport(0.0f, 0.0f, (float)w * 2, (float)h * 2);
		context->SetScissorRect(0.0f, 0.0f, (float)w * 2, (float)h * 2);
		context->TransitionResource(mRTColor, D3D12_RESOURCE_STATE_RENDER_TARGET, true);
		context->SetRenderTargets(mRTColor, mRTDepth);
		context->ClearColor(mRTColor, 0.0f, 0.0f, 0.0f, 1.0f);
		context->ClearDepth(mRTDepth, 1.0f, 0);
		context->SetPrimitiveTopology(Engine::Graphics::TRIANGLELIST);
		//context->SetConstantMatrix(0, (float*)&view);
		//context->SetConstantMatrix(1, (float*)&proj);
		context->SetConstantBuffer(0, mConstantBuffer->GetGpuVirtualAddress());

		for (const Engine::RenderNode& node : nodes)
		{
			bool hasTexture = false;

			if (node.mObject->Has<Engine::TextureComponent>())
			{
				if (node.mObject->Get<Engine::TextureComponent>()->GetTexture() != nullptr)
				{
					context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(1, node.mObject->Get<Engine::TextureComponent>()->GetTexture()->GetSRV().mGpuHandle);
					hasTexture = true;
				}
			}

			if (node.mObject->Has<Engine::MeshComponent>() && hasTexture)
			{
				node.mObject->Get<Engine::MeshComponent>()->GetMesh()->Render(context);
			}
		}

		context->TransitionResource(mRTColor, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, true);

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
		context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(0, mRTColor->GetSRV().mGpuHandle);
		mFullscreenQuad->Render(context);

		context->TransitionResource(mSwapChain->GetBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, true);
		context->Finish();

		mSwapChain->SwapBuffers();

		angle += 0.002f;
	}

	void Handle(const Engine::Keyboard::KeyPressed& kp)
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
	}

	void Handle(const Engine::Keyboard::KeyReleased& kp)
	{
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
	}

	void Handle(const Engine::Window::Resize& r)
	{
		mSwapChain->Resize(r.mWidth, r.mHeight);

		mRTColor->Release();
		mRTColor->Create(r.mWidth * 2, r.mHeight * 2, 1, Engine::Graphics::Format::RGBA8, false, 8, 0);

		mRTDepth->Release();
		mRTDepth->Create(r.mWidth * 2, r.mHeight * 2, Engine::Graphics::Format::D32F, false, 8, 0);
	}
};

int main()
{
	Engine::Log* log = new Engine::Log();
	log->AddOutput(new std::ofstream("Output.log"), Engine::Log::LOG_DEFAULT);

	Engine::Constants* options = new Engine::Constants(log, "../Data/Example05/Config.conf");

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

	std::cin.get();

	return 0;
}