#ifndef __ASSIMP__H__
#define __ASSIMP__H__

#pragma comment(lib, "assimp-vc140-mt.lib")

#include "Core/Log/Log.h"
#include "Core/Files/Files.h"
#include "Graphics/Common/Mesh/Model.h"
#include "../Scene/Scene.h"
#include "DevIL.h"
#include <Assimp/Importer.hpp>
#include <Assimp/scene.h>
#include <Assimp/postprocess.h>

namespace Engine
{
	class LoaderAssimp
	{
	public:
		enum class Flags : unsigned int
		{
			LOAD_INSTANCES = 1
		};

	private:
		Log* mLog;
		D3DRenderer* mRenderer;
		std::vector<LoaderDevIL::Image*> mMaterialsTmp;
		std::vector<LoaderDevIL::Image*> mNormalsTmp;
		
		Manager<Model>* mModelManager;
		Manager<Mesh>* mMeshManager;
		Manager<Texture>* mTextureManager;
		DescriptorHeap* mTextureHeap;

		/// <summary>
		/// Initialize single mesh from assimp-compatible file.
		///
		/// Geometry is re-centered so that the center is at the center of the created mesh (based 
		/// AABB).
		/// </summary>
		/// <param name="mesh">Pointer to Assimp mesh</param>
		/// <param name="materialId">Pointer to material ID used by mesh</param>
		/// <param name="translate">Translation (due to re-pivoting mesh)</param>
		/// <param name="name">Name of the mesh</param>
		Mesh* InitMesh(const aiMesh* mesh, unsigned int* materialId, float4& translate, std::string& name);

		void InitMaterial(const LoaderDevIL& loader, const std::string& path, const aiMaterial* mat);
		Model* LoadFromScene(const std::string& path, const std::string& filename, const aiScene* scene, unsigned int flags);
		void InitializeTexture(Engine::LoaderDevIL::Image* image, const std::string& imageName, std::vector<Engine::Texture*>& textureList);

	public:
		LoaderAssimp(Log* l, D3DRenderer* r)
		{
			mLog = l;
			mRenderer = r;

			mMeshManager = nullptr;
			mModelManager = nullptr;
			mTextureManager = nullptr;
		}

		void SetManagers(Manager<Mesh>* meshManager, Manager<Model>* modelManager, Manager<Texture>* textureManager)
		{
			mMeshManager = meshManager;
			mModelManager = modelManager;
			mTextureManager = textureManager;
		}

		void SetTextureHeap(DescriptorHeap* textureHeap)
		{
			mTextureHeap = textureHeap;
		}

		Model* Load(const std::string& filename, unsigned int flags = 0);

		ALIGNED_NEW_DELETE("Game::Loader::Assimp")
	};
}

#endif