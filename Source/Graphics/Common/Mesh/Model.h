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
		/// <summary>List of meshes</summary>
		std::vector<Mesh*> mMeshes;

		/// <summary>Name of model</summary>
		std::string mName;

		/// <summary>Mesh manager (nullptr means meshes are non-managed = deleted after removal of model)</summary>
		Manager<Mesh>* mMeshManager;

	public:
		/// <summary>Model constructor</summary>
		/// <param name="name">Name of the model</param>
		Model(const std::string& name);

		/// <summary>Model destructor</summary>
		virtual ~Model();

		/// <summary>Set mesh manager (for removing meshes)</summary>
		/// <param name="manager">Pointer to manager</param>
		void SetMeshManager(Manager<Mesh>* manager)
		{
			mMeshManager = manager;
		}

		/// <summary>Adds new mesh into model</summary>
		/// <param name="mesh">Pointer to mesh to add</param>
		void AddMesh(Mesh* mesh)
		{
			mMeshes.push_back(mesh);
		}

		ALIGNED_NEW_DELETE("Engine::Graphics::Model")
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOH

#endif
