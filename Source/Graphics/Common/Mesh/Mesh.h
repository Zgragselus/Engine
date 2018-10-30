///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Mesh.h
//
// Following file contains class defining basic mesh object
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __MESH_H__
#define __MESH_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// Header section

#include "Graphics/D3D12/Resource/GpuBuffer.h"
#include "Graphics/D3D12/D3DRenderer.h"
#include "Core/Math/Primitive/AABB.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class & Structures definition

namespace Engine
{
	/// <summary>
	/// The following class describes mesh object.
	/// </summary>
	class Mesh
	{
	private:
		AABB mBounds;
		GpuBuffer* mVertexBuffer;
		GpuBuffer* mIndexBuffer;
		std::string mName;

	public:
		/// <summary>Mesh constructor</summary>
		/// <param name="vboData">Mesh per-vertex data</param>
		/// <param name="itemSize">Size of single per-vertex item</param>
		/// <param name="itemCount">Number of vertices</param>
		/// <param name="indices">Indices data</param>
		/// <param name="indexCount">Number of indices</param>
		Mesh(D3DRenderer* renderer,
			void* vertices,
			size_t vertexSize,
			size_t vertexCount,
			unsigned int* indices,
			size_t indexCount);

		/// <summary>Mesh destructor</summary>
		virtual ~Mesh();
		
		inline void Render(GraphicsContext* context)
		{
			context->SetIndexBuffer(mIndexBuffer->IndexBufferView(0, (unsigned int)mIndexBuffer->Size()));
			context->SetVertexBuffer(0, mVertexBuffer->VertexBufferView(0, (unsigned int)mVertexBuffer->Size(), (unsigned int)mVertexBuffer->ElementSize()));
			context->DrawIndexed((unsigned int)mIndexBuffer->NumElements(), 0, 0);
		}

		inline AABB& Bounds()
		{
			return mBounds;
		}

		inline GpuBuffer* GetVertexBuffer()
		{
			return mVertexBuffer;
		}

		inline GpuBuffer* GetIndexBuffer()
		{
			return mIndexBuffer;
		}

		inline void SetName(const std::string& name)
		{
			mName = name;
		}

		inline const std::string& GetName() const
		{
			return mName;
		}

		friend inline bool operator==(const Mesh& a, const Mesh& b)
		{

		}

		ALIGNED_NEW_DELETE("Engine::Graphics::Mesh")
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOH

#endif
