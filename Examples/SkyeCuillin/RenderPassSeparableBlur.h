#ifndef __RENDER_PASS_SEPARABLE_BLUR__H__
#define __RENDER_PASS_SEPARABLE_BLUR__H__

#include "RenderPassFullscreen.h"

namespace SkyeCuillin
{
	class RenderPassSeparableBlur : public RenderPassFullscreen
	{
	protected:
		Engine::RootSignature* mBlurRS;
		Engine::PipelineState* mBlurPS;
		Engine::D3DShader* mBlur;

		Engine::ColorBuffer* mColorBuffer;
		Engine::DepthBuffer* mDepthBuffer;

		Engine::GpuMappedBuffer* mConstBuffer;
		void* mConstBufferPtr;

		Engine::ColorBuffer* mInput;
		Engine::ColorBuffer* mOutput;

		Engine::Graphics::Format mFormat;

	public:
		RenderPassSeparableBlur(Engine::D3DRenderer* renderer, unsigned int width, unsigned int height, Engine::Graphics::Format format);
		virtual ~RenderPassSeparableBlur();
		void Process(Engine::DescriptorHeap* heap, Engine::GraphicsContext* context, unsigned int direction);
		virtual void Resize(unsigned int width, unsigned int height);

		void SetSources(Engine::ColorBuffer* input, Engine::ColorBuffer* output)
		{
			mInput = input;
			mOutput = output;
		}

		inline Engine::ColorBuffer* GetBuffer()
		{
			return mColorBuffer;
		}
	};
}

#endif