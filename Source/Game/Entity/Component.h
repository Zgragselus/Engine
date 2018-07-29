#ifndef __COMPONENT__H__
#define __COMPONENT__H__

#include "Core/Memory/Memory.h"
#include "Core/Util/String.h"

namespace Engine
{
	// Toplevel component class used as a base-class for Entity-components
	class __declspec(align(16)) Component
	{
	private:
		static std::string mComponentName;

	protected:
		Component() = default;

	public:
		virtual ~Component() {}

		static std::string& GetName() { return mComponentName; }

		virtual bool Editor(std::string& prev, std::string& next) = 0;

		virtual std::string Serialize() = 0;

		virtual void Deserialize(const std::string&) = 0;

		ALIGNED_NEW_DELETE("Engine::Component")
	};
}

#endif