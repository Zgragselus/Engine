///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Texture1D.h
//
// Defines 1-dimensional texture data
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __TEXTURE__1D__H__
#define __TEXTURE__1D__H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "Texture.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition
namespace Engine
{
	template <typename T>
	class Texture1D : public Texture
	{
	private:
		std::vector<T*>* mData;		// Texture data (vector of data -> each record represent one mipmap)
		std::vector<int>* mWidth;	// Texture width size (vector of ints -> each record represent one mipmap)
		unsigned int mTexture;		// OpenGL texture handle

	public:
		/// <summary>
		/// Constructor for single texture image. For 1-dimensional texture BC types are disallowed!
		/// </summary>
		Texture1D(int width, T* data, Texture::Type type, Texture::Format format, bool mipmaps) : Texture()
		{
			// Allocate memory
			mData = new std::vector<T*>();
			mWidth = new std::vector<int>();

			// Prepare level 0
			mWidth->push_back(width);
			mData->push_back(new T[(*mWidth)[0] * Texture::mOpenGLChannelCount[(int)format]]);
			memcpy((*mData)[0], data, sizeof(T)* (*mWidth)[0] * Texture::mOpenGLChannelCount[(int)format]);

			mType = type;
			mFormat = format;

			// Generate mipmaps if needed
			if (mipmaps == true)
			{
				GenerateMipmaps();
			}

			glGenTextures(1, &mTexture);
			glBindTexture(GL_TEXTURE_1D, mTexture);
			for (unsigned int i = 0; i < mData->size(); i++)
			{
				glTexImage1D(GL_TEXTURE_1D,
					i,
					Texture::mOpenGLInternalFormat[(int)mFormat],
					(*mWidth)[i],
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
		virtual ~Texture1D()
		{
			// Free GPU memory
			glDeleteTextures(1, &mTexture);

			// Free memory
			for (unsigned int i = 0; i < mData->size(); i++)
			{
				delete[](*mData)[i];
			}
			delete mData;
			delete mWidth;
		}

		/// <summary>
		/// Synchronize between device and host
		/// </summary>
		/// <param name="deviceToHost">Prefer GPU memory (GPU->CPU) or CPU memory (CPU->GPU)</param>
		virtual void Synchronize(bool deviceToHost)
		{
			if (deviceToHost)
			{
				glBindTexture(GL_TEXTURE_1D, mTexture);
				for (unsigned int i = 0; i < mData->size(); i++)
				{
					glGetTexImage(GL_TEXTURE_1D, i, Texture::mOpenGLFormat[(int)mFormat], Texture::mOpenGLType[(int)mType], (*mData)[i]);
				}
			}
			else
			{
				glBindTexture(GL_TEXTURE_1D, mTexture);
				for (unsigned int i = 0; i < mData->size(); i++)
				{
					glTexImage1D(GL_TEXTURE_1D,
						i,
						Texture::mOpenGLInternalFormat[(int)mFormat],
						(*mWidth)[i],
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
			return 1;
		}

		/// <summary>
		/// Get texture total size (in bytes)
		/// </summary>
		virtual int GetSize(int miplevel = 0)
		{
			return (*mWidth)[miplevel] * sizeof(T);
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
			return 1;
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
			glBindTexture(GL_TEXTURE_1D, mTexture);
		}

		/// <summary>
		/// Set texture wrapping mode
		/// </summary>
		/// <param name="wrap">Texture wrapping mode</param>
		virtual void SetWrap(Texture::Wrap wrap)
		{
			mWrap = wrap;
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, Texture::mOpenGLWrap[(int)wrap]);
		}

		/// <summary>
		/// Set texture filter
		/// </summary>
		/// <param name="wrap">Texture filtering mode</param>
		virtual void SetFilter(Texture::Filter filter)
		{
			mFilter = filter;
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, Texture::mOpenGLMinificationFilter[(int)filter]);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, Texture::mOpenGLMagnificationFilter[(int)filter]);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAX_LEVEL, mWidth->size() - 1);
		}

		/// <summary>
		/// Generate mipmaps, the function assumes that there is texture fully prepared in texture level 0.
		/// </summary>
		virtual void GenerateMipmaps(MipMapGeneration mode = MIPMAP_GENERATE_COLOR, float threshold = 0.5f)
		{
			// Calculate the number of miplevels
			int levels = 0;
			int width = (*mWidth)[0];
			while (width != 0)
			{
				width /= 2;
				levels++;
			}

			// Resize continers
			mWidth->resize(1);
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

			// Start mipmap generation
			width = (*mWidth)[0];
			for (int level = 0; level < levels; level++)
			{
				if (level == 0)
				{
					width /= 2;
					continue;
				}

				// Generate mipmap using averaging scheme
				T* data = new T[width * Texture::mOpenGLChannelCount[(int)mFormat]];
				int ch = Texture::mOpenGLChannelCount[(int)mFormat];
				for (int i = 0; i < width; i++)
				{
					for (int j = 0; j < ch; j++)
					{
						data[i * ch + j] = ((*mData)[level - 1][(i * 2 + 0) * ch + j] + (*mData)[level - 1][(i * 2 + 1) * ch + j]) / T(2);
					}
				}

				mWidth->push_back(width);
				mData->push_back(data);

				width /= 2;
			}
		}

		ALIGNED_NEW_DELETE("Engine::Texture1D")
	};
}

#endif