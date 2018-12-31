#ifndef __RENDER_PASS_CONE_TRACING_REFLECTION__H__
#define __RENDER_PASS_CONE_TRACING_REFLECTION__H__

#include "RenderPassFullscreen.h"
#include "Voxelize.h"

namespace SkyeCuillin
{
	class RenderPassConeTracingReflection : public RenderPassFullscreen
	{
	protected:
		Engine::RootSignature* mReflectionRS;
		Engine::PipelineState* mReflectionPS;
		Engine::D3DShader* mReflection;

		Engine::ColorBuffer* mColorBuffer;
		Engine::DepthBuffer* mDepthBuffer;

		Engine::GpuMappedBuffer* mConstBuffer;
		void* mConstBufferPtr;

		Engine::ColorBuffer* mSourceNormals;
		Engine::DepthBuffer* mSourceDepth;
		Voxelize* mVoxelData;

		unsigned int mSamples;

	public:
		RenderPassConeTracingReflection(Engine::D3DRenderer* renderer, unsigned int width, unsigned int height, unsigned int samples);
		virtual ~RenderPassConeTracingReflection();
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