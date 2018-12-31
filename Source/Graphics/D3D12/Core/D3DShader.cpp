#include "D3DShader.h"

using namespace Engine;

std::string D3DShader::ShaderTypeTarget[] =
{
	"cs_5_1",
	"ds_5_1",
	"hs_5_1",
	"gs_5_1",
	"ps_5_1",
	"vs_5_1"
};

D3DShader::D3DShader(const std::string& filename, const std::vector<ShaderEntryPoint>& params, const std::vector<ShaderDefine>& defines, unsigned int compilerFlags)
{
	std::wstring wFilename = std::wstring(filename.begin(), filename.end());

	D3D_SHADER_MACRO* shaderDefs = nullptr;

	if (defines.size() > 0)
	{
		shaderDefs = new D3D_SHADER_MACRO[defines.size() + 1];

		for (unsigned int i = 0; i < defines.size(); i++)
		{
			shaderDefs[i].Name = defines[i].name.c_str();
			shaderDefs[i].Definition = defines[i].value.c_str();
		}

		shaderDefs[defines.size()].Name = nullptr;
		shaderDefs[defines.size()].Definition = nullptr;
	}

	for (auto it = params.begin(); it != params.end(); it++)
	{
		ID3DBlob* blob;
		ID3DBlob* error;

		D3DCompileFromFile(wFilename.c_str(), shaderDefs, D3D_COMPILE_STANDARD_FILE_INCLUDE, it->entryPoint.c_str(), ShaderTypeTarget[it->type].c_str(), compilerFlags, 0, &blob, &error);

		if (error == NULL)
		{
			mShaders.insert(std::pair<ShaderType, ID3DBlob*>(it->type, blob));
		}
		else
		{
			OutputDebugString((LPCTSTR)error->GetBufferPointer()); 
			error->Release();
		}
	}
}

D3DShader::~D3DShader()
{
	for (auto it = mShaders.begin(); it != mShaders.end(); it++)
	{
		it->second->Release();
	}
}