#ifndef __COMPUTE_H__
#define __COMPUTE_H__

#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#include <CL/cl.hpp>
#include "Core/DllDef.h"

namespace Engine
{
	class Compute
	{
	public:
		enum ContextType
		{
			CONTEXT_CPU = 0,		/// CPU (general CPU - x86_64 AMD, x86_64 Intel + several ARM architectures supported)
			CONTEXT_GPU,			/// GPU (any general purpose GPU from NVidia and AMD)
			CONTEXT_ACCELERATOR		/// Generic accelerator (Intel Xeon Phi)
		};

	private:
		ContextType mType;					/// Context device type
		cl::Context mContext;				/// Context handle
		cl::CommandQueue mCommandQueue;		/// Command queue for communication with the device
		std::vector<cl::Device> mDevices;	/// List of devices

	public:
		Compute(const ContextType& type);

		~Compute();

		cl::Context& GetContext() { return mContext; }

		cl::CommandQueue& GetCommandQueue() { return mCommandQueue; }

		std::vector<cl::Device>& GetDevices() { return mDevices; }
	};
}

#endif
