#ifndef __MATERIAL_COMPONENT__
#define __MATERIAL_COMPONENT__

#include "../Component.h"
#include "Graphics/D3D12/Texture/Texture.h"
#include "Graphics/Common/Manager/Manager.h"

namespace Engine
{
	class __declspec(align(16)) MaterialComponent : public Component
	{
	private:
		static std::string mComponentName;
		Manager<Texture>* mTextureManager;
		Manager<Texture>::Node* mDiffuseMap;
		Manager<Texture>::Node* mNormalsMap;
		Manager<Texture>::Node* mHeightMap;
		Manager<Texture>::Node* mRoughnessMap;
		Manager<Texture>::Node* mMetallicMap;

	public:
		MaterialComponent(Texture* diffuseMap, Texture* normalsMap, Texture* heightMap, Texture* roughnessMap, Texture* metallicMap, Manager<Texture>* manager) : Component()
		{
			if (diffuseMap)
			{
				mDiffuseMap = manager->GetNode(diffuseMap);
			}

			if (normalsMap)
			{
				mNormalsMap = manager->GetNode(normalsMap);
			}

			if (heightMap)
			{
				mHeightMap = manager->GetNode(heightMap);
			}

			if (roughnessMap)
			{
				mRoughnessMap = manager->GetNode(roughnessMap);
			}

			if (metallicMap)
			{
				mMetallicMap = manager->GetNode(metallicMap);
			}

			mTextureManager = manager;
		}

		MaterialComponent(const MaterialComponent& m)
		{
			mDiffuseMap = m.mDiffuseMap;
			mNormalsMap = m.mNormalsMap;
			mHeightMap = m.mHeightMap;
			mRoughnessMap = m.mRoughnessMap;
			mMetallicMap = m.mMetallicMap;
			mTextureManager = m.mTextureManager;
		}

		virtual ~MaterialComponent()
		{
			if (mDiffuseMap)
			{
				mTextureManager->Remove(mDiffuseMap->mItem);
				mDiffuseMap = nullptr;
			}

			if (mNormalsMap)
			{
				mTextureManager->Remove(mNormalsMap->mItem);
				mNormalsMap = nullptr;
			}

			if (mHeightMap)
			{
				mTextureManager->Remove(mHeightMap->mItem);
				mHeightMap = nullptr;
			}

			if (mRoughnessMap)
			{
				mTextureManager->Remove(mRoughnessMap->mItem);
				mRoughnessMap = nullptr;
			}

			if (mMetallicMap)
			{
				mTextureManager->Remove(mMetallicMap->mItem);
				mMetallicMap = nullptr;
			}
		}

		inline Texture* GetDiffuseMap()
		{
			return mDiffuseMap->mItem;
		}

		inline Texture* GetNormalsMap()
		{
			return mNormalsMap->mItem;
		}

		inline Texture* GetHeightMap()
		{
			return mHeightMap->mItem;
		}

		inline Texture* GetRoughnessMap()
		{
			return mRoughnessMap->mItem;
		}

		inline Texture* GetMetallicMap()
		{
			return mMetallicMap->mItem;
		}

		static std::string& GetName()
		{
			return mComponentName;
		}

		virtual bool Editor(std::string& prev, std::string& next);

		virtual std::string Serialize()
		{
			std::stringstream ss;

			ss << "MaterialComponent" << std::endl;
			ss << mDiffuseMap->mName << std::endl;
			ss << mNormalsMap->mName << std::endl;
			ss << mHeightMap->mName << std::endl;
			ss << mRoughnessMap->mName << std::endl;
			ss << mMetallicMap->mName << std::endl;

			return ss.str();
		}

		virtual void Deserialize(const std::string& s)
		{
			std::vector<std::string> lines = String::Split(s, '\n');
			mDiffuseMap = mTextureManager->GetNode(lines[1]);
			mNormalsMap = mTextureManager->GetNode(lines[2]);
			mHeightMap = mTextureManager->GetNode(lines[3]);
			mRoughnessMap = mTextureManager->GetNode(lines[4]);
			mMetallicMap = mTextureManager->GetNode(lines[5]);
		}

		ALIGNED_NEW_DELETE("Engine::MaterialComponent")
	};
}

#endif