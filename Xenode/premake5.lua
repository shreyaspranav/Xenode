project "Xenode"

	kind "SharedLib"
	language "C++"

	targetdir ("%{wks.location}/bin/" .. bin_folder .. "/")
	objdir ("%{wks.location}/bin/" .. bin_folder .. "/obj/")

	pchheader "pch"
	pchsource "src/pch/pch.cpp"

	files {
		"src/core/app/**.cpp",
		"src/core/app/**.h",

		"src/pch/pch.cpp",
		"src/pch/pch",

		"src/Xenode.h",
		"src/Core.h"
	}

	includedirs {
		"src/",
		"src/pch/",

		"%{IncludeDir.spdlog}"
	}

	cppdialect "C++20"
	pic "on"

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