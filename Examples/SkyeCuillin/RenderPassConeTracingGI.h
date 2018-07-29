#ifndef __RENDER_PASS_CONE_TRACING_GI__H__
#define __RENDER_PASS_CONE_TRACING_GI__H__

#include "RenderPassFullscreen.h"
#include "Voxelize.h"

namespace SkyeCuillin
{
	class RenderPassConeTracingGI : public RenderPassFullscreen
	{
	protected:
		Engine::RootSignature* mGlobalIllumRS;
		Engine::PipelineState* mGlobalIllumPS;
		Engine::D3DShader* mGlobalIllum;

		Engine::ColorBuffer* mColorBuffer;
		Engine::DepthBuffer* mDepthBuffer;

		Engine::GpuMappedBuffer* mConstBuffer;
		void* mConstBufferPtr;

		Engine::ColorBuffer* mSourceNormals;
		Engine::DepthBuffer* mSourceDepth;
		Voxelize* mVoxelData;

		unsigned int mSamples;

	public:
		RenderPassConeTracingGI(Engine::D3DRenderer* renderer, unsigned int width, unsigned int height, unsigned int samples);
		virtual ~RenderPassConeTracingGI();
		void Process(Engine::Entity* camera, Engine::DescriptorHeap* heap, Engine::GraphicsContext* context, std::vector<Engine::RenderNode>& nodes);
		virtual void Resize(unsigned int width, unsigned int height);

		void SetSources(Engine::ColorBuffer* normals, Engine::DepthBuffer* depth, Voxelize* svo)
		{
			mSourceNormals = normals;
			mSourceDepth = depth;
			mVoxelData = svo;
		}

		inline Engine::ColorBuffer* GetBuffer()
		{
			return mColorBuffer;
		}
	};
}

#endif