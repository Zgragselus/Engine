#ifndef __RIGIDBODY_COMPONENT__H__
#define __RIGIDBODY_COMPONENT__H__

#include "../Component.h"
#include "Physics/Body/RigidBody.h"

namespace Engine
{
	class __declspec(align(16)) RigidBodyComponent : public Component
	{
	private:
		static std::string mComponentName;
		RigidBody* mBody;

	public:
		RigidBodyComponent(RigidBody* body)
		{
			mBody = body;
		}

		virtual ~RigidBodyComponent()
		{
			delete mBody;
		}

		RigidBody* Get()
		{
			return mBody;
		}

		static std::string& GetName()
		{
			return mComponentName;
		}

		virtual bool Editor(std::string& prev, std::string& next);

		virtual std::string Serialize()
		{
			std::stringstream ss;

			ss << "RigidBodyComponent" << std::endl;

			return (ss.str() + mBody->Serialize());
		}

		virtual void Deserialize(const std::string& s);

		ALIGNED_NEW_DELETE("Engine::RigidBodyComponent")
	};
}

#endif