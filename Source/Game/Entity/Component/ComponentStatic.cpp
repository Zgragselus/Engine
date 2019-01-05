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

int LightComponent::mResolutionTableSize = 6;
int LightComponent::mResolutionTable[6] = { 64, 128, 256, 512, 1024, 2048 };

bool LightComponent::Editor(std::string& prev, std::string& next)
{
	bool change = false;

	float4 color = mLight->mColor;

	if (ImGui::ColorEdit4("Light color", (float*)&color)) {	change = true; }

	int type = (int)mLight->mType;
	if (ImGui::Combo("Light type", &type, "Point\0Spot\0Directional"))
	{
		if (type != (int)mLight->mType)
		{
			change = true;
		}
	}

	float temperature = mLight->mTemperature;
	if (ImGui::SliderFloat("Temperature (K)", &temperature, 1500.0f, 15000.0f)) { change = true; }

	float power = mLight->mPower;
	if (ImGui::InputFloat("Power (lm)", &power)) { change = true; }

	float nearPlane = mLight->mNear;
	if (ImGui::InputFloat("Shadow Near", (float*)&nearPlane)) { change = true; }

	float farPlane = mLight->mFar;
	if (ImGui::InputFloat("Shadow Far", (float*)&farPlane)) { change = true; }

	bool shadowsEnabled = mLight->mShadowEnabled == 1 ? true : false;
	if (ImGui::Checkbox("Shadows Enabled", &shadowsEnabled)) { change = true; }

	int shadowsFilter = (int)mLight->mFilter;
	if (ImGui::Combo("Shadows Filter", &shadowsFilter, "Nearest\0Bilinear\0Bicubic\0PCF\0PCSS\0PCMLSM\0BACKPROJECTION\0")) { change = true; }

	float filterSize = mLight->mFilterSize;
	if (ImGui::DragFloat("Shadows Filter Size", &filterSize, 0.0005f, 0.0f, 1.0f, "%.4f")) { change = true; }

	int resolution = 0;
	for (int i = 0; i < mResolutionTableSize; i++)
	{
		if (mResolutionTable[i] == (int)mLight->GetResolution())
		{
			resolution = i;
		}
	}
	if (ImGui::Combo("Shadow Map Resolution", &resolution, "   64 \0  128 \0  256 \0  512 \0 1024 \0 2048 ")) { change = true; }

	float lightSize = mLight->mLightSize;
	switch (mLight->mFilter)
	{
	case Light::ShadowFilter::NEAREST:
		break;

	case Light::ShadowFilter::BILINEAR:
		break;

	case Light::ShadowFilter::BICUBIC:
		break;

	case Light::ShadowFilter::PCF:
		if (ImGui::DragFloat("Light Size", &lightSize, 1.0f, 0.0f, 5.0f, "%.4f")) { change = true; }
		break;

	case Light::ShadowFilter::PCSS:
		if (ImGui::DragFloat("Light Size", &lightSize, 0.05f, 0.0f, 10.0f, "%.4f")) { change = true; }
		break;

	case Light::ShadowFilter::PCMLSM:
		if (ImGui::DragFloat("Light Size", &lightSize, 0.05f, 0.0f, 20.0f, "%.4f")) { change = true; }
		break;

	case Light::ShadowFilter::BACKPROJECTION:
		if (ImGui::DragFloat("Light Size", &lightSize, 0.05f, 0.0f, 50.0f, "%.4f")) { change = true; }
		break;
	}

	float linearAtt;
	float quadraticAtt;
	float spotAngle;
	float4 direction;

	switch (mLight->mType)
	{
	case Light::Type::POINT:
		linearAtt = ((PointLight*)mLight)->mLinearAtt;
		quadraticAtt = ((PointLight*)mLight)->mQuadraticAtt;
		if (ImGui::InputFloat("Linear Attenuation", &linearAtt)) { change = true; }
		if (ImGui::InputFloat("Quadratic Attenuation", &quadraticAtt)) { change = true; }
		break;

	case Light::Type::SPOT:
		linearAtt = ((SpotLight*)mLight)->mLinearAtt;
		quadraticAtt = ((SpotLight*)mLight)->mQuadraticAtt;
		spotAngle = ((SpotLight*)mLight)->mSpotAngle;
		direction = ((SpotLight*)mLight)->mDirection;
		if (ImGui::InputFloat("Linear Attenuation", &linearAtt)) { change = true; }
		if (ImGui::InputFloat("Quadratic Attenuation", &quadraticAtt)) { change = true; }
		if (ImGui::InputFloat("Spot Angle", &spotAngle)) { change = true; }
		if (ImGui::InputFloat4("Direction", (float*)&direction)) { change = true; }
		break;

	case Light::Type::DIRECTIONAL:
		direction = ((DirectionalLight*)mLight)->mDirection;
		if (ImGui::InputFloat4("Direction", (float*)&direction)) { change = true; }
		break;
	}

	if (change)
	{
		prev = Serialize().c_str();

		mLight->mColor = color;

		if (type != (int)mLight->mType)
		{
			switch (type)
			{
			case 0:
				delete mLight;
				mLight = new PointLight(color, 0.001f, 0.001f);
				break;

			case 1:
				delete mLight;
				mLight = new SpotLight(color, float4(1.0f, 0.0f, 0.0f, 0.0f), Math::PI / 4.0f, 0.001f, 0.001f);
				break;

			case 2:
				delete mLight;
				mLight = new DirectionalLight(color, float4(0.0f, -1.0f, 0.0f, 0.0f));
				break;

			default:
				break;
			}
		}

		if (mLight->mFilter != (Light::ShadowFilter)shadowsFilter)
		{
			mLight->mLightSize = 1.0f;
		}

		mLight->mNear = nearPlane;
		mLight->mFar = farPlane;
		mLight->mShadowEnabled = shadowsEnabled ? 1 : 0;
		mLight->mFilter = (Light::ShadowFilter)shadowsFilter;
		mLight->mFilterSize = filterSize;
		mLight->mResolution = mResolutionTable[resolution];
		mLight->mLightSize = lightSize;
		mLight->mTemperature = temperature;
		mLight->mPower = power;

		switch (mLight->mType)
		{
		case Light::Type::POINT:
			((PointLight*)mLight)->mLinearAtt = linearAtt;
			((PointLight*)mLight)->mQuadraticAtt = quadraticAtt;
			break;

		case Light::Type::SPOT:
			((PointLight*)mLight)->mLinearAtt = linearAtt;
			((SpotLight*)mLight)->mQuadraticAtt = quadraticAtt;
			((SpotLight*)mLight)->mSpotAngle = spotAngle;
			((SpotLight*)mLight)->mDirection = normalize(direction);
			break;

		case Light::Type::DIRECTIONAL:
			((DirectionalLight*)mLight)->mDirection = normalize(direction);
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

void LightComponent::Deserialize(const std::string& s)
{
	std::stringstream ss(s);

	std::string name;
	float tmp;
	int type;
	ss >> name >> tmp >> tmp >> tmp >> tmp >> type;

	if (type != (int)(mLight->mType))
	{
		switch (type)
		{
		case 0:
			delete mLight;
			mLight = new PointLight(float4(0.0f, 0.0f, 0.0f, 0.0f), 0.001f, 0.001f);
			break;

		case 1:
			delete mLight;
			mLight = new SpotLight(float4(0.0f, 0.0f, 0.0f, 0.0f), float4(1.0f, 0.0f, 0.0f, 0.0f), Math::PI / 4.0f, 0.001f, 0.001f);
			break;

		case 2:
			delete mLight;
			mLight = new DirectionalLight(float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, -1.0f, 0.0f, 0.0f));
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

	mLight->Deserialize(data);
}

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

MaterialComponent* ComponentStatic::mEditedComponent = nullptr;
std::string ComponentStatic::mEditedDiffuse;
std::string ComponentStatic::mEditedNormals;
std::string ComponentStatic::mEditedHeight;
std::string ComponentStatic::mEditedRoughness;
std::string ComponentStatic::mEditedMetallic;
int ComponentStatic::mEditedTexture = -1;

bool MaterialComponent::Editor(std::string& prev, std::string& next)
{
	if (ComponentStatic::mEditedComponent == nullptr &&
		ComponentStatic::mEditedTexture == -1)
	{
		ComponentStatic::mEditedDiffuse = mDiffuseMap->mItem->GetName();
		ComponentStatic::mEditedNormals = mNormalsMap->mItem->GetName();
		ComponentStatic::mEditedHeight = mHeightMap->mItem->GetName();
		ComponentStatic::mEditedRoughness = mRoughnessMap->mItem->GetName();
		ComponentStatic::mEditedMetallic = mMetallicMap->mItem->GetName();
	}

	bool dragdropChange = false;

	prev = Serialize().c_str();

	ImGui::Text("Diffuse Map");
	//if (ImGui::Button(ComponentStatic::mEditedDiffuse.c_str()))
	ImGui::ImageButton((ImTextureID)(mDiffuseMap->mItem->GetSRV().mGpuHandle.ptr), ImVec2(64, 64));
	if (ImGui::BeginDragDropSource())
	{
		ImGui::SetDragDropPayload("RESOURCE_TEXTURE", &mDiffuseMap, sizeof(void*));
		ImGui::Image((ImTextureID)(mDiffuseMap->mItem->GetSRV().mGpuHandle.ptr), ImVec2(64, 64));
		ImGui::EndDragDropSource();
	}
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RESOURCE_TEXTURE"))
		{
			mDiffuseMap = *((Engine::Manager<Engine::Texture>::Node**)payload->Data);
			dragdropChange = true;
		}
		ImGui::EndDragDropTarget();
	}
	if (ImGui::Button("Select Diffuse Map"))
	{
		ComponentStatic::mEditedComponent = this;
		ComponentStatic::mEditedTexture = 0;
		printf("Diffuse Map\n");
	}

	ImGui::Text("Normals Map");
	//if (ImGui::Button(ComponentStatic::mEditedNormals.c_str()))
	ImGui::ImageButton((ImTextureID)(mNormalsMap->mItem->GetSRV().mGpuHandle.ptr), ImVec2(64, 64));
	if (ImGui::BeginDragDropSource())
	{
		ImGui::SetDragDropPayload("RESOURCE_TEXTURE", &mNormalsMap, sizeof(void*));
		ImGui::Image((ImTextureID)(mNormalsMap->mItem->GetSRV().mGpuHandle.ptr), ImVec2(64, 64));
		ImGui::EndDragDropSource();
	}
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RESOURCE_TEXTURE"))
		{
			mNormalsMap = *((Engine::Manager<Engine::Texture>::Node**)payload->Data);
			dragdropChange = true;
		}
		ImGui::EndDragDropTarget();
	}
	if (ImGui::Button("Select Normals Map"))
	{
		ComponentStatic::mEditedComponent = this;
		ComponentStatic::mEditedTexture = 1;
	}

	ImGui::Text("Height Map");
	//if (ImGui::Button(ComponentStatic::mEditedHeight.c_str()))
	ImGui::ImageButton((ImTextureID)(mHeightMap->mItem->GetSRV().mGpuHandle.ptr), ImVec2(64, 64));
	if (ImGui::BeginDragDropSource())
	{
		ImGui::SetDragDropPayload("RESOURCE_TEXTURE", &mHeightMap, sizeof(void*));
		ImGui::Image((ImTextureID)(mHeightMap->mItem->GetSRV().mGpuHandle.ptr), ImVec2(64, 64));
		ImGui::EndDragDropSource();
	}
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RESOURCE_TEXTURE"))
		{
			mHeightMap = *((Engine::Manager<Engine::Texture>::Node**)payload->Data);
			dragdropChange = true;
		}
		ImGui::EndDragDropTarget();
	}
	if (ImGui::Button("Select Height Map"))
	{
		ComponentStatic::mEditedComponent = this;
		ComponentStatic::mEditedTexture = 2;
	}

	ImGui::Text("Roughness Map");
	//if (ImGui::Button(ComponentStatic::mEditedRoughness.c_str()))
	ImGui::ImageButton((ImTextureID)(mRoughnessMap->mItem->GetSRV().mGpuHandle.ptr), ImVec2(64, 64));
	if (ImGui::BeginDragDropSource())
	{
		ImGui::SetDragDropPayload("RESOURCE_TEXTURE", &mRoughnessMap, sizeof(void*));
		ImGui::Image((ImTextureID)(mRoughnessMap->mItem->GetSRV().mGpuHandle.ptr), ImVec2(64, 64));
		ImGui::EndDragDropSource();
	}
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RESOURCE_TEXTURE"))
		{
			mRoughnessMap = *((Engine::Manager<Engine::Texture>::Node**)payload->Data);
			dragdropChange = true;
		}
		ImGui::EndDragDropTarget();
	}
	if (ImGui::Button("Select Roughness Map"))
	{
		ComponentStatic::mEditedComponent = this;
		ComponentStatic::mEditedTexture = 3;
	}

	ImGui::Text("Metallic Map");
	//if (ImGui::Button(ComponentStatic::mEditedMetallic.c_str()))
	ImGui::ImageButton((ImTextureID)(mMetallicMap->mItem->GetSRV().mGpuHandle.ptr), ImVec2(64, 64));
	if (ImGui::BeginDragDropSource())
	{
		ImGui::SetDragDropPayload("RESOURCE_TEXTURE", &mMetallicMap, sizeof(void*));
		ImGui::Image((ImTextureID)(mMetallicMap->mItem->GetSRV().mGpuHandle.ptr), ImVec2(64, 64));
		ImGui::EndDragDropSource();
	}
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RESOURCE_TEXTURE"))
		{
			mMetallicMap = *((Engine::Manager<Engine::Texture>::Node**)payload->Data);
			dragdropChange = true;
		}
		ImGui::EndDragDropTarget();
	}
	if (ImGui::Button("Select Metallic Map"))
	{
		ComponentStatic::mEditedComponent = this;
		ComponentStatic::mEditedTexture = 4;
	}

	if (dragdropChange)
	{
		next = Serialize().c_str();
	}
	else
	{
		std::stringstream ss;

		ss << "MaterialComponent" << std::endl;
		ss << ComponentStatic::mEditedDiffuse << std::endl;
		ss << ComponentStatic::mEditedNormals << std::endl;
		ss << ComponentStatic::mEditedHeight << std::endl;
		ss << ComponentStatic::mEditedRoughness << std::endl;
		ss << ComponentStatic::mEditedMetallic << std::endl;

		next = ss.str();
	}

	if (next != prev)
	{
		switch (ComponentStatic::mEditedTexture)
		{
		case 0:
			mDiffuseMap = mTextureManager->GetNode(ComponentStatic::mEditedDiffuse);
			break;

		case 1:
			mNormalsMap = mTextureManager->GetNode(ComponentStatic::mEditedNormals);
			break;

		case 2:
			mHeightMap = mTextureManager->GetNode(ComponentStatic::mEditedHeight);
			break;

		case 3:
			mRoughnessMap = mTextureManager->GetNode(ComponentStatic::mEditedRoughness);
			break;

		case 4:
			mMetallicMap = mTextureManager->GetNode(ComponentStatic::mEditedMetallic);
			break;

		default:
			break;
		}

		ComponentStatic::mEditedTexture = -1;

		return true;
	}

	return false;
}

MeshComponent* ComponentStatic::mEditedMeshComponent;
std::string ComponentStatic::mEditedMeshName;
int ComponentStatic::mEditedMesh = -1;

bool MeshComponent::Editor(std::string& prev, std::string& next)
{
	if (ComponentStatic::mEditedMeshComponent == nullptr &&
		ComponentStatic::mEditedMesh == -1)
	{
		ComponentStatic::mEditedMeshName = mMesh->mItem->GetName();
	}

	bool dragdropChange = false;

	prev = Serialize().c_str();

	if (ImGui::Button(mMesh->mItem->GetName().c_str()))
	{
		ComponentStatic::mEditedMeshComponent = this;
		ComponentStatic::mEditedMesh = 0;
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RESOURCE_MESH"))
		{
			ComponentStatic::mEditedMeshName = (*((Engine::Mesh**)payload->Data))->GetName();
			dragdropChange = true;
		}

		ImGui::EndDragDropTarget();
	}

	std::stringstream ss;

	ss << "MeshComponent" << std::endl;
	ss << ComponentStatic::mEditedMeshName << std::endl;

	next = ss.str();

	if (next != prev)
	{
		mMesh = mManager->GetNode(ComponentStatic::mEditedMeshName);
		ComponentStatic::mEditedMesh = -1;

		return true;
	}

	return false;
}

void MeshComponent::Deserialize(const std::string& s)
{
	std::vector<std::string> lines = String::Split(s, '\n');
	mMesh = mManager->GetNode(lines[1]);
}

bool CollisionComponent::Editor(std::string& prev, std::string& next)
{
	bool change = false;
	float4 data;

	int type = (int)mShape->GetType();
	if (ImGui::Combo("Shape type", &type, "Box\0Capsule\0Compound\0Cone\0Convex Hull\0Cylinder\0Sphere\0Trimesh"))
	{
		if (type != (int)mShape->GetType())
		{
			change = true;
		}
	}

	switch ((Shape::Type)mShape->GetType())
	{
	case Shape::Type::BOX:
		data = ((Box*)mShape)->GetExtents();
		if (ImGui::InputFloat3("Extents", &(data[0]))) { change = true; }
		break;

	case Shape::Type::CAPSULE:
		data[0] = ((Capsule*)mShape)->GetRadius();
		data[1] = ((Capsule*)mShape)->GetHeight();
		if (ImGui::InputFloat("Radius", &(data[0]))) { change = true; }
		if (ImGui::InputFloat("Height", &(data[1]))) { change = true; }
		break;

	case Shape::Type::COMPOUND:
		//delete mShape;
		//mShape = new Compound(float4(1.0f, 1.0f, 1.0f, 1.0f));
		break;

	case Shape::Type::CONE:
		data[0] = ((Cone*)mShape)->GetRadius();
		data[1] = ((Cone*)mShape)->GetHeight();
		if (ImGui::InputFloat("Radius", &(data[0]))) { change = true; }
		if (ImGui::InputFloat("Height", &(data[1]))) { change = true; }
		break;

	case Shape::Type::CONVEX_HULL:
		//delete mShape;
		//mShape = new Box(float4(1.0f, 1.0f, 1.0f, 1.0f));
		break;

	case Shape::Type::CYLINDER:
		data = ((Cylinder*)mShape)->GetHalfExtents();
		if (ImGui::InputFloat3("Half extents", &(data[0]))) { change = true; }
		break;

	case Shape::Type::SPHERE:
		data[0] = ((Sphere*)mShape)->GetRadius();
		if (ImGui::InputFloat("Radius", &(data[0]))) { change = true; }
		break;

	case Shape::Type::TRIANGLE_MESH:
		break;
	}
	
	if (change)
	{
		prev = Serialize().c_str();

		if (type != (int)mShape->GetType())
		{
			switch ((Shape::Type)type)
			{
			case Shape::Type::BOX:
				delete mShape;
				mShape = new Box(float4(1.0f, 1.0f, 1.0f, 1.0f));
				break;

			case Shape::Type::CAPSULE:
				delete mShape;
				mShape = new Capsule(1.0f, 1.0f);
				break;

			case Shape::Type::COMPOUND:
				//delete mShape;
				//mShape = new Compound(float4(1.0f, 1.0f, 1.0f, 1.0f));
				break;

			case Shape::Type::CONE:
				delete mShape;
				mShape = new Cone(1.0f, 1.0f);
				break;

			case Shape::Type::CONVEX_HULL:
				//delete mShape;
				//mShape = new Box(float4(1.0f, 1.0f, 1.0f, 1.0f));
				break;

			case Shape::Type::CYLINDER:
				delete mShape;
				mShape = new Cylinder(float4(1.0f, 1.0f, 1.0f, 1.0f));
				break;

			case Shape::Type::SPHERE:
				delete mShape;
				mShape = new Sphere(1.0f);
				break;

			case Shape::Type::TRIANGLE_MESH:
				delete mShape;
				mShape = new TriangleMesh();
				break;

			default:
				break;
			}
		}

		switch ((Shape::Type)type)
		{
		case Shape::Type::BOX:
			((Box*)mShape)->mExtents = data;
			break;

		case Shape::Type::CAPSULE:
			((Capsule*)mShape)->mRadius = data[0];
			((Capsule*)mShape)->mHeight = data[1];
			break;

		case Shape::Type::COMPOUND:
			break;

		case Shape::Type::CONE:
			((Cone*)mShape)->mRadius = data[0];
			((Cone*)mShape)->mHeight = data[1];
			break;

		case Shape::Type::CONVEX_HULL:
			break;

		case Shape::Type::CYLINDER:
			((Cylinder*)mShape)->mHalfExtents = data;
			break;

		case Shape::Type::SPHERE:
			((Sphere*)mShape)->mRadius = data[0];
			break;

		case Shape::Type::TRIANGLE_MESH:
			break;

		default:
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

void CollisionComponent::Deserialize(const std::string& s)
{
	std::stringstream ss(s);

	std::string name;
	int type;
	ss >> name >> type;

	if (type != (int)(mShape->GetType()))
	{
		Shape::Type t = (Shape::Type)type;

		switch (t)
		{
		case Shape::Type::BOX:
			delete mShape;
			mShape = new Box(float4(1.0f, 1.0f, 1.0f, 1.0f));
			break;

		case Shape::Type::CAPSULE:
			delete mShape;
			mShape = new Capsule(1.0f, 1.0f);
			break;

		case Shape::Type::COMPOUND:
			//delete mShape;
			//mShape = new Compound(float4(1.0f, 1.0f, 1.0f, 1.0f));
			break;

		case Shape::Type::CONE:
			delete mShape;
			mShape = new Cone(1.0f, 1.0f);
			break;

		case Shape::Type::CONVEX_HULL:
			//delete mShape;
			//mShape = new Box(float4(1.0f, 1.0f, 1.0f, 1.0f));
			break;

		case Shape::Type::CYLINDER:
			delete mShape;
			mShape = new Cylinder(float4(1.0f, 1.0f, 1.0f, 1.0f));
			break;

		case Shape::Type::SPHERE:
			delete mShape;
			mShape = new Sphere(1.0f);
			break;

		case Shape::Type::TRIANGLE_MESH:
			delete mShape;
			mShape = new TriangleMesh();
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

	mShape->Deserialize(data);
}

bool RigidBodyComponent::Editor(std::string& prev, std::string& next)
{
	bool change = false;

	float mass = mBody->GetMass();
	if (ImGui::InputFloat("Mass", &mass))
	{
		change = true;
	}
	
	if (change)
	{
		prev = Serialize().c_str();

		mBody->mMass = mass;

		next = Serialize().c_str();

		if (next == prev)
		{
			change = false;
		}
	}

	return change;
}

void RigidBodyComponent::Deserialize(const std::string& s)
{
	std::vector<std::string> lines = String::Split(s, '\n');

	std::vector<std::string> dataVec;
	dataVec.push_back(lines[1]);
	std::string data = String::Join(dataVec, '\n');

	mBody->Deserialize(data);
}