#pragma once

#include <Core.h>
#include "ScriptLang.h"
#include "Script.h"

#include <core/scene/Scene.h>

namespace Xen 
{
	class XEN_API ScriptEngine
	{
	public:
		static ScriptEngine* InitScriptEngine();

		virtual void OnStart(const Ref<Script>& script, Entity entity) = 0;

		virtual void OnUpdate(const Ref<Script>& script, Entity entity, double timestep) = 0;
		virtual void OnFixedUpdate(const Ref<Script>& script, Entity entity) = 0;
	};
}

