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

	public:
		void Save()
		{

		}

		void Load()
		{

		}
	};

	class HeightTile
	{
	private:
	public:

	};
}

#endif 