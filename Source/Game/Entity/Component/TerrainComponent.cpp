#include "ComponentStatic.h"
#include "Imgui/imgui.h"

namespace Engine
{
	bool TerrainComponent::Editor(std::string& prev, std::string& next)
	{
		bool change = false;

		float lodscale = mTerrainGrid->GetLodScale();
		if (ImGui::InputFloat("LOD Scale", &lodscale))
		{
			change = true;
		}

		if (change)
		{
			prev = Serialize().c_str();

			mTerrainGrid->SetLodScale(lodscale);

			next = Serialize().c_str();

			if (next == prev)
			{
				change = false;
			}
		}

		/*mTerrainGrid->Deserialize 

		mSize = tileSize;
		mTiles[0] = tilesX;
		mTiles[1] = tilesY;
		mOffset[0] = offsetX;
		mOffset[1] = offsetY;
		mTileTessellation = tileTessellation;
		mLodScale = lodscale;

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
		}*/

		return change;
	}

	void TerrainComponent::Deserialize(const std::string& s)
	{
		/*std::vector<std::string> lines = String::Split(s, '\n');

		std::vector<std::string> dataVec;
		dataVec.push_back(lines[1]);
		std::string data = String::Join(dataVec, '\n');

		mBody->Deserialize(data);*/
	}
}