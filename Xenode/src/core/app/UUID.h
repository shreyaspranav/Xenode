#pragma once
#include <Core.h>

#include <xhash>

namespace Xen {
	class XEN_API UUID {
	public:
		UUID();
		UUID(uint64_t id);
		~UUID();

		operator uint64_t() const { return m_ID; }
	private:
		uint64_t m_ID;
	};
}

// TODO: Learn hashing:
namespace std {
	template<>
	struct hash<Xen::UUID> {
		std::size_t operator()(const Xen::UUID& id) const { return hash<uint64_t>()((uint64_t)id); }
	};
}