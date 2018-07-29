#include "RenderNode.h"
#include "Game/Entity/Entity.h"

using namespace Engine;

RenderNode::RenderNode(const mat4& world, GameObject* obj, int id) :
	mWorld(world), mObject(obj), mID(id)
{
	if (obj->Has<MeshComponent>())
	{
		mBounds = obj->Get<MeshComponent>()->GetMesh()->Bounds().TransformAABB(world);
		mCentroid = (mBounds.mMin + mBounds.mMax) * 0.5f;
	}
	else
	{
		mBounds = AABB(float4(0.0f, 0.0f, 0.0f, 1.0f)).TransformAABB(world);
		mCentroid = mBounds.mMin;
	}
}