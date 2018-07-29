#ifndef __TEXTURE_COMPONENT__
#define __TEXTURE_COMPONENT__

#include "../Component.h"
#include "Graphics/D3D12/Texture/Texture.h"
#include "Graphics/Common/Manager/Manager.h"

namespace Engine
{
	class __declspec(align(16)) TextureComponent : public Component
	{
	private:
		static std::string mComponentName;
		Manager<Texture>* mTextureManager;
		Texture* mTexture;

	public:
		TextureComponent(Texture* texture, Manager<Texture>* manager = nullptr) : Component()
		{
			mTexture = texture;
			mTextureManager = manager;
		}
		
		virtual ~TextureComponent()
		{
			if (mTextureManager)
			{
				mTextureManager->Remove(mTexture);
			}

			if (mTexture)
			{
				mTexture = nullptr;
			}
		}

		inline Texture* GetTexture()
		{
			return mTexture;
		}

		static std::string& GetName()
		{
			return mComponentName;
		}

		virtual bool Editor(std::string& prev, std::string& next)
		{
			return false;
		}

		virtual std::string Serialize()
		{

		}

		virtual void Deserialize(const std::string&)
		{

		}

		ALIGNED_NEW_DELETE("Engine::TextureComponent")
	};
}

#endif