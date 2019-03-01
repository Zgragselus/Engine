#ifndef __HEIGHT_TILE__H__
#define __HEIGHT_TILE__H__

#include "Core/Math/Numeric/Float4.h"
#include "Core/Math/Numeric/Mat4.h"
#include "Graphics/D3D12/Resource/GpuMappedBuffer.h"

namespace Engine
{
	class HeightFile
	{
	private:
		struct TileHeader
		{
			size_t mOffset;
			size_t mSize;
			int mChildren[4];
			int mParent;
		};

		size_t mTotalResolution;
		size_t mPageResolution;
		size_t mPages;
		size_t mLevels;
		FILE* mFile;

		TileHeader* mHeader;

		void RecursiveGenerate(TileHeader* tiles, int current, int x0, int y0, int x1, int y1)
		{
			if (tiles[current].mChildren[0] == -1)
			{
				float* result = new float[mPageResolution * mPageResolution];
				LoadPage(&tiles[current], result);

				int half = mTotalResolution / 2;

				int k = 0;

				for (int j = 0; j < mPageResolution; j++)
				{
					for (int i = 0; i < mPageResolution; i++)
					{
						float x = (float)(x0 + i - half) / (float)half;
						float y = (float)(y0 + j - half) / (float)half;

						float v = 1.0f - sqrtf(x * x + y * y);

						result[k++] = v;
					}
				}

				delete[] result;
			}
			else
			{
				RecursiveGenerate(tiles, tiles[current].mChildren[0], x0, y0, (x0 + x1) / 2, (y0 + y1) / 2);
				RecursiveGenerate(tiles, tiles[current].mChildren[1], (x0 + x1) / 2, y0, x1, (y0 + y1) / 2);
				RecursiveGenerate(tiles, tiles[current].mChildren[2], x0, (y0 + y1) / 2, (x0 + x1) / 2, y1);
				RecursiveGenerate(tiles, tiles[current].mChildren[3], (x0 + x1) / 2, (y0 + y1) / 2, x1, y1);

				UpdatePage(&(tiles[current]));
			}
		}

		int RecursiveBuild(TileHeader* tiles, int* id, int parent, size_t level, size_t* offset, size_t tileSize)
		{
			int current = (*id)++;

			tiles[current].mOffset = (*offset);
			tiles[current].mSize = tileSize;
			tiles[current].mParent = parent;

			(*offset) += tileSize;

			if (level < mLevels - 1)
			{
				tiles[current].mChildren[0] = RecursiveBuild(tiles, id, current, level + 1, offset, tileSize);
				tiles[current].mChildren[1] = RecursiveBuild(tiles, id, current, level + 1, offset, tileSize);
				tiles[current].mChildren[2] = RecursiveBuild(tiles, id, current, level + 1, offset, tileSize);
				tiles[current].mChildren[3] = RecursiveBuild(tiles, id, current, level + 1, offset, tileSize);
			}
			else
			{
				tiles[current].mChildren[0] = -1;
				tiles[current].mChildren[1] = -1;
				tiles[current].mChildren[2] = -1;
				tiles[current].mChildren[3] = -1;
			}

			return current;
		}

		void New(const std::string& filename)
		{
			mFile = fopen(filename.c_str(), "wb");
			if (!mFile)
			{
				return;
			}

			size_t tileSize = mPageResolution * mPageResolution * sizeof(float);
			size_t headerSize = sizeof(TileHeader) * mPages;
			size_t offset = headerSize + sizeof(size_t) * 4;

			TileHeader* headerData = new TileHeader[mPages];
			float* tileData = new float[mPageResolution * mPageResolution];

			for (int i = 0; i < mPageResolution * mPageResolution; i++)
			{
				tileData[i] = 0.0f;
			}

			// TODO: This needs to be removed later
			float step = 1.0f / (mPageResolution * mPageResolution);
			for (int j = 0; j < mPageResolution * mPageResolution; j++)
			{
				tileData[j] = j * step;
			}

			int id = 0;
			RecursiveBuild(headerData, &id, 0, 0, &offset, tileSize);

			fwrite(&mTotalResolution, sizeof(size_t), 1, mFile);
			fwrite(&mPageResolution, sizeof(size_t), 1, mFile);
			fwrite(&mPages, sizeof(size_t), 1, mFile);
			fwrite(&mLevels, sizeof(size_t), 1, mFile);

			fwrite(&headerData, sizeof(TileHeader), mPages, mFile);

			for (int i = 0; i < mPages; i++)
			{
				fwrite(tileData, sizeof(float), mPageResolution * mPageResolution, mFile);
			}

			delete[] headerData;
			
			fclose(mFile);
		}

		void SavePage(TileHeader* page, float* data)
		{
			fseek(mFile, page->mOffset, SEEK_SET);
			fwrite(data, page->mSize, 1, mFile);
		}

		void LoadPage(TileHeader* page, float* data)
		{
			fseek(mFile, page->mOffset, SEEK_SET);
			fread(data, page->mSize, 1, mFile);
		}

	public:
		HeightFile(const std::string& filename, size_t totalResolution, size_t pageResolution)
		{
			mTotalResolution = totalResolution;
			mPageResolution = pageResolution;

			mPages = 0;
			size_t count = mTotalResolution / mPageResolution;
			while (count != 0)
			{
				mPages += count * count;
				mLevels++;
				count /= 2;
			}

			mFile = fopen(filename.c_str(), "rb+");
			if (mFile == nullptr)
			{
				New(filename);
				mFile = fopen(filename.c_str(), "rb+");
			}
			
			fread(&mTotalResolution, sizeof(size_t), 1, mFile);
			fread(&mPageResolution, sizeof(size_t), 1, mFile);
			fread(&mPages, sizeof(size_t), 1, mFile);
			fread(&mLevels, sizeof(size_t), 1, mFile);

			mHeader = new TileHeader[mPages];
			fread(mHeader, sizeof(TileHeader), mPages, mFile);

			RecursiveGenerate(mHeader, 0, 0, 0, mTotalResolution, mTotalResolution);
		}

		~HeightFile()
		{
			delete[] mHeader;

			fclose(mFile);
			mFile = nullptr;
		}

		void SavePage(int page, float* data)
		{
			fseek(mFile, mHeader[page].mOffset, SEEK_SET);
			fwrite(data, mHeader[page].mSize, 1, mFile);
		}

		void LoadPage(int page, float* data)
		{
			fseek(mFile, mHeader[page].mOffset, SEEK_SET);
			fread(data, mHeader[page].mSize, 1, mFile);
		}

		void UpdatePage(TileHeader* page)
		{
			float* result = new float[mPageResolution * mPageResolution];

			LoadPage(page, result);

			float** pages;
			pages = new float*[4];
			pages[0] = new float[mPageResolution * mPageResolution];
			pages[1] = new float[mPageResolution * mPageResolution];
			pages[2] = new float[mPageResolution * mPageResolution];
			pages[3] = new float[mPageResolution * mPageResolution];

			LoadPage(&mHeader[page->mChildren[0]], pages[0]);
			LoadPage(&mHeader[page->mChildren[1]], pages[1]);
			LoadPage(&mHeader[page->mChildren[2]], pages[2]);
			LoadPage(&mHeader[page->mChildren[3]], pages[3]);

			int offset = mPageResolution / 2;
			int tmp = 0;

			for (int j = 0; j < mPageResolution; j++)
			{
				for (int i = 0; i < mPageResolution; i++)
				{
					result[(i / 2) + (j / 2) * mPageResolution] = (pages[0][tmp] + pages[0][tmp + 1] + pages[0][tmp + mPageResolution] + pages[0][tmp + mPageResolution + 1]) * 0.25f;
					result[(i / 2) + offset + (j / 2) * mPageResolution] = (pages[0][tmp] + pages[0][tmp + 1] + pages[0][tmp + mPageResolution] + pages[0][tmp + mPageResolution + 1]) * 0.25f;
					result[(i / 2) + (offset + j / 2) * mPageResolution] = (pages[0][tmp] + pages[0][tmp + 1] + pages[0][tmp + mPageResolution] + pages[0][tmp + mPageResolution + 1]) * 0.25f;
					result[(i / 2) + offset + (offset + j / 2) * mPageResolution] = (pages[0][tmp] + pages[0][tmp + 1] + pages[0][tmp + mPageResolution] + pages[0][tmp + mPageResolution + 1]) * 0.25f;

					tmp++;
				}
			}

			delete[] result;
			delete[] pages[3];
			delete[] pages[2];
			delete[] pages[1];
			delete[] pages[0];
			delete[] pages;
		}
	};

	class HeightTile
	{
	private:
	public:

	};
}

#endif 