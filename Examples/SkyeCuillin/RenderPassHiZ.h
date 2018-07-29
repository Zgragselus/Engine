#ifndef __RENDER_PASS_PICKING__H__
#define __RENDER_PASS_PICKING__H__

#include "RenderPass.h"

namespace SkyeCuillin
{
	class RenderPassHiZ : public RenderPass
	{
	protected:
		Engine::RootSignature* mHiZRS;
		Engine::PipelineState* mHiZPS;
		Engine::D3DShader* mHiZ;

		Engine::ColorBuffer* mColorBuffer;
		Engine::DepthBuffer* mDepthBuffer;

		Engine::GpuMappedBuffer* mCameraBuffer;
		void* mCameraBufferPtr;

	public:
		RenderPassHiZ(Engine::D3DRenderer* renderer, unsigned int width, unsigned int height);
		virtual ~RenderPassHiZ();
		void Process(Engine::Entity* camera, Engine::DescriptorHeap* heap, Engine::GraphicsContext* context, std::vector<Engine::RenderNode>& nodes, Engine::GpuMappedBuffer* matrices);
		virtual void Resize(unsigned int width, unsigned int height);

		inline Engine::ColorBuffer* GetBuffer()
		{
			return mColorBuffer;
		}

		inline Engine::DepthBuffer* GetDepthBuffer()
		{
			return mDepthBuffer;
		}
	};
}

#endif