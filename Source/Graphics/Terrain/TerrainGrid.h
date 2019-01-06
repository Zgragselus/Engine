#ifndef __TERRAIN__GRID__H__
#define __TERRAIN__GRID__H__

#include "TerrainTile.h"

namespace Engine
{
	class TerrainGrid
	{
	private:
		TerrainTile* mTile;
		unsigned int mTiles[2];
		unsigned int** mLod;
		unsigned int** mLevel;
		unsigned int** mIbo;
		float mOffset[2];
		float mSize;

		int GetLevel(int base)
		{
			int odds = base / 2;
			return base + odds * 14;
		}

	public:
		TerrainGrid(D3DRenderer* renderer, Log* log, int tileTessellation, float tileSize, int tilesX, int tilesY, float offsetX, float offsetY, float lodscale)
		{
			mSize = tileSize;
			mTile = new TerrainTile(renderer, log, tileTessellation, tileSize);
			mTiles[0] = tilesX;
			mTiles[1] = tilesY;
			mOffset[0] = offsetX;
			mOffset[1] = offsetY;

			mIbo = new unsigned int*[mTiles[0]];
			mLod = new unsigned int*[mTiles[0]];
			mLevel = new unsigned int*[mTiles[0]];
			for (unsigned int i = 0; i < mTiles[0]; i++)
			{
				mIbo[i] = new unsigned int[mTiles[1]];
				mLod[i] = new unsigned int[mTiles[1]];
				mLevel[i] = new unsigned int[mTiles[1]];
			}

			/*for (unsigned int i = 0; i < mTile->GetLevelsCount(); i++)
			{
				int lvl = GetLevel(i);
				printf("%d\n", lvl);
			}*/

			for (unsigned int i = 0; i < mTiles[0]; i++)
			{
				for (unsigned int j = 0; j < mTiles[1]; j++)
				{
					float x = -mOffset[0] + mSize * i;
					float y = -mOffset[1] + mSize * j;
					float d = sqrt(x * x + y * y);
					int lod = (int)(d / lodscale);
					if (lod < 0)
					{
						lod = 0;
					}
					else if (lod >= (int)mTile->GetLevelsCount())
					{
						lod = mTile->GetLevelsCount() - 1;
					}
					mLevel[i][j] = mTile->GetLevelsCount() - 1 - lod;
					mLod[i][j] = GetLevel(mLevel[i][j]);
				}
			}

			for (unsigned int i = 1; i < mTiles[0] - 1; i++)
			{
				for (unsigned int j = 1; j < mTiles[1] - 1; j++)
				{
					int tile = 0;
					tile |= mLevel[i - 1][j] > mLevel[i][j] ? TerrainTile::WEST : 0;
					tile |= mLevel[i + 1][j] > mLevel[i][j] ? TerrainTile::EAST : 0;
					tile |= mLevel[i][j + 1] > mLevel[i][j] ? TerrainTile::NORTH : 0;
					tile |= mLevel[i][j - 1] > mLevel[i][j] ? TerrainTile::SOUTH : 0;

					if (mLevel[i][j] % 2 == 1)
					{
						mLod[i][j] += mTile->GetOffset(tile);
					}
				}
			}
		}

		~TerrainGrid()
		{
			delete mTile;
		}

		/*void Render(Shader* shader)
		{
			shader->SetFloat(shader->GetVariable("Size"), mSize);
			shader->SetFloat2(shader->GetVariable("Offset"), mOffset[0], mOffset[1]);
			shader->SetInt2(shader->GetVariable("Tiles"), mTiles[0], mTiles[1]);
			mTile->GetVAO()->Enable();
			for (unsigned int i = 0; i < mTiles[0]; i++)
			{
				for (unsigned int j = 0; j < mTiles[1]; j++)
				{
					int lvl = mLod[i][j];
					shader->SetInt2(shader->GetVariable("Tile"), i, j);
					mTile->GetIBO()[lvl]->Enable();
					mTile->GetVBO()->DrawElements(GL_TRIANGLES, (int)mTile->GetIBO()[lvl]->GetNumElements());
				}
			}
			mTile->GetVAO()->Disable();
		}*/

		ALIGNED_NEW_DELETE("Engine::TerrainGrid")
	};
}

#endif