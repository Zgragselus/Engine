#include "ComponentTypeId.h"

using namespace Engine;

unsigned int ComponentTypeId::mCounter = 0;
std::map<std::string, ComponentId> ComponentTypeId::mNameComponent;
std::map<ComponentId, std::string> ComponentTypeId::mComponentName;