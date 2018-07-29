#ifndef __COMPONENT_TYPE_ID__H__
#define __COMPONENT_TYPE_ID__H__

#include "Component.h"
#include <vector>
#include <map>

namespace Engine
{
	typedef unsigned int ComponentId;
	typedef std::vector<ComponentId> ComponentIdList;

	class ComponentTypeId
	{
	private:
		static unsigned int mCounter;
		static std::map<std::string, ComponentId> mNameComponent;
		static std::map<ComponentId, std::string> mComponentName;

	public:
		template <typename T>
		static const ComponentId Get()
		{
			static_assert((std::is_base_of<Component, T>::value && !std::is_same<Component, T>::value), "Component class must be derived from Component");
			static ComponentId id = mCounter++;
			mNameComponent.insert(std::pair<std::string, ComponentId>(T::GetName(), id));
			mComponentName.insert(std::pair<ComponentId, std::string>(id, T::GetName()));
			return id;
		}

		static ComponentId Get(const std::string& name)
		{
			return mNameComponent[name];
		}

		static const std::string& GetName(ComponentId component)
		{
			return mComponentName[component];
		}

		static unsigned int Count()
		{
			return mCounter;
		}

		ALIGNED_NEW_DELETE("Engine::ComponentTypeId")
	};
}

#endif