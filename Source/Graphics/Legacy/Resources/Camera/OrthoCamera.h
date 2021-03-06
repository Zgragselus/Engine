///////////////////////////////////////////////////////////////////////////////////////////////////
//
// OrthoCamera.h
//
// Orthogonal camera class header, a specific camera with orthogonal projection
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __ORTHO__CAMERA__H__
#define __ORTHO__CAMERA__H__

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
	class OrthoCamera : public Camera
	{
	protected:
		/// <summary> Specifies distances of clipping planes from origin</summary>
		float mLeft;
		float mRight;
		float mTop;
		float mBottom;
		float mNear;
		float mFar;

		/// <summary>Is our projection matrix a valid one?</summary>
		bool mProjectionValid = false;

	public:
		/// <summary>Constructor</summary>
		/// <param name="position">Camera position</param>
		/// <param name="target">Camera look-at target</param>
		/// <param name="up">Camera up vector</param>
		/// <param name="leftPlane">Distance to left plane</param>
		/// <param name="rightPlane">Distance to right plane</param>
		/// <param name="topPlane">Distance to top plane</param>
		/// <param name="bottomPlane">Distance to bottom plane</param>
		/// <param name="nearPlane">Distance to near plane</param>
		/// <param name="farPlane">Distance to far plane</param>
		/// <param name="speed">Camera translation speed</param>
		OrthoCamera(const float4& position,
			const float4& target,
			const float4& up,
			float leftPlane,
			float rightPlane,
			float topPlane,
			float bottomPlane,
			float nearPlane,
			float farPlane,
			float speed = 1.0f) : Camera(position, target, up, speed)
		{
			mLeft = leftPlane;
			mRight = rightPlane;
			mTop = topPlane;
			mBottom = bottomPlane;
			mNear = nearPlane;
			mFar = farPlane;
			mProjectionValid = false;
		}

		/// <summary>Virtual destructor</summary>
		virtual ~OrthoCamera()
		{
		}

		/// <summary>Getter for camera projection matrix</summary>
		virtual const mat4& GetProjectionMatrix()
		{
			if (mProjectionValid == false)
			{
				mProjectionMatrix = ortho(mLeft, mRight, mTop, mBottom, mNear, mFar);
				mProjectionValid = true;
			}

			return mProjectionMatrix;
		}

		ALIGNED_NEW_DELETE("Engine::OrthoCamera")
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOH

#endif