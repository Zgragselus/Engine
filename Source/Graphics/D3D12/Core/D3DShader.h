#ifndef __D3D__SHADER__H__
#define __D3D__SHADER__H__

#include <d3d12.h>
#include <d3dcompiler.h>
#include <string>
#include <map>
#include <vector>

namespace Engine
{
	class D3DShader
	{
	public:
		enum ShaderType
		{
			COMPUTE_SHADER = 0,
			DOMAIN_SHADER,
			HULL_SHADER,
			GEOMETRY_SHADER,
			PIXEL_SHADER,
			VERTEX_SHADER
		};

		static std::string ShaderTypeTarget[];

		struct ShaderEntryPoint
		{
			enum ShaderType type;
			std::string entryPoint;

			ShaderEntryPoint(ShaderType t, const std::string& ep)
			{
				type = t;
				entryPoint = ep;
			}
		};

		struct ShaderDefine
		{
			std::string name;
			std::string value;

			ShaderDefine(const std::string& defineName, const std::string& defineValue)
			{
				name = defineName;
				value = defineValue;
			}
		};
				
	private:
		std::map<ShaderType, ID3DBlob*> mShaders;

	public:
		D3DShader(const std::string& filename, std::vector<ShaderEntryPoint>& params, std::vector<ShaderDefine>& defines = std::vector<ShaderDefine>(), unsigned int compilerFlags = 0);

		~D3DShader();

		inline void* GetBytecodePtr(ShaderType type)
		{
			if (mShaders.find(type) != mShaders.end())
			{
				return mShaders[type]->GetBufferPointer();
			}
			else
			{
				return NULL;
			}
		}

		inline size_t GetBytecodeLength(ShaderType type)
		{
			if (mShaders.find(type) != mShaders.end())
			{
				return mShaders[type]->GetBufferSize();
			}
			else
			{
				return NULL;
			}
		}
	};
}

#endif