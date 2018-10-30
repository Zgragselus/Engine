///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Model.cpp
//
// Model is an array of pointers to meshes, it has associated name
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

#include "Model.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Definition section

using namespace Engine;

/// <summary>Model constructor</summary>
/// <param name="vboData">Mesh per-vertex data</param>
Model::Model(const std::string& name) : mName(name)
{
	mMeshManager = nullptr;
	mTextureManager = nullptr;
}

/// <summary>Model destructor</summary>
Model::~Model()
{
	// Release transform matrices
	for (int i = 0; i < mTransformations.size(); i++)
	{
		delete mTransformations[i];
	}
	mTransformations.clear();

	// If textures are managed, use manager to take care of their removal, otherwise just free memory
	// used by them
	if (mTextureManager)
	{
		for (std::vector<Manager<Texture>::Node*>& texArray : mTextures)
		{
			for (Manager<Texture>::Node* t : texArray)
			{
				mTextureManager->Remove(t->mItem);
			}
		}
	}
	else
	{
		for (std::vector<Manager<Texture>::Node*>& texArray : mTextures)
		{
			for (Manager<Texture>::Node* t : texArray)
			{
				delete t;
			}
		}
	}

	// Clear list
	mTextures.clear();

	// If meshes are managed, use manager to take care of their removal otherwise just free memory
	// used by them
	if (mMeshManager)
	{
		for (Mesh* m : mMeshes)
		{
			mMeshManager->Remove(m);
		}
	}
	else
	{
		for (Mesh* m : mMeshes)
		{
			delete m;
		}
	}

	// Clear list
	mMeshes.clear();
}