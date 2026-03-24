#pragma once
#include <Core.h>

namespace Xen {
	class XEN_API AudioEngine {
	public:
		static void Init();
		static void Shutdown();
		static void SetMasterVolume(float volume);
		static float GetMasterVolume();
	};
}
