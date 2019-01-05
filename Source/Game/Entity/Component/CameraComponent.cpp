#include "ComponentStatic.h"
#include "Imgui/imgui.h"
#include "Core/Math/Numeric/Common.h"

namespace Engine
{
	bool CameraComponent::Editor(std::string& prev, std::string& next)
	{
		bool change = false;

		float4 position = mCamera->mPosition;
		float4 target = mCamera->mTarget;
		float4 up = mCamera->mUp;

		float speed = mCamera->mSpeed;
		if (ImGui::InputFloat("Speed", &speed))
		{
			change = true;
		}

		int type = (int)mCamera->mType;
		if (ImGui::Combo("Camera type", &type, "Orthogonal\0Perspective"))
		{
			if (type != (int)mCamera->mType)
			{
				change = true;
			}
		}

		float leftPlane, rightPlane, topPlane, bottomPlane, nearPlane, farPlane;
		float fov, aspect;

		switch (mCamera->mType)
		{
		case Camera::Type::ORTHOGONAL:
			leftPlane = ((OrthoCamera*)mCamera)->mLeft;
			rightPlane = ((OrthoCamera*)mCamera)->mRight;
			topPlane = ((OrthoCamera*)mCamera)->mTop;
			bottomPlane = ((OrthoCamera*)mCamera)->mBottom;
			nearPlane = ((OrthoCamera*)mCamera)->mNear;
			farPlane = ((OrthoCamera*)mCamera)->mFar;
			if (ImGui::InputFloat("Left Plane", &leftPlane)) { change = true; }
			if (ImGui::InputFloat("Right Plane", &rightPlane)) { change = true; }
			if (ImGui::InputFloat("Top Plane", &topPlane)) { change = true; }
			if (ImGui::InputFloat("Bottom Plane", &bottomPlane)) { change = true; }
			if (ImGui::InputFloat("Near Plane", &nearPlane)) { change = true; }
			if (ImGui::InputFloat("Far Plane", &farPlane)) { change = true; }
			break;

		case Camera::Type::PERSPECTIVE:
			nearPlane = ((PerspectiveCamera*)mCamera)->mNear;
			farPlane = ((PerspectiveCamera*)mCamera)->mFar;
			fov = ((PerspectiveCamera*)mCamera)->mFov;
			aspect = ((PerspectiveCamera*)mCamera)->mAspect;
			if (ImGui::InputFloat("Near Clipping Plane", &nearPlane)) { change = true; }
			if (ImGui::InputFloat("Far Clipping Plane", &farPlane)) { change = true; }
			if (ImGui::DragFloat("Field-of-view", &fov, 0.01f, 0.01f, Math::PI)) { change = true; }
			break;
		}

		if (change)
		{
			prev = Serialize().c_str();

			if (type != (int)mCamera->mType)
			{
				switch (type)
				{
				case 0:
					delete mCamera;
					mCamera = new OrthoCamera(position, target, up, leftPlane, rightPlane, topPlane, bottomPlane, nearPlane, farPlane, speed);
					break;

				case 1:
					delete mCamera;
					mCamera = new PerspectiveCamera(position, target, up, fov, aspect, nearPlane, farPlane, speed);
					break;

				default:
					break;
				}
			}

			switch (mCamera->mType)
			{
			case Camera::Type::ORTHOGONAL:
				((OrthoCamera*)mCamera)->mLeft = leftPlane;
				((OrthoCamera*)mCamera)->mRight = rightPlane;
				((OrthoCamera*)mCamera)->mTop = topPlane;
				((OrthoCamera*)mCamera)->mBottom = bottomPlane;
				((OrthoCamera*)mCamera)->mNear = nearPlane;
				((OrthoCamera*)mCamera)->mFar = farPlane;
				((OrthoCamera*)mCamera)->mSpeed = speed;
				((OrthoCamera*)mCamera)->mValid = false;
				((OrthoCamera*)mCamera)->mProjectionValid = false;
				printf("Right %f\n", ((OrthoCamera*)mCamera)->mRight);
				break;

			case Camera::Type::PERSPECTIVE:
				((PerspectiveCamera*)mCamera)->mFov = fov;
				((PerspectiveCamera*)mCamera)->mNear = nearPlane;
				((PerspectiveCamera*)mCamera)->mFar = farPlane;
				((PerspectiveCamera*)mCamera)->mSpeed = speed;
				((PerspectiveCamera*)mCamera)->mValid = false;
				((PerspectiveCamera*)mCamera)->mProjectionValid = false;
				break;
			}

			next = Serialize().c_str();

			if (next == prev)
			{
				change = false;
			}
		}

		return change;
	}

	void CameraComponent::Deserialize(const std::string& s)
	{
		std::stringstream ss(s);

		std::string name;
		float tmp;
		int type;
		ss >> name >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp >> type;

		if (type != (int)(mCamera->mType))
		{
			switch (type)
			{
			case 0:
				delete mCamera;
				mCamera = new OrthoCamera(Engine::float4(0.0f, 0.0f, 0.0f, 1.0f),
					Engine::float4(0.0f, 0.0f, 1.0f, 1.0f),
					Engine::float4(0.0f, 1.0f, 0.0f, 1.0f),
					-1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f);
				break;

			case 1:
				delete mCamera;
				mCamera = new PerspectiveCamera(Engine::float4(0.0f, 0.0f, 0.0f, 1.0f),
					Engine::float4(0.0f, 0.0f, 0.0f, 1.0f),
					Engine::float4(0.0f, 0.0f, 0.0f, 1.0f),
					0.5f, 1.0f, 1.0f, 1000.0f, 1.0f);
				break;

			default:
				break;
			}
		}

		std::vector<std::string> lines = String::Split(s, '\n');

		std::vector<std::string> dataVec;
		dataVec.push_back(lines[1]);
		dataVec.push_back(lines[2]);
		std::string data = String::Join(dataVec, '\n');

		mCamera->Deserialize(data);
	}
}