#ifndef __CONVEX_HULL__H__
#define __CONVEX_HULL__H__

#include "Physics/Shapes/Shape.h"

namespace Engine
{
	class __declspec(align(16)) ConvexHull : Shape
	{
	protected:
		float4* mVertices;
		unsigned int mVerticesCount;

	private:
		float pad;

	public:
		virtual ~ConvexHull()
		{
			delete[] mVertices;
		}

		inline ConvexHull(float4* vertices, unsigned int verticesCount) : Shape()
		{
			mType = Type::CONVEX_HULL;
			mVerticesCount = verticesCount;
			mVertices = new float4[mVerticesCount];
			memcpy(mVertices, vertices, sizeof(float4) * verticesCount);
		}

		friend class CollisionComponent;

		ALIGNED_NEW_DELETE("Engine::ConvexHull")
	};
}

#endif