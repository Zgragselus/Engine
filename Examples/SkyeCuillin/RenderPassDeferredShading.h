#ifndef __RENDER_PASS_DEFERRED_SHADING__H__
#define __RENDER_PASS_DEFERRED_SHADING__H__

#include "RenderPassFullscreen.h"
#include "LightingSystem.h"

namespace SkyeCuillin
{
	class RenderPassDeferredShading : public RenderPassFullscreen
	{
	protected:
		Engine::RootSignature* mDeferredRS;
		Engine::PipelineState* mDeferredPS;
		Engine::D3DShader* mDeferred;

		Engine::ColorBuffer* mColorBuffer;
		Engine::DepthBuffer* mDepthBuffer;

		Engine::GpuMappedBuffer* mConstBuffer;
		void* mConstBufferPtr;

		unsigned int mSamples;

		Engine::ColorBuffer* mSourceAlbedo;
		Engine::ColorBuffer* mSourceNormals;
		Engine::ColorBuffer* mSourceReflection;
		Engine::ColorBuffer* mSourceAO;
		Engine::ColorBuffer* mSourceGI;
		Engine::DepthBuffer* mSourceDepth;

		Engine::Texture* mBRDFLookup;

		LightingSystem* mLightingSystem;

	public:
		/// <summary>Constructor for Deferred shading render pass</summary>
		/// <param name="log">Log output</param>
		/// <param name="renderer">D3D12 Renderer ptr</param>
		/// <param name="width">Render pass output and input width</param>
		/// <param name="height">Render pass output and input height</param>
		/// <param name="samples">Input and output samples per pixel</param>
		RenderPassDeferredShading(Engine::Log* log, Engine::D3DRenderer* renderer, LightingSystem* lightingSystem, unsigned int width, unsigned int height, unsigned int samples, unsigned int samplesGI);
		virtual ~RenderPassDeferredShading();
		void Process(Engine::Entity* camera, Engine::DescriptorHeap* heap, Engine::GraphicsContext* context, std::vector<Engine::RenderNode>& nodes);
		virtual void Resize(unsigned int width, unsigned int height);

		void SetSources(Engine::ColorBuffer* albedo, Engine::ColorBuffer* normals, Engine::ColorBuffer* reflection, Engine::ColorBuffer* ao, Engine::ColorBuffer* gi, Engine::DepthBuffer* depth)
		{
			mSourceAlbedo = albedo;
			mSourceNormals = normals;
			mSourceReflection = reflection;
			mSourceAO = ao;
			mSourceGI = gi;
			mSourceDepth = depth;
		}

		inline Engine::ColorBuffer* GetBuffer()
		{
			return mColorBuffer;
		}
	};
}

#endif