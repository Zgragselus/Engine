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

		float offset[2];
		mTerrainGrid->GetOffset(offset[0], offset[1]);
		if (ImGui::InputFloat2("Offset", offset))
		{
			change = true;
		}
		
		if (change)
		{
			prev = Serialize().c_str();

			mTerrainGrid->SetLodScale(lodscale);
			mTerrainGrid->SetOffset(offset[0], offset[1]);

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
		std::vector<std::string> lines = String::Split(s, '\n');

		std::vector<std::string> dataVec;
		dataVec.push_back(lines[1]);
		std::string data = String::Join(dataVec, '\n');

		mTerrainGrid->Deserialize(data);
	}
}