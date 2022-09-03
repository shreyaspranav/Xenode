workspace "Xenode"
	configurations { "Debug", "Release_Debug", "Distribution" }
	architecture "x64"

	bin_folder = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- dependencies:
--include "Xenode/deps/glfw"

-- Main projects:

include "Xenode/"
include "SandboxApp/"
