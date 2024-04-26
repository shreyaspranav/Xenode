#include "pch"
#include "UUID.h"

namespace Xen {

	static std::random_device s_RandomDevice;
	static std::mt19937_64 s_Engine(s_RandomDevice());
	static std::uniform_int_distribution<uint64_t> s_UniformIntDistribution;

	UUID::UUID()
	{
		m_ID = s_UniformIntDistribution(s_Engine);
		//XEN_ENGINE_LOG_WARN("UUID: {0} Generated!", m_ID);
	}
	UUID::UUID(uint64_t id)
	{
		m_ID = id;
	}
	UUID::~UUID()
	{
	}
}