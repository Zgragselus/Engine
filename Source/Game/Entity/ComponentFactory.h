#ifndef __COMPONENT_FACTORY__H__
#define __COMPONENT_FACTORY__H__

#include "GameObject.h"
#include "Component.h"
#include "Component/CameraComponent.h"
#include "Component/MeshComponent.h"
#include "Component/TextureComponent.h"
#include "Component/MaterialComponent.h"
#include "Component/LightComponent.h"
#include "Component/CollisionComponent.h"
#include "Component/RigidBodyComponent.h"
#include "Component/ComponentStatic.h"

namespace Engine
{
	class ComponentFactory
	{
	private:
		static Manager<Mesh>* mMeshManager;
		static Manager<Texture>* mTextureManager;

	public:
		static void Initialize(Manager<Mesh>* meshManager, Manager<Texture>* textureManager)
		{
			mMeshManager = meshManager;
			mTextureManager = textureManager;

			ComponentTypeId::Get<CameraComponent>();
			ComponentTypeId::Get<CollisionComponent>();
			ComponentTypeId::Get<LightComponent>();
			ComponentTypeId::Get<MaterialComponent>();
			ComponentTypeId::Get<MeshComponent>();
			ComponentTypeId::Get<RigidBodyComponent>();
		}

		static Component* CreateComponent(ComponentId component)
		{
			if (component == ComponentTypeId::Get<CameraComponent>())
			{
				return new CameraComponent(new PerspectiveCamera(float4(0.0f, 0.0f, 0.0f, 1.0f), float4(0.0f, 0.0f, 1.0f, 1.0f), float4(0.0f, 1.0f, 0.0f, 0.0f), Math::DegToRad(45.0f), 1.0f, 1.0f, 1000.0f, 1.0f));
			}
			else if (component == ComponentTypeId::Get<CollisionComponent>())
			{
				return new CollisionComponent(new Box(float4(1.0f, 1.0f, 1.0f, 0.0f)));
			}
			else if (component == ComponentTypeId::Get<LightComponent>())
			{
				return new LightComponent(new PointLight(float4(0.0f, 0.0f, 0.0f, 1.0f), 0.0f, 0.00001f));
			}
			else if (component == ComponentTypeId::Get<MaterialComponent>())
			{
				return new MaterialComponent(nullptr, nullptr, nullptr, nullptr, nullptr, mTextureManager);
			}
			else if (component == ComponentTypeId::Get<MeshComponent>())
			{
				return new MeshComponent(nullptr, mMeshManager);
			}
			else if (component == ComponentTypeId::Get<RigidBodyComponent>())
			{
				return new RigidBodyComponent(new RigidBody(0.0f));
			}
			else
			{
				return nullptr;
			}
		}
	};
}

#endif