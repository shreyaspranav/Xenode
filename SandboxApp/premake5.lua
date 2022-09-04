project "SandboxApp"

	kind "ConsoleApp"
	language "C++"

	targetdir ("%{wks.location}/bin/" .. bin_folder .. "/")
	objdir ("%{wks.location}/bin/" .. bin_folder .. "/obj/")

	files {
		"src/**.cpp",
		"src/**.h",
	}

	includedirs {
		"%{wks.location}/Xenode/src/core/",
	}

	links { "Xenode" }

	cppdialect "C++20"
	pic "on"

	filter "system:windows"

		systemversion "latest"
		flags { "MultiProcessorCompile" }

		includedirs {
			"%{wks.location}/Xenode/src/platform/windows",
			"%{wks.location}/Xenode/src"
		}

		defines { "XEN_PLATFORM_WINDOWS", "XEN_BUILD_EXE" }

	filter "system:linux"

		systemversion "latest"

		includedirs {
			"%{wks.location}/Xenode/src/platform/linux",
			"%{wks.location}/Xenode/src"
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
        buildoptions "/MTd"

    filter { "configurations:Release_Debug", "system:windows" }
        buildoptions "/MT"

    filter { "configurations:Production", "system:windows" }
        buildoptions "/MT"