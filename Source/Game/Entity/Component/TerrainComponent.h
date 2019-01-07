#ifndef __TERRAIN_COMPONENT__H__
#define __TERRAIN_COMPONENT__H__

#include "../Component.h"
#include "Graphics/Terrain/TerrainGrid.h"

namespace Engine
{
	class __declspec(align(16)) TerrainComponent : public Component
	{
	private:
		static std::string mComponentName;
		TerrainGrid* mTerrainGrid;

	public:
		TerrainComponent(TerrainGrid* terrainGrid) : Component()
		{
			mTerrainGrid = terrainGrid;
		}

		/*TerrainComponent(const TerrainComponent& m)
		{
			mTerrainGrid = m.mTerrainGrid;
		}*/

		virtual ~TerrainComponent()
		{
			delete mTerrainGrid;
		}

		static std::string& GetName()
		{
			return mComponentName;
		}

		virtual bool Editor(std::string& prev, std::string& next)
		{
			return false;
		}

		virtual std::string Serialize()
		{
			std::stringstream ss;

			ss << "TerrainComponent" << std::endl;

			return ss.str();
		}

		virtual void Deserialize(const std::string& s)
		{

		}

		inline TerrainGrid* GetTerrain()
		{
			return mTerrainGrid;
		}

		ALIGNED_NEW_DELETE("Engine::TerrainComponent")
	};
}

#endif