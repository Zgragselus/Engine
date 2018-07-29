#ifndef __CAMERA_COMPONENT__H__
#define __CAMERA_COMPONENT__H__

#include "../Component.h"
#include "Graphics/Common/Camera/Camera.h"
#include "Graphics/Common/Camera/PerspectiveCamera.h"
#include "Graphics/Common/Camera/OrthoCamera.h"

namespace Engine
{
	class __declspec(align(16)) CameraComponent : public Component
	{
	private:
		static std::string mComponentName;
		Camera* mCamera;

	public:
		CameraComponent(Camera* camera)
		{
			mCamera = camera;
		}

		virtual ~CameraComponent()
		{
			delete mCamera;
			//mCamera = nullptr;
		}

		Camera* Get()
		{
			return mCamera;
		}

		static std::string& GetName()
		{
			return mComponentName;
		}

		virtual bool Editor(std::string& prev, std::string& next);
		
		virtual std::string Serialize()
		{
			std::stringstream ss;

			ss << "CameraComponent" << std::endl;

			return (ss.str() + mCamera->Serialize());
		}

		virtual void Deserialize(const std::string& s);

		ALIGNED_NEW_DELETE("Engine::CameraComponent")
	};
}

#endif