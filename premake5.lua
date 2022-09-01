workspace "Xenode"
	configurations { "Debug", "Release_Debug", "Distribution" }
	architecture "x64"

	bin_folder = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Xenode"

	kind "SharedLib"
	language "C++"
	location "Xenode/"

	targetdir ("%{wks.location}/bin/" .. bin_folder .. "/")
	objdir ("%{wks.location}/bin/" .. bin_folder .. "/obj/")

	pchheader "pch"
	pchsource "Xenode/src/pch/pch.cpp"

	files {
		"Xenode/src/core/**.cpp",
		"Xenode/src/core/**.h",

		"Xenode/src/pch/pch.cpp",
		"Xenode/src/pch/pch.h",

		"Xenode/src/Xenode.h",
		"Xenode/src/Core.h"
	}

	includedirs {
		"Xenode/src/",
		"Xenode/src/pch/"
	}

	cppdialect "C++20"
	pic "on"

	filter "system:windows"

		systemversion "latest"
		flags { "MultiProcessorCompile" }

		files { 
			"Xenode/platform/windows/**.cpp",
			"Xenode/platform/windows/**.h",  
		}

		defines { "XEN_PLATFORM_WINDOWS", "XEN_BUILD_DLL" }

	filter "system:linux"

		systemversion "latest"

		files { 
			"Xenode/platform/linux/**.cpp",
			"Xenode/platform/linux/**.h"  
		}

		defines { "XEN_PLATFORM_LINUX", "XEN_BUILD_SL" }

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

project "SandboxApp"

	kind "ConsoleApp"
	language "C++"
	location "SandboxApp/"

	targetdir ("%{wks.location}/bin/" .. bin_folder .. "/")
	objdir ("%{wks.location}/bin/" .. bin_folder .. "/obj/")

	files {
		"SandboxApp/src/**.cpp",
		"SandboxApp/src/**.h",
	}

	includedirs {
		"Xenode/src/core/",
	}

	links { "Xenode" }

	cppdialect "C++20"
	pic "on"

	filter "system:windows"

		systemversion "latest"
		flags { "MultiProcessorCompile" }

		includedirs {
			"Xenode/src/platform/windows",
			"Xenode/src"
		}

		defines { "XEN_PLATFORM_WINDOWS", "XEN_BUILD_EXE" }

	filter "system:linux"

		systemversion "latest"

		includedirs {
			"Xenode/src/platform/linux",
			"Xenode/src"
		}

		defines { "XEN_PLATFORM_LINUX", "XEN_BUILD_EXEC" }

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

