#pragma once

#include <Core.h>
#include <core/app/DesktopApplication.h>

namespace Xen {
	class XEN_API ScenePhysics
	{
	public:
		ScenePhysics(GameMode mode);
		~ScenePhysics();

		void InitPhysics();
		void EndPhysics();
	private:
		GameMode m_CurrentGameMode;
	};
}