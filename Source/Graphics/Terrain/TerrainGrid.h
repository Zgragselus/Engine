#ifndef __TERRAIN__GRID__H__
#define __TERRAIN__GRID__H__

#include "TerrainTile.h"
#include "Core/Math/Numeric/Float4.h"
#include "Core/Math/Numeric/Mat4.h"
#include "Graphics/D3D12/Resource/GpuMappedBuffer.h"

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
		int mTileTessellation;
		float mLodScale;
		
		GpuMappedBuffer* mMatricesBuffer;
		void* mMatricesBufferPtr;

		unsigned int mHeightmapSize;
		float** mHeightmapData;

		int GetLevel(int base)
		{
			int odds = base / 2;
			return base + odds * 14;
		}

	public:
		inline float GetLodScale()
		{
			return mLodScale;
		}

		inline void SetLodScale(float lodscale)
		{
			mLodScale = lodscale;

			UpdateGrid();
		}

		inline void GetOffset(float& x, float& y)
		{
			x = mOffset[0];
			y = mOffset[1];
		}

		inline void SetOffset(float x, float y)
		{
			mOffset[0] = x;
			mOffset[1] = y;
		}

		inline void UpdateGrid()
		{
			for (unsigned int i = 0; i < mTiles[0]; i++)
			{
				for (unsigned int j = 0; j < mTiles[1]; j++)
				{
					float x = mOffset[0] + mSize * i + mSize * 0.5f;
					float y = mOffset[1] + mSize * j + mSize * 0.5f;
					float d = sqrt(x * x + y * y);
					int lod = (int)(d / mLodScale);
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

			for (unsigned int i = 0; i < mTiles[0]; i++)
			{
				for (unsigned int j = 0; j < mTiles[1]; j++)
				{
					int left = ((int)i - 1) < 0 ? 0 : ((int)i - 1);
					int right = ((int)i + 1) < ((int)mTiles[0] - 1) ? ((int)i + 1) : ((int)mTiles[0] - 1);
					int top = ((int)j + 1) < ((int)mTiles[1] - 1) ? ((int)j + 1) : ((int)mTiles[1] - 1);
					int bottom = ((int)j - 1) < 0 ? 0 : ((int)j - 1);

					int tile = 0;
					tile |= mLevel[left][j] > mLevel[i][j] ? TerrainTile::SOUTH : 0;
					tile |= mLevel[right][j] > mLevel[i][j] ? TerrainTile::NORTH : 0;
					tile |= mLevel[i][top] > mLevel[i][j] ? TerrainTile::EAST : 0;
					tile |= mLevel[i][bottom] > mLevel[i][j] ? TerrainTile::WEST : 0;

					if (mLevel[i][j] % 2 == 1)
					{
						mLod[i][j] += mTile->GetOffset(tile);
					}
				}
			}
		}

		TerrainGrid(D3DRenderer* renderer, Log* log, int tileTessellation, float tileSize, int tilesX, int tilesY, float offsetX, float offsetY, float lodscale)
		{
			mSize = tileSize;
			mTiles[0] = tilesX;
			mTiles[1] = tilesY;
			mOffset[0] = offsetX;
			mOffset[1] = offsetY;
			mTileTessellation = tileTessellation;
			mLodScale = lodscale;

			mTile = new TerrainTile(renderer, log, mTileTessellation, tileSize);

			mIbo = new unsigned int*[mTiles[0]];
			mLod = new unsigned int*[mTiles[0]];
			mLevel = new unsigned int*[mTiles[0]];
			for (unsigned int i = 0; i < mTiles[0]; i++)
			{
				mIbo[i] = new unsigned int[mTiles[1]];
				mLod[i] = new unsigned int[mTiles[1]];
				mLevel[i] = new unsigned int[mTiles[1]];
			}

			UpdateGrid();
			
			mMatricesBuffer = new Engine::GpuMappedBuffer();
			int tilesCount = GetNumTiles();
			tilesCount = ((tilesCount % 4 > 0 ? 1 : 0) + tilesCount / 4) * 4;
			mMatricesBuffer->Init(renderer, tilesCount * 2, sizeof(float) * 16);
			mMatricesBufferPtr = mMatricesBuffer->Map();
			mat4* tmp = new mat4[tilesCount * 2];
			FillMatrices(tmp);
			memcpy(mMatricesBufferPtr, tmp, sizeof(float) * 16 * tilesCount * 2);
			delete[] tmp;
		}

		~TerrainGrid()
		{
			delete mTile;
			delete mMatricesBuffer;
		}

		inline void Render(GraphicsContext* context)
		{
			int batchBound = -1;
			for (unsigned int i = 0; i < mTiles[0]; i++)
			{
				for (unsigned int j = 0; j < mTiles[1]; j++)
				{
					int id = i + j * mTiles[0];
					int batch = id / 16;

					if (batchBound != batch)
					{
						batchBound = batch;
						context->SetConstantBuffer(6, mMatricesBuffer->GetGpuVirtualAddress(batch * 16 * sizeof(float) * 16 * 2));
					}
					context->SetConstants(7, Engine::DWParam(id % 16));

					int lvl = mLod[i][j];

					Engine::GpuBuffer** tmp = mTile->GetIndexBuffersArray();
					Engine::GpuBuffer* ibo = tmp[lvl];

					context->SetIndexBuffer(ibo->IndexBufferView(0, (unsigned int)ibo->Size()));
					context->SetVertexBuffer(0, mTile->GetVertexBuffer()->VertexBufferView(0, (unsigned int)mTile->GetVertexBuffer()->Size(), (unsigned int)mTile->GetVertexBuffer()->ElementSize()));
					context->DrawIndexed((unsigned int)ibo->NumElements(), 0, 0);
				}
			}
		}

		inline int GetNumTiles()
		{
			return mTiles[0] * mTiles[1];
		}

		inline void FillMatrices(mat4* matrices)
		{
			for (unsigned int i = 0; i < mTiles[0]; i++)
			{
				for (unsigned int j = 0; j < mTiles[1]; j++)
				{
					float4 position = float4(mOffset[0] + i * mSize, 0.0f, mOffset[1] + j * mSize, 1.0f);
					mat4 world = mat4(position);
					mat4 worldIT = transpose(inverse(world));
					matrices[(i + j * mTiles[0]) * 2 + 0] = world;
					matrices[(i + j * mTiles[0]) * 2 + 1] = worldIT;
				}
			}
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

		inline std::string Serialize()
		{
			std::stringstream ss;

			ss << mSize << " " << mTiles[0] << " " << mTiles[1] << " " << mOffset[0] << " " << mOffset[1] << " " << mTileTessellation << " " << mLodScale << std::endl;

			printf("TerrainGrid - serialize: %s\n", ss.str().c_str());

			return ss.str();
		}

		inline void Deserialize(const std::string& s)
		{
			std::stringstream ss(s);

			printf("TerrainGrid - deserialize: %s\n", s.c_str());

			ss >> mSize >> mTiles[0] >> mTiles[1] >> mOffset[0] >> mOffset[1] >> mTileTessellation >> mLodScale;
		}

		ALIGNED_NEW_DELETE("Engine::TerrainGrid")
	};
}

#endif