#ifndef __DEVIL_H__
#define __DEVIL_H__

#pragma comment(lib, "DevIL.lib")
#pragma comment(lib, "ILU.lib")
#pragma comment(lib, "ILUT.lib")

#include "Core/Log/Log.h"
#include "Core/Files/Files.h"
#include "../Scene/Scene.h"
#include <IL/il.h>
#include <IL/ilu.h>

namespace Engine
{
	class LoaderDevIL
	{
	private:
		Log* mLog;

		float GetCoverage(size_t width, size_t height, unsigned char* data, float alphaCoverageRatio, float scale) const;
		void ScaleCoverage(size_t width, size_t height, unsigned char* data, float alphaCoverageRatio, const float desiredCoverage) const;

		inline void ReadNormal(size_t pos, unsigned char* data, float* x, float *y, float *z) const
		{
			unsigned char r = data[pos + 0];
			unsigned char g = data[pos + 1];
			unsigned char b = data[pos + 2];
			*x = (float)r / 255.0f * 2.0f - 1.0f;
			*y = (float)g / 255.0f * 2.0f - 1.0f;
			*z = (float)b / 255.0f * 2.0f - 1.0f;
		}

		inline void WriteNormal(size_t pos, unsigned char* data, float x, float y, float z) const
		{
			float l = 1.0f / sqrtf(x * x + y * y + z * z);
			x = ((x * l) * 0.5f + 0.5f) * 255.0f;
			y = ((y * l) * 0.5f + 0.5f) * 255.0f;
			z = ((z * l) * 0.5f + 0.5f) * 255.0f;
			int r = (int)x;
			int g = (int)y;
			int b = (int)z;
			r = r > 255 ? 255 : (r <= 0 ? 0 : r);
			g = g > 255 ? 255 : (g <= 0 ? 0 : g);
			b = b > 255 ? 255 : (b <= 0 ? 0 : b);
			data[pos + 0] = (unsigned char)r;
			data[pos + 1] = (unsigned char)g;
			data[pos + 2] = (unsigned char)b;
		}

	public:
		struct Image
		{
			size_t mWidth;
			size_t mHeight;
			size_t mBpp;
			size_t mChannels;
			size_t mMipLevels;
			unsigned char* mData;
			unsigned char** mMipmaps;
			bool mUseAlphaMipmap;
			std::string mName;

			~Image()
			{
				if (mMipmaps)
				{
					for (size_t i = 0; i < mMipLevels; i++)
					{
						if (mMipmaps[i])
						{
							delete mMipmaps[i];
							mMipmaps[i] = nullptr;
						}

						if (i == 0)
						{
							mData = nullptr;
						}
					}

					delete mMipmaps;
					mMipmaps = nullptr;
				}

				if (mData)
				{
					delete mData;
					mData = nullptr;
				}
			}

			ALIGNED_NEW_DELETE("Game::Loader::DevIL::Image")
		};

		LoaderDevIL(Log* l)
		{
			mLog = l;

			ilInit();
		}

		Image* Load(const std::string& filename) const;
		void GenerateMipmaps(Image* image) const;
		void GenerateMipmapsNormal(Image* image) const;

		ALIGNED_NEW_DELETE("Game::Loader::DevIL")
	};
}

#endif