#ifndef __GRAPHICS_PROFILER__H__
#define __GRAPHICS_PROFILER__H__

#include "Graphics/D3D12/Heap/QueryHeap.h"
#include "Graphics/D3D12/Resource/GpuMappedBuffer.h"

namespace SkyeCuillin
{
	/// <summary>
	/// Measure time-elapsed on GPU and allow for structured output
	/// </summary>
	class GraphicsProfiler
	{
	private:
		Engine::Log* mLog;								// Log to write errors into

		Engine::QueryHeap* mQueryHeap;					// Heap to store query results
		Engine::QueryHandle* mTimestamps;				// Handle for each timestamp in heap (separately)

		Engine::GpuMappedBuffer* mTimestampResults;		// Mapped buffer to read-back query results
		void* mTimestampResultsPtr;						// CPU pointer to mapped memory

		uint32_t mTimestampCount;						// Number of timestamps

		uint64_t mFrequency;							// Rate at which GPU counter increments
		
		std::map<std::string, double> mChannels;		// Profiling channels
		std::map<std::string, uint32_t> mChannelIDs;	// Profiling channel IDs
		uint32_t mChannelCount;							// Number of profiling channels

	public:
		/// <summary>
		/// Constructor, creates heap for queries, initializes mapped memory for it
		/// </summary>
		/// <param name="renderer">Associated renderer (device)</param>
		/// <param name="timestampCount">Number of timestamps (upper bound)</param>
		GraphicsProfiler(Engine::D3DRenderer* renderer, Engine::Log* log, uint32_t timestampCount)
		{
			mLog = log;
			mChannelCount = 0;

			// Make sure that our buffer size is a multiply of 256-bytes
			mTimestampCount = (timestampCount / 32) * 32 + (timestampCount % 32 > 0 ? 32 : 0);

			// Create heap
			mQueryHeap = new Engine::QueryHeap(renderer->GetDevice(), Engine::QueryHeapType::TIMESTAMP, mTimestampCount);
			// Initialize handle for each timestamp
			mTimestamps = new Engine::QueryHandle[mTimestampCount];
			for (size_t i = 0; i < mTimestampCount; i++)
			{
				mTimestamps[i] = mQueryHeap->Allocate();
			}

			// Create buffer to map timestamps to CPU memory (in readback mode)
			mTimestampResults = new Engine::GpuMappedBuffer();
			mTimestampResults->Init(renderer, mTimestampCount, sizeof(uint64_t), Engine::GpuMappedBuffer::Mode::READBACK);

			// Add default profiling channel
			AddChannel("Default");
		}

		/// <summary>
		/// Destructor
		/// </summary>
		~GraphicsProfiler()
		{
			delete mTimestampResults;
			delete[] mTimestamps;
			delete mQueryHeap;
		}

		/// <summary>
		/// Add new profiling channel
		/// </summary>
		/// <param name="channelName">Profiling channel name</param>
		inline void AddChannel(const std::string& channelName)
		{
			if (mChannelCount >= (mTimestampCount / 2))
			{
				mLog->Print("SkyeCuillin::GraphicsProfiler", std::string("Error: Not enough timestamps allocated!"));
			}
			else
			{
				if (GetChannel(channelName) == (uint32_t)(-1))
				{
					mChannels.insert(std::pair<std::string, double>(channelName, 0.0));
					mChannelIDs.insert(std::pair<std::string, uint32_t>(channelName, mChannelCount));
					mChannelCount++;
				}
				else
				{
					mLog->Print("SkyeCuillin::GraphicsProfiler", std::string("Error: Attempting to add Profiling channel '") + channelName + std::string("' which already exists!"));
				}
			}
		}

		/// <summary>
		/// Get profiling channel ID
		/// </summary>
		/// <param name="channelName">Profiling channel name</param>
		inline uint32_t GetChannel(const std::string& channelName)
		{
			auto it = mChannelIDs.find(channelName);
			if (it != mChannelIDs.end())
			{
				return it->second;
			}
			else
			{
				return (uint32_t)(-1);
			}
		}

		/// <summary>
		/// Get time elapsed for channel
		/// </summary>
		/// <param name="channelName">Profiling channel name</param>
		inline double GetTime(const std::string& channelName)
		{
			auto it = mChannels.find(channelName);
			if (it != mChannels.end())
			{
				return it->second;
			}
			else
			{
				return NAN;
			}
		}

		/// <summary>
		/// Begin profiling on specified channel. (Command added into CommandQueue for GPU Context)
		/// </summary>
		/// <param name="context">Context to which profiling is bound</param>
		/// <param name="channelName">Profiling channel name</param>
		inline void BeginProfile(Engine::Context* context, const std::string& channelName)
		{
			uint32_t channel = GetChannel(channelName);
			if (channel != (uint32_t)(-1))
			{
				context->EndQuery(mQueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, mTimestamps[channel * 2]);
			}
			else
			{
				mLog->Print("SkyeCuillin::GraphicsProfiler", std::string("Error: Attempting to begin profile on channel '") + channelName + std::string("' which does not exists!"));
			}
		}

		/// <summary>
		/// Finish profiling on specified channel. (Command added into CommandQueue for GPU Context)
		/// </summary>
		/// <param name="context">Context to which profiling is bound</param>
		/// <param name="channelName">Profiling channel name</param>
		inline void EndProfile(Engine::Context* context, const std::string& channelName)
		{
			uint32_t channel = GetChannel(channelName);
			if (channel != (uint32_t)(-1))
			{
				context->EndQuery(mQueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, mTimestamps[channel * 2 + 1]);
			}
			else
			{
				mLog->Print("SkyeCuillin::GraphicsProfiler", std::string("Error: Attempting to end profile on channel '") + channelName + std::string("' which does not exists!"));
			}
		}

		/// <summary>
		/// Reset profiling (NOTE: Must be called after context->Begin(...)).
		///
		/// Reads GPU counter frequency, enqueues in command queue sampling of beginning of the profiling timestamp into 'Default' channel.
		/// </summary>
		/// <param name="context">Context to which profiling is bound</param>
		inline void Reset(Engine::Context* context)
		{
			mFrequency = context->GetTimestampFrequency();
			BeginProfile(context, "Default");
		}

		/// <summary>
		/// Resolve profiling (NOTE: Must be called right before context->Finish(...), ideally between present of backbuffer and finish for GraphicsContext).
		///
		/// Enqueues in command queue sampling end of the profiling timestamp into 'Default' channel. Right after enqueues resolve command, to read back all timestamps (for all channels).
		/// </summary>
		/// <param name="context">Context to which profiling is bound</param>
		inline void Resolve(Engine::Context* context)
		{
			EndProfile(context, "Default");
			context->ResolveQueryData(mQueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, mTimestamps[0], mTimestampCount, mTimestampResults, 0);
		}

		/// <summary>
		/// Process profiling (NOTE: Must be called after context->Finish(...))
		///
		/// To ensure that this is executed after synchronization between GPU and CPU. Reads back all timestamps and calculates time spent for each channel.
		/// </summary>
		inline void Process()
		{
			mTimestampResultsPtr = mTimestampResults->Map(0, sizeof(uint64_t) * mTimestampCount);
			for (auto it = mChannels.begin(); it != mChannels.end(); it++)
			{
				uint32_t id = GetChannel(it->first) * 2;
				it->second = 1000.0 / mFrequency * (((uint64_t*)mTimestampResultsPtr)[id + 1] - ((uint64_t*)mTimestampResultsPtr)[id]);
			}
			mTimestampResults->Unmap();
		}
	};
}

#endif