///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Material.h
//
// Material is class type that stands between the shader and mesh. For each mesh, there is a 
// material along with parameters passed into it by the mesh.
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __MATERIAL_H__
#define __MATERIAL_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "Graphics/Resources/Shader/Shader.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	/// <summary>
	/// Material class stands in between the shader and actual geometry that is rendered. Its main 
	/// purpose is to set up all the uniforms during the rendering.
	/// </summary>
	class Material
	{
	public:
		/// <summary>Uniform data types</summary>
		enum UniformParamType
		{
			UNIFORM_PARAM_INT,
			UNIFORM_PARAM_INT2,
			UNIFORM_PARAM_INT3,
			UNIFORM_PARAM_INT4,
			UNIFORM_PARAM_FLOAT,
			UNIFORM_PARAM_FLOAT2,
			UNIFORM_PARAM_FLOAT3,
			UNIFORM_PARAM_FLOAT4,
			UNIFORM_PARAM_MAT2X2,
			UNIFORM_PARAM_MAT3X3,
			UNIFORM_PARAM_MAT4X4,
			UNIFORM_PARAM_TEXTURE
		};

		/// <summary>Structure for holding the definition of single uniform parameter</summary>
		struct UniformParam
		{
			std::string mName;
			UniformParamType mType;

			UniformParam(const std::string& name, UniformParamType type)
			{
				mName = name;
				mType = type;
			}
		};

	protected:
		/// <summary>An array containing uniform parameter names and types for shader</summary>
		std::vector<UniformParam> mParams;

		/// <summary>Variable indexes got from shader</summary>
		std::vector<int> mShaderVars;
		
		/// <summary>Material shader</summary>
		Shader* mShader;
	
	public:
		/// <summary>Mesh constructor</summary>
		/// <param name="vboData">Mesh per-vertex data</param>
		Material(Shader* shader)
		{
			mShader = shader;
		}

		/// <summary>Mesh destructor</summary>
		virtual ~Material()
		{
		}

		/// <summary>Add another uniform parameter</summary>
		/// <param name="param">Uniform parameter name-type pair</param>
		void SetUniform(const UniformParam& param)
		{
			mParams.push_back(param);
		}

		/// <summary>Add another uniform parameter</summary>
		/// <param name="name">Uniform parameter name</param>
		/// <param name="texel">Texture unit id</param>
		void SetUniform(const std::string& name, UniformParamType type)
		{
			mParams.push_back(UniformParam(name, type));
		}

		/// <summary>Once material is set up - obtain all shader uniforms</summary>
		void BuildUniformIndex()
		{
			for (unsigned int i = 0; i < mParams.size(); i++)
			{
				mShaderVars.push_back(mShader->GetVariable(mParams[i].mName));
			}
		}

		/// <summary>Attach a material, as a parameter we pass pointer to structure holding data for given mesh we render</summary>
		/// <param name="params>Pointer to data used for setting up uniforms</param>
		void Attach(void* params)
		{
			char* ptr = (char*)params;

			mShader->TurnOn();

			for (unsigned int i = 0; i < mParams.size(); i++)
			{
				switch (mParams[i].mType)
				{
				case UNIFORM_PARAM_TEXTURE:
				case UNIFORM_PARAM_INT:
					mShader->SetInt(mShaderVars[i], *(int*)ptr);
					ptr += sizeof(int);
					break;

				case UNIFORM_PARAM_INT2:
					mShader->SetInt2(mShaderVars[i], *((int*)ptr), *((int*)ptr + 1));
					ptr += sizeof(int) * 2;
					break;

				case UNIFORM_PARAM_INT3:
					mShader->SetInt3(mShaderVars[i], *((int*)ptr), *((int*)ptr + 1), *((int*)ptr + 2));
					ptr += sizeof(int) * 3;
					break;

				case UNIFORM_PARAM_INT4:
					mShader->SetInt4(mShaderVars[i], *((int*)ptr), *((int*)ptr + 1), *((int*)ptr + 2), *((int*)ptr + 3));
					ptr += sizeof(int) * 4;
					break;

				case UNIFORM_PARAM_FLOAT:
					mShader->SetFloat(mShaderVars[i], *(float*)ptr);
					ptr += sizeof(float);
					break;

				case UNIFORM_PARAM_FLOAT2:
					mShader->SetFloat2(mShaderVars[i], *((float*)ptr), *((float*)ptr + 1));
					ptr += sizeof(float) * 2;
					break;

				case UNIFORM_PARAM_FLOAT3:
					mShader->SetFloat3(mShaderVars[i], *((float*)ptr), *((float*)ptr + 1), *((float*)ptr + 2));
					ptr += sizeof(float) * 3;
					break;

				case UNIFORM_PARAM_FLOAT4:
					mShader->SetFloat4(mShaderVars[i], *((float*)ptr), *((float*)ptr + 1), *((float*)ptr + 2), *((float*)ptr + 3));
					ptr += sizeof(float) * 4;
					break;

				case UNIFORM_PARAM_MAT2X2:
					mShader->SetMat2(mShaderVars[i], 1, false, (float*)ptr);
					ptr += sizeof(float) * 4;
					break;

				case UNIFORM_PARAM_MAT3X3:
					mShader->SetMat3(mShaderVars[i], 1, false, (float*)ptr);
					ptr += sizeof(float) * 9;
					break;

				case UNIFORM_PARAM_MAT4X4:
					mShader->SetMat4(mShaderVars[i], 1, false, (float*)ptr);
					ptr += sizeof(float) * 16;
					break;
				}
			}
		}

		/// <summary>Detach shader</summary>
		void Detach()
		{
			mShader->TurnOff();
		}
		
		ALIGNED_NEW_DELETE("Engine::Graphics::Material")
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOH

#endif
