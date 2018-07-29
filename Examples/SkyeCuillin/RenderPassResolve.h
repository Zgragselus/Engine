#ifndef __RENDER_PASS_RESOLVE__H__
#define __RENDER_PASS_RESOLVE__H__

#include "RenderPassFullscreen.h"

namespace SkyeCuillin
{
	class RenderPassResolve : public RenderPassFullscreen
	{
	protected:
		Engine::RootSignature* mResolveRS;
		Engine::PipelineState* mResolvePS;
		Engine::D3DShader* mResolve;

		Engine::GpuMappedBuffer* mConstBuffer;
		void* mConstBufferPtr;

		Engine::ColorBuffer* mInput;
		Engine::DepthBuffer* mInputDepth;

		Engine::ColorBuffer* mInitialLuminanceRT;
		Engine::PipelineState* mInitialLuminancePS;
		Engine::D3DShader* mInitialLuminance;

		Engine::ColorBuffer* mAdaptLuminanceRT[2];
		Engine::PipelineState* mAdaptLuminancePS;
		Engine::D3DShader* mAdaptLuminance;

		Engine::ColorBuffer* mBloomRT[2];
		Engine::PipelineState* mBloomPS[2];
		Engine::D3DShader* mBloom[2];

		Engine::RootSignature* mMipmapRS;
		Engine::PipelineState* mMipmapPS;
		Engine::D3DShader* mMipmap;

		Engine::Texture* mLensDirt;

		Engine::Graphics::Format mFormat;

		unsigned int mCurrentLuminanceTarget;

		float ShoulderStrength;
		float LinearStrength;
		float LinearAngle;
		float ToeStrength;
		float ToeNumerator;
		float ToeDenominator;
		float LinearWhite;
		float BloomBlurSigma;
		float BloomThreshold;
		float BloomMagnitude;

		void GenerateMipmaps(Engine::DescriptorHeap* heap, Engine::ComputeContext* context, Engine::ColorBuffer* image);

	public:
		RenderPassResolve(Engine::Log* log, Engine::D3DRenderer* renderer, unsigned int width, unsigned int height, unsigned int samples);
		virtual ~RenderPassResolve();
		void Process(Engine::D3DSwapChain* swapChain, Engine::DescriptorHeap* heap, Engine::GraphicsContext* context);
		virtual void Resize(unsigned int width, unsigned int height);

		void SetInput(Engine::ColorBuffer* input)
		{
			mInput = input;
		}

		void SetInputDepth(Engine::DepthBuffer* inputDepth)
		{
			mInputDepth = inputDepth;
		}

		Engine::ColorBuffer* GetBuffer(int id)
		{
			switch (id)
			{
			case 0:
				return mInitialLuminanceRT;
				break;

			case 1:
				return mAdaptLuminanceRT[0];
				break;

			default:
			case 2:
				return mAdaptLuminanceRT[1];
				break;
			}
		}
	};
}

#endif