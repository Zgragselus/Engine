#ifndef __DEFERRED_RENDERER__H__
#define __DEFERRED_RENDERER__H__

#include "RenderPass.h"

namespace SkyeCuillin
{
	class RenderPassGBuffer : public RenderPass
	{
	protected:
		Engine::RootSignature* mGBufferRS;
		Engine::PipelineState* mGBufferPS;
		Engine::D3DShader* mGBuffer;

		Engine::RootSignature* mGBufferTerrainRS;
		Engine::PipelineState* mGBufferTerrainPS;
		Engine::D3DShader* mGBufferTerrain;

		Engine::ColorBuffer** mColorBuffers;
		Engine::DepthBuffer* mDepthBuffer;
		
		Engine::GpuMappedBuffer* mCameraBuffer;
		void* mCameraBufferPtr;

		unsigned int mSamples;

	public:
		RenderPassGBuffer(Engine::D3DRenderer* renderer, unsigned int width, unsigned int height, unsigned int samples);
		virtual ~RenderPassGBuffer();
		void Process(Engine::Entity* camera, Engine::DescriptorHeap* heap, Engine::GraphicsContext* context, std::vector<Engine::RenderNode>& nodes, Engine::GpuMappedBuffer* matrices);
		virtual void Resize(unsigned int width, unsigned int height);

		inline Engine::ColorBuffer* GetBuffer(unsigned int id)
		{
			return mColorBuffers[id];
		}

		inline Engine::DepthBuffer* GetDepthBuffer()
		{
			return mDepthBuffer;
		}
	};
}

#endif