#ifndef __SPOT_LIGHT__H__
#define __SPOT_LIGHT__H__

#include "Light.h"

namespace Engine
{
	class SpotLight : public Light
	{
	protected:
		float4 mDirection;
		float mSpotAngle;
		float mLinearAtt;
		float mQuadraticAtt;
		float mPad;

	public:
		SpotLight(const float4& color, const float4& direction, float spotAngle, float lAtt, float qAtt) : Light(color)
		{
			mType = Light::Type::SPOT;

			mDirection = normalize(direction);
			mSpotAngle = spotAngle;
			mLinearAtt = lAtt;
			mQuadraticAtt = qAtt;
		}

		virtual ~SpotLight()
		{

		}

		inline void SetDirection(const float4& dir) { mDirection = normalize(dir); }
		inline float4 GetDirection() { return mDirection; }
		inline void SetSpotAngle(float angle) { mSpotAngle = angle; }
		inline float GetSpotAngle() { return mSpotAngle; }
		inline float GetSpotAngleRad() { return Math::DegToRad(mSpotAngle); }
		inline void SetLinearAttenuation(float lAtt) { mLinearAtt = lAtt; }
		inline float GetLinearAttenuation() { return mLinearAtt; }
		inline void SetQuadraticAttenuation(float qAtt) { mQuadraticAtt = qAtt; }
		inline float GetQuadraticAttenuation() { return mQuadraticAtt; }

		virtual std::string Serialize()
		{
			std::stringstream ss;

			ss << mDirection.x << " " << mDirection.y << " " << mDirection.z << " " << mDirection.w << " " << mSpotAngle << " " << mLinearAtt << " " << mQuadraticAtt << std::endl;

			return (Light::Serialize() + ss.str());
		}

		virtual void Deserialize(const std::string& s)
		{
			std::vector<std::string> lines = String::Split(s, '\n');

			Light::Deserialize(lines[0]);

			std::stringstream ss(lines[1]);
			ss >> mDirection.x >> mDirection.y >> mDirection.z >> mDirection.w >> mSpotAngle >> mLinearAtt >> mQuadraticAtt;
		}

		friend class LightComponent;

		ALIGNED_NEW_DELETE("Engine::SpotLight")
	};
}

#endif