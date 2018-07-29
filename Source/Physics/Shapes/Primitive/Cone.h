#ifndef __CONE_H__
#define __CONE_H__

#include "Physics/Shapes/Shape.h"

namespace Engine
{
	class __declspec(align(16)) Cone : public Shape
	{
	protected:
		float mRadius;
		float mHeight;

	private:
		float pad[2];

	public:
		inline Cone(float radius, float height) : Shape()
		{
			mType = Type::CONE;
			mRadius = radius;
			mHeight = height;
		}

		inline float GetRadius()
		{
			return mRadius;
		}

		inline float GetHeight()
		{
			return mHeight;
		}

		virtual std::string Serialize()
		{
			std::stringstream ss;

			ss << mRadius << " " << mHeight << std::endl;

			return (Shape::Serialize() + ss.str());
		}

		virtual void Deserialize(const std::string& s)
		{
			std::vector<std::string> lines = String::Split(s, '\n');

			Shape::Deserialize(lines[0]);

			std::stringstream ss(lines[1]);
			ss >> mRadius >> mHeight;
		}

		friend class CollisionComponent;

		ALIGNED_NEW_DELETE("Engine::Cone")
	};
}

#endif