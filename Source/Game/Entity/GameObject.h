#ifndef __GAME_OBJECT__H__
#define __GAME_OBJECT__H__

#include "ComponentTypeId.h"
#include <map>

namespace Engine
{
	class Entity;

	class Scene;

	class GameObject
	{
	private:
		std::map<ComponentId, Component*> mComponents;
		Entity* mEntity;

	public:
		GameObject* AddComponent(const ComponentId compIdx, Component* component)
		{
			mComponents[compIdx] = component;
			return NULL;
		}

		GameObject* RemoveComponent(const ComponentId compIdx)
		{
			delete mComponents[compIdx];
			mComponents.erase(compIdx);
			return NULL;
		}

		Component* GetComponent(const ComponentId compIdx) const
		{
			return mComponents.at(compIdx);
		}

		bool HasComponent(const ComponentId compIdx) const
		{
			return (mComponents.find(compIdx) != mComponents.end());
		}

	protected:
		template <typename T, typename... TArgs> inline Component* CreateComponent(TArgs&&... args);

	public:
		~GameObject()
		{
			while (mComponents.size() > 0)
			{
				RemoveComponent(mComponents.begin()->first);
			}

			mComponents.clear();
		}

		void SetEntity(Entity* ent)
		{
			mEntity = ent;
		}

		Entity* GetEntity()
		{
			return mEntity;
		}

		template <typename T, typename... TArgs> inline GameObject* Add(TArgs&... args);
		template <typename T> inline GameObject* Remove();
		template <typename T> inline T* Get() const;
		template <typename T> inline bool Has() const;

		size_t GetCount() { return mComponents.size(); }
		std::map<ComponentId, Component*>::iterator ComponentsItBegin() { return mComponents.begin(); }
		std::map<ComponentId, Component*>::iterator ComponentsItEnd() { return mComponents.end(); }
		
		ALIGNED_NEW_DELETE("Engine::GameObject")
	};

	template <typename T, typename... TArgs>
	Component* GameObject::CreateComponent(TArgs&&... args)
	{
		T* component = new T(std::forward<TArgs>(args)...);
		return component;
	}

	template <typename T, typename... TArgs> GameObject* GameObject::Add(TArgs&... args)
	{
		return AddComponent(ComponentTypeId::Get<T>(), CreateComponent<T>(std::forward<TArgs>(args)...));
	}

	template <typename T> GameObject* GameObject::Remove()
	{
		return RemoveComponent(ComponentTypeId::Get<T>());
	}

	template <typename T> T* GameObject::Get() const
	{
		return static_cast<T*>(GetComponent(ComponentTypeId::Get<T>()));
	}

	template <typename T> bool GameObject::Has() const
	{
		return HasComponent(ComponentTypeId::Get<T>());
	}
}

#endif