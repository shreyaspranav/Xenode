workspace "Xenode"
	configurations { "Debug", "Release_Debug", "Production" }
	architecture "x64"

	bin_folder = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	startproject "SandboxApp"

-- dependencies:
include "Xenode/deps/glfw"

IncludeDir = {}
IncludeDir["spdlog"] = "%{wks.location}/Xenode/deps/spdlog/include"
IncludeDir["GLFW"] = "%{wks.location}/Xenode/deps/glfw/include"
IncludeDir["glad"] = "%{wks.location}/Xenode/deps/glad/include"
IncludeDir["stb"] = "%{wks.location}/Xenode/deps/stb"

newoption {
	trigger     = "gfxapi",
	value       = "API",
	description = "Choose a particular 3D API for rendering",
	default     = "opengl",
	allowed = {
	   { "opengl",		"OpenGL" },
	   { "vulkan",		"Vulkan" },
	   { "directx",		"Direct3D (Windows only)" },
	   { "metal",		"Metal (iOS only)"},
	   { "opengles",	"OpenGL ES (Mobile platforms only)" }
	}
}

-- Main projects:

include "Xenode/"
include "SandboxApp/"
