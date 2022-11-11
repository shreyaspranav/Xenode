project "Xenode"

	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	pic "on"

	targetdir ("%{wks.location}/bin/" .. bin_folder .. "/")
	objdir ("%{wks.location}/bin/" .. bin_folder .. "/obj/")

	pchheader "pch"
	pchsource "src/pch/pch.cpp"

	files {
		"src/core/app/**.cpp",
		"src/core/app/**.h",

		"src/core/renderer/**.cpp",
		"src/core/renderer/**.h",

		"src/core/scene/**.cpp",
		"src/core/scene/**.h",

		"src/imgui/**.cpp",
		"src/imgui/**.h",

		"src/gfxapi/OpenGL/**.cpp",
		"src/gfxapi/OpenGL/**.h",

		"src/gfxapi/window/glfw/**.cpp",
		"src/gfxapi/window/glfw/**.h",

		"src/pch/pch.cpp",
		"src/pch/pch",

		"src/Xenode.h",
		"src/Core.h"
	}

	links { 
		"GLFW", 
		"ImGui",
		--"%{VULKAN_SDK_ROOT}/Lib/vulkan-1.lib",
		--"%{VULKAN_SDK_ROOT}/Lib/VkLayer_utils.lib",
	}

	includedirs {
		"src/",
		"src/pch/",

		"%{IncludeDir.spdlog}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.stb}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glad}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.Taskflow}",
		"%{IncludeDir.entt}",
		
		--"%{IncludeDir.VulkanSDK}",
	}

	

	filter "system:windows"

		systemversion "latest"
		flags { "MultiProcessorCompile" }

		files {
			"src/platform/windows/**.cpp",
			"src/platform/windows/**.h",
		}

		defines { "XEN_PLATFORM_WINDOWS", "XEN_BUILD_DLL" }

	filter "system:linux"

		systemversion "latest"

		files {
			"src/platform/linux/**.cpp",
			"src/platform/linux/**.h"
		}

		defines { "XEN_PLATFORM_LINUX", "XEN_BUILD_SL" }
		buildoptions "-std=gnu++2b"
		links { "pthread", "dl" } -- IMP: GLFW fails to link without these

	filter "configurations:Debug"
		defines {"XEN_DEBUG", "XEN_LOG_ON"}
		symbols "On"

	filter "configurations:Release_Debug"
		defines {"XEN_RELEASE", "XEN_LOG_ON"}
		optimize "On"

	filter "configurations:Production"
		defines {"XEN_PRODUCTION", "XEN_LOG_OFF"}
		optimize "On"

	filter { "configurations:Debug", "system:windows" }
		buildoptions "/MDd"
	
	filter { "configurations:Release_Debug", "system:windows" }
		buildoptions "/MD"
	
	filter { "configurations:Production", "system:windows" }
		buildoptions "/MD"