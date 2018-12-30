#ifndef __RENDER_PASS_CUBE_SHADOW_MAP__H__
#define __RENDER_PASS_CUBE_SHADOW_MAP__H__

#include "RenderPass.h"

namespace SkyeCuillin
{
	class RenderPassCubeShadowMap : public RenderPass
	{
	protected:
		Engine::RootSignature* mDepthRS;
		Engine::PipelineState* mDepthPS;
		Engine::D3DShader* mDepth;

		Engine::ColorBuffer* mShadowMap;
		Engine::DepthBuffer* mDepthMap;

		Engine::GpuMappedBuffer* mConstBuffer;
		void* mConstBufferPtr;

		Engine::GameObject* mShadowCaster;

		Engine::mat4* mView;
		Engine::mat4* mProjection;

	public:
		RenderPassCubeShadowMap(Engine::D3DRenderer* renderer, unsigned int width, unsigned int height);
		virtual ~RenderPassCubeShadowMap();
		void Process(Engine::Entity* light, Engine::DescriptorHeap* heap, Engine::GraphicsContext* context, std::vector<Engine::RenderNode>& nodes, Engine::GpuMappedBuffer* matrices);
		virtual void Resize(unsigned int width, unsigned int height);

		inline Engine::ColorBuffer* GetBuffer()
		{
			return mShadowMap;
		}

		inline Engine::mat4* GetView()
		{
			return mView;
		}

		inline Engine::mat4* GetProjection()
		{
			return mProjection;
		}
	};
}

#endif