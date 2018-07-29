#ifndef __RENDER_PASS_PICKING__H__
#define __RENDER_PASS_PICKING__H__

#include "RenderPass.h"
#include "RenderGizmo.h"

namespace SkyeCuillin
{
	class RenderPassPicking : public RenderPass
	{
	protected:
		Engine::RootSignature* mPickingRS;
		Engine::PipelineState* mPickingPS;
		Engine::D3DShader* mPicking;

		Engine::RootSignature* mPickingGizmoRS;
		Engine::PipelineState* mPickingGizmoPS;
		Engine::D3DShader* mPickingGizmo;

		Engine::ColorBuffer* mColorBuffer;
		Engine::DepthBuffer* mDepthBuffer;

		Engine::GpuMappedBuffer* mCameraBuffer;
		void* mCameraBufferPtr;

		Engine::GpuMappedBuffer* mReadbackBuffer;
		unsigned int mRowPitch;

		SkyeCuillin::RenderGizmo* mGizmoRenderer;

	public:
		RenderPassPicking(Engine::D3DRenderer* renderer, unsigned int width, unsigned int height, SkyeCuillin::RenderGizmo* gizmoRenderer);
		virtual ~RenderPassPicking();
		void Process(Engine::Entity* camera, Engine::DescriptorHeap* heap, Engine::GraphicsContext* context, std::vector<Engine::RenderNode>& nodes, Engine::GpuMappedBuffer* matrices);
		void Readback(Engine::GraphicsContext* context);
		virtual void Resize(unsigned int width, unsigned int height);

		inline Engine::ColorBuffer* GetBuffer()
		{
			return mColorBuffer;
		}

		inline Engine::DepthBuffer* GetDepthBuffer()
		{
			return mDepthBuffer;
		}

		inline unsigned int GetValue(int x, int y)
		{
			unsigned int pos = x + y * mRowPitch / sizeof(unsigned int);
			unsigned int* ptr = (unsigned int*)mReadbackBuffer->Map(0, mRowPitch * mHeight * sizeof(unsigned int));
			unsigned int value = ptr[pos];
			mReadbackBuffer->Unmap();
			return value;
		}
	};
}

#endif