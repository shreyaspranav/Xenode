#pragma once

#include <Core.h>

namespace Xen {
	namespace Utils {
		std::string OpenFileDialogOpen(const char* filter);
		std::string OpenFileDialogSave(const char* filter);
	}
}