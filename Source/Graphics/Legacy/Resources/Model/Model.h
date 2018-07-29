///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Model.h
//
// Model class stands as a container for material-mesh connection; Apart from that it also connects
// all parameters passed into material, as textures.
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

#include "Core/Math/Numeric/Mat4.h"
#include "../Material/Material.h"
#include "../Texture/Texture.h"
#include "../Mesh/Mesh.h"
#include "../Texture/TextureUnitState.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	/// <summary>
	/// Material class stands in between the shader and actual geometry that is rendered. Its main 
	/// purpose is to set up all the uniforms during the rendering.
	/// </summary>
	class Model
	{
	protected:
		std::vector<Mesh*> mMeshes;
		std::vector<Material*> mMaterials;
		std::vector<void*> mMaterialsParams;
		std::vector<TextureUnitState> mTextureState;

	public:
		/// <summary>Model blank constructor</summary>
		Model()
		{

		}

		/// <summary>Destructor</summary>
		~Model()
		{
			for (void* ptr : mMaterialsParams)
			{
				if (ptr)
				{
					SharedFree(ptr, "Engine::Model");
				}
			}
		}

		/// <summary>Add mesh-material-texture triplet</summary>
		void Add(Mesh* mesh, Material* mat, void* param, size_t param_size, TextureUnitState state)
		{
			mMeshes.push_back(mesh);

			mMaterials.push_back(mat);
			
			void* param_copy = SharedMalloc(param_size, 16, "Engine::Model");
			memcpy(param_copy, param, param_size);
			mMaterialsParams.push_back(param_copy);
			
			mTextureState.push_back(state);
		}

		/// <summary>Render model</summary>
		void Render(const mat4& world, const mat4& view, const mat4& projection)
		{
			for (unsigned int i = 0; i < mMeshes.size(); i++)
			{
				mat4* ptr = (mat4*)(mMaterialsParams[i]);
				ptr[0] = world;
				ptr[1] = view;
				ptr[2] = projection;

				mTextureState[i].Enable();
				mMaterials[i]->Attach(mMaterialsParams[i]);
				mMeshes[i]->GetVAO()->Enable();
				mMeshes[i]->GetVBO()->DrawElements(GL_TRIANGLES, mMeshes[i]->GetIBO()->Count());
				mMeshes[i]->GetVAO()->Disable();
			}
		}

		ALIGNED_NEW_DELETE("Engine::Graphics::Model")
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOH

#endif
