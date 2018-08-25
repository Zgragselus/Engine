#ifndef __LIGHTING_SYSTEM__H__
#define __LIGHTING_SYSTEM__H__

#include "Graphics/Graphics.h" 
#include "Graphics/Common/Texture/TextureAtlas.h"
#include "Game/Game.h"
#include "Game/Scene/BVH.h"
#include "Core/Math/Numeric/Float4.h"

namespace SkyeCuillin
{
	class LightingSystem
	{
	private:
		/// <summary>Cube map directions</summary>
		Engine::float4 mCubeDirections[6];

		/// <summary>Cube map up vectors</summary>
		Engine::float4 mCubeUps[6];

		/// <summary>Depth map for render target for whole shadow atlas</summary>
		Engine::DepthBuffer* mShadowDepthMap;

		/// <summary>Shadow map data for render target for whole shadow atlas</summary>
		Engine::ColorBuffer* mShadowColorMap;

		/// <summary>Resolution of shadow map atlas</summary>
		int mShadowAtlasResolution;

		/// <summary>Texture atlas</summary>
		Engine::TextureAtlas* mShadowAtlas;

		struct __declspec(align(16)) ShadowAtlasRecord
		{
			Engine::mat4 mMatrix;
			float mOffset[2];
			float mSize[2];
		};

		/// <summary>CPU pointer for shadow atlas data</summary>
		ShadowAtlasRecord* mShadowAtlasData;

		/// <summary>Structured buffer for shadow atlas</summary>
		Engine::StructuredBuffer* mShadowAtlasBuffer;

		/// <summary>Light data structure used on GPU</summary>
		struct __declspec(align(16)) LightData
		{
			Engine::float4 position;
			Engine::float4 positionVS;
			Engine::float4 color;

			union
			{
				// Spot light
				struct
				{
					Engine::float4 spotDirection;
					Engine::float4 spotDirectionVS;

					float spotAngle;
					float spotFalloff;
					float spotLinearAtt;
					float spotQuadraticAtt;
				};

				// Point light
				struct
				{
					float pointLinearAtt;
					float pointQuadraticAtt;

					int cubeShadowID[6];
				};

				// Directional light
				struct
				{
					Engine::float4 dirDirection;
					Engine::float4 dirDirectionVS;
				};
			};

			Engine::Light::Type type;
			int shadowID;
			float shadowNear;
			float shadowFar;

			float shadowResolution;
			float shadowScale;
			int shadowFilter;
			float filterSize;

			float lightSize;
			float pad[3];

			LightData()
			{

			}

			~LightData()
			{

			}

			ALIGNED_NEW_DELETE("SkyeCuillin::LightingSystem::LightData")
		};

		/// <summary>CPU-side light data buffer</summary>
		LightData* mLightBufferData;

		/// <summary>GPU-side light data buffer</summary>
		Engine::StructuredBuffer* mLightBuffer;

		/// <summary>Max. light count in scene</summary>
		int mMaxLights;

		/// <summary>No. of lights in scene</summary>
		int mLights;

		Engine::RootSignature* mDepthRS;
		Engine::PipelineState* mDepthPS;
		Engine::PipelineState* mDepthCubePS;
		Engine::D3DShader* mDepth;
		Engine::D3DShader* mDepthCube;

		Engine::RootSignature* mMipmapRS;
		Engine::PipelineState* mMipmapPS;
		Engine::D3DShader* mMipmap;
		unsigned int mMiplevels;

		Engine::GpuMappedBuffer* mConstBuffer;
		void* mConstBufferPtr;
		
		/// <summary>Initialize point light</summary>
		/// <param name="node">Scenegraph node holding the light object</param>
		/// <param name="light">Pointer to light object to initialize</param>
		void InitializePointLight(Engine::Camera* camera, Engine::RenderNode* node, Engine::PointLight* light)
		{
			assert(mLights < mMaxLights - 1);

			mLightBufferData[mLights].position = Engine::float4(0.0f, 0.0f, 0.0f, 1.0f) * node->mWorld;
			mLightBufferData[mLights].positionVS = camera->GetViewMatrix() * (Engine::float4(0.0f, 0.0f, 0.0f, 1.0f) * node->mWorld);
			mLightBufferData[mLights].color = light->GetColor() * TemperatureToColor(light->GetTemperature()) * light->GetPower() / (Engine::Math::PI * 4.0f);

			mLightBufferData[mLights].pointLinearAtt = light->GetLinearAttenuation();
			mLightBufferData[mLights].pointQuadraticAtt = light->GetQuadraticAttenuation();

			mLightBufferData[mLights].type = Engine::Light::Type::POINT;
			mLightBufferData[mLights].shadowID = -1;
			mLightBufferData[mLights].shadowNear = light->GetNearPlane();
			mLightBufferData[mLights].shadowFar = light->GetFarPlane();
			mLightBufferData[mLights].shadowFilter = light->GetShadowsFilter();
			mLightBufferData[mLights].filterSize = light->GetFilterSize();
			mLightBufferData[mLights].shadowResolution = (float)mShadowAtlas->GetWidth();
			mLightBufferData[mLights].shadowScale = 1.0f / (float)mShadowAtlas->GetWidth();

			mLightBufferData[mLights].lightSize = light->GetLightSize();

			if (light->GetShadowsEnabled() == 1)
			{
				mLightBufferData[mLights].cubeShadowID[0] = mShadowAtlas->RequestTexture(light->GetResolution(), light->GetResolution());
				mLightBufferData[mLights].cubeShadowID[1] = mShadowAtlas->RequestTexture(light->GetResolution(), light->GetResolution());
				mLightBufferData[mLights].cubeShadowID[2] = mShadowAtlas->RequestTexture(light->GetResolution(), light->GetResolution());
				mLightBufferData[mLights].cubeShadowID[3] = mShadowAtlas->RequestTexture(light->GetResolution(), light->GetResolution());
				mLightBufferData[mLights].cubeShadowID[4] = mShadowAtlas->RequestTexture(light->GetResolution(), light->GetResolution());
				mLightBufferData[mLights].cubeShadowID[5] = mShadowAtlas->RequestTexture(light->GetResolution(), light->GetResolution());

				for (int i = 0; i < 6; i++)
				{
					if (mLightBufferData[mLights].cubeShadowID[i] >= 0)
					{
						unsigned int position[2];
						unsigned int size[2];

						mShadowAtlas->GetTile(mLightBufferData[mLights].cubeShadowID[i], position[0], position[1], size[0], size[1]);

						mShadowAtlasData[mLightBufferData[mLights].cubeShadowID[i]].mMatrix =
							Engine::mat4(1.0f, 0.0f, 0.0f, 0.0f,
								0.0f, 1.0f, 0.0f, 0.0f,
								0.0f, 0.0f, 1.0f, 0.0f,
								-mLightBufferData[mLights].position.x, -mLightBufferData[mLights].position.y, -mLightBufferData[mLights].position.z, 1.0f) *
							Engine::lookat(Engine::float4(0.0f, 0.0f, 0.0f, 1.0f), Engine::float4(0.0f, 0.0f, 0.0f, 1.0f) + mCubeDirections[i], mCubeUps[i]) *
							Engine::perspective(Engine::Math::PI * 0.5f, 1.0f, mLightBufferData[mLights].shadowNear, mLightBufferData[mLights].shadowFar);
						mShadowAtlasData[mLightBufferData[mLights].cubeShadowID[i]].mOffset[0] = (float)position[0] / (float)mShadowAtlasResolution;
						mShadowAtlasData[mLightBufferData[mLights].cubeShadowID[i]].mOffset[1] = (float)position[1] / (float)mShadowAtlasResolution;
						mShadowAtlasData[mLightBufferData[mLights].cubeShadowID[i]].mSize[0] = (float)size[0] / (float)mShadowAtlasResolution;
						mShadowAtlasData[mLightBufferData[mLights].cubeShadowID[i]].mSize[1] = (float)size[1] / (float)mShadowAtlasResolution;
					}
				}
			}
			else
			{
				mLightBufferData[mLights].cubeShadowID[0] = -1;
				mLightBufferData[mLights].cubeShadowID[1] = -1;
				mLightBufferData[mLights].cubeShadowID[2] = -1;
				mLightBufferData[mLights].cubeShadowID[3] = -1;
				mLightBufferData[mLights].cubeShadowID[4] = -1;
				mLightBufferData[mLights].cubeShadowID[5] = -1;
			}

			mLights++;
		}

		/// <summary>Initialize spot light</summary>
		/// <param name="node">Scenegraph node holding the light object</param>
		/// <param name="light">Pointer to light object to initialize</param>
		void InitializeSpotLight(Engine::Camera* camera, Engine::RenderNode* node, Engine::SpotLight* light)
		{
			assert(mLights < mMaxLights - 1);

			mLightBufferData[mLights].position = Engine::float4(0.0f, 0.0f, 0.0f, 1.0f) * node->mWorld;
			mLightBufferData[mLights].positionVS = camera->GetViewMatrix() * (Engine::float4(0.0f, 0.0f, 0.0f, 1.0f) * node->mWorld);
			mLightBufferData[mLights].color = light->GetColor() * TemperatureToColor(light->GetTemperature()) * light->GetPower() / (Engine::Math::PI * 2.0f * (1.0f - cosf(light->GetSpotAngleRad() * 0.5f)));
			
			mLightBufferData[mLights].spotDirection = light->GetDirection();
			mLightBufferData[mLights].spotDirectionVS = camera->GetViewMatrix() * (((Engine::SpotLight*)light)->GetDirection());
			mLightBufferData[mLights].spotAngle = light->GetSpotAngleRad();
			mLightBufferData[mLights].spotFalloff = cosf(light->GetSpotAngleRad());
			mLightBufferData[mLights].spotLinearAtt = light->GetLinearAttenuation();
			mLightBufferData[mLights].spotQuadraticAtt = light->GetQuadraticAttenuation();

			mLightBufferData[mLights].type = Engine::Light::Type::SPOT;
			mLightBufferData[mLights].shadowNear = light->GetNearPlane();
			mLightBufferData[mLights].shadowFar = light->GetFarPlane();
			mLightBufferData[mLights].shadowFilter = light->GetShadowsFilter();
			mLightBufferData[mLights].filterSize = light->GetFilterSize();
			mLightBufferData[mLights].shadowResolution = (float)mShadowAtlas->GetWidth();
			mLightBufferData[mLights].shadowScale = 1.0f / (float)mShadowAtlas->GetWidth();

			mLightBufferData[mLights].lightSize = light->GetLightSize();

			if (light->GetShadowsEnabled() == 1)
			{
				mLightBufferData[mLights].shadowID = mShadowAtlas->RequestTexture(light->GetResolution(), light->GetResolution());

				if (mLightBufferData[mLights].shadowID >= 0)
				{
					unsigned int position[2];
					unsigned int size[2];

					mShadowAtlas->GetTile(mLightBufferData[mLights].shadowID, position[0], position[1], size[0], size[1]);

					mShadowAtlasData[mLightBufferData[mLights].shadowID].mMatrix =
						Engine::lookat(mLightBufferData[mLights].position, mLightBufferData[mLights].position + mLightBufferData[mLights].spotDirection, Engine::float4(0.0f, 1.0f, 0.0f, 0.0f)) *
						Engine::perspective(Engine::Math::PI - mLightBufferData[mLights].spotAngle * 2.0f, 1.0f, mLightBufferData[mLights].shadowNear, mLightBufferData[mLights].shadowFar);
					mShadowAtlasData[mLightBufferData[mLights].shadowID].mOffset[0] = (float)position[0] / (float)mShadowAtlasResolution;
					mShadowAtlasData[mLightBufferData[mLights].shadowID].mOffset[1] = (float)position[1] / (float)mShadowAtlasResolution;
					mShadowAtlasData[mLightBufferData[mLights].shadowID].mSize[0] = (float)size[0] / (float)mShadowAtlasResolution;
					mShadowAtlasData[mLightBufferData[mLights].shadowID].mSize[1] = (float)size[1] / (float)mShadowAtlasResolution;
				}
			}
			else
			{
				mLightBufferData[mLights].shadowID = -1;
			}
			
			mLights++;
		}

		/// <summary>Initialize directional light</summary>
		/// <param name="node">Scenegraph node holding the light object</param>
		/// <param name="light">Pointer to light object to initialize</param>
		void InitializeDirectionalLight(Engine::Camera* camera, Engine::RenderNode* node, Engine::DirectionalLight* light)
		{
			assert(mLights < mMaxLights - 1);

			mLightBufferData[mLights].position = Engine::float4(0.0f, 0.0f, 0.0f, 1.0f) * node->mWorld;
			mLightBufferData[mLights].color = light->GetColor();
			mLightBufferData[mLights].dirDirection = light->GetDirection();
			mLightBufferData[mLights].type = Engine::Light::Type::DIRECTIONAL;

			mLights++;
		}

		/// <summary>Convert temperature of black body into RGB color</summary>
		/// <param name="temperature">Temperature of black body (in Kelvin)</param>
		Engine::float4 TemperatureToColor(float temperature)
		{
			float tmp = temperature / 100.0f;
			Engine::float4 result;

			if (tmp <= 66.0f)
			{
				result.x = 255.0f;

				result.y = tmp;
				result.y = 99.4708025861f * log(result.y) - 161.1195681661f;

				if (tmp <= 19.0f) 
				{
					result.z = 0.0f;
				}
				else 
				{
					result.z = tmp - 10.0f;
					result.z = 138.5177312231f * log(result.z) - 305.0447927307f;
				}
			}
			else 
			{
				result.x = tmp - 60.0f;
				result.x = 329.698727446f * pow(result.x, -0.1332047592f);

				result.y = tmp - 60.0f;
				result.y = 288.1221695283f * pow(result.y, -0.0755148492f);

				result.z = 255.0f;
			}

			return result / 255.0f;
		}

		void GenerateMipmaps(Engine::DescriptorHeap* heap, Engine::ComputeContext* context)
		{
			context->SetPipelineState(mMipmapPS);
			context->SetRootSignature(mMipmapRS);
			context->SetDescriptorHeap(Engine::DescriptorHeap::CBV_SRV_UAV, heap);
			int todo = mMiplevels - 1;
			int base = 0;
			int dimension = mShadowAtlasResolution;

			while (todo != 0)
			{
				int mipLevels = 1;

				context->SetConstants(0, Engine::DWParam(base), Engine::DWParam(mipLevels), Engine::DWParam(1.0f / (float)dimension), Engine::DWParam(1.0f / (float)dimension));
				context->SetDescriptorTable(1, mShadowColorMap->GetSRV());
				context->SetDescriptorTable(2, mShadowColorMap->GetUAV(base + 1));
				context->SetDescriptorTable(3, mShadowColorMap->GetUAV(base + 1));
				context->SetDescriptorTable(4, mShadowColorMap->GetUAV(base + 1));
				context->SetDescriptorTable(5, mShadowColorMap->GetUAV(base + 1));
				context->Dispatch(dimension / 2, dimension / 2, 1);
				context->TransitionResource(mShadowColorMap, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, true);

				todo -= 1;
				base += 1;
				dimension /= 2;
			}
		}

	public:
		/// <summary>Lighting system constructor</summary>
		/// <param name="renderer">Renderer</param>
		/// <param name="maxLights">Maximal number of lights in the scene</param>
		/// <param name="resolution">Resolution of shadow texture atlas</param>
		LightingSystem(Engine::D3DRenderer* renderer, int maxLights, int resolution)
		{
			mShadowAtlasResolution = resolution;

			mShadowColorMap = new Engine::ColorBuffer(renderer, 1.0f, 1.0f, 0.0f, 1.0f);
			mShadowColorMap->Create(mShadowAtlasResolution, mShadowAtlasResolution, 0, Engine::Graphics::Format::RG32F);
			mMiplevels = mShadowColorMap->GetNumMipMaps();

			mShadowDepthMap = new Engine::DepthBuffer(renderer, 1.0f, 0);
			mShadowDepthMap->Create(mShadowAtlasResolution, mShadowAtlasResolution, Engine::Graphics::Format::D32F);

			mShadowAtlas = new Engine::TextureAtlas(mShadowAtlasResolution, mShadowAtlasResolution);

			mMaxLights = maxLights;

			mShadowAtlasData = new ShadowAtlasRecord[mMaxLights * 6];

			mShadowAtlasBuffer = new Engine::StructuredBuffer();
			mShadowAtlasBuffer->Init(renderer, mMaxLights, sizeof(float) * 20 * 6, nullptr);

			mLightBufferData = new LightData[mMaxLights];

			mLightBuffer = new Engine::StructuredBuffer();
			mLightBuffer->Init(renderer, mMaxLights, sizeof(LightData), nullptr);

			mDepthRS = new Engine::RootSignature(renderer->GetDevice(), 6, 1);
			(*mDepthRS)[0].InitAsConstantBuffer(0);
			(*mDepthRS)[1].InitAsDescriptorTable(1);
			(*mDepthRS)[1].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);
			(*mDepthRS)[2].InitAsConstantBuffer(1);
			(*mDepthRS)[3].InitAsConstants(2, 3);
			(*mDepthRS)[4].InitAsDescriptorTable(1);
			(*mDepthRS)[4].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
			(*mDepthRS)[5].InitAsDescriptorTable(1);
			(*mDepthRS)[5].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 1);
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

			mDepth = new Engine::D3DShader("../Data/SkyeCuillin/VirtualShadowMap.hlsl",
				std::vector<Engine::D3DShader::ShaderEntryPoint>
			{
				Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::VERTEX_SHADER, "VS"),
				Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::PIXEL_SHADER, "PS")
			});

			mDepthCube = new Engine::D3DShader("../Data/SkyeCuillin/VirtualShadowMap.hlsl",
				std::vector<Engine::D3DShader::ShaderEntryPoint>
			{
				Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::VERTEX_SHADER, "VSCube"),
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
			rtvFormats.push_back(Engine::Graphics::Format::RG32F);

			mDepthPS = new Engine::PipelineState(renderer->GetDevice(),
				mDepthRS,
				mDepth,
				Engine::BlendState::CreateStateD3D(true),
				Engine::RasterizerState::CreateStateD3D(Engine::RasterizerState::SOLID, Engine::RasterizerState::FRONT),
				//Engine::RasterizerState::CreateStateD3D(),
				&dsState,
				&inputLayout,
				Engine::PipelineState::PrimitiveType::TRIANGLE,
				rtvFormats,
				Engine::Graphics::Format::D32F,
				1,
				0);

			mDepthCubePS = new Engine::PipelineState(renderer->GetDevice(),
				mDepthRS,
				mDepthCube,
				Engine::BlendState::CreateStateD3D(true),
				Engine::RasterizerState::CreateStateD3D(Engine::RasterizerState::SOLID, Engine::RasterizerState::FRONT),
				//Engine::RasterizerState::CreateStateD3D(),
				&dsState,
				&inputLayout,
				Engine::PipelineState::PrimitiveType::TRIANGLE,
				rtvFormats,
				Engine::Graphics::Format::D32F,
				1,
				0);

			mMipmapRS = new Engine::RootSignature(renderer->GetDevice(), 6, 1);
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
				Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::COMPUTE_SHADER, "GenerateMipmapsMinMax")
			});

			mMipmapPS = new Engine::PipelineState(renderer->GetDevice(),
				mMipmapRS,
				mMipmap);

			// Initialize constant buffer (holds default matrices for shadow cube-map)
			mConstBuffer = new Engine::GpuMappedBuffer();
			mConstBuffer->Init(renderer, 128, sizeof(float));
			mConstBufferPtr = mConstBuffer->Map();

			// Initialize default shadow cube-map matrices, direction & up vectors
			Engine::mat4 proj = Engine::perspective(Engine::Math::PI * 0.5f, 1.0f, 20.0f, 1500.0f);
			Engine::mat4 view[6];

			mCubeDirections[0] = Engine::float4(-1.0f, 0.0f, 0.0f, 0.0f);
			mCubeDirections[1] = Engine::float4(1.0f, 0.0f, 0.0f, 0.0f);
			mCubeDirections[2] = Engine::float4(0.0f, 1.0f, 0.0f, 0.0f);
			mCubeDirections[3] = Engine::float4(0.0f, -1.0f, 0.0f, 0.0f);
			mCubeDirections[4] = Engine::float4(0.0f, 0.0f, 1.0f, 0.0f);
			mCubeDirections[5] = Engine::float4(0.0f, 0.0f, -1.0f, 0.0f);

			mCubeUps[0] = Engine::float4(0.0f, 1.0f, 0.0f, 0.0f);
			mCubeUps[1] = Engine::float4(0.0f, 1.0f, 0.0f, 0.0f);
			mCubeUps[2] = Engine::float4(0.0f, 0.0f, -1.0f, 0.0f);
			mCubeUps[3] = Engine::float4(0.0f, 0.0f, 1.0f, 0.0f);
			mCubeUps[4] = Engine::float4(0.0f, 1.0f, 0.0f, 0.0f);
			mCubeUps[5] = Engine::float4(0.0f, 1.0f, 0.0f, 0.0f);

			view[0] = Engine::lookat(Engine::float4(0.0f, 0.0f, 0.0f, 1.0f), Engine::float4(0.0f, 0.0f, 0.0f, 1.0f) + mCubeDirections[0], mCubeUps[0]);
			view[1] = Engine::lookat(Engine::float4(0.0f, 0.0f, 0.0f, 1.0f), Engine::float4(0.0f, 0.0f, 0.0f, 1.0f) + mCubeDirections[1], mCubeUps[1]);
			view[2] = Engine::lookat(Engine::float4(0.0f, 0.0f, 0.0f, 1.0f), Engine::float4(0.0f, 0.0f, 0.0f, 1.0f) + mCubeDirections[2], mCubeUps[2]);
			view[3] = Engine::lookat(Engine::float4(0.0f, 0.0f, 0.0f, 1.0f), Engine::float4(0.0f, 0.0f, 0.0f, 1.0f) + mCubeDirections[3], mCubeUps[3]);
			view[4] = Engine::lookat(Engine::float4(0.0f, 0.0f, 0.0f, 1.0f), Engine::float4(0.0f, 0.0f, 0.0f, 1.0f) + mCubeDirections[4], mCubeUps[4]);
			view[5] = Engine::lookat(Engine::float4(0.0f, 0.0f, 0.0f, 1.0f), Engine::float4(0.0f, 0.0f, 0.0f, 1.0f) + mCubeDirections[5], mCubeUps[5]);

			memcpy(mConstBufferPtr, &proj, sizeof(float) * 16);
			memcpy((float*)mConstBufferPtr + 16, &view[0], sizeof(float) * 16);
			memcpy((float*)mConstBufferPtr + 32, &view[1], sizeof(float) * 16);
			memcpy((float*)mConstBufferPtr + 48, &view[2], sizeof(float) * 16);
			memcpy((float*)mConstBufferPtr + 64, &view[3], sizeof(float) * 16);
			memcpy((float*)mConstBufferPtr + 80, &view[4], sizeof(float) * 16);
			memcpy((float*)mConstBufferPtr + 96, &view[5], sizeof(float) * 16);
		}

		/// <summary>Lighting system destructor</summary>
		~LightingSystem()
		{
			delete mShadowColorMap;
			delete mShadowDepthMap;
			delete mShadowAtlas;
			delete[] mShadowAtlasData;
			delete mShadowAtlasBuffer;
			delete[] mLightBufferData;
			delete mLightBuffer;

			delete mDepthRS;
			delete mDepthPS;
			delete mDepth;

			delete mMipmapRS;
			delete mMipmapPS;
			delete mMipmap;

			mConstBuffer->Unmap();
			delete mConstBuffer;
		}

		/// <summary>Process the lighting system</summary>
		/// <param name="context">Rendering context</param>
		void Process(Engine::Entity* camera, std::vector<Engine::RenderNode>& nodes, Engine::GraphicsContext* context, Engine::DescriptorHeap* heap, Engine::GpuMappedBuffer* matrices, Engine::BVH* bvh)
		{
			// Initialize light buffer
			mLights = 0;

			Engine::Camera* cam = camera->GameObject().Get<Engine::CameraComponent>()->Get();

			for (Engine::RenderNode& node : nodes)
			{
				if (node.mObject->Has<Engine::LightComponent>())
				{
					Engine::Light* light = node.mObject->Get<Engine::LightComponent>()->GetLight();

					switch (light->GetType())
					{
					case Engine::Light::Type::POINT:
						InitializePointLight(cam, &node, (Engine::PointLight*)light);
						break;

					case Engine::Light::Type::DIRECTIONAL:
						InitializeDirectionalLight(cam, &node, (Engine::DirectionalLight*)light);
						break;

					case Engine::Light::Type::SPOT:
						InitializeSpotLight(cam, &node, (Engine::SpotLight*)light);
						break;
					}
				}
			}

			// Copy lights buffer into StructuredBuffer
			context->TransitionResource(mLightBuffer, D3D12_RESOURCE_STATE_COPY_DEST, true);
			context->WriteBuffer(mLightBuffer, 0, mLightBufferData, sizeof(LightData) * mMaxLights);
			context->TransitionResource(mLightBuffer, D3D12_RESOURCE_STATE_GENERIC_READ, true);

			// Copy texture atlas buffer into StructuredBuffer
			context->TransitionResource(mShadowAtlasBuffer, D3D12_RESOURCE_STATE_COPY_DEST, true);
			context->WriteBuffer(mShadowAtlasBuffer, 0, mShadowAtlasData, sizeof(float) * mMaxLights * 20 * 6);
			context->TransitionResource(mShadowAtlasBuffer, D3D12_RESOURCE_STATE_GENERIC_READ, true);

			// Refresh virtual shadow maps
			context->SetPipelineState(mDepthPS);
			context->SetRootSignature(mDepthRS);
			context->SetDescriptorHeap(Engine::DescriptorHeap::CBV_SRV_UAV, heap);
			context->TransitionResource(mShadowColorMap, D3D12_RESOURCE_STATE_RENDER_TARGET, true);
			context->SetRenderTargets(mShadowColorMap, mShadowDepthMap);
			context->ClearColor(mShadowColorMap, 1.0f, 1.0f, 0.0f, 1.0f);
			context->ClearDepth(mShadowDepthMap, 1.0f, 0);
			context->SetPrimitiveTopology(Engine::Graphics::TRIANGLELIST);
			context->SetConstantBuffer(0, mConstBuffer->GetGpuVirtualAddress());
			context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(4, mLightBuffer->GetSRV().mGpuHandle);
			context->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(5, mShadowAtlasBuffer->GetSRV().mGpuHandle);

			for (int i = 0; i < mLights; i++)
			{
				if (mLightBufferData[i].shadowID >= 0)
				{
					if (mLightBufferData[i].type == Engine::Light::Type::SPOT)
					{
						unsigned int tilePosition[2];
						unsigned int tileSize[2];

						mShadowAtlas->GetTile(mLightBufferData[i].shadowID, tilePosition[0], tilePosition[1], tileSize[0], tileSize[1]);

						context->SetViewport((float)tilePosition[0], (float)tilePosition[1], (float)tileSize[0], (float)tileSize[1]);
						context->SetScissorRect((float)tilePosition[0], (float)tilePosition[1], (float)tileSize[0], (float)tileSize[1]);

						std::vector<Engine::RenderNode> renderNodes = bvh->Render(mLightBufferData[i].position, mLightBufferData[i].spotDirection, Engine::float4(0.0f, 1.0f, 0.0f, 0.0f),
							mLightBufferData[i].spotAngle * 2.0f, 1.0f, mLightBufferData[i].shadowNear, mLightBufferData[i].shadowFar);

						int batchBound = -1;
						for (const Engine::RenderNode& node : renderNodes)
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
								context->SetConstants(3, Engine::DWParam(node.mID % 16), Engine::DWParam(i), Engine::DWParam(0));
								node.mObject->Get<Engine::MeshComponent>()->GetMesh()->Render(context);
							}
						}
					}
				}
			}

			context->SetPipelineState(mDepthCubePS);

			for (int i = 0; i < mLights; i++)
			{
				if (mLightBufferData[i].type == Engine::Light::Type::POINT)
				{
					for (unsigned int j = 0; j < 6; j++)
					{
						if (mLightBufferData[i].cubeShadowID[j] >= 0)
						{
							unsigned int tilePosition[2];
							unsigned int tileSize[2];

							mShadowAtlas->GetTile(mLightBufferData[i].cubeShadowID[j], tilePosition[0], tilePosition[1], tileSize[0], tileSize[1]);

							context->SetViewport((float)tilePosition[0], (float)tilePosition[1], (float)tileSize[0], (float)tileSize[1]);
							context->SetScissorRect((float)tilePosition[0], (float)tilePosition[1], (float)tileSize[0], (float)tileSize[1]);

							std::vector<Engine::RenderNode> renderNodes = bvh->Render(mLightBufferData[i].position, mCubeDirections[j], mCubeUps[j],
								Engine::Math::PI * 0.5f, 1.0f, mLightBufferData[i].shadowNear, mLightBufferData[i].shadowFar);

							int batchBound = -1;
							for (const Engine::RenderNode& node : renderNodes)
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
									context->SetConstants(3, Engine::DWParam(node.mID % 16), Engine::DWParam(i), Engine::DWParam(j));
									node.mObject->Get<Engine::MeshComponent>()->GetMesh()->Render(context);
								}
							}
						}
					}
				}
			}

			context->TransitionResource(mShadowColorMap, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, true);

			GenerateMipmaps(heap, reinterpret_cast<Engine::ComputeContext*>(context));

			context->TransitionResource(mShadowColorMap, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, true);

			mShadowAtlas->Clear(mShadowAtlasResolution, mShadowAtlasResolution);
		}

		/// <summary>Get light buffer as StructuredBuffer</summary>
		/// <return>StructuredBuffer with all lights in scene</return>
		inline Engine::StructuredBuffer* GetLightBuffer()
		{
			return mLightBuffer;
		}

		/// <summary>Get shadow atlas as StructuredBuffer</summary>
		/// <return>StructuredBuffer with shadow atlas data</return>
		inline Engine::StructuredBuffer* GetShadowAtlasBuffer()
		{
			return mShadowAtlasBuffer;
		}

		/// <summary>Get shadow map</summary>
		/// <return>Color buffer holding shadow map atlas</return>
		inline Engine::ColorBuffer* GetShadowMap()
		{
			return mShadowColorMap;
		}

		/// <summary>Get number of lights</summary>
		inline int GetLights()
		{
			return mLights;
		}

		ALIGNED_NEW_DELETE("SkyeCuillin::LightingSystem")
	};
}

#endif