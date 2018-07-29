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

#include "Core/Math/Numeric/Float4.h"
#include "Core/Math/Numeric/Quat.h"
#include "Core/Math/Numeric/Mat4.h"
#include "Core/Memory/Memory.h"
#include "Transformation.h"
#include <vector>

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

		/// <summary>Entity parent</summary>
		Entity* mParent;

		/// <summary>Entity children</summary>
		std::vector<Entity*> mChildren;

	private:
		/// <summary>Entity counter - used for F.e. naming entites</summary>
		static int mEntityCounter;

	public:
		/// <summary>Constructor</summary>
		/// <param name="parent">Parent entity - NULL means that this is root entity</param>
		Entity(Entity* parent = NULL)
		{
			mTransform = Transformation();
			mParent = parent;
		}

		/// <summary>Destructor, make sure to remove children and detach from parent</summary>
		virtual ~Entity()
		{
			// Remove children
			for (auto it = mChildren.begin(); it != mChildren.end(); it++)
			{
				delete (*it);
			}
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

		/// <summary>Render the entity</summary>
		virtual void Render(const mat4& world, const mat4& view, const mat4& projection)
		{

		}

		/// <summary>Add new child for this entity</summary>
		/// <param name="e">Pointer to child entity</param>
		virtual void AddChild(Entity* e)
		{
			mChildren.push_back(e);
		}

		/// <summary>Remove one of entity childs</summary>
		/// <param name="e">Pointer to entity which we want to remove from children of this entity</param>
		/// <param name="cleanup">Do we want to free the entity 'e'</param>
		virtual void RemoveChild(Entity* e, bool cleanup = true)
		{
			for (auto it = mChildren.begin();
				it != mChildren.end();
				it++)
			{
				if ((*it) == e)
				{
					if (cleanup)
					{
						delete (*it);
					}

					mChildren.erase(it);
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

		ALIGNED_NEW_DELETE("Engine::Entity")
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOH

#endif