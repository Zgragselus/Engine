///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Entity.h
//
// Generalized Entity on which whole Entity system stands. Entity instances are held in scene 
// and represent all actual objects inside it - be it geometry, triggers, etc.
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __ENTITY__H__
#define __ENTITY__H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#define NOMINMAX

#include "Core/Math/Numeric/Float4.h"
#include "Core/Math/Numeric/Quat.h"
#include "Core/Math/Numeric/Mat4.h"
#include "Core/Memory/Memory.h"
#include "Transformation.h"
#include <vector>
#include <stack>
#include "ComponentFactory.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	/// <summary>
	///	Entity class is a most-generic class type. Any usable entity-class should derive
	/// </summary>
	class Entity
	{
	protected:
		/// <summary>Entity transformation</summary>
		Transformation mTransform;

		/// <summary>Game Object of Entity</summary>
		GameObject mObject;

		/// <summary>Entity parent</summary>
		Entity* mParent;

		/// <summary>Entity children</summary>
		std::vector<Entity*> mChildren;

		/// <summary>Entity name</summary>
		std::string mName;

	private:
		/// <summary>Entity counter - used for F.e. naming entites</summary>
		static int mEntityCounter;

	public:
		/// <summary>Scene-assigned ID</summary>
		unsigned int mSceneID;

		/// <summary>Constructor</summary>
		/// <param name="parent">Parent entity - NULL means that this is root entity</param>
		Entity(const std::string& name, Entity* parent = NULL)
		{
			mName = name;
			mTransform = Transformation();
			mParent = parent;
			mObject.SetEntity(this);

			if (mParent)
			{
				mParent->AddChild(this);
			}
		}

		/// <summary>Destructor, make sure to remove children and detach from parent</summary>
		virtual ~Entity()
		{
			// Get list of children to remove
			std::vector<Entity*> clear = std::vector<Entity*>(mChildren.size());
			for (auto it = mChildren.begin(); it != mChildren.end(); it++)
			{
				clear.push_back(*it);
			}

			// Destroy each object (NOTE: this will also clear their iterator from mChildren)
			for (auto e : clear)
			{
				delete e;
			}

			// Clear any remaining
			mChildren.clear();

			// Detach from parent (if any)
			if (mParent)
			{
				Entity* parent = mParent;
				mParent = NULL;
				parent->RemoveChild(this, false);
			}
		}

		/// <summary>Update entity</summary>
		virtual void Update()
		{
			mTransform.Update();
		}

		/// <summary>Add new child for this entity</summary>
		/// <param name="e">Pointer to child entity</param>
		virtual void AddChild(Entity* e)
		{
			if (e->GetParent())
			{
				e->GetParent()->RemoveChild(e, false);
			}
			e->SetParent(this);
			mChildren.push_back(e);
		}

		/// <summary>Remove one of entity childs</summary>
		/// <param name="e">Pointer to entity which we want to remove from children of this entity</param>
		/// <param name="cleanup">Do we want to free the entity 'e'</param>
		virtual void RemoveChild(Entity* e, bool cleanup = true)
		{
			for (auto it = mChildren.begin();
				it != mChildren.end();
				)
			{
				if ((*it) == e)
				{
					if (cleanup)
					{
						delete (*it);
					}

					it = mChildren.erase(it);
				}
				else
				{
					it++;
				}
			}
		}

		/// <summary>Get reference to children </summary>
		virtual std::vector<Entity*>& Children()
		{
			return mChildren;
		}

		/// <summary>Get reference to entity transformation</summary>
		virtual Transformation& Transformation()
		{
			return mTransform;
		}

		/// <summary>Get reference to game object</summary>
		virtual GameObject& GameObject()
		{
			return mObject;
		}

		virtual mat4 GetWorldMatrix()
		{
			std::stack<mat4> matrices;

			mat4 result = mat4();

			Entity* t = this;
			do
			{
				matrices.push(t->Transformation().Get());
				t = t->mParent;
			} while (t != nullptr);

			while (!matrices.empty())
			{
				result = result * matrices.top();
				matrices.pop();
			}

			return result;
		}

		inline void SetParent(Entity* parent)
		{
			mParent = parent;
		}

		inline Entity* GetParent()
		{
			return mParent;
		}

		inline const std::string& GetName() const
		{
			return mName;
		}

		inline void SetName(const std::string& name)
		{
			mName = name;
		}

		std::string Serialize();

		void Deserialize(Scene* scene, const std::string& s, Entity* parent = nullptr);

		ALIGNED_NEW_DELETE("Engine::Entity")
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOH

#endif