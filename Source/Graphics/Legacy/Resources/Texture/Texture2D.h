///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Texture1D.h
//
// Defines 2-dimensional texture data
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __TEXTURE__2D__H__
#define __TEXTURE__2D__H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "Texture.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition
namespace Engine
{
	template <typename T>
	class Texture2D : public Texture
	{
	private:
		std::vector<T*>* mData;		// Texture data (vector of data -> each record represent one mipmap)
		std::vector<int>* mWidth;	// Texture width size (vector of ints -> each record represent one mipmap)
		std::vector<int>* mHeight;	// Texture height size (vector of ints -> each record represent one mipmap)
		unsigned int mTexture;		// OpenGL texture handle

	public:
		/// <summary>
		/// Constructor for single texture image. For 1-dimensional texture BC types are disallowed!
		/// </summary>
		Texture2D(int width, int height, T* data, Texture::Type type, Texture::Format format, bool mipmaps) : Texture()
		{
			// Allocate memory
			mData = new std::vector<T*>();
			mWidth = new std::vector<int>();
			mHeight = new std::vector<int>();

			// Prepare level 0
			mWidth->push_back(width);
			mHeight->push_back(height);
			mData->push_back(new T[(*mWidth)[0] * (*mHeight)[0] * Texture::mOpenGLChannelCount[(int)format]]);
			memcpy((*mData)[0], data, sizeof(T)* (*mWidth)[0] * (*mHeight)[0] * Texture::mOpenGLChannelCount[(int)format]);

			mType = type;
			mFormat = format;

			// Generate mipmaps if needed
			if (mipmaps == true)
			{
				GenerateMipmaps();
			}

			glGenTextures(1, &mTexture);
			glBindTexture(GL_TEXTURE_2D, mTexture);
			for (unsigned int i = 0; i < mData->size(); i++)
			{
				glTexImage2D(GL_TEXTURE_2D,
					i,
					Texture::mOpenGLInternalFormat[(int)mFormat],
					(*mWidth)[i],
					(*mHeight)[i],
					0,
					Texture::mOpenGLChannels[(int)mFormat],
					Texture::mOpenGLFormat[(int)mFormat],
					(*mData)[i]);
			}
			SetFilter(Texture::Filter::TEXTURE_FILTER_LINEAR);
			SetWrap(Texture::Wrap::TEXTURE_WRAP_REPEAT);
		}

		/// <summary>
		/// Virtual destructor for Texture
		/// </summary>
		virtual ~Texture2D()
		{
			// Free GPU memory
			glDeleteTextures(1, &mTexture);

			// Free memory
			for (unsigned int i = 0; i < mData->size(); i++)
			{
				delete[] (*mData)[i];
			}
			delete mData;
			delete mWidth;
			delete mHeight;
		}

		/// <summary>
		/// Synchronize between device and host
		/// </summary>
		/// <param name="deviceToHost">Prefer GPU memory (GPU->CPU) or CPU memory (CPU->GPU)</param>
		virtual void Synchronize(bool deviceToHost)
		{
			if (deviceToHost)
			{
				glBindTexture(GL_TEXTURE_2D, mTexture);
				for (unsigned int i = 0; i < mData->size(); i++)
				{
					glGetTexImage(GL_TEXTURE_2D, i, Texture::mOpenGLFormat[(int)mFormat], Texture::mOpenGLType[(int)mType], (*mData)[i]);
				}
			}
			else
			{
				glBindTexture(GL_TEXTURE_2D, mTexture);
				for (unsigned int i = 0; i < mData->size(); i++)
				{
					glTexImage2D(GL_TEXTURE_2D,
						i,
						Texture::mOpenGLInternalFormat[(int)mFormat],
						(*mWidth)[i],
						(*mHeight)[i],
						0,
						Texture::mOpenGLChannels[(int)mFormat],
						Texture::mOpenGLFormat[(int)mFormat],
						(*mData)[i]);
				}
			}
		}

		/// <summary>
		/// Get number of texture dimensions
		/// </summary>
		virtual int GetDimensions()
		{
			return 2;
		}

		/// <summary>
		/// Get texture total size (in bytes)
		/// </summary>
		virtual int GetSize(int miplevel = 0)
		{
			return (*mWidth)[miplevel] * (*mHeight)[miplevel] * sizeof(T);
		}

		/// <summary>
		/// Get width (X-dimension size)
		/// </summary>
		virtual int GetWidth(int miplevel = 0)
		{
			return (*mWidth)[miplevel];
		}

		/// <summary>
		/// Get height (Y-dimension size)
		/// </summary>
		virtual int GetHeight(int miplevel = 0)
		{
			return (*mHeight)[miplevel];
		}

		/// <summary>
		/// Get texture depth (Z-dimension size)
		/// </summary>
		virtual int GetDepth(int miplevel = 0)
		{
			return 1;
		}

		/// <summary>
		/// Get OpenGL texture handle
		/// </summary>
		virtual unsigned int GetTexture()
		{
			return mTexture;
		}

		/// <summary>
		/// Bind texture to specified texture unit
		/// </summary>
		/// <param name="textureUnit">Texture unit where to bind the texture</param>
		virtual void BindTexture(int textureUnit = 0)
		{
			glActiveTexture(GL_TEXTURE0 + textureUnit);
			glBindTexture(GL_TEXTURE_2D, mTexture);
		}

		/// <summary>
		/// Set texture wrapping mode
		/// </summary>
		/// <param name="wrap">Texture wrapping mode</param>
		virtual void SetWrap(Texture::Wrap wrap)
		{
			mWrap = wrap;
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, Texture::mOpenGLWrap[(int)wrap]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, Texture::mOpenGLWrap[(int)wrap]);
		}

		/// <summary>
		/// Set texture filter
		/// </summary>
		/// <param name="wrap">Texture filtering mode</param>
		virtual void SetFilter(Texture::Filter filter)
		{
			mFilter = filter;
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Texture::mOpenGLMinificationFilter[(int)filter]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Texture::mOpenGLMagnificationFilter[(int)filter]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mWidth->size() - 1);
		}

	private:
		/// <summary>
		/// Calculate alpha coverage for given threshold
		/// </summary>
		float CalculateAlphaCoverage(float threshold, int level)
		{
			float coverage = 0.0f;
			for (int i = 0; i < (*mHeight)[level]; i++)
			{
				for (int j = 0; j < (*mWidth)[level]; j++)
				{
					coverage += ((float)(*mData)[level][(i * (*mWidth)[0] + j) * Texture::mOpenGLChannelCount[(int)mFormat] + 3] / 255.0f) > threshold ? 1.0f : 0.0f;
				}
			}
			coverage /= ((*mWidth)[level] * (*mHeight)[level]);
			return coverage;
		}

	public:
		/// <summary>
		/// Generate mipmaps, the function assumes that there is texture fully prepared in texture level 0.
		/// </summary>
		virtual void GenerateMipmaps(MipMapGeneration mode = MIPMAP_GENERATE_COLOR, float threshold = 0.5f)
		{
			// Calculate the number of miplevels
			int levels = 0;
			int maxdim = (*mWidth)[0] > (*mHeight)[0] ? (*mWidth)[0] : (*mHeight)[0];
			while (maxdim != 0)
			{
				maxdim /= 2;
				levels++;
			}

			// Resize containers
			mWidth->resize(1);
			mHeight->resize(1);
			for (unsigned int i = 0; i < mData->size(); i++)
			{
				if (i == 0)
				{
					continue;
				}
				else
				{
					delete[] (*mData)[i];
				}
			}
			mData->resize(1);

			// If we're generating mipmaps of texture with alpha, calculate coverage
			float coverage = 0.0f;
			int ch = Texture::mOpenGLChannelCount[(int)mFormat];
			if (mode == MIPMAP_GENERATE_ALPHA && ch == 4)
			{
				coverage = CalculateAlphaCoverage(threshold, 0);
			}

			// Start mipmap generation
			int width = (*mWidth)[0];
			int height = (*mHeight)[0];
			for (int level = 0; level < levels; level++)
			{
				if (level == 0)
				{
					width /= 2;
					height /= 2;
					continue;
				}

				// Generate mipmap using averaging scheme
				T* data = new T[width * height * Texture::mOpenGLChannelCount[(int)mFormat]];
				for (int i = 0; i < height; i++)
				{
					for (int j = 0; j < width; j++)
					{
						// Normal map mipmapping
						if (mode == MIPMAP_GENERATE_NORMAL)
						{
							float4 normal = float4();
							normal.x = ((*mData)[level - 1][((i * 2 + 0) * (width * 2) + j * 2 + 0) * ch + 0] / 255.0f * 2.0f - 1.0f +
								(*mData)[level - 1][((i * 2 + 1) * (width * 2) + j * 2 + 0) * ch + 0] / 255.0f * 2.0f - 1.0f +
								(*mData)[level - 1][((i * 2 + 1) * (width * 2) + j * 2 + 1) * ch + 0] / 255.0f * 2.0f - 1.0f +
								(*mData)[level - 1][((i * 2 + 0) * (width * 2) + j * 2 + 1) * ch + 0] / 255.0f * 2.0f - 1.0f);
							normal.y = ((*mData)[level - 1][((i * 2 + 0) * (width * 2) + j * 2 + 0) * ch + 1] / 255.0f * 2.0f - 1.0f +
								(*mData)[level - 1][((i * 2 + 1) * (width * 2) + j * 2 + 0) * ch + 1] / 255.0f * 2.0f - 1.0f +
								(*mData)[level - 1][((i * 2 + 1) * (width * 2) + j * 2 + 1) * ch + 1] / 255.0f * 2.0f - 1.0f +
								(*mData)[level - 1][((i * 2 + 0) * (width * 2) + j * 2 + 1) * ch + 1] / 255.0f * 2.0f - 1.0f);
							normal.z = ((*mData)[level - 1][((i * 2 + 0) * (width * 2) + j * 2 + 0) * ch + 2] / 255.0f * 2.0f - 1.0f +
								(*mData)[level - 1][((i * 2 + 1) * (width * 2) + j * 2 + 0) * ch + 2] / 255.0f * 2.0f - 1.0f +
								(*mData)[level - 1][((i * 2 + 1) * (width * 2) + j * 2 + 1) * ch + 2] / 255.0f * 2.0f - 1.0f +
								(*mData)[level - 1][((i * 2 + 0) * (width * 2) + j * 2 + 1) * ch + 2] / 255.0f * 2.0f - 1.0f);
							normal.w = 0.0f;
							normal = normalize(normal);
							data[(i * width + j) * ch + 0] = T((normal.x * 0.5f + 0.5f) * 255.0f);
							data[(i * width + j) * ch + 1] = T((normal.y * 0.5f + 0.5f) * 255.0f);
							data[(i * width + j) * ch + 2] = T((normal.z * 0.5f + 0.5f) * 255.0f);
							if (ch == 4)
							{
								data[(i * width + j) * ch + 3] = T(0);
							}
						}
						else
						{
							for (int k = 0; k < ch; k++)
							{
								data[(i * width + j) * ch + k] = ((*mData)[level - 1][((i * 2 + 0) * (width * 2) + j * 2 + 0) * ch + k] +
									(*mData)[level - 1][((i * 2 + 1) * (width * 2) + j * 2 + 0) * ch + k] +
									(*mData)[level - 1][((i * 2 + 1) * (width * 2) + j * 2 + 1) * ch + k] +
									(*mData)[level - 1][((i * 2 + 0) * (width * 2) + j * 2 + 1) * ch + k]) / T(4);
							}
						}
					}
				}

				// Use bisection to calculate coverage for current mip map
				if (mode == MIPMAP_GENERATE_ALPHA && ch == 4)
				{
					float scale = 0.0f;

					float a = 0.0f;
					float b = 1.0f;
					for (int n = 0; n < 128; n++)
					{
						float c = (a + b) * 0.5f;
						float cCoverage = CalculateAlphaCoverage(c, level);
						scale = coverage / cCoverage;

						if (fabsf(cCoverage - coverage) < 0.01f)
						{
							break;
						}

						if (fabsf(cCoverage - CalculateAlphaCoverage(a, level)) < fabsf(cCoverage - CalculateAlphaCoverage(b, level)))
						{
							b = c;
						}
						else
						{
							a = c;
						}

						for (int i = 0; i < (*mHeight)[level]; i++)
						{
							for (int j = 0; j < (*mWidth)[level]; j++)
							{
								data[(i * width + j) * ch + 3] = T(data[(i * width + j) * ch + 3] * scale);
							}
						}
					}
				}

				mWidth->push_back(width);
				mHeight->push_back(height);
				mData->push_back(data);

				width /= 2;
				height /= 2;
			}
		}

		ALIGNED_NEW_DELETE("Engine::Texture2D")
	};
}

#endif