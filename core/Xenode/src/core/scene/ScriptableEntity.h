#pragma once
#ifdef XEN_DEVICE_DESKTOP

#include "Scene.h"
#include "core/app/input/KeyboardInput.h"
#include "core/app/input/MouseInput.h"
#include <core/app/GameApplication.h>

namespace Xen {
	class ScriptableEntity
	{
	public:
		virtual ~ScriptableEntity() {}

		template<typename T>
		inline T& GetComponent() { return m_Entity.GetComponent<T>(); }

		virtual void OnCreate() {}
		virtual void OnDestroy() {}
		virtual void OnUpdate(double timestep) {}

	private:
		Entity m_Entity;
	};
}
#endif

#ifdef XEN_DEVICE_MOBILE
namespace Xen {
	class ScriptableEntity
	{
	public:
	};
}
#endif