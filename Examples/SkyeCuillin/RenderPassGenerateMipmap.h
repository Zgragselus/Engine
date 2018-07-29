#ifndef __RENDER_PASS_GENERATE_MIPMAP__H__
#define __RENDER_PASS_GENERATE_MIPMAP__H__

#include "RenderPass.h"

namespace SkyeCuillin
{
	class RenderPassGenerateMipmap : public RenderPass
	{
	private:
		union FP32
		{
			uint32_t u;
			float f;
			struct
			{
				uint32_t Mantissa : 23;
				uint32_t Exponent : 8;
				uint32_t Sign : 1;
			};
		};

		union FP16
		{
			uint16_t u;
			struct
			{
				uint16_t Mantissa : 10;
				uint16_t Exponent : 5;
				uint16_t Sign : 1;
			};
		};

		static inline FP32 half_to_float_full(FP16 h)
		{
			FP32 o = { 0 };

			// From ISPC ref code
			if (h.Exponent == 0 && h.Mantissa == 0) // (Signed) zero
				o.Sign = h.Sign;
			else
			{
				if (h.Exponent == 0) // Denormal (will convert to normalized)
				{
					// Adjust mantissa so it's normalized (and keep track of exp adjust)
					int e = -1;
					uint32_t m = h.Mantissa;
					do
					{
						e++;
						m <<= 1;
					} while ((m & 0x400) == 0);

					o.Mantissa = (m & 0x3ff) << 13;
					o.Exponent = 127 - 15 - e;
					o.Sign = h.Sign;
				}
				else if (h.Exponent == 0x1f) // Inf/NaN
				{
					// NOTE: It's safe to treat both with the same code path by just truncating
					// lower Mantissa bits in NaNs (this is valid).
					o.Mantissa = h.Mantissa << 13;
					o.Exponent = 255;
					o.Sign = h.Sign;
				}
				else // Normalized number
				{
					o.Mantissa = h.Mantissa << 13;
					o.Exponent = 127 - 15 + h.Exponent;
					o.Sign = h.Sign;
				}
			}

			return o;
		}

	protected:
		Engine::RootSignature* mMipmapRS;
		Engine::PipelineState* mMipmapPS;
		Engine::D3DShader* mMipmap;

		Engine::ColorBuffer* mTexture;

		unsigned int mMiplevels;

		Engine::GpuMappedBuffer* mReadbackBuffer;
		unsigned int mRowPitch;

	public:
		RenderPassGenerateMipmap(Engine::D3DRenderer* renderer, unsigned int width, unsigned int height);
		virtual ~RenderPassGenerateMipmap();
		void Process(Engine::DescriptorHeap* heap, Engine::ComputeContext* context, Engine::ColorBuffer* source);
		void Readback(Engine::GraphicsContext* context);
		virtual void Resize(unsigned int width, unsigned int height);

		inline Engine::ColorBuffer* GetBuffer()
		{
			return mTexture;
		}

		inline float GetValue(int x, int y)
		{
			unsigned int pos = x + y * mRowPitch / sizeof(unsigned int);
			FP16* ptr = (FP16*)mReadbackBuffer->Map(0, mRowPitch * 1 * sizeof(unsigned int));
			FP32 r = half_to_float_full(ptr[pos + 0]);
			FP32 g = half_to_float_full(ptr[pos + 1]);
			FP32 b = half_to_float_full(ptr[pos + 2]);
			FP32 a = half_to_float_full(ptr[pos + 3]);
			mReadbackBuffer->Unmap();
			return r.f * 0.299f + g.f * 0.587f + b.f * 0.114f;
		}
	};
}

#endif