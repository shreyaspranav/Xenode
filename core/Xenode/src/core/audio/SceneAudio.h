#pragma once
#include <Core.h>
#include <core/scene/Scene.h>

namespace Xen {
	class XEN_API SceneAudio {
	public:
		static void Initialize();
		static void SetActiveScene(const Ref<Scene>& scene);

		static void RuntimeStart();
		static void RuntimeUpdate();
		static void RuntimeEnd();
	};
}
