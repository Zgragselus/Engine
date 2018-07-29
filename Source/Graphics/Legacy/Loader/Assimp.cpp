#include "Assimp.h"
#include <Assimp/Importer.hpp>
#include <Assimp/scene.h>
#include <Assimp/postprocess.h>

using namespace Engine;

static Mesh* InitMesh(const aiMesh* mesh)
{
	size_t VERTEX_SIZE = 16;
	unsigned int mNumVertices = mesh->mNumVertices;
	float* mVertexData = new float[mNumVertices * VERTEX_SIZE];

	aiVector3D zero;
	zero.x = 0.0f;
	zero.y = 0.0f;
	zero.z = 0.0f;

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

	unsigned int mNumIndices = mesh->mNumFaces * 3;
	unsigned int* mIndices = new unsigned int[mNumIndices];

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		const aiFace* f = &(mesh->mFaces[i]);
		mIndices[i * 3 + 0] = f->mIndices[0];
		mIndices[i * 3 + 1] = f->mIndices[1];
		mIndices[i * 3 + 2] = f->mIndices[2];
	}

	std::vector<Mesh::Attribute> attributeSet;
	attributeSet.push_back(Mesh::Attribute(0, 3, Mesh::Attribute::FLOAT));
	attributeSet.push_back(Mesh::Attribute(1, 3, Mesh::Attribute::FLOAT));
	attributeSet.push_back(Mesh::Attribute(2, 3, Mesh::Attribute::FLOAT));
	attributeSet.push_back(Mesh::Attribute(3, 3, Mesh::Attribute::FLOAT));
	attributeSet.push_back(Mesh::Attribute(4, 2, Mesh::Attribute::FLOAT));
	attributeSet.push_back(Mesh::Attribute(5, 2, Mesh::Attribute::FLOAT));

	Mesh* m = new Mesh(mVertexData, sizeof(float) * VERTEX_SIZE, mNumVertices,
		mIndices, mNumIndices, attributeSet);

	delete[] mIndices;
	delete[] mVertexData;

	return m;
}

static Model* LoadFromScene(const aiScene* scene)
{
	int meshes = scene->mNumMeshes;

	Model* mdl = new Model();

	Engine::Shader* s = new Engine::Shader("../Data/Example05/Base.glsl");

	struct color_uniform
	{
		float world[16];
		float view[16];
		float projection[16];
	};

	color_uniform uniform_data = { 0.0f };

	Engine::Material* mat = new Engine::Material(s);
	mat->SetUniform("Sc_World", Engine::Material::UniformParamType::UNIFORM_PARAM_MAT4X4);
	mat->SetUniform("Sc_View", Engine::Material::UniformParamType::UNIFORM_PARAM_MAT4X4);
	mat->SetUniform("Sc_Projection", Engine::Material::UniformParamType::UNIFORM_PARAM_MAT4X4);
	mat->BuildUniformIndex();

	Engine::TextureUnitState state;

	for (int i = 0; i < meshes; i++)
	{
		Mesh* m = InitMesh(scene->mMeshes[i]);

		mdl->Add(m, mat, &uniform_data, sizeof(uniform_data), state);
	}

	return mdl;
}

Model* LoaderAssimp::Load(const std::string& filename)
{
	if (!Files::Exists(filename))
	{
		mLog->Print("LoaderAssimp", std::string("File ") + filename + std::string(" does not exist!"));
		return NULL;
	}

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filename.c_str(), 
		aiProcess_Triangulate | 
		aiProcess_GenNormals | 
		aiProcess_JoinIdenticalVertices | 
		aiProcess_CalcTangentSpace | 
		aiProcess_LimitBoneWeights | 
		aiProcess_ImproveCacheLocality);

	if (scene != NULL)
	{
		Model* mModel = LoadFromScene(scene);
		return mModel;
	}
	else
	{
		mLog->Print("LoaderAssimp", std::string("Loading of file ") + filename + std::string(" failed!"));
		return NULL;
	}
}