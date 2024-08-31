#pragma once

#include <Core.h>

namespace Xen
{
	enum class GraphicsAPI
	{
		XEN_OPENGL_API,
		XEN_OPENGLES_API,
		XEN_VULKAN_API,
		XEN_METAL_API,
		XEN_DIRECT3D_API
	};

	enum class GameType { _2D, _3D };

	enum class ScriptLang { Lua, CSharp };

	class XEN_API GameApplication
	{
	public:
		static constexpr uint32_t FIXED_FRAME_PER_SECONDS = 60;
		static constexpr float FIXED_TIME_STEP = (1.0f / static_cast<float>(FIXED_FRAME_PER_SECONDS)) * 1000.0f;

	public:
		virtual inline GameType GetGameType() const = 0;
		virtual inline GraphicsAPI GetGraphicsAPI() const = 0;
		virtual inline ScriptLang GetScriptLang() const = 0;
	};
	
	GameApplication* GetApplicationInstance();
}