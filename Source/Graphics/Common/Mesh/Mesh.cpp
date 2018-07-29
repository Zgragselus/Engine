///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Mesh.cpp
//
// Following file contains class defining basic mesh object
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

#include "Mesh.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Definition section

using namespace Engine;

/// <summary>Mesh constructor</summary>
/// <param name="vboData">Mesh per-vertex data</param>
/// <param name="itemSize">Size of single per-vertex item</param>
/// <param name="itemCount">Number of vertices</param>
/// <param name="indices">Indices data</param>
/// <param name="indexCount">Number of indices</param>
Mesh::Mesh(D3DRenderer* renderer,
	void* vertices,
	size_t vertexSize,
	size_t vertexCount,
	unsigned int* indices,
	size_t indexCount)
{
	mVertexBuffer = new TypedBuffer(Graphics::R32F);
	mVertexBuffer->Init(renderer, vertexCount, vertexSize, vertices);

	mIndexBuffer = new TypedBuffer(Graphics::R32);
	mIndexBuffer->Init(renderer, indexCount, sizeof(unsigned int), indices);
}

/// <summary>Mesh destructor</summary>
Mesh::~Mesh()
{
	delete mVertexBuffer;
	delete mIndexBuffer;
}