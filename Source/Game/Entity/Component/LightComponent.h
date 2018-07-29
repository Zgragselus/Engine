#ifndef __LIGHT_COMPONENT__H__
#define __LIGHT_COMPONENT__H__

#include "../Component.h"
#include "Graphics/Common/Light/Light.h"
#include "Graphics/Common/Light/PointLight.h"
#include "Graphics/Common/Light/SpotLight.h"
#include "Graphics/Common/Light/DirectionalLight.h"

namespace Engine
{
	class __declspec(align(16)) LightComponent : public Component
	{
	private:
		static std::string mComponentName;
		static int mResolutionTableSize;
		static int mResolutionTable[6];
		unsigned int mLightID;			// ID to global buffer for lights (maps information to cbuffer)
		Light* mLight;

	public:
		LightComponent(Light* light) : Component()
		{
			mLight = light;
		}

		~LightComponent()
		{
			delete mLight;
		}

		inline Light* GetLight()
		{
			return mLight;
		}

		static std::string& GetName()
		{
			return mComponentName;
		}

		virtual bool Editor(std::string& prev, std::string& next);

		virtual std::string Serialize()
		{
			std::stringstream ss;

			ss << "LightComponent" << std::endl;

			return (ss.str() + mLight->Serialize());
		}

		virtual void Deserialize(const std::string& s);

		ALIGNED_NEW_DELETE("Engine::LightComponent")
	};
}

#endif // !__LIGHT_COMPONENT__H__
