#include "DirectoryTree.h"

using namespace SkyeCuillin;

DirectoryTree::Record* DirectoryTree::CreateSubtree(const std::string& directory)
{
	std::vector<std::string> list;

	Engine::Files::DirectoryRead(directory, list);

	Record* r = new Record(Type::DIRECTORY, directory.substr(directory.find_last_of('/') + 1));

	for (size_t i = 0; i < list.size(); i++)
	{
		if (list[i][list[i].length() - 1] == '/')
		{
			r->mChildren.push_back(CreateSubtree(directory + "/" + list[i].substr(0, list[i].length() - 1)));
		}
		else
		{
			printf("%s\n", (directory + "/" + list[i]).c_str());
			Record* rec = new Record(Type::FILE, list[i]);
			ProcessFile(rec, directory + "/" + list[i]);
			r->mChildren.push_back(rec);
		}
	}

	return r;
}

void DirectoryTree::ProcessFile(Record* r, const std::string& file)
{
	Engine::LoaderDevIL textureLoader(mLog);
	Engine::LoaderAssimp modelLoader(mLog, mRenderer);

	if (file.find(".tga") != std::string::npos)
	{
		Engine::Manager<Engine::Texture>::Node* node = mTextureManager->GetNode(file);

		Engine::LoaderDevIL::Image* image = textureLoader.Load(file);
		if (image != nullptr)
		{
			image->mUseAlphaMipmap = false;
			textureLoader.GenerateMipmaps(image);
		}
		Engine::Texture* t = new Engine::Texture(file);
		t->InitMipmaps(mRenderer, image->mWidth, image->mHeight, 1, image->mMipLevels, Engine::Graphics::RGBA8, (const void**)image->mMipmaps);
		t->SetName(file);
		if (node == nullptr)
		{
			mTextureManager->Insert<Engine::Texture>(file, t);
			node = mTextureManager->GetNode(file);
		}
		else
		{
			delete node->mItem;
			node->mItem = t;
		}
		delete image;

		r->mResourceType = ResourceType::TEXTURE;
		r->mResource = node;
	}
	else if (file.find(".obj") != std::string::npos)
	{
		Engine::Manager<Engine::Model>::Node* node = mModelManager->GetNode(file);

		/*Engine::Manager<Engine::Model>::Node* node = mModelManager->GetNode(file);
		if (node != nullptr)
		{
			r->mResourceType = ResourceType::MODEL;
			r->mResource = node;
		}*/
	}
}

DirectoryTree::Record* DirectoryTree::GetSubtreeRoot(std::string& directory)
{
	Record* r = mRoot;

	std::string::iterator itDir = directory.begin();
	std::string::iterator itNode = r->mName.begin();
	while (itDir != directory.end() && itNode != r->mName.end() && (*itDir) == (*itNode))
	{
		itDir++;
		itNode++;
	}

	std::vector<std::string> path = Engine::String::Split(directory.substr(itDir - directory.begin()), '/');
	for (size_t i = 0; i < path.size(); i++)
	{
		if (path[i].length() > 0)
		{
			for (size_t j = 0; j < r->mChildren.size(); j++)
			{
				if (r->mChildren[j]->mName == path[i])
				{
					r = r->mChildren[j];
					break;
				}
			}
		}
	}

	return r;
}

void DirectoryTree::UpdateSubtree(std::string& directory)
{
	std::vector<std::string> list;

	Engine::Files::DirectoryRead(directory, list);

	Record* r = GetSubtreeRoot(directory);

	std::set<std::string> nodes;
	for (Record* t : r->mChildren)
	{
		nodes.insert(t->mName);
	}

	for (size_t i = 0; i < list.size(); i++)
	{
		bool found = false;

		std::string name = list[i];

		if (list[i][list[i].length() - 1] == '/')
		{
			name = list[i].substr(0, list[i].length() - 1);
		}

		for (size_t j = 0; j < r->mChildren.size(); j++)
		{
			if (r->mChildren[j]->mName == name)
			{
				found = true;
				nodes.erase(name);
				break;
			}
		}

		if (!found)
		{
			if (list[i][list[i].length() - 1] == '/')
			{
				r->mChildren.push_back(CreateSubtree(directory + "/" + list[i].substr(0, list[i].length() - 1)));
			}
			else
			{
				r->mChildren.push_back(new Record(Type::FILE, list[i]));
			}
		}
	}

	for (const std::string& s : nodes)
	{
		for (size_t j = 0; j < r->mChildren.size(); j++)
		{
			if (r->mChildren[j]->mName == s)
			{
				r->mChildren.erase(r->mChildren.begin() + j);
				break;
			}
		}
	}
}

void DirectoryTree::_ImguiRenderRecord(Record* r)
{
	for (size_t i = 0; i < r->mChildren.size(); i++)
	{
		bool nodeOpen = false;
		
		if (ImGui::TreeNodeEx(r->mChildren[i]->mName.c_str(),
			(r->mChildren[i]->mType == Type::FILE ? ImGuiTreeNodeFlags_Leaf : 0) |
			ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen))
		{
			nodeOpen = true;

			if (r->mChildren[i]->mType != Type::DIRECTORY)
			{
				if (ImGui::BeginDragDropSource())
				{
					printf("%d\n", r->mChildren[i]->mResourceType);
					switch (r->mChildren[i]->mResourceType)
					{
					case ResourceType::MODEL:
						ImGui::SetDragDropPayload("RESOURCE_MODEL", &(r->mChildren[i]), sizeof(Record*));
						break;

					case ResourceType::TEXTURE:
						ImGui::SetDragDropPayload("RESOURCE_TEXTURE", &(r->mChildren[i]->mResource), sizeof(void*));
						ImGui::Image((ImTextureID)(((Engine::Manager<Engine::Texture>::Node*)r->mChildren[i]->mResource)->mItem->GetSRV().mGpuHandle.ptr), ImVec2(64, 64));
						break;

					default:
						break;
					}
					ImGui::EndDragDropSource();
				}

				if (ImGui::IsItemClicked())
				{
					printf("%s\n", r->mChildren[i]->mName.c_str());
				}
			}
		}

		if (r->mChildren[i]->mType == Type::DIRECTORY)
		{
			if (nodeOpen)
			{
				_ImguiRenderRecord(r->mChildren[i]);
			}
		}

		if (nodeOpen)
		{
			ImGui::TreePop();
		}
	}
}

DirectoryTree::DirectoryTree(const std::string& directory, Engine::Constants* options, Engine::Log* log, Engine::D3DRenderer* renderer)
{
	std::string path = directory;
	std::replace(path.begin(), path.end(), '\\', '/');

	mPath = path;
	mOptions = options;
	mLog = log;
	mRenderer = renderer;
}

DirectoryTree::~DirectoryTree()
{
	delete mRoot;
}

void DirectoryTree::Initialize()
{
	mRoot = CreateSubtree(mPath);
}

void DirectoryTree::SetManagers(Engine::Manager<Engine::Mesh>* meshManager, Engine::Manager<Engine::Model>* modelManager, Engine::Manager<Engine::Texture>* textureManager)
{
	mMeshManager = meshManager;
	mModelManager = modelManager;
	mTextureManager = textureManager;
}

void DirectoryTree::Imgui()
{
	_ImguiRenderRecord(mRoot);
}

void DirectoryTree::NotifyChange(std::string location)
{
	mChanges.push_back(location);
}

void DirectoryTree::ProcessChanges()
{
	for (const std::string& s : mChanges)
	{
		printf("Processing: '%s'\n", s.c_str());
		std::string path = mOptions->Get<std::string>("Editor.Resources") + "/" + s;
		std::replace(path.begin(), path.end(), '\\', '/');
		Engine::Files::Type type = Engine::Files::GetType(path);

		if (type == Engine::Files::Type::DIRECTORY)
		{
			UpdateSubtree(path);
		}
		else
		{
			std::string directory = path.substr(0, path.find_last_of('/'));
			mFiles.push_back(path);
			UpdateSubtree(directory);
		}

		mTextureManager->Print();
	}

	mChanges.clear();

	for (const std::string& s : mFiles)
	{
		Engine::LoaderDevIL loader(mLog);

		printf("%s\n", s.c_str());
		if (s.find(".tga") != std::string::npos)
		{
			Engine::Manager<Engine::Texture>::Node* node = mTextureManager->GetNode(s);
			if (node != nullptr)
			{
				Engine::LoaderDevIL::Image* image = loader.Load(s);
				if (image != nullptr)
				{
					image->mUseAlphaMipmap = false;
					loader.GenerateMipmaps(image);
				}
				Engine::Texture* t = new Engine::Texture(s);
				t->InitMipmaps(mRenderer, image->mWidth, image->mHeight, 1, image->mMipLevels, Engine::Graphics::RGBA8, (const void**)image->mMipmaps);
				t->SetName(s);
				delete node->mItem;
				node->mItem = t;
				delete image;
			}
		}
	}

	mFiles.clear();
}