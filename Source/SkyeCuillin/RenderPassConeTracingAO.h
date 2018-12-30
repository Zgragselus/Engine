#ifndef __RENDER_PASS_CONE_TRACING_AO__H__
#define __RENDER_PASS_CONE_TRACING_AO__H__

#include "RenderPassFullscreen.h"
#include "Voxelize.h"

namespace SkyeCuillin
{
	class RenderPassConeTracingAO : public RenderPassFullscreen
	{
	protected:
		Engine::RootSignature* mOcclusionRS;
		Engine::PipelineState* mOcclusionPS;
		Engine::D3DShader* mOcclusion;

		Engine::ColorBuffer* mColorBuffer;
		Engine::DepthBuffer* mDepthBuffer;

		Engine::GpuMappedBuffer* mConstBuffer;
		void* mConstBufferPtr;

		Engine::DepthBuffer* mSourceDepth;
		Engine::ColorBuffer* mSourceNormals;
		Voxelize* mVoxelData;

		unsigned int mSamples;

	public:
		RenderPassConeTracingAO(Engine::D3DRenderer* renderer, unsigned int width, unsigned int height, unsigned int samples);
		virtual ~RenderPassConeTracingAO();
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