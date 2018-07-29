#ifndef __LIGHT__H__
#define __LIGHT__H__

#include "Core/Memory/Memory.h"
#include "Core/Math/Numeric/Float4.h"
#include "Core/Math/Numeric/Mat4.h"
#include "Core/Math/Numeric/Common.h"
#include "Core/Util/String.h"

namespace Engine
{
	class LightComponent;

	class Light
	{
	public:
		enum class Type : unsigned int
		{
			POINT = 0,
			SPOT = 1,
			DIRECTIONAL = 2
		};

		enum class ShadowFilter : int
		{
			NEAREST = 0,
			BILINEAR = 1,
			BICUBIC = 2,
			PCF = 3,
			PCSS = 4,
			PCMLSM = 5,
			BACKPROJECTION = 6
		};

	protected:
		float4 mColor;
		
		Type mType;
		float mNear;
		float mFar;
		unsigned int mShadowEnabled;

		ShadowFilter mFilter;
		float mFilterSize;
		unsigned int mResolution;
		float mLightSize;

		float mTemperature;
		float mPower;
		float mPad[2];

	public:
		Light(const float4& color)
		{
			mColor = color;
			mNear = 1.0f;
			mFar = 2000.0f;
			mShadowEnabled = 1;
			mFilter = ShadowFilter::NEAREST;
			mFilterSize = 0.01f;
			mResolution = 256;
			mLightSize = 1.0f;
			mTemperature = 5000.0f;
			mPower = 900.0f;
		}

		virtual ~Light()
		{

		}

		Type GetType()
		{
			return mType;
		}

		inline void SetColor(const float4& color) { mColor = color; }
		inline float4 GetColor() { return mColor; }

		virtual std::string Serialize()
		{
			std::stringstream ss;

			ss << mColor.x << " " << mColor.y << " " << mColor.z << " " << mColor.w << " " << 
				(int)mType << " " << mNear << " " << mFar << " " << mShadowEnabled << " " << 
				(int)mFilter << " " << mFilterSize << " " << mResolution << " " << mLightSize << " " << 
				mTemperature << " " << mPower << " " << std::endl;

			return ss.str();
		}

		virtual void Deserialize(const std::string& s)
		{
			std::stringstream ss(s);

			int type;
			int filter;

			ss >> mColor.x >> mColor.y >> mColor.z >> mColor.w >> type >> mNear >> mFar >> mShadowEnabled >> filter >> mFilterSize >> mResolution >> mLightSize >> mTemperature >> mPower;
			
			mType = (Type)type;
			mFilter = (ShadowFilter)filter;
		}

		inline float GetNearPlane()
		{
			return mNear;
		}

		inline float GetFarPlane()
		{
			return mFar;
		}

		inline unsigned int GetShadowsEnabled()
		{
			return mShadowEnabled;
		}

		inline int GetShadowsFilter()
		{
			return (int)mFilter;
		}

		inline float GetFilterSize()
		{
			return mFilterSize;
		}

		inline unsigned int GetResolution()
		{
			return mResolution;
		}

		inline float GetLightSize()
		{
			return mLightSize;
		}

		inline float GetTemperature()
		{
			return mTemperature;
		}

		inline float GetPower()
		{
			return mPower;
		}

		friend class LightComponent;

		ALIGNED_NEW_DELETE("Engine::Light")
	};
}

#endif