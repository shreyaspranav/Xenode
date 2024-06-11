#pragma once

#include <Core.h>
#include "Scene.h"
#include <core/renderer/Structs.h>

namespace Xen {
	class XEN_API ScenePhysics
	{
	public:
		static void Initialize(double fixedTimeStep);
		static void SetActiveScene(const Ref<Scene>& scene);

		static void RuntimeStart(const Vec3& gravity);

		static void Step(double timestep);
		static void FixedStepUpdate();

		static void RuntimeEnd();
	};
}