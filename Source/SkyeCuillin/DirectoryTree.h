#ifndef __DIRECTORY_TREE__H__
#define __DIRECTORY_TREE__H__

#include "Graphics/Graphics.h"
#include "Game/Game.h"
#include "ImguiSubsystem.h"

namespace SkyeCuillin
{
	class DirectoryTree
	{
	public:
		enum class ResourceType
		{
			TEXTURE,
			MODEL,
			NONE
		};

	private:
		enum class Type
		{
			DIRECTORY,
			FILE
		};

		std::vector<std::string> mChanges;		// Directories to update
		std::vector<std::string> mFiles;		// Files to reload
		Engine::Constants* mOptions;
		Engine::Log* mLog;
		Engine::D3DRenderer* mRenderer;

		Engine::Manager<Engine::Mesh>* mMeshManager;
		Engine::Manager<Engine::Model>* mModelManager;
		Engine::Manager<Engine::Texture>* mTextureManager;

		std::string mPath;						// Root directory tree path

		struct Record
		{
			Type mType;
			std::string mName;
			Record* mParent;
			ResourceType mResourceType;
			void* mResource;
			std::vector<Record*> mChildren;

			Record(Type type, const std::string& name)
			{
				mType = type;
				mName = name;
				mResourceType = ResourceType::NONE;
			}

			~Record()
			{
				for (auto it = mChildren.begin(); it != mChildren.end(); it++)
				{
					delete (*it);
				}
			}
		};

		Record* mRoot;

		Record* CreateSubtree(const std::string& directory);

		Record* GetSubtreeRoot(std::string& directory);

		void UpdateSubtree(std::string& directory);

		void _ImguiRenderRecord(Record* r);

		void ProcessFile(Record* r, const std::string& file);

	public:
		DirectoryTree(const std::string& directory, Engine::Constants* options, Engine::Log* log, Engine::D3DRenderer* renderer);

		~DirectoryTree();

		void Initialize();

		void SetManagers(Engine::Manager<Engine::Mesh>* meshManager, Engine::Manager<Engine::Model>* modelManager, Engine::Manager<Engine::Texture>* textureManager);

		void Imgui();

		void NotifyChange(std::string location);

		void ProcessChanges();
	};
}

#endif