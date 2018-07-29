#ifndef __RIGID_BODY__H__
#define __RIGID_BODY__H__

#include "Core/Math/Numeric/Mat4.h"
#include "Core/Memory/Memory.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

namespace Engine
{
	class RigidBodyComponent;

	class __declspec(align(16)) RigidBody
	{
	protected:
		mat4 mTransform;
		float4 mTranslation;
		quat mRotation;
		float4 mScale;
		float mMass;
		btRigidBody* mBody;

	private:
		float pad;

	public:
		inline RigidBody(float mass)
		{
			mMass = mass;
		}

		inline void SetTransform(const mat4& matrix)
		{
			Engine::mat4 tmp = matrix;
			float* m = (float*)&tmp;

			memcpy(tmp, m, sizeof(float) * 16);

			// Extract translation
			mTranslation = float4(m[3], m[7], m[11], m[15]);

			// Extract scale
			{
				mScale = float4(
					length(float4(m[0], m[1], m[2], 0.0f)),
					length(float4(m[4], m[5], m[6], 0.0f)),
					length(float4(m[8], m[9], m[10], 0.0f)),
					1.0f
				);

				m[0] /= mScale.x; m[1] /= mScale.x; m[2] /= mScale.x;
				m[4] /= mScale.y; m[5] /= mScale.y; m[6] /= mScale.y;
				m[8] /= mScale.z; m[9] /= mScale.z; m[10] /= mScale.z;
			}

			mRotation = mat_to_quat(tmp);
		}

		inline mat4 GetTransform()
		{
			return mTransform;
		}

		inline void UpdateTransform(const float4& translate, const quat& rotate)
		{
			mTranslation = translate;
			mRotation = rotate;
			mTransform = mat4(mTranslation) * mat4(mRotation);
		}

		inline float4 GetTranslation()
		{
			return mTranslation;
		}

		inline void SetTranslation(const float4& f)
		{
			mTranslation = f;
		}

		inline quat GetRotation()
		{
			return mRotation;
		}

		inline void SetRotation(const quat& q)
		{
			mRotation = q;
		}

		inline float GetMass()
		{
			return mMass;
		}

		inline void SetBody(btRigidBody* body)
		{
			mBody = body;
		}

		inline btRigidBody* GetBody()
		{
			return mBody;
		}

		inline std::string Serialize()
		{
			std::stringstream ss;

			ss << mMass << std::endl;

			return ss.str();
		}

		inline void Deserialize(const std::string& s)
		{
			std::stringstream ss(s);

			ss >> mMass;
		}

		friend class RigidBodyComponent;

		ALIGNED_NEW_DELETE("Engine::RigidBody")
	};
}

#endif