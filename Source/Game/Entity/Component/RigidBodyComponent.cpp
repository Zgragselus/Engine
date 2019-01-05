#include "ComponentStatic.h"
#include "Imgui/imgui.h"

namespace Engine
{
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
}