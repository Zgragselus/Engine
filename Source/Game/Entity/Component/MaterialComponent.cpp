#include "ComponentStatic.h"
#include "Imgui/imgui.h"

namespace Engine
{
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
}