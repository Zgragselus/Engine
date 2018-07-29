///////////////////////////////////////////////////////////////////////////////////////////////////
//
// PerspectiveCamera.h
//
// Perspective camera class header, a specific camera with perspective projection
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PERSPECTIVE__CAMERA__H__
#define __PERSPECTIVE__CAMERA__H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "Camera.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	/// <summary>
	///	Generic camera class
	/// </summary>
	class PerspectiveCamera : public Camera
	{
	protected:
		/// <summary>Camere field-of-view angle</summary>
		float mFov;

		/// <summary>Camere aspect ratio</summary>
		float mAspect;

		/// <summary>Camere near clipping plane</summary>
		float mNear;

		/// <summary>Camere far clipping plane</summary>
		float mFar;

		/// <summary>Is our projection matrix a valid one?</summary>
		bool mProjectionValid = false;

	public:
		/// <summary>Constructor</summary>
		/// <param name="position">Camera position</param>
		/// <param name="target">Camera look-at target</param>
		/// <param name="up">Camera up vector</param>
		/// <param name="fov">Camera field of view angle</param>
		/// <param name="aspect">Camera aspect ratio</param>
		/// <param name="nearPlane">Distance to near plane</param>
		/// <param name="farPlane">Distance to far plane</param>
		/// <param name="speed">Camera translation speed</param>
		PerspectiveCamera(const float4& position,
			const float4& target,
			const float4& up,
			float fov,
			float aspect,
			float nearPlane,
			float farPlane,
			float speed = 1.0f) : Camera(position, target, up, speed)
		{
			mFov = fov;
			mAspect = aspect;
			mNear = nearPlane;
			mFar = farPlane;
			mProjectionValid = false;
		}

		/// <summary>Virtual destructor</summary>
		virtual ~PerspectiveCamera()
		{
		}

		/// <summary>Getter for camera projection matrix</summary>
		virtual const mat4& GetProjectionMatrix()
		{
			if (mProjectionValid == false)
			{
				mProjectionMatrix = perspective(mFov, mAspect, mNear, mFar);
				mProjectionValid = true;
			}

			return mProjectionMatrix;
		}

		ALIGNED_NEW_DELETE("Engine::PerspectiveCamera")
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOH

#endif