#pragma once
#include <Core.h>

namespace Xen {
	class XEN_API Script
	{
	public:
		static Ref<Script> CreateScript(const std::string& filePath);


		virtual inline const std::string& GetFilePath() const = 0;
	};
}
