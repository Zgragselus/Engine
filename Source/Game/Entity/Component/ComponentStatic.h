#ifndef __COMPONENT_STATIC__H__
#define __COMPONENT_STATIC__H__

#include "CameraComponent.h"
#include "CollisionComponent.h"
#include "LightComponent.h"
#include "MaterialComponent.h"
#include "MeshComponent.h"
#include "RigidBodyComponent.h"
#include "TextureComponent.h"

namespace Engine
{
	class ComponentStatic
	{
	public:
		static MaterialComponent* mEditedComponent;
		static std::string mEditedDiffuse;
		static std::string mEditedNormals;
		static std::string mEditedHeight;
		static std::string mEditedRoughness;
		static std::string mEditedMetallic;
		static int mEditedTexture;

		static MeshComponent* mEditedMeshComponent;
		static std::string mEditedMeshName;
		static int mEditedMesh;
	};
}

#endif // !__COMPONENT_STATIC__H__
