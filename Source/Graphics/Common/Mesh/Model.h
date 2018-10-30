///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Model.h
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

#ifndef __MODEL__H__
#define __MODEL__H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "Mesh.h"
#include "Graphics/D3D12/Texture/Texture.h"
#include "Graphics/Common/Manager/Manager.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	/// <summary>
	/// Model class, is actually just a named list of meshes.
	/// </summary>
	class Model
	{
	private:
		/// <summary>Transformation matrices for meshes</summary>
		std::vector<float4*> mTransformations;

		/// <summary>List of meshes</summary>
		std::vector<Mesh*> mMeshes;

		/// <summary>List of textures per mesh</summary>
		std::vector<std::vector<Manager<Texture>::Node*> > mTextures;

		/// <summary>Name of model</summary>
		std::string mName;

		/// <summary>Mesh manager (nullptr means meshes are non-managed = deleted after removal of model)</summary>
		Manager<Mesh>* mMeshManager;

		/// <summary>Texture manager (nullptr means texture are non-managed = deleted after removal of model)</summary>
		Manager<Texture>* mTextureManager;

	public:
		/// <summary>Model constructor</summary>
		/// <param name="name">Name of the model</param>
		Model(const std::string& name);

		/// <summary>Model destructor</summary>
		virtual ~Model();

		/// <summary>Set mesh manager (for removing meshes)</summary>
		/// <param name="manager">Pointer to manager</param>
		void SetManagers(Manager<Mesh>* meshManager, Manager<Texture>* textureManager)
		{
			mMeshManager = meshManager;
			mTextureManager = textureManager;
		}

		/// <summary>Adds new mesh into model</summary>
		/// <param name="mesh">Pointer to mesh to add</param>
		void AddMesh(const float4& transform, Mesh* mesh, const std::vector<Manager<Texture>::Node*>& textures)
		{
			mTransformations.push_back(new float4(transform));
			mMeshes.push_back(mesh);
			mTextures.push_back(textures);
		}

		inline size_t GetMeshesCount() const
		{
			return mMeshes.size();
		}

		inline Mesh* GetMesh(size_t meshID)
		{
			return mMeshes[meshID];
		}

		inline Texture* GetTexture(size_t meshID, size_t slotID)
		{
			if (mTextures[meshID].size() <= slotID)
			{
				return nullptr;
			}
			else
			{
				return mTextures[meshID][slotID]->mItem;
			}
		}

		inline const float4& GetTransformation(size_t meshID) const
		{
			return *(mTransformations[meshID]);
		}

		ALIGNED_NEW_DELETE("Engine::Graphics::Model")
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOH

#endif
