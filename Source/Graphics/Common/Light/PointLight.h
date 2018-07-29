#ifndef __POINT_LIGHT__H__
#define __POINT_LIGHT__H__

#include "Light.h"

namespace Engine
{
	class PointLight : public Light
	{
	protected:
		float mLinearAtt;
		float mQuadraticAtt;
		float mPad[2];

	public:
		PointLight(const float4& color, float lAtt, float qAtt) : Light(color)
		{
			mType = Light::Type::POINT;

			mLinearAtt = lAtt;
			mQuadraticAtt = qAtt;
		}

		virtual ~PointLight()
		{

		}

		inline void SetLinearAttenuation(float lAtt) { mLinearAtt = lAtt; }
		inline float GetLinearAttenuation() { return mLinearAtt; }
		inline void SetQuadraticAttenuation(float qAtt) { mQuadraticAtt = qAtt; }
		inline float GetQuadraticAttenuation() { return mQuadraticAtt; }

		virtual std::string Serialize()
		{
			std::stringstream ss;

			ss << mLinearAtt << " " << mQuadraticAtt << std::endl;

			return (Light::Serialize() + ss.str());
		}

		virtual void Deserialize(const std::string& s)
		{
			std::vector<std::string> lines = String::Split(s, '\n');

			Light::Deserialize(lines[0]);

			std::stringstream ss(lines[1]);
			ss >> mLinearAtt >> mQuadraticAtt;
		}

		friend class LightComponent;

		ALIGNED_NEW_DELETE("Engine::PointLight")
	};
}

#endif