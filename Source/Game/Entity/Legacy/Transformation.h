///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Transformation.h
//
// Transformation matrix class
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __TRANSFORMATION__H__
#define __TRANSFORMATION__H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "Core/Memory/Memory.h"
#include "Core/Math/Numeric/Common.h"
#include "Core/Math/Numeric/Float4.h"
#include "Core/Math/Numeric/Mat4.h"
#include "Core/Math/Numeric/Quat.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	/// <summary>
	/// A class for holding the transformation (Translation, Rotation and Scale)
	/// </summary>
	class Transformation
	{
	private:
		/// <summary>Transformation matrix</summary>
		mat4 mTransformation;
		/// <summary>Translation vector</summary>
		float4 mTranslation;
		/// <summary>Rotational quaternion</summary>
		quat mRotation;
		/// <summary>Scale vector</summary>
		float4 mScale;
		/// <summary>Is the transformation matrix valid (does it reflect actual data in translation, rotation and scale)</summary>
		bool mValid;

	public:
		/// <summary>
		/// Blank constructor
		/// </summary>
		Transformation()
		{
			mTranslation = float4();
			mRotation = quat();
			mScale = float4(1.0f);
			mValid = false;
		}

		/// <summary>
		/// Parametrized constructor
		/// </summary>
		/// <param name="translation">Vector describing translation</param>
		/// <param name="rotation">Quaternion describing wanted rotation</param>
		/// <param name="scale">Scale parameter, note that default scale is 1.0</param>
		Transformation(const float4& translation,
			const quat& rotation = quat(),
			const float4& scale = float4(1.0f))
		{
			mTranslation = translation;
			mRotation = rotation;
			mScale = scale;
			mValid = false;
		}

		/// <summary>
		/// Matrix constructor
		/// </summary>
		/// <param name="matrix">Matrix from which we construct transformation</param>
		Transformation(const mat4& matrix)
		{
			float* m = (float*)&matrix;

			mTranslation = float4(m[12], m[13], m[14], m[15]);
			mRotation = mat_to_quat(matrix);
			mScale = float4(1.0f);
			mValid = true;
		}

		/// <summary>
		/// Re-computes the transformation matrix (if necessary)
		/// </summary>
		void Update()
		{
			if (mValid == false)
			{
				mValid = true;
				mTransformation = mat4(mTranslation) *
					mat4(mRotation) *
					mat4(mScale.x, 0, 0, 0,
					0, mScale.y, 0, 0,
					0, 0, mScale.z, 0,
					0, 0, 0, 1.0f);
			}
		}

		/// <summary>
		/// Returns transformation matrix
		/// </summary>
		const mat4& Get()
		{
			return mTransformation;
		}

		/// <summary>
		/// Sets translation
		/// </summary>
		/// <param name="translation">Vector describing translation</param>
		void SetTranslation(const float4& translation)
		{
			mTranslation = translation;
			mValid = false;
		}

		/// <summary>
		/// Sets rotation
		/// </summary>
		/// <param name="rotation">Quaternion describing wanted rotation</param>
		void SetRotation(const quat& rotation)
		{
			mRotation = rotation;
			mValid = false;
		}

		/// <summary>
		/// Sets scale
		/// </summary>
		/// <param name="scale">Scale parameter, note that default scale is 1.0</param>
		void SetScale(const float4& scale)
		{
			mScale = scale;
			mValid = false;
		}

		/// <summary>
		/// Returns translation
		/// </summary>
		const float4& GetTranslation()
		{
			return mTranslation;
		}

		/// <summary>
		/// Returns rotation
		/// </summary>
		const quat& GetRotation()
		{
			return mRotation;
		}

		/// <summary>
		/// Returns scale
		/// </summary>
		const float4& GetScale()
		{
			return mScale;
		}

		ALIGNED_NEW_DELETE("Engine::Transformation")
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOH

#endif