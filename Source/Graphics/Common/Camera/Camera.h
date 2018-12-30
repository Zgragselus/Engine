///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Camera.h
//
// Camera class header, describes the abstract class of camera used for calculating view and 
// projection transformation matrices. 
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __CAMERA__H__
#define __CAMERA__H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "Core/Memory/Memory.h"
#include "Core/Math/Numeric/Float4.h"
#include "Core/Math/Numeric/Mat4.h"
#include "Core/Math/Primitive/Frustum.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	/// <summary>
	///	Generic camera class
	/// </summary>
	class Camera
	{
	public:
		/// <summary>Camera type</summary>
		enum class Type : unsigned int
		{
			ORTHOGONAL = 0,
			PERSPECTIVE = 1
		};

	protected:
		/// <summary>Camera view matrix</summary>
		mat4 mViewMatrix;

		/// <summary>Camera projection matrix</summary>
		mat4 mProjectionMatrix;

		/// <summary>Camera position</summary>
		float4 mPosition;

		/// <summary>Camera look-at target</summary>
		float4 mTarget;

		/// <summary>Camera forward vector</summary>
		float4 mForward;

		/// <summary>Camera up vector</summary>
		float4 mUp;

		/// <summary>Camera movement direction, so that our forward+strafe is not faster</summary>
		float4 mDirection;

		/// <summary>Camera movement speed</summary>
		float mSpeed;

		/// <summary>Is our view matrix really representing current state</summary>
		bool mValid;

		/// <summary>Pointer to frustum of the camera</summary>
		Frustum* mFrustum;

		/// <summary>Camera type</summary>
		Type mType;

		/// <summary>Updates frustum of the camera</summary>
		virtual void UpdateFrustum() = 0;

	public:
		/// <summary>Constructor</summary>
		/// <param name="position">Camera position</param>
		/// <param name="target">Camera look-at target</param>
		/// <param name="up">Camera up vector</param>
		/// <param name="speed">Camera translation speed</param>
		Camera(	const float4& position,
				const float4& target,
				const float4& up,
				float speed = 1.0f)
		{
			mPosition = position;
			mTarget = target;
			mUp = up;
			mSpeed = speed;
			mForward = normalize(target - position);
			mValid = false;
			mFrustum = new Frustum();
		}

		/// <summary>Virtual destructor</summary>
		virtual ~Camera()
		{
			delete mFrustum;
		}

		/// <summary>Strafe camera into side</summary>
		/// <param name="dir">Whether into positive or negative direction</param>
		void Strafe(float dir = 1.0f)
		{
			float4 ortho = normalize(float4(mForward.z, 0.0f, -mForward.x, 0.0f));
			mDirection += ortho * dir;
		}

		/// <summary>Move camera</summary>
		/// <param name="dir">Whether into positive or negative direction</param>
		void Move(float dir = 1.0f)
		{
			mDirection += mForward * dir;
		}

		/// <summary>Rotate camera direction</summary>
		/// <param name="yaw">Delta yaw angle</param>
		/// <param name="pitch">Delta pitch angle</param>
		void Rotate(float yaw, float pitch)
		{
			// Calculate quaternion rotation of delta yaw and delta pitch
			quat rotation;
			float4 ortho = normalize(float4(mForward.z, 0.0f, -mForward.x, 0.0f));
			rotation = from_axisangle(float4(0, 1, 0, 0), yaw);
			rotation = rotation * from_axisangle(ortho, -pitch);

			// Apply this rotation to camera forward vector
			mForward = rotate(rotation, mForward);

			// Update camera target position
			mTarget = mPosition + mForward;

			// Invalidate matrices
			mValid = false;
		}

		/// <summary>
		/// Camera process step
		///
		/// Moves camera by distance computed as ds = v * dt - e.g. using camera speed constant and 
		/// delta time (between 2 steps of game engine).
		/// </summary>
		/// <param name="up">Camera up vector</param>
		void Process(float timeStep)
		{
			// Normalize direction of the camera
			if (length(mDirection) > 0.0f)
			{
				mDirection = normalize(mDirection);
			}

			// Perform differential step
			mPosition += mDirection * mSpeed * timeStep;
			mTarget += mDirection * mSpeed * timeStep;

			// Zero-out direction and invalidate matrices
			mDirection = float4(0.0f, 0.0f, 0.0f, 0.0f);
			mValid = false;
		}

		/// <summary>Getter for camera view matrix</summary>
		virtual const mat4& GetViewMatrix()
		{
			mViewMatrix = lookat(mPosition, mTarget, mUp);
			//mViewMatrix = LookAtLH(mPosition, mTarget, mUp);
			mValid = true;

			return mViewMatrix;
		}

		inline float4 GetPosition()
		{
			return mPosition;
		}

		inline Frustum* GetFrustum()
		{
			UpdateFrustum();
			return mFrustum;
		}

		inline float4 GetForward()
		{
			return mForward;
		}

		inline float4 GetRight()
		{
			return cross(mForward, mUp);
		}

		inline float4 GetUp()
		{
			return mUp;
		}

		inline Camera::Type GetType()
		{
			return mType;
		}

		/// <summary>Getter for camera projection matrix (abstract)</summary>
		virtual const mat4& GetProjectionMatrix() = 0;

		/// <summary>Serialize object into string</summary>
		virtual std::string Serialize()
		{
			std::stringstream ss;

			ss << mPosition.x << " " << mPosition.y << " " << mPosition.z << " " << mPosition.w << " " << 
				mTarget.x << " " << mTarget.y << " " << mTarget.z << " " << mTarget.w << " " << 
				mUp.x << " " << mUp.y << " " << mUp.z << " " << mUp.w << " " <<
				mDirection.x << " " << mDirection.y << " " << mDirection.z << " " << mDirection.w << " " <<
				mSpeed << " " << (int)mType << std::endl;

			return ss.str();
		}

		/// <summary>Deseraliaze object from string</summary>
		/// <param name="s">String to serialize from</param>
		virtual void Deserialize(const std::string& s)
		{
			std::stringstream ss(s);

			int type;
			ss >> mPosition.x >> mPosition.y >> mPosition.z >> mPosition.w >> 
				mTarget.x >> mTarget.y >> mTarget.z >> mTarget.w >>
				mUp.x >> mUp.y >> mUp.z >> mUp.w >>
				mDirection.x >> mDirection.y >> mDirection.z >> mDirection.w >>
				mSpeed >> type;
			mType = (Type)type;

			mForward = normalize(mTarget - mPosition);

			mValid = false;
		}

		friend class CameraComponent;

		ALIGNED_NEW_DELETE("Engine::Camera")
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOH

#endif