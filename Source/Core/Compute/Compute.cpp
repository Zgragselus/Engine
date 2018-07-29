#include "Compute.h"
#include <vector>

using namespace Engine;

Compute::Compute(const ContextType& type)
{
	// Get compatible platform
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	cl_context_properties props[] =
	{
		CL_CONTEXT_PLATFORM,
		(cl_context_properties)(platforms[0])(),
		0
	};

	// Get context
	mContext = cl::Context(type == CONTEXT_CPU ? CL_DEVICE_TYPE_CPU : 
		(type == CONTEXT_GPU ? CL_DEVICE_TYPE_GPU : CL_DEVICE_TYPE_ACCELERATOR), props);

	// Get devices list
	mDevices = mContext.getInfo<CL_CONTEXT_DEVICES>();

	// Generate command queue for device nr. 0 (Main CPU (for the case of multi-socket motherboards) and 
	// Main GPU (for laptops it is the more powerful one))
	mCommandQueue = cl::CommandQueue(mContext, mDevices[0]);
}

Compute::~Compute()
{
	// We don't need to do anything here (as of now), all cl handles are removed within scope
}