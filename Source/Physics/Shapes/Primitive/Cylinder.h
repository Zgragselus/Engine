#ifndef __CYLINDER_H__
#define __CYLINDER_H__

#include "Physics/Shapes/Shape.h"

namespace Engine
{
	class __declspec(align(16)) Cylinder : public Shape
	{
	protected:
		float4 mHalfExtents;

	public:
		inline Cylinder(const float4& halfExtents) : Shape()
		{
			mType = Type::CYLINDER;
			mHalfExtents = halfExtents;
		}

		inline float4 GetHalfExtents()
		{
			return mHalfExtents;
		}

		virtual std::string Serialize()
		{
			std::stringstream ss;

			ss << mHalfExtents[0] << " " << mHalfExtents[1] << " " << mHalfExtents[2] << " " << mHalfExtents[3] << std::endl;

			return (Shape::Serialize() + ss.str());
		}

		virtual void Deserialize(const std::string& s)
		{
			std::vector<std::string> lines = String::Split(s, '\n');

			Shape::Deserialize(lines[0]);

			std::stringstream ss(lines[1]);
			ss >> mHalfExtents[0] >> mHalfExtents[1] >> mHalfExtents[2] >> mHalfExtents[3];
		}

		friend class CollisionComponent;

		ALIGNED_NEW_DELETE("Engine::Cylinder")
	};
}

#endif