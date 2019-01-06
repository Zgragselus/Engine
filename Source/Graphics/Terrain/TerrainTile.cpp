#include "TerrainTile.h"
#include <queue>

namespace Engine
{
	/// <summary>Compute index buffer for specific tessellation level on NxN grid</summary>
	/// <param name="out">Output index buffer</param>
	/// <param name="s">Total tessellation factor (2^N+1)</param>
	/// <param name="b">Current bounds which we tessellate</param>
	/// <param name="additional">Specified binary sum of which direction to tessellate (flags)</param>
	void TerrainTile::Tessellate(std::vector<unsigned int>& out, int s, const TerrainTile::Bounds& b, int additional)
	{
		int mid = (b.t00 + b.t11) / 2;
		int flags = additional & FULL;

		// Handle south flag
		if (additional & SOUTH)
		{
			if (b.t00 < s)
			{
				flags |= SOUTH;
			}
		}

		// Handle north flag
		if (additional & NORTH)
		{
			if (b.t11 > (s * (s - 1)))
			{
				flags |= NORTH;
			}
		}

		// Handle east flag
		if (additional & EAST)
		{
			if ((b.t10 % s) == s - 1)
			{
				flags |= EAST;
			}
		}

		// Handle west flag
		if (additional & WEST)
		{
			if ((b.t00 % s) == 0)
			{
				flags |= WEST;
			}
		}

		// Bottom
		if (flags & SOUTH || flags & FULL)
		{
			int tmp = (b.t00 + b.t10) / 2;

			out.push_back(b.t00);
			out.push_back(tmp);
			out.push_back(mid);

			out.push_back(mid);
			out.push_back(tmp);
			out.push_back(b.t10);
		}
		else
		{
			out.push_back(b.t00);
			out.push_back(b.t10);
			out.push_back(mid);
		}

		// Right
		if (flags & EAST || flags & FULL)
		{
			int tmp = (b.t10 + b.t11) / 2;

			out.push_back(b.t10);
			out.push_back(tmp);
			out.push_back(mid);

			out.push_back(mid);
			out.push_back(tmp);
			out.push_back(b.t11);
		}
		else
		{
			out.push_back(b.t10);
			out.push_back(b.t11);
			out.push_back(mid);
		}

		// Top
		if (flags & NORTH || flags & FULL)
		{
			int tmp = (b.t11 + b.t01) / 2;

			out.push_back(b.t11);
			out.push_back(tmp);
			out.push_back(mid);

			out.push_back(mid);
			out.push_back(tmp);
			out.push_back(b.t01);
		}
		else
		{
			out.push_back(b.t11);
			out.push_back(b.t01);
			out.push_back(mid);
		}

		// Left
		if (flags & WEST || flags & FULL)
		{
			int tmp = (b.t01 + b.t00) / 2;

			out.push_back(b.t01);
			out.push_back(tmp);
			out.push_back(mid);

			out.push_back(mid);
			out.push_back(tmp);
			out.push_back(b.t00);
		}
		else
		{
			out.push_back(b.t01);
			out.push_back(b.t00);
			out.push_back(mid);
		}
	}

	/// <summary>Constructor</summary>
	/// <param name="renderer">D3D12 renderer</param>
	/// <param name="log">Logger class</param>
	/// <param name="n">Tessellation level parameter (tessellation factor = 2^N+1)</param>
	/// <param name="scale">Resulting size (in world space) of single tile</param>
	TerrainTile::TerrainTile(D3DRenderer* renderer, Log* log, int n, float scale)
	{
		// The count of vertices on X and Z axes equals to (2^n + 1)
		int tessellation[2] = { (int)pow(2.0, n) + 1, (int)pow(2.0, n) + 1 };
		// Total number of levels for level-of-detail
		mLevels = 2 * n + 1;

		// Prepare full vertex buffer (fully tesselalted tile), actual tessellation is done through index buffers
		unsigned int verts = tessellation[0] * tessellation[1];
		std::vector<std::vector<unsigned int> > indices;
		float* vertices = new float[4 * verts];

		float step[2] = { scale / (float)(tessellation[0] - 1), scale / (float)(tessellation[1] - 1) };

		for (int i = 0; i < tessellation[1]; i++)
		{
			for (int j = 0; j < tessellation[0]; j++)
			{
				vertices[(j + i * tessellation[0]) * 4 + 0] = j * step[0] - scale * 0.5f;
				vertices[(j + i * tessellation[0]) * 4 + 1] = 1.0f;
				vertices[(j + i * tessellation[0]) * 4 + 2] = i * step[1] - scale * 0.5f;
				vertices[(j + i * tessellation[0]) * 4 + 3] = 1.0f;
			}
		}

		// Generate all levels
		std::queue<Bounds> q;
		q.push(Bounds(tessellation[0]));
		int lvl = 0;
		for (unsigned int i = 0; i < mLevels; i++)
		{
			if (i % 2 == 1)
			{
				for (int j = 0; j < 15; j++)
				{
					indices.push_back(std::vector<unsigned int>());
				}

				std::queue<Bounds> tmp;
				while (!q.empty())
				{
					const Bounds& b = q.front();
					Tessellate(indices[lvl + 0], tessellation[0], b);
					Tessellate(indices[lvl + 1], tessellation[0], b, NORTH);
					Tessellate(indices[lvl + 2], tessellation[0], b, WEST);
					Tessellate(indices[lvl + 3], tessellation[0], b, SOUTH);
					Tessellate(indices[lvl + 4], tessellation[0], b, EAST);
					Tessellate(indices[lvl + 5], tessellation[0], b, NORTH | SOUTH);
					Tessellate(indices[lvl + 6], tessellation[0], b, WEST | EAST);
					Tessellate(indices[lvl + 7], tessellation[0], b, NORTH | WEST);
					Tessellate(indices[lvl + 8], tessellation[0], b, WEST | SOUTH);
					Tessellate(indices[lvl + 9], tessellation[0], b, SOUTH | EAST);
					Tessellate(indices[lvl + 10], tessellation[0], b, EAST | NORTH);
					Tessellate(indices[lvl + 11], tessellation[0], b, NORTH | WEST | SOUTH);
					Tessellate(indices[lvl + 12], tessellation[0], b, WEST | SOUTH | EAST);
					Tessellate(indices[lvl + 13], tessellation[0], b, SOUTH | EAST | NORTH);
					Tessellate(indices[lvl + 14], tessellation[0], b, EAST | NORTH | WEST);
					tmp.push(b);
					q.pop();
				}

				while (!tmp.empty())
				{
					const Bounds& b = tmp.front();
					q.push(b);
					tmp.pop();
				}
				lvl += 15;
			}
			// Each even level, perform FULL tessellation and store it inside index buffer
			else if (i > 0)
			{
				indices.push_back(std::vector<unsigned int>());
				std::queue<Bounds> tmp;
				while (!q.empty())
				{
					const Bounds& b = q.front();
					Tessellate(indices[lvl], tessellation[0], b, FULL);
					tmp.push(b);
					q.pop();
				}

				while (!tmp.empty())
				{
					const Bounds& b = tmp.front();
					q.push(b);
					tmp.pop();
				}

				// Split to 4 sub-trees for next level tessellation
				while (!q.empty())
				{
					const Bounds& b = q.front();
					int mid = (b.t00 + b.t11) / 2;
					int x0 = (b.t00 + b.t10) / 2;
					int x1 = (b.t01 + b.t11) / 2;
					int y0 = (b.t00 + b.t01) / 2;
					int y1 = (b.t10 + b.t11) / 2;
					tmp.push(Bounds(b.t00, x0, y0, mid));
					tmp.push(Bounds(x0, b.t10, mid, y1));
					tmp.push(Bounds(y0, mid, b.t01, x1));
					tmp.push(Bounds(mid, y1, x1, b.t11));
					q.pop();
				}

				while (!tmp.empty())
				{
					const Bounds& b = tmp.front();
					q.push(b);
					tmp.pop();
				}
				lvl++;
			}
			// Base tessellation level, just 2 triangles cover - enqueued bounds into front
			else
			{
				indices.push_back(std::vector<unsigned int>());
				const Bounds& b = q.front();
				indices[lvl].push_back(b.t00);
				indices[lvl].push_back(b.t10);
				indices[lvl].push_back(b.t01);

				indices[lvl].push_back(b.t11);
				indices[lvl].push_back(b.t01);
				indices[lvl].push_back(b.t10);
				q.pop();

				q.push(Bounds(indices[lvl][0], indices[lvl][1], indices[lvl][2], indices[lvl][3]));
				lvl++;
			}
		}

		mVertexBuffer = new TypedBuffer(Graphics::R32F);
		mVertexBuffer->Init(renderer, verts, sizeof(float) * 4, vertices);

		mIndexBufferCount = (unsigned int)indices.size();
		mIndexBuffer = new GpuBuffer*[mIndexBufferCount];
		for (unsigned int i = 0; i < mIndexBufferCount; i++)
		{
			mIndexBuffer[i] = new TypedBuffer(Graphics::R32);
			mIndexBuffer[i]->Init(renderer, indices[i].size(), sizeof(unsigned int), indices[i].data());
		}

		log->Print("TerrainTile", "Definition of terrain: " + std::to_string(mLevels) + std::string(" Levels, ") + std::to_string(indices.size()) + std::string(" Index buffers"));

		delete[] vertices;
	}

	/// <summary>Destructor - free used memory</summary>
	TerrainTile::~TerrainTile()
	{
		delete mVertexBuffer;

		for (unsigned int i = 0; i < mIndexBufferCount; i++)
		{
			delete mIndexBuffer[i];
		}
		delete[] mIndexBuffer;
	}
}