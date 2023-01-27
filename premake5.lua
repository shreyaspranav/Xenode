workspace "Xenode"
	configurations { "Debug", "Release_Debug", "Production" }
	architecture "x64"

	bin_folder = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	startproject "Xen"

-- dependencies:

group "Dependencies"
	include "Xenode/deps/glfw"
	include "Xenode/deps/imgui"
	include "Xenode/deps/yaml-cpp"
	include "Xenode/deps/freetype"
group ""

IncludeDir = {}
IncludeDir["spdlog"] = "%{wks.location}/Xenode/deps/spdlog/include"
IncludeDir["GLFW"] = "%{wks.location}/Xenode/deps/glfw/include"
IncludeDir["glad"] = "%{wks.location}/Xenode/deps/glad/include"
IncludeDir["glm"] = "%{wks.location}/Xenode/deps/glm/include"
IncludeDir["stb"] = "%{wks.location}/Xenode/deps/stb"
IncludeDir["ImGui"] = "%{wks.location}/Xenode/deps/imgui"
IncludeDir["Taskflow"] = "%{wks.location}/Xenode/deps/taskflow"
IncludeDir["entt"] = "%{wks.location}/Xenode/deps/entt/include"
IncludeDir["yaml_cpp"] = "%{wks.location}/Xenode/deps/yaml-cpp/include"
IncludeDir["freetype"] = "%{wks.location}/Xenode/deps/freetype/include"

-- Main projects:

include "Xenode/"
include "Xen/"
include "SandboxApp/"

include "SandboxAppAndroid/SandboxAndroidBuild.lua"
