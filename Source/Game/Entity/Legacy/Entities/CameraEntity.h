///////////////////////////////////////////////////////////////////////////////////////////////////
//
// CameraEntity.h
//
// Generalizes all cameras under camera entity class. 
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __CAMERA__ENTITY__H__
#define __CAMERA__ENTITY__H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "../Entity.h"
//#include "Graphics/Resources/Camera/Camera.h"
//#include "Graphics/Resources/Camera/PerspectiveCamera.h"
//#include "Graphics/Resources/Camera/OrthoCamera.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	/// <summary>
	///	Camera Entity class derives Entity class and defines basic parameters for cameras
	/// </summary>
	class CameraEntity : public Entity
	{
	protected:
		/// <summary>Pointer to camera held by the camera entity</summary>
		//Camera* mCamera;

	private:
		/// <summary>Entity counter - used for F.e. naming entites</summary>
		static int mEntityCounter;

	public:
		/// <summary>Constructor</summary>
		/// <param name="parent">Parent entity - NULL means that this is root entity</param>
		CameraEntity(/*Camera* camera, */Entity* parent = NULL) : Entity(parent)
		{
			//mCamera = camera;
		}

		/// <summary>Virtual destructor</summary>
		virtual ~CameraEntity()
		{
		}

		/// <summary>Update entity</summary>
		virtual void Update()
		{
			mTransform.Update();
			//mCamera->Process(1.0f);
		}
		
		/// <summary>Getter for camera pointer</summary>
		/*Camera* Get()
		{
			return mCamera;
		}*/

		ALIGNED_NEW_DELETE("Engine::CameraEntity")
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOH

#endif