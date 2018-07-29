#ifndef __RENDER_NODE__H__
#define __RENDER_NODE__H__

#include "Core/Math/Numeric/Mat4.h"
#include "Core/Math/Numeric/Float4.h"
#include "Core/Math/Primitive/AABB.h"
#include "Game/Entity/GameObject.h"

namespace Engine
{
	struct __declspec(align(16)) RenderNode
	{
	public:
		mat4 mWorld;			// 64-bytes
		AABB mBounds;			// 32-bytes
		float4 mCentroid;		// 16-bytes
		GameObject* mObject;	// 8-bytes
		int mID;				// 4-bytes

	private:
		int pad;				// 4-bytes

	public:
		RenderNode(const mat4& world, GameObject* obj, int id);

		ALIGNED_NEW_DELETE("Engine::RenderNode")
	};
}

#endif