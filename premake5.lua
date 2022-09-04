workspace "Xenode"
	configurations { "Debug", "Release_Debug", "Distribution" }
	architecture "x64"

	bin_folder = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	startproject "SandboxApp"

-- dependencies:
--include "Xenode/deps/glfw"

IncludeDir = {}
IncludeDir["spdlog"] = "%{wks.location}/Xenode/deps/spdlog/include"

-- Main projects:

include "Xenode/"
include "SandboxApp/"
