#pragma once

#include <Core.h>

namespace Xen {
	class XEN_API Layer
	{
	public:
		uint32_t layerID;
		std::string layerName;

	public:
		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(double timestep) {}
	};
}