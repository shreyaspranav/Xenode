project "Xenode"

	kind "StaticLib"
	language "C++"
	pic "on"
	staticruntime "On"

	targetdir ("%{wks.location}/bin/" .. bin_folder .. "/")
	objdir ("%{wks.location}/bin/" .. bin_folder .. "/obj/")

	files {
		-- Main project source files:
		"src/core/**.h",
		"src/gfxapi/**.h",
		"src/imgui/**.h",
		"src/scripting/**.h",

		"src/core/**.cpp",
		"src/gfxapi/**.cpp",
		"src/imgui/**.cpp",
		"src/scripting/**.cpp",

		"src/Core.h",
		"src/Xenode.h",

		"src/pch/pch.cpp",

		--ImGuizmo source files
		"../deps/ImGuizmo/*.cpp",
		"../deps/ImGuizmo/*.h",
	}

	links { 
		"GLFW", 
		"ImGui",
		"yaml-cpp",
		"Box2D",
		"Lua"
	}

	includedirs {
		"src/",
		"src/pch/",

		"../SandboxAppAndroid/src/cpp",

		"%{IncludeDir.spdlog}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.stb}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glad}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.Taskflow}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.freetype}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.Box2D}",
		"%{IncludeDir.Optick}",
		"%{IncludeDir.Lua}",
	}

	defines {"_SILENCE_ALL_CXX23_DEPRECATION_WARNINGS", "_CRT_SECURE_NO_WARNINGS"}

	filter { "options:enable-profiling"}
		files {
			"../deps/optick/src/*.cpp",
			"../deps/optick/src/*.h"
		}

		defines { "XEN_ENABLE_PROFILING" }

		removefiles {
			"../deps/optick/src/optick_gpu.vulkan.cpp" -- Remove vulkan for now
		}

	filter "files:../deps/ImGuizmo/**.cpp"
		flags { "NoPCH" }
	filter "files:../deps/optick/**.cpp"
		flags { "NoPCH" }

	filter "system:windows"

		systemversion "latest"

		files {
			"src/platform/windows/**.cpp",
			"src/platform/windows/**.h",
		}

		defines { "XEN_PLATFORM_WINDOWS", "XEN_BUILD_LIB", "XEN_DEVICE_DESKTOP" }

	filter "system:linux"

		systemversion "latest"

		files {
			"src/platform/linux/**.cpp",
			"src/platform/linux/**.h"
		}

		defines { "XEN_PLATFORM_LINUX", "XEN_BUILD_LIB", "XEN_DEVICE_DESKTOP" }

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

	filter "action:vs*"
		pchheader "pch"
		pchsource "src/pch/pch.cpp"

		buildoptions "/std:c++latest"	
		flags { "MultiProcessorCompile" }

		-- Disable warnings that I don't care:
		disablewarnings {
			"4244", -- conversion from 'type1' to 'type2'
			"4267", -- conversion from 'size_t' to other numeric data type
			"4018", -- Signed/Unsigned mismatch
			"4146", -- Unary minus operator applied to unsigned type: the number will still remain unsigned and show weird results  
			"4312", -- type cast to greater size
		}

		defines {" _SILENCE_ALL_CXX23_DEPRECATION_WARNINGS" }
	filter "action:gmake*"
		buildoptions { "-std=c++2b", "-Wa,-mbig-obj" }

