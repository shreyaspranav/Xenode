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

-- Main projects:

include "Xenode/"
include "SandboxApp/"
