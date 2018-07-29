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

/// <summary>Attribute type size - sizeof of each single attribute</summary>
size_t Mesh::Attribute::mTypeSize[2] =
{
	sizeof(float),
	sizeof(int)
};

/// <summary>Attribute type (GLuint)</summary>
GLuint Mesh::Attribute::mTypeGL[2] =
{
	GL_FLOAT,
	GL_INT
};

/// <summary>Mesh constructor</summary>
/// <param name="vboData">Mesh per-vertex data</param>
/// <param name="itemSize">Size of single per-vertex item</param>
/// <param name="itemCount">Number of vertices</param>
/// <param name="indices">Indices data</param>
/// <param name="indexCount">Number of indices</param>
/// <param name="attributeSet">Attribute description (VBO layout)</param>
Mesh::Mesh(void* vboData,
	size_t itemSize,
	size_t itemCount,
	unsigned int* indices,
	size_t indexCount,
	std::vector<Attribute> attributeSet)
{
	// Sort attribute description by channel
	std::sort(attributeSet.begin(), attributeSet.end(), AttributeLess());

	mVertexBuffer = new VertexBuffer(vboData, itemSize, itemCount, GL_STATIC_DRAW);
	mIndexBuffer = new IndexBuffer(indices, sizeof(unsigned int), indexCount, GL_STATIC_DRAW);
	mVertexArray = new VertexArray();

	mVertexArray->Enable();
	mIndexBuffer->Enable();
	mVertexBuffer->Enable();
	
	// Set up VAO layout
	size_t offset = 0;
	for (std::vector<Attribute>::iterator it = attributeSet.begin();
		it != attributeSet.end();
		it++)
	{
		mVertexArray->EnableAttribute(it->mChannelID);
		mVertexArray->SetAttribute(it->mChannelID,
			it->mDimensions,
			Attribute::mTypeGL[it->mType],
			GL_FALSE,
			itemSize,
			(void*)offset);
		offset += Attribute::mTypeSize[it->mType] * it->mDimensions;
	}

	mVertexArray->Disable();
}

/// <summary>Mesh destructor</summary>
Mesh::~Mesh()
{
	delete mVertexArray;
	delete mVertexBuffer;
	delete mIndexBuffer;
}