#include "CameraComponent.h"
#include "LightComponent.h"
#include "MaterialComponent.h"
#include "MeshComponent.h"
#include "TextureComponent.h"
#include "CollisionComponent.h"
#include "RigidBodyComponent.h"
#include "ComponentStatic.h"
#include "Imgui/imgui.h"

using namespace Engine;

std::string Component::mComponentName = "Undefined";
std::string CameraComponent::mComponentName = "CameraComponent";
std::string LightComponent::mComponentName = "LightComponent";
std::string MaterialComponent::mComponentName = "MaterialComponent";
std::string MeshComponent::mComponentName = "MeshComponent";
std::string TextureComponent::mComponentName = "TextureComponent";
std::string CollisionComponent::mComponentName = "CollisionComponent";
std::string RigidBodyComponent::mComponentName = "RigidBodyComponent";
