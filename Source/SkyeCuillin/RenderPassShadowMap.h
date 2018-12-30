#ifndef __RENDER_PASS_SHADOW_MAP__H__
#define __RENDER_PASS_SHADOW_MAP__H__

#include "RenderPass.h"

namespace SkyeCuillin
{
	class RenderPassShadowMap : public RenderPass
	{
	protected:
		Engine::RootSignature* mDepthRS;
		Engine::PipelineState* mDepthPS;
		Engine::D3DShader* mDepth;

		Engine::RootSignature* mMipmapRS;
		Engine::PipelineState* mMipmapPS;
		Engine::D3DShader* mMipmap;

		Engine::ColorBuffer* mShadowMap;
		Engine::DepthBuffer* mDepthMap;

		Engine::GpuMappedBuffer* mConstBuffer;
		void* mConstBufferPtr;

		Engine::GameObject* mShadowCaster;

		Engine::mat4* mView;
		Engine::mat4* mProjection;

		float mNear;
		float mFar;
		unsigned int mMiplevels;

	public:
		RenderPassShadowMap(Engine::D3DRenderer* renderer, unsigned int width, unsigned int height, float nearClip, float farClip);
		virtual ~RenderPassShadowMap();
		void Process(Engine::Entity* light, Engine::DescriptorHeap* heap, Engine::GraphicsContext* context, std::vector<Engine::RenderNode>& nodes, Engine::GpuMappedBuffer* matrices);
		void GenerateMipmaps(Engine::DescriptorHeap* heap, Engine::ComputeContext* context);
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