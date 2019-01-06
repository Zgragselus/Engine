#ifndef __TERRAIN__TILE__H__
#define __TERRAIN__TILE__H__

#include "Core/Log/Log.h"
#include "Graphics/D3D12/Resource/GpuBuffer.h"
#include "Graphics/D3D12/D3DRenderer.h"
#include <vector>

namespace Engine
{
	class TerrainTile
	{
	public:
		/// <summary>
		/// Defines in which direction terrain is tessellated for adjacent terrain nodes
		/// </summary>
		enum Direction
		{
			NORTH = 0x1,
			SOUTH = 0x2,
			EAST = 0x4,
			WEST = 0x8,
			FULL = 0x10
		};

	private:
		GpuBuffer* mVertexBuffer;
		GpuBuffer** mIndexBuffer;
		unsigned int mIndexBufferCount;
		unsigned int mLevels;

		struct Bounds
		{
			int t00;
			int t10;
			int t01;
			int t11;

			/// <param name="s">The number of vertices in X respectively Z axes</param>
			Bounds(int s)
			{
				t00 = 0;
				t10 = s - 1;
				t01 = s * (s - 1);
				t11 = s * s - 1;
			}

			Bounds(int s00, int s10, int s01, int s11, int f = 0)
			{
				t00 = s00;
				t10 = s10;
				t01 = s01;
				t11 = s11;
			}
		};

		/// <summary>Compute index buffer for specific tessellation level on NxN grid</summary>
		/// <param name="out">Output index buffer</param>
		/// <param name="s">Total tessellation factor (2^N+1)</param>
		/// <param name="b">Current bounds which we tessellate</param>
		/// <param name="additional">Specified binary sum of which direction to tessellate (flags)</param>
		void Tessellate(std::vector<unsigned int>& out, int s, const Bounds& b, int additional = 0);

	public:
		/// <summary>Constructor</summary>
		/// <param name="renderer">D3D12 renderer</param>
		/// <param name="log">Logger class</param>
		/// <param name="n">Tessellation level parameter (tessellation factor = 2^N+1)</param>
		/// <param name="scale">Resulting size (in world space) of single tile</param>
		TerrainTile(D3DRenderer* renderer, Log* log, int n, float scale);

		/// <summary>Destructor - free used memory</summary>
		virtual ~TerrainTile();

		inline int GetOffset(int flags)
		{
			switch (flags)
			{
			case NORTH:
				return 1;
			case WEST:
				return 2;
			case SOUTH:
				return 3;
			case EAST:
				return 4;
			case NORTH | SOUTH:
				return 5;
			case WEST | EAST:
				return 6;
			case NORTH | WEST:
				return 7;
			case WEST | SOUTH:
				return 8;
			case SOUTH | EAST:
				return 9;
			case EAST | NORTH:
				return 10;
			case NORTH | WEST | SOUTH:
				return 11;
			case WEST | SOUTH | EAST:
				return 12;
			case SOUTH | EAST | NORTH:
				return 13;
			case EAST | NORTH | WEST:
				return 14;
			default:
				return 0;
			}
		}

		/*virtual void Render()
		{
			GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
			mVertexArray->Enable();
			mIndexBuffer[0]->Enable();
			mVertexBuffer->DrawElements(GL_TRIANGLES, (int)mIndexBuffer[0]->GetNumElements());
			mVertexArray->Disable();
			GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
		}*/

		inline unsigned int GetLevelsCount()
		{
			return mLevels;
		}

		/*inline VertexArray* GetVAO()
		{
			return mVertexArray;
		}

		inline VertexBuffer* GetVBO()
		{
			return mVertexBuffer;
		}

		inline IndexBuffer** GetIBO()
		{
			return mIndexBuffer;
		}*/

		ALIGNED_NEW_DELETE("Engine::TerrainTile")
	};
}

#endif