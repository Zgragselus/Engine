#ifndef __ASSIMP__H__
#define __ASSIMP__H__

#include "../Resources/Model/Model.h"

#pragma comment(lib, "assimp-vc140-mt.lib")

namespace Engine
{
	class LoaderAssimp
	{
	private:
		Log* mLog;

	public:
		LoaderAssimp(Log* l)
		{
			mLog = l;
		}

		Model* Load(const std::string& filename);
	};
}

#endif