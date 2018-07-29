#ifndef __BOX_H__
#define __BOX_H__

#include "Physics/Shapes/Shape.h"

namespace Engine
{
	class __declspec(align(16)) Box : public Shape
	{
	protected:
		float4 mExtents;

	public:
		inline Box(const float4& extents) : Shape()
		{
			mType = Type::BOX;
			mExtents = extents;
		}

		inline float4 GetExtents()
		{
			return mExtents;
		}

		virtual std::string Serialize()
		{
			std::stringstream ss;

			ss << mExtents[0] << " " << mExtents[1] << " " << mExtents[2] << " " << mExtents[3] << std::endl;

			return (Shape::Serialize() + ss.str());
		}

		virtual void Deserialize(const std::string& s)
		{
			std::vector<std::string> lines = String::Split(s, '\n');

			Shape::Deserialize(lines[0]);

			std::stringstream ss(lines[1]);
			ss >> mExtents[0] >> mExtents[1] >> mExtents[2] >> mExtents[3];
		}

		friend class CollisionComponent;

		ALIGNED_NEW_DELETE("Engine::Box")
	};
}

#endif