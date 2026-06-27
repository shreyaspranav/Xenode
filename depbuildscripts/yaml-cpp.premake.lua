local root = path.getabsolute("../deps/yaml-cpp")

project "yaml-cpp"
	kind "StaticLib"
	language "C++"

	-- To stop those annoying dll export warnings:
	warnings "Off"

	targetdir ("%{wks.location}/bin/" .. bin_folder .. "/bin/%{prj.name}")
	objdir ("%{wks.location}/bin/" .. bin_folder .. "/obj/%{prj.name}")

	files {
		root .. "/src/**.h",
		root .. "/src/**.cpp",

		root .. "/include/**.h"
	}

	includedirs {
		root .. "/include/"
	}

	defines {
        -- "yaml_cpp_EXPORTS",      -- If building a SharedLib
        "YAML_CPP_STATIC_DEFINE" -- If building a StaticLib
    }

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++17"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release_Debug"
		runtime "Release"
		optimize "on"

	filter "configurations:Production"
		runtime "Release"
		optimize "on"
		symbols "off"

	filter "action:vs*"
		buildoptions "/w"
