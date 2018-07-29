///////////////////////////////////////////////////////////////////////////////////////////////////
//
// MeshEntity.h
//
// Generic mesh entity class, without any additional properties.
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __MODEL__ENTITY__H__
#define __MODEL__ENTITY__H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "../Entity.h"
//#include "Graphics/Resources/Model/Model.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	/// <summary>
	///	Camera Entity class derives Entity class and defines basic parameters for cameras
	/// </summary>
	class ModelEntity : public Entity
	{
	protected:
		//Model* mModel;

	private:
		/// <summary>Entity counter - used for F.e. naming entites</summary>
		static int mEntityCounter;

	public:
		/// <summary>Constructor</summary>
		/// <param name="parent">Parent entity - NULL means that this is root entity</param>
		ModelEntity(/*Model* model, */Entity* parent = NULL) : Entity(parent)
		{
			//mModel = model;
		}

		/// <summary>Virtual destructor</summary>
		virtual ~ModelEntity()
		{
		}

		/// <summary>Getter for camera pointer</summary>
		/*Model* Get()
		{
			return mModel;
		}*/

		/// <summary>Render the model entity</summary>
		virtual void Render(const mat4& world, const mat4& view, const mat4& projection)
		{
			//mModel->Render(world, view, projection);
		}

		ALIGNED_NEW_DELETE("Engine::CameraEntity")
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOH

#endif