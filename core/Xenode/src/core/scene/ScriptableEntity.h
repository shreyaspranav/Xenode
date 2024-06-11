#pragma once
#ifdef XEN_DEVICE_DESKTOP

#include "Scene.h"
#include "core/app/Input.h"
#include "core/app/DesktopApplication.h"

namespace Xen {
	class ScriptableEntity
	{
	public:
		virtual ~ScriptableEntity() {}

		template<typename T>
		inline T& GetComponent() { return m_Entity.GetComponent<T>(); }

		inline Ref<Input> GetInput() 
		{
			Ref<Input> input = Input::GetInputInterface();
			input->SetWindow(DesktopApplication::GetWindow());
			return input;
		}

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