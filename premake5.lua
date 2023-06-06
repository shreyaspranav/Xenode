workspace "Xenode"
	configurations { "Debug", "Release_Debug", "Production" }
	architecture "x64"

	bin_folder = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	startproject "Xen"

	--toolset "clang"

-- dependencies:

group "Dependencies"
	include "deps/glfw"
	include "deps/imgui"
	include "deps/yaml-cpp"
	include "deps/box2d"
group ""

IncludeDir = {}
IncludeDir["spdlog"] 		= "%{wks.location}/deps/spdlog/include"
IncludeDir["GLFW"] 			= "%{wks.location}/deps/glfw/include"
IncludeDir["glad"] 			= "%{wks.location}/deps/glad/include"
IncludeDir["glm"] 			= "%{wks.location}/deps/glm/include"
IncludeDir["stb"] 			= "%{wks.location}/deps/stb"
IncludeDir["ImGui"]			= "%{wks.location}/deps/imgui"
IncludeDir["Taskflow"] 		= "%{wks.location}/deps/taskflow"
IncludeDir["entt"] 			= "%{wks.location}/deps/entt/include"
IncludeDir["yaml_cpp"] 		= "%{wks.location}/deps/yaml-cpp/include"
IncludeDir["ImGuizmo"] 		= "%{wks.location}/deps/ImGuizmo"
IncludeDir["Optick"] 		= "%{wks.location}/deps/optick/include"
IncludeDir["Box2D"] 		= "%{wks.location}/deps/box2d/include"

newoption {
	trigger = "enable-profiling", 
	description = "Enables profiling support to the build."
}

-- Main projects:

include "Xenode/"
include "Xen/"
include "SandboxApp/"

--include "SandboxAppAndroid/SandboxAndroidBuild.lua"
