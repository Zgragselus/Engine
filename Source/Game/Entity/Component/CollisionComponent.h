#ifndef __COLLISION_COMPONENT__H__
#define __COLLISION_COMPONENT__H__

#include "../Component.h"
#include "Physics/Shapes/Shape.h"
#include "Physics/Shapes/Compound/Compound.h"
#include "Physics/Shapes/Mesh/ConvexHull.h"
#include "Physics/Shapes/Mesh/TriangleMesh.h"
#include "Physics/Shapes/Primitive/Box.h"
#include "Physics/Shapes/Primitive/Capsule.h"
#include "Physics/Shapes/Primitive/Cone.h"
#include "Physics/Shapes/Primitive/Cylinder.h"
#include "Physics/Shapes/Primitive/Sphere.h"

namespace Engine
{
	class __declspec(align(16)) CollisionComponent : public Component
	{
	private:
		static std::string mComponentName;
		Shape* mShape;

	public:
		CollisionComponent(Shape* shape)
		{
			mShape = shape;
		}

		virtual ~CollisionComponent()
		{
			delete mShape;
		}

		Shape* Get()
		{
			return mShape;
		}

		static std::string& GetName()
		{
			return mComponentName;
		}

		virtual bool Editor(std::string& prev, std::string& next);

		virtual std::string Serialize()
		{
			std::stringstream ss;

			ss << "CollisionComponent" << std::endl;

			return (ss.str() + mShape->Serialize());
		}

		virtual void Deserialize(const std::string& s);

		ALIGNED_NEW_DELETE("Engine::CollisionComponent")
	};
}

#endif