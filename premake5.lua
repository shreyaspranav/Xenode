workspace "Xenode"
	configurations { "Debug", "Release_Debug", "Production" }
	architecture "x64"

	bin_folder = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	startproject "Xen"

	--toolset "clang"

-- Vulkan SDK is used for shaderc and spirv-cross
VULKAN_SDK_PATH = os.getenv("VULKAN_SDK")

-- Paths of various resources
COMMON_RESOURCES_PATH   = "%{wks.location}/resources/CommonResources"
EDITOR_RESOURCES_PATH   = "%{wks.location}/resources/EditorResources"
PROJECTS_PATH           = "%{wks.location}/resources/projects"

-- Dependencies:
group "Dependencies"
	include "deps/glfw"
	include "deps/imgui"
	include "deps/yaml-cpp"
	include "deps/box2d"
	include "deps/lua"
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
IncludeDir["ImGradientHDR"] = "%{wks.location}/deps/ImGradientHDR"
IncludeDir["Optick"] 		= "%{wks.location}/deps/optick/include"
IncludeDir["Box2D"] 		= "%{wks.location}/deps/box2d/include"
IncludeDir["Lua"] 			= "%{wks.location}/deps/lua/include"
IncludeDir["SHA256"] 		= "%{wks.location}/deps/SHA256/include"


IncludeDir["VulkanSDK"] 	= "%{VULKAN_SDK_PATH}/Include"

Library = {}

-- Windows only for now.
-- ShaderC
Library["ShaderC_Debug"] 			= "%{VULKAN_SDK_PATH}/Lib/shaderc_sharedd.lib"
Library["ShaderC_Release"] 			= "%{VULKAN_SDK_PATH}/Lib/shaderc_shared.lib"

-- SpirV Cross
Library["SpirVCrossCore_Debug"] 	= "%{VULKAN_SDK_PATH}/Lib/spirv-cross-cored.lib"
Library["SpirVCrossCore_Release"] 	= "%{VULKAN_SDK_PATH}/Lib/spirv-cross-core.lib"
Library["SpirVCrossGLSL_Debug"] 	= "%{VULKAN_SDK_PATH}/Lib/spirv-cross-glsld.lib"
Library["SpirVCrossGLSL_Release"] 	= "%{VULKAN_SDK_PATH}/Lib/spirv-cross-glsl.lib"

newoption {
	trigger = "enable-profiling", 
	description = "Enables profiling support to the build."
}

-- Main projects:
group "Core"
	include "core/Xenode/Xenode.premake.lua"
	include "core/Xen/Xen.premake.lua"
	include "core/XenodeRuntime/XenodeRuntime.premake.lua"
group ""

group "Sandbox"
	include "sandbox/SandboxApp/SandboxApp.premake.lua"
group ""

group "Resources"
	include "resources/Resources.premake.lua"
group ""

--include "SandboxAppAndroid/SandboxAndroidBuild.lua"
