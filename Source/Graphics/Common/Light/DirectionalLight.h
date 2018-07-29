#ifndef __DIRECTIONAL_LIGHT__H__
#define __DIRECTIONAL_LIGHT__H__

#include "Light.h"

namespace Engine
{
	class DirectionalLight : public Light
	{
	protected:
		float4 mDirection;

	public:
		DirectionalLight(const float4& color, const float4& direction) : Light(color)
		{
			mType = Light::Type::DIRECTIONAL;

			mDirection = direction;
		}

		virtual ~DirectionalLight()
		{

		}

		inline void SetDirection(const float4& dir) { mDirection = dir; }
		inline float4 GetDirection() { return mDirection; }

		virtual std::string Serialize()
		{
			std::stringstream ss;

			ss << mDirection.x << " " << mDirection.y << " " << mDirection.z << " " << mDirection.w << std::endl;

			return (Light::Serialize() + ss.str());
		}

		virtual void Deserialize(const std::string& s)
		{
			std::vector<std::string> lines = String::Split(s, '\n');

			Light::Deserialize(lines[0]);

			std::stringstream ss(lines[1]);			
			ss >> mDirection.x >> mDirection.y >> mDirection.z >> mDirection.w;
		}

		friend class LightComponent;

		ALIGNED_NEW_DELETE("Engine::DirectionalLight")
	};
}

#endif