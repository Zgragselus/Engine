#ifndef __SPHERE_H__
#define __SPHERE_H__

#include "Physics/Shapes/Shape.h"

namespace Engine
{
	class __declspec(align(16)) Sphere : public Shape
	{
	protected:
		float mRadius;

	private:
		float pad[3];

	public:
		inline Sphere(float radius) : Shape()
		{
			mType = Type::SPHERE;
			mRadius = radius;
		}

		inline float GetRadius() const
		{
			return mRadius;
		}

		virtual std::string Serialize()
		{
			std::stringstream ss;

			ss << mRadius << std::endl;

			return (Shape::Serialize() + ss.str());
		}

		virtual void Deserialize(const std::string& s)
		{
			std::vector<std::string> lines = String::Split(s, '\n');

			Shape::Deserialize(lines[0]);

			std::stringstream ss(lines[1]);
			ss >> mRadius;
		}

		friend class CollisionComponent;

		ALIGNED_NEW_DELETE("Engine::Sphere")
	};
}

#endif