///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Texture.cpp
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

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "Texture.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Definition section

GLuint Engine::Texture::mOpenGLType[] =
{
	GL_TEXTURE_1D,
	GL_TEXTURE_2D,
	GL_TEXTURE_3D,
	GL_TEXTURE_CUBE_MAP,
	GL_TEXTURE_2D_ARRAY,
	GL_TEXTURE_2D_MULTISAMPLE
};

GLuint Engine::Texture::mOpenGLInternalFormat[] =
{
	GL_RGBA8,

	// 8-bit integer formats
	GL_R8,
	GL_RG8,
	GL_RGB8,
	GL_RGBA8,

	// 16-bit floating-point formats
	GL_R16F,
	GL_RG16F,
	GL_RGB16F,
	GL_RGBA16F,

	// 32-bit floating-point formats
	GL_R32F,
	GL_RG32F,
	GL_RGB32F,
	GL_RGBA32F,

	// Block-compression formats
	GL_COMPRESSED_RGB_S3TC_DXT1_EXT,
	GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
	GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
	GL_COMPRESSED_RED_RGTC1,
	GL_COMPRESSED_RG_RGTC2,

	// Depth formats
	GL_DEPTH_COMPONENT32,

	// Depth-stencil format
	GL_DEPTH24_STENCIL8
};

GLuint Engine::Texture::mOpenGLFormat[] =
{
	GL_UNSIGNED_BYTE,

	// 8-bit integer formats
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,

	// 16-bit floating-point formats
	GL_HALF_FLOAT,
	GL_HALF_FLOAT,
	GL_HALF_FLOAT,
	GL_HALF_FLOAT,

	// 32-bit floating-point formats
	GL_FLOAT,
	GL_FLOAT,
	GL_FLOAT,
	GL_FLOAT,

	// Block-compression formats
	GL_COMPRESSED_RGB_S3TC_DXT1_EXT,
	GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
	GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
	GL_COMPRESSED_RED_RGTC1,
	GL_COMPRESSED_RG_RGTC2,

	// Depth formats
	GL_UNSIGNED_BYTE,

	// Depth-stencil format
	GL_UNSIGNED_BYTE
};

bool Engine::Texture::mOpenGLCompression[] =
{
	false,

	// 8-bit integer formats
	false,
	false,
	false,
	false,

	// 16-bit floating-point formats
	false,
	false,
	false,
	false,

	// 32-bit floating-point formats
	false,
	false,
	false,
	false,

	// Block-compression formats
	true,
	true,
	true,
	true,
	true,

	// Depth formats
	false,

	// Depth-stencil format
	false
};

unsigned int Engine::Texture::mOpenGLChannelCount[] =
{
	4,

	// 8-bit integer formats
	1,
	2,
	3,
	4,

	// 16-bit floating-point formats
	1,
	2,
	3,
	4,

	// 32-bit floating-point formats
	1,
	2,
	3,
	4,

	// Block-compression formats
	3,
	4,
	4,
	1,
	2,

	// Depth formats
	1,

	// Depth-stencil format
	1
};

GLuint Engine::Texture::mOpenGLChannels[] =
{
	GL_RGBA,

	// 8-bit integer formats
	GL_RED,
	GL_RG,
	GL_RGB,
	GL_RGBA,

	// 16-bit floating-point formats
	GL_RED,
	GL_RG,
	GL_RGB,
	GL_RGBA,

	// 32-bit floating-point formats
	GL_RED,
	GL_RG,
	GL_RGB,
	GL_RGBA,

	// Block-compression formats
	GL_RGB,
	GL_RGBA,
	GL_RGBA,
	GL_RED,
	GL_RG,

	// Depth formats
	GL_DEPTH_COMPONENT,

	// Depth-stencil format
	GL_DEPTH_STENCIL
};

GLuint Engine::Texture::mOpenGLWrap[] =
{
	GL_REPEAT,
	GL_CLAMP,
	GL_CLAMP_TO_EDGE,
	GL_MIRRORED_REPEAT,
	GL_CLAMP,
	GL_CLAMP_TO_EDGE
};

GLuint Engine::Texture::mOpenGLMagnificationFilter[] =
{
	GL_NEAREST,
	GL_LINEAR,
	GL_LINEAR,
	GL_LINEAR
};

GLuint Engine::Texture::mOpenGLMinificationFilter[] =
{
	GL_NEAREST,
	GL_LINEAR,
	GL_LINEAR_MIPMAP_LINEAR,
	GL_LINEAR_MIPMAP_LINEAR
};