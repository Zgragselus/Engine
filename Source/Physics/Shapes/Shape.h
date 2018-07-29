#ifndef __SHAPE_H__
#define __SHAPE_H__

#include "Core/Memory/Memory.h"
#include "Core/Math/Numeric/Float4.h"
#include "Core/Util/String.h"

namespace Engine
{
	class CollisionComponent;

	class __declspec(align(16)) Shape
	{
	public:
		enum class Type : uint32_t
		{
			BOX = 0,
			CAPSULE,
			COMPOUND,
			CONE,
			CONVEX_HULL,
			CYLINDER,
			SPHERE,
			TRIANGLE_MESH
		};

	protected:
		Type mType;

	private:
		uint32_t pad[3];

	public:
		virtual ~Shape()
		{

		}

		inline Type GetType()
		{
			return mType;
		}

		virtual std::string Serialize()
		{
			std::stringstream ss;

			ss << (int)mType << std::endl;

			return ss.str();
		}

		virtual void Deserialize(const std::string& s)
		{
			std::stringstream ss(s);

			int type;

			ss >> type;

			mType = (Type)type;
		}

		friend class CollisionComponent;

		ALIGNED_NEW_DELETE("Engine::Shape")
	};
}

#endif