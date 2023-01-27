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

		"%{IncludeDir.spdlog}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.freetype}",
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

		defines { "XEN_PLATFORM_WINDOWS", "XEN_BUILD_EXE", "XEN_DEVICE_DESKTOP" }

	filter "system:linux"

		systemversion "latest"

		includedirs {
			"%{wks.location}/Xenode/src/platform/linux",
			"%{wks.location}/Xenode/src"
		}
		buildoptions "-std=gnu++2b"
		defines { "XEN_PLATFORM_LINUX", "XEN_BUILD_EXEC", "XEN_DEVICE_DESKTOP" }

	filter "configurations:Debug"
        defines {"XEN_DEBUG", "XEN_LOG_ON"}
        symbols "On"

    filter "configurations:Release_Debug"
        defines {"XEN_RELEASE", "XEN_LOG_ON"}
        optimize "On"

    filter "configurations:Production"
		kind "WindowedApp"
        defines {"XEN_PRODUCTION", "XEN_LOG_OFF"}
        optimize "On"

    filter { "configurations:Debug", "system:windows" }
        buildoptions "/MDd"

    filter { "configurations:Release_Debug", "system:windows" }
        buildoptions "/MD"

    filter { "configurations:Production", "system:windows" }
        buildoptions "/MD"
