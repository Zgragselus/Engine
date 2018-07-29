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
			Extract(matrix);
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
		/// Apply transformation matrix
		/// </summary>
		void Extract(const mat4& matrix)
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

			mValid = true;
		}

		/// <summary>
		/// Apply transformation matrix
		/// </summary>
		void Apply(const mat4& m)
		{
			Update();
			mTransformation = m * mTransformation;
			Extract(mTransformation);
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
		float4& GetTranslation()
		{
			return mTranslation;
		}

		/// <summary>
		/// Returns rotation
		/// </summary>
		quat& GetRotation()
		{
			return mRotation;
		}

		/// <summary>
		/// Returns scale
		/// </summary>
		float4& GetScale()
		{
			return mScale;
		}

		/// <summary>
		/// Prints transformation matrix
		/// </summary>
		void Print() const
		{
			const float* mat = (const float*)mTransformation;
			printf("Debug\n%f %f %f %f\n", mat[0], mat[1], mat[2], mat[3]);
			printf("%f %f %f %f\n", mat[4], mat[5], mat[6], mat[7]);
			printf("%f %f %f %f\n", mat[8], mat[9], mat[10], mat[11]);
			printf("%f %f %f %f\n\n", mat[12], mat[13], mat[14], mat[15]);
		}

		/// <summary>
		/// Invalidate
		/// </summary>
		void Invalidate()
		{
			mValid = false;
		}

		/// <summary>
		/// Serialize transformation into string
		/// </summary>
		std::string Serialize()
		{
			std::stringstream ss;

			ss << "Transformation " << std::endl;
			ss << mTranslation.x << " " << mTranslation.y << " " << mTranslation.z << std::endl;
			ss << mRotation.x << " " << mRotation.y << " " << mRotation.z << " " << mRotation.w << std::endl;
			ss << mScale.x << " " << mScale.y << " " << mScale.z << std::endl;

			return ss.str();
		}

		/// <summary>
		/// Deserialize transformation from string
		/// </summary>
		void Deserialize(const std::string& str)
		{
			std::stringstream ss(str);

			std::string name;

			ss >> name >> mTranslation.x >> mTranslation.y >> mTranslation.z >> mRotation.x >> mRotation.y >> mRotation.z >> mRotation.w >> mScale.x >> mScale.y >> mScale.z;
		}

		ALIGNED_NEW_DELETE("Engine::Transformation")
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOH

#endif