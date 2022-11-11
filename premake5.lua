workspace "Xenode"
	configurations { "Debug", "Release_Debug", "Production" }
	architecture "x64"

	bin_folder = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	startproject "Xen"
-- dependencies:

--VULKAN_SDK_ROOT = "C:/VulkanSDK/1.3.216.0"

group "Dependencies"
	include "Xenode/deps/glfw"
	include "Xenode/deps/imgui"
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

--IncludeDir["VulkanSDK"] = "%{VULKAN_SDK_ROOT}/Include"

-- Main projects:

include "Xenode/"
include "Xen/"
include "SandboxApp/"
