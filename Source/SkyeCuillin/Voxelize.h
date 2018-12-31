#ifndef __VOXELIZE__H__
#define __VOXELIZE__H__

#include "Graphics/Graphics.h"
#include "Game/Game.h"
#include "LightingSystem.h"

namespace SkyeCuillin
{
	class Voxelize
	{
	public:
		enum class MipmapCalculation
		{
			MIPMAP_1,
			MIPMAP_2,
			MIPMAP_3
		};

	public:
		Engine::mat4* mProjections;
		Engine::mat4* mScale;

		Engine::RootSignature* mVoxelizeRS;
		Engine::PipelineState* mVoxelizePS;
		Engine::D3DShader* mVoxelize;

		Engine::RootSignature* mClearRS;
		Engine::PipelineState* mClearPS;
		Engine::D3DShader* mClear;

		Engine::RootSignature* mMipmapRS;
		Engine::PipelineState* mMipmapPS;
		Engine::D3DShader* mMipmap;

		Engine::Texture* mColorTexture[2];

		Engine::GpuMappedBuffer* mVoxelizeParams;
		void* mVoxelizeParamsPtr;

		unsigned int mDimensions;
		unsigned int mMiplevels;

		unsigned int mInput = 0;
		unsigned int mOutput = 1;

		MipmapCalculation mMipMethod;
		
		LightingSystem* mLightingSystem;

	public:
		Voxelize(Engine::D3DRenderer* renderer, LightingSystem* lightingSystem, unsigned int resolution, MipmapCalculation method);

		~Voxelize();

		void Clear(Engine::DescriptorHeap* heap, Engine::ComputeContext* context);

		void GenerateMipmaps(Engine::DescriptorHeap* heap, Engine::ComputeContext* context);

		void Generate(Engine::DescriptorHeap* heap, Engine::GraphicsContext* context, std::vector<Engine::RenderNode>& nodes, Engine::GpuMappedBuffer* matrices);

		inline Engine::Texture* GetColorTexture() { return mColorTexture[mOutput]; }
		inline unsigned int GetResolution() { return mDimensions; }
	};

	class VoxelizeOctree
	{
	public:
		Engine::mat4* mProjections;
		Engine::mat4* mScale;

		Engine::RootSignature* mVoxelizeRS;
		Engine::PipelineState* mVoxelizePS;
		Engine::D3DShader* mVoxelize;

		Engine::RootSignature* mOctreeNodeFlagRS;
		Engine::PipelineState* mOctreeNodeFlagPS;
		Engine::D3DShader* mOctreeNodeFlag;

		Engine::RootSignature* mOctreeNodeAllocRS;
		Engine::PipelineState* mOctreeNodeAllocPS;
		Engine::D3DShader* mOctreeNodeAlloc;

		Engine::RootSignature* mOctreeNodeInitRS;
		Engine::PipelineState* mOctreeNodeInitPS;
		Engine::D3DShader* mOctreeNodeInit;

		Engine::RootSignature* mOctreeDataClearRS;
		Engine::PipelineState* mOctreeDataClearPS;
		Engine::D3DShader* mOctreeDataClear;

		Engine::RootSignature* mOctreeDataFillRS;
		Engine::PipelineState* mOctreeDataFillPS;
		Engine::D3DShader* mOctreeDataFill;

		Engine::RootSignature* mOctreeDataFilterRS;
		Engine::PipelineState* mOctreeDataFilterPS;
		Engine::D3DShader* mOctreeDataFilter;

		Engine::RootSignature* mOctreeDataInteriorRS;
		Engine::PipelineState* mOctreeDataInteriorPS;
		Engine::D3DShader* mOctreeDataInterior;

		Engine::StructuredBuffer* mVoxelBuffer;
		Engine::StructuredBuffer* mVoxelColorBuffer;

		Engine::StructuredBuffer* mOctreeNodeBuffer;
		Engine::Texture* mOctreeDataBuffer;
		Engine::ByteAddressBuffer* mOctreeCounterBuffer;
		unsigned int mOctreeLevels;
		unsigned int mOctreeNodes;
		unsigned int mOctreeDataSize[3];

		Engine::GpuMappedBuffer* mVoxelizeParams;
		void* mVoxelizeParamsPtr;

		Engine::GpuMappedBuffer* mVoxelizeReadback;
		void* mVoxelizeReadbackPtr;

		Engine::GpuMappedBuffer* mOctreeReadback;
		void* mOctreeReadbackPtr;

		/*Engine::GpuMappedBuffer* mDataReadback;
		void* mDataReadbackPtr;

		Engine::GpuMappedBuffer* mTreeReadback;
		void* mTreeReadbackPtr;*/

		unsigned int mDimensions;

		Engine::GpuBuffer* mVertexBuffer;
		Engine::GpuBuffer* mIndexBuffer;

		unsigned int mInstances;
		unsigned int mVoxelArraySize;

		unsigned int CalculateOctreeNodes(unsigned int levels, unsigned int leafs)
		{
			unsigned int nodes = 0;
			unsigned int tmp = 1;
			for (unsigned int i = 0; i < levels; i++)
			{
				nodes += (tmp < leafs ? tmp : leafs);
				tmp *= 8;
			}
			return nodes;
		}

	public:
		VoxelizeOctree(Engine::D3DRenderer* renderer, unsigned int resolution);

		void Prepare(Engine::D3DRenderer* renderer);

		void BuildOctree(Engine::D3DRenderer* renderer, Engine::QueryHeap* heap, Engine::QueryHandle* timestamps);

		void Generate(Engine::DescriptorHeap* heap, Engine::GraphicsContext* context, std::vector<Engine::RenderNode>& nodes);

		void DrawVoxels(Engine::GraphicsContext* context);
		
		void ReadbackCounter(Engine::GraphicsContext* context);
	};
}

#endif