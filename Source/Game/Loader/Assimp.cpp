#include "Assimp.h"

using namespace Engine;

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
Mesh* LoaderAssimp::InitMesh(const aiMesh* mesh, unsigned int* materialId, float4& translate, std::string& name)
{
	size_t VERTEX_SIZE = 16;
	unsigned int mNumVertices = mesh->mNumVertices;
	float* mVertexData = new float[mNumVertices * VERTEX_SIZE];

	name = std::string(mesh->mName.C_Str());

	*materialId = mesh->mMaterialIndex;

	aiVector3D zero;
	zero.x = 0.0f;
	zero.y = 0.0f;
	zero.z = 0.0f;

	AABB bounds;

	for (unsigned int i = 0; i < mNumVertices; i++)
	{
		aiVector3D* vertex = &(mesh->mVertices[i]);
		aiVector3D* normal = mesh->HasNormals() ? &(mesh->mNormals[i]) : &zero;
		aiVector3D* tangent = mesh->HasTangentsAndBitangents() ? &(mesh->mTangents[i]) : &zero;
		aiVector3D* bitangent = mesh->HasTangentsAndBitangents() ? &(mesh->mBitangents[i]) : &zero;
		aiVector3D* texCoord0 = &mesh->mTextureCoords[0][i];
		aiVector3D* texCoord1 = &mesh->mTextureCoords[1][i];

		mVertexData[i * VERTEX_SIZE + 0] = vertex->x;
		mVertexData[i * VERTEX_SIZE + 1] = vertex->y;
		mVertexData[i * VERTEX_SIZE + 2] = vertex->z;
		mVertexData[i * VERTEX_SIZE + 3] = normal->x;
		mVertexData[i * VERTEX_SIZE + 4] = normal->y;
		mVertexData[i * VERTEX_SIZE + 5] = normal->z;
		mVertexData[i * VERTEX_SIZE + 6] = tangent->x;
		mVertexData[i * VERTEX_SIZE + 7] = tangent->y;
		mVertexData[i * VERTEX_SIZE + 8] = tangent->z;
		mVertexData[i * VERTEX_SIZE + 9] = bitangent->x;
		mVertexData[i * VERTEX_SIZE + 10] = bitangent->y;
		mVertexData[i * VERTEX_SIZE + 11] = bitangent->z;

		bounds.Expand(float4(vertex->x, vertex->y, vertex->z, 1.0f));

		if (mesh->HasTextureCoords(0))
		{
			mVertexData[i * VERTEX_SIZE + 12] = texCoord0->x;
			mVertexData[i * VERTEX_SIZE + 13] = texCoord0->y;
		}
		else
		{
			mVertexData[i * VERTEX_SIZE + 12] = 0.0f;
			mVertexData[i * VERTEX_SIZE + 13] = 0.0f;
		}

		if (mesh->HasTextureCoords(1))
		{
			mVertexData[i * VERTEX_SIZE + 14] = texCoord1->x;
			mVertexData[i * VERTEX_SIZE + 15] = texCoord1->y;
		}
		else
		{
			mVertexData[i * VERTEX_SIZE + 14] = 0.0f;
			mVertexData[i * VERTEX_SIZE + 15] = 0.0f;
		}
	}

	float4 centroid = (bounds.mMin + bounds.mMax) * 0.5f;
	translate = centroid;

	for (unsigned int i = 0; i < mNumVertices; i++)
	{
		mVertexData[i * VERTEX_SIZE + 0] -= centroid.x;
		mVertexData[i * VERTEX_SIZE + 1] -= centroid.y;
		mVertexData[i * VERTEX_SIZE + 2] -= centroid.z;
	}

	bounds = AABB();

	for (unsigned int i = 0; i < mNumVertices; i++)
	{
		bounds.Expand(float4(mVertexData[i * VERTEX_SIZE + 0], mVertexData[i * VERTEX_SIZE + 1], mVertexData[i * VERTEX_SIZE + 2], 1.0f));
	}

	unsigned int mNumIndices = mesh->mNumFaces * 3;
	unsigned int* mIndices = new unsigned int[mNumIndices];

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		const aiFace* f = &(mesh->mFaces[i]);
		mIndices[i * 3 + 0] = f->mIndices[0];
		mIndices[i * 3 + 1] = f->mIndices[1];
		mIndices[i * 3 + 2] = f->mIndices[2];
	}

	Mesh* m = new Mesh(mRenderer, mVertexData, sizeof(float) * 16, mNumVertices, mIndices, mNumIndices);
	m->Bounds() = bounds;

	mLog->Print("LoaderAssimp", "Initialized mesh with " + std::to_string(mNumVertices) + " vertices and " + std::to_string(mNumIndices) + " indexes");
	
	delete[] mIndices;
	delete[] mVertexData;

	return m;
}

void LoaderAssimp::InitMaterial(const LoaderDevIL& loader, const std::string& path, const aiMaterial* mat)
{
	if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0)
	{
		aiString assimpPath;
		mat->GetTexture(aiTextureType_DIFFUSE, 0, &assimpPath);
		std::string file = path + assimpPath.data;
		LoaderDevIL::Image* image = loader.Load(file);
		if (image != nullptr)
		{
			loader.GenerateMipmaps(image);
			mMaterialsTmp[mMaterialsTmp.size() - 5] = image;
		}
	}

	if (mat->GetTextureCount(aiTextureType_NORMALS) > 0)
	{
		aiString assimpPath;
		mat->GetTexture(aiTextureType_NORMALS, 0, &assimpPath);
		std::string file = path + assimpPath.data;
		LoaderDevIL::Image* image = loader.Load(file);
		if (image != nullptr)
		{
			image->mUseAlphaMipmap = false;
			loader.GenerateMipmapsNormal(image);
			mMaterialsTmp[mMaterialsTmp.size() - 4] = image;
		}
	}

	if (mat->GetTextureCount(aiTextureType_AMBIENT) > 0)
	{
		aiString assimpPath;
		mat->GetTexture(aiTextureType_AMBIENT, 0, &assimpPath);
		std::string file = path + assimpPath.data;
		LoaderDevIL::Image* image = loader.Load(file);
		if (image != nullptr)
		{
			image->mUseAlphaMipmap = false;
			loader.GenerateMipmaps(image);
			mMaterialsTmp[mMaterialsTmp.size() - 3] = image;
		}
	}

	if (mat->GetTextureCount(aiTextureType_SPECULAR) > 0)
	{
		aiString assimpPath;
		mat->GetTexture(aiTextureType_SPECULAR, 0, &assimpPath);
		std::string file = path + assimpPath.data;
		LoaderDevIL::Image* image = loader.Load(file);
		if (image != nullptr)
		{
			image->mUseAlphaMipmap = false;
			loader.GenerateMipmaps(image);
			mMaterialsTmp[mMaterialsTmp.size() - 2] = image;
		}
	}
	
	if (mat->GetTextureCount(aiTextureType_HEIGHT) > 0)
	{
		aiString assimpPath;
		mat->GetTexture(aiTextureType_HEIGHT, 0, &assimpPath);
		std::string file = path + assimpPath.data;
		LoaderDevIL::Image* image = loader.Load(file);
		if (image != nullptr)
		{
			image->mUseAlphaMipmap = false;
			loader.GenerateMipmaps(image);
			mMaterialsTmp[mMaterialsTmp.size() - 1] = image;
		}
	}
}

void LoaderAssimp::InitializeTexture(Engine::LoaderDevIL::Image* image, const std::string& imageName, std::vector<Engine::Texture*>& textureList)
{
	if (image == nullptr)
	{
		if (mTextureManager)
		{
			int* refs = mTextureManager->GetReferences("Default");
			Texture* t = mTextureManager->Create<Texture>("Default");
			t->SetName("Default");

			if (refs == nullptr)
			{
				size_t width = 8;
				size_t height = 8;
				unsigned char buffer[8 * 8 * 4];
				memset(buffer, 0, sizeof(buffer));
				t->Init(mRenderer, width, height, 1, Engine::Graphics::RGBA8, (const void*)buffer);
			}

			textureList.push_back(t);
		}
		else
		{
			textureList.push_back(nullptr);
		}
	}
	else
	{
		Texture* t = nullptr;
		std::string name = image->mName;
		std::replace(name.begin(), name.end(), '\\', '/');

		if (mTextureManager)
		{
			t = mTextureManager->Create<Texture>(name);
		}
		else
		{
			t = new Texture();
		}

		LoaderDevIL::Image* img = image;
		t->InitMipmaps(mRenderer, img->mWidth, img->mHeight, 1, img->mMipLevels, Engine::Graphics::RGBA8, (const void**)img->mMipmaps);
		t->SetName(name);
		textureList.push_back(t);

		delete img;
	}
}

Entity* LoaderAssimp::LoadFromScene(const std::string& path, const std::string& filename, const aiScene* scene, unsigned int flags)
{
	int materials = scene->mNumMaterials;

	LoaderDevIL devIL = LoaderDevIL(mLog);

	std::vector<Texture*> textures;

	for (int i = 0; i < materials; i++)
	{
		mMaterialsTmp.push_back(nullptr);
		mMaterialsTmp.push_back(nullptr);
		mMaterialsTmp.push_back(nullptr);
		mMaterialsTmp.push_back(nullptr);
		mMaterialsTmp.push_back(nullptr);
		InitMaterial(devIL, path, scene->mMaterials[i]);

		aiString name;
		scene->mMaterials[i]->Get(AI_MATKEY_NAME, name);

		std::string nameTmp = std::string(name.C_Str());

		InitializeTexture(mMaterialsTmp[mMaterialsTmp.size() - 5], std::string(name.C_Str()) + "_diffuse", textures);
		InitializeTexture(mMaterialsTmp[mMaterialsTmp.size() - 4], std::string(name.C_Str()) + "_normals", textures);
		InitializeTexture(mMaterialsTmp[mMaterialsTmp.size() - 3], std::string(name.C_Str()) + "_metallic", textures);
		InitializeTexture(mMaterialsTmp[mMaterialsTmp.size() - 2], std::string(name.C_Str()) + "_roughness", textures);
		InitializeTexture(mMaterialsTmp[mMaterialsTmp.size() - 1], std::string(name.C_Str()) + "_height", textures);
	}

	int meshes = scene->mNumMeshes;

	std::string modelName = String::Filename(filename);

	Entity* ent = new Entity(modelName);

	for (int i = 0; i < meshes; i++)
	{
		unsigned int matID;
		float4 translate = mat4();
		std::string name;
		Mesh* m = InitMesh(scene->mMeshes[i], &matID, translate, name);

		name = modelName + "_" + name;

		if (mMeshManager && name.length() > 0)
		{
			// The current problem is, when there are meshes with the same name. 
			//
			// Search the manager if there is a mesh of same name, but different one (this assumes 
			// different vertex count or indices count
			if ((flags & (unsigned int)Flags::LOAD_INSTANCES) > 0)
			{
				Manager<Mesh>::Node* stored = mMeshManager->GetNode(name);
				unsigned int temp = 0;
				while (stored && (stored->Get()->GetIndexBuffer()->NumElements() != m->GetIndexBuffer()->NumElements() ||
								  stored->Get()->GetVertexBuffer()->NumElements() != m->GetVertexBuffer()->NumElements()))
				{
					mLog->Print("LoaderAssimp", std::string("Warning: Mesh name '") + name + std::string("' is not unique!"));
					name = name + std::to_string(temp);
					stored = mMeshManager->GetNode(name);
				}
			}
			else
			{
				Manager<Mesh>::Node* stored = mMeshManager->GetNode(name);
				unsigned int temp = 0;
				while (stored)
				{
					mLog->Print("LoaderAssimp", std::string("Warning: Mesh name '") + name + std::string("' is not unique!"));
					name = name + std::to_string(temp);
					stored = mMeshManager->GetNode(name);
				}
			}

			m = mMeshManager->Insert<Mesh>(name, m);
		}

		mLog->Print("LoaderAssimp", name);
		Entity* child = new Entity(name, ent);
		child->GameObject().Add<Engine::MeshComponent>(m, mMeshManager);
		//child->GameObject().Add<Engine::TextureComponent>(textures[matID * 5 + 4]);
		child->GameObject().Add<Engine::MaterialComponent>(textures[matID * 5], textures[matID * 5 + 1], textures[matID * 5 + 4], textures[matID * 5 + 3], textures[matID * 5 + 2], mTextureManager);
		//child->Transformation().Extract(transpose(trans));
		//child->Transformation().Update();
		child->Transformation().SetTranslation(translate);
	}

	mLog->Print("LoaderAssimp", "Model initialized!");

	mMaterialsTmp.clear();

	return ent;
}

Entity* LoaderAssimp::Load(const std::string& filename, unsigned int flags)
{
	if (!Files::Exists(filename))
	{
		mLog->Print("LoaderAssimp", std::string("File ") + filename + std::string(" does not exist!"));
		return NULL;
	}

	mLog->Print("LoaderAssimp", std::string("Importing file: ") + filename);

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filename.c_str(),
		aiProcess_Triangulate |
		aiProcess_GenNormals |
		aiProcess_JoinIdenticalVertices |
		aiProcess_CalcTangentSpace |
		aiProcess_LimitBoneWeights |
		aiProcess_ImproveCacheLocality |
		aiProcess_FlipUVs);

	if (scene != NULL)
	{
		std::string path = Files::GetFolder(filename);
		Entity* ent = LoadFromScene(path, filename, scene, flags);
		return ent;
	}
	else
	{
		printf("%s\n", importer.GetErrorString());
		mLog->Print("LoaderAssimp", std::string("Loading of file ") + filename + std::string(" failed!"));
		return NULL;
	}
}