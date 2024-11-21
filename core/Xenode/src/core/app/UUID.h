#pragma once
#include <Core.h>

namespace Xen
{
	class XEN_API UUID
	{
	public:
		UUID();
		UUID(uint64_t id);
		~UUID();

		operator uint64_t() const { return m_ID; }

		bool operator==(UUID id) { return m_ID == id.m_ID; }
		bool operator!=(UUID id) { return m_ID != id.m_ID; }

		bool operator< (UUID id) { return m_ID < id.m_ID; }
		bool operator> (UUID id) { return m_ID > id.m_ID; }
		bool operator<=(UUID id) { return m_ID <= id.m_ID; }
		bool operator>=(UUID id) { return m_ID >= id.m_ID; }

		bool operator!()         { return !m_ID; }

	private:
		uint64_t m_ID;
	};

}
// This is used to handle null id's
#define NULL_ID Xen::UUID(0)

// TODO: Learn hashing:
namespace std {
	template<>
	struct hash<Xen::UUID> {
		std::size_t operator()(const Xen::UUID& id) const { return hash<uint64_t>()((uint64_t)id); }
	};
}