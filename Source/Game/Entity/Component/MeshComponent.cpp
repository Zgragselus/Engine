#include "ComponentStatic.h"
#include "Imgui/imgui.h"

namespace Engine
{
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
}