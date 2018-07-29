///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Texture.h
//
// Defines super-class for any texture type inside engine
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __TEXTURE__H__
#define __TEXTURE__H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "Core/Memory/Memory.h"
#include "Core/Files/Files.h"
#include "Graphics/Renderer.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	/// <summary>
	/// Abstract Texture class, superclass for any texture type inside the engine
	/// </summary>
	class Texture
	{
	public:
		/// <summary>Texture type enumeration</summary>
		enum Type
		{
			/// <summary>1-dimensional texture</summary>
			TEXTURE_TYPE_1D,
			/// <summary>2-dimensional texture</summary>
			TEXTURE_TYPE_2D,
			/// <summary>3-dimensional aka volumetric texture</summary>
			TEXTURE_TYPE_3D,
			/// <summary>Cube map</summary>
			TEXTURE_TYPE_CUBE,
			/// <summary>Array of 2-dimensional textures</summary>
			TEXTURE_TYPE_2D_ARRAY,
			/// <summary>Multisampled 2-dimensional texture</summary>
			TEXTURE_TYPE_2D_MULTISAMPLE,
			/// <summary>Do not use! MAX_VALUE for enum</summary>
			TEXTURE_TYPE_COUNT
		};

		/// <summary>Texture format enumeration</summary>
		enum Format
		{
			/// <summary></summary>
			TEXTURE_FORMAT_NONE = 0,

			// 8-bit integer formats
			/// <summary>8-bit red channel</summary>
			TEXTURE_FORMAT_R8,
			/// <summary>8-bit red and green channel</summary>
			TEXTURE_FORMAT_RG8,
			/// <summary>8-bit red, green and blue channel</summary>
			TEXTURE_FORMAT_RGB8,
			/// <summary>8-bit red, green, blue and alpha channel</summary>
			TEXTURE_FORMAT_RGBA8,

			// 16-bit floating-point formats
			/// <summary>16-bit fp red channel</summary>
			TEXTURE_FORMAT_R16F,
			/// <summary>16-bit fp red and green channel</summary>
			TEXTURE_FORMAT_RG16F,
			/// <summary>16-bit fp red, green and blue channel</summary>
			TEXTURE_FORMAT_RGB16F,
			/// <summary>16-bit fp red, green, blue and alpha channel</summary>
			TEXTURE_FORMAT_RGBA16F,

			// 32-bit floating-point formats
			/// <summary>32-bit fp red channel</summary>
			TEXTURE_FORMAT_R32F,
			/// <summary>32-bit fp red and green channel</summary>
			TEXTURE_FORMAT_RG32F,
			/// <summary>32-bit fp red, green and blue channel</summary>
			TEXTURE_FORMAT_RGB32F,
			/// <summary>32-bit fp red, green, blue and alpha channel</summary>
			TEXTURE_FORMAT_RGBA32F,

			// Block-compression formats
			/// <summary>Block-compression 1 -> S3TC DXT1 compression</summary>
			TEXTURE_FORMAT_BC1,
			/// <summary>Block-compression 2 -> S3TC DXT3 compression</summary>
			TEXTURE_FORMAT_BC2,
			/// <summary>Block-compression 3 -> S3TC DXT5 compression</summary>
			TEXTURE_FORMAT_BC3,
			/// <summary>Block-compression 4 -> RGTC 1-channel compression</summary>
			TEXTURE_FORMAT_BC4,
			/// <summary>Block-compression 5 -> RGTC 2-channel compression</summary>
			TEXTURE_FORMAT_BC5,

			// Depth formats
			/// <summary>32-bit depth</summary>
			TEXTURE_FORMAT_D32,

			// Depth-stencil format
			/// <summary>24-bit depth plus 8-bit stencil</summary>
			TEXTURE_FORMAT_D24S8,

			/// <summary>Do not use! MAX_VALUE for enum</summary>
			TEXTURE_FORMAT_COUNT
		};

		/// <summary>Texture filter enumeration</summary>
		enum Wrap
		{
			/// <summary>Repeat wrap</summary>
			TEXTURE_WRAP_REPEAT,
			/// <summary>Clamp wrap (include border)</summary>
			TEXTURE_WRAP_CLAMP,
			/// <summary>Clamp wrap (exclude border)</summary>
			TEXTURE_WRAP_CLAMP_TO_EDGE,
			/// <summary>Repeat and mirror wrap</summary>
			TEXTURE_WRAP_MIRROR_REPEAT,
			/// <summary>Clamp and mirror wrap (include border)</summary>
			TEXTURE_WRAP_MIRROR_CLAMP,
			/// <summary>Clamp and mirror wrap (exclude border)</summary>
			TEXTURE_WRAP_MIRROR_CLAMP_TO_EDGE,
			/// <summary>Do not use! MAX_VALUE for enum</summary>
			TEXTURE_WRAP_COUNT
		};

		/// <summary>Texture filter enumeration</summary>
		enum Filter
		{
			/// <summary>Point, aka nearest-neighbour texture filter</summary>
			TEXTURE_FILTER_POINT,
			/// <summary>Linear texture filter</summary>
			TEXTURE_FILTER_LINEAR,
			/// <summary>Mip-mapped texture filter</summary>
			TEXTURE_FILTER_MIPMAP,
			/// <summary>Anisotropic texture filter</summary>
			TEXTURE_FILTER_ANISOTROPIC,
			/// <summary>Do not use! MAX_VALUE for enum</summary>
			TEXTURE_FILTER_COUNT
		};

		/// <summary>Mipmap generation method</summary>
		enum MipMapGeneration
		{
			/// <summary>Color map</summary>
			MIPMAP_GENERATE_COLOR,
			/// <summary>Normal map texture</summary>
			MIPMAP_GENERATE_NORMAL,
			/// <summary>Alpha masked texture</summary>
			MIPMAP_GENERATE_ALPHA,
			/// <summary>Do not use! MAX_VALUE for enum</summary>
			MIPMAP_GENERATE_COUNT
		};

	protected:
		Type mType;
		Format mFormat;
		Wrap mWrap;
		Filter mFilter;

		static GLuint mOpenGLType[TEXTURE_TYPE_COUNT];
		static GLuint mOpenGLFormat[TEXTURE_FORMAT_COUNT];
		static GLuint mOpenGLInternalFormat[TEXTURE_FORMAT_COUNT];
		static bool mOpenGLCompression[TEXTURE_FORMAT_COUNT];
		static unsigned int mOpenGLChannelCount[TEXTURE_FORMAT_COUNT];
		static GLuint mOpenGLChannels[TEXTURE_FORMAT_COUNT];
		static GLuint mOpenGLWrap[TEXTURE_WRAP_COUNT];
		static GLuint mOpenGLMagnificationFilter[TEXTURE_FILTER_COUNT];
		static GLuint mOpenGLMinificationFilter[TEXTURE_FILTER_COUNT];

	public:
		/// <summary>
		/// Default constructor
		/// </summary>
		Texture() 
		{
		}

		/// <summary>
		/// Empty constructor
		/// </summary>
		Texture(const std::string& filename) {}

		/// <summary>
		/// Virtual destructor for Texture
		/// </summary>
		virtual ~Texture() {}

		/// <summary>
		/// Synchronize between device and host
		/// </summary>
		/// <param name="deviceToHost">Prefer GPU memory (GPU->CPU) or CPU memory (CPU->GPU)</param>
		virtual void Synchronize(bool deviceToHost) {}

		/// <summary>
		/// Get number of texture dimensions
		/// </summary>
		virtual int GetDimensions() { return 0; }

		/// <summary>
		/// Get texture total size (in bytes)
		/// </summary>
		/// <param name="miplevel">Mipmap level for which we are seeking info</param>
		virtual int GetSize(int miplevel = 0) { return 0; }

		/// <summary>
		/// Get width (X-dimension size)
		/// </summary>
		/// <param name="miplevel">Mipmap level for which we are seeking info</param>
		virtual int GetWidth(int miplevel = 0) { return 0; }

		/// <summary>
		/// Get height (Y-dimension size)
		/// </summary>
		/// <param name="miplevel">Mipmap level for which we are seeking info</param>
		virtual int GetHeight(int miplevel = 0) { return 0; }

		/// <summary>
		/// Get texture depth (Z-dimension size)
		/// </summary>
		/// <param name="miplevel">Mipmap level for which we are seeking info</param>
		virtual int GetDepth(int miplevel = 0) { return 0; }

		/// <summary>
		/// Get OpenGL texture handle
		/// </summary>
		virtual unsigned int GetTexture() { return 0; }

		/// <summary>
		/// Bind texture to specified texture unit
		/// </summary>
		/// <param name="textureUnit">Texture unit where to bind the texture</param>
		virtual void BindTexture(int textureUnit = 0) {}

		/// <summary>
		/// Set texture wrapping mode
		/// </summary>
		/// <param name="wrap">Texture wrapping mode</param>
		virtual void SetWrap(Wrap wrap) {}

		/// <summary>
		/// Set texture filter
		/// </summary>
		/// <param name="wrap">Texture filtering mode</param>
		virtual void SetFilter(Filter wrap) {}

		/// <summary>
		/// Generate mipmaps
		/// </summary>
		virtual void GenerateMipmaps(MipMapGeneration mode = MIPMAP_GENERATE_COLOR, float threshold = 0.5f) {}

		ALIGNED_NEW_DELETE("Engine::Texture")
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOH

#endif