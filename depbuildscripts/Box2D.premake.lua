local root = path.getabsolute("../deps/box2d")

project "Box2D"
    kind "StaticLib"
    language "C++"

	targetdir ("%{wks.location}/bin/" .. bin_folder .. "/bin/%{prj.name}")
	objdir ("%{wks.location}/bin/" .. bin_folder .. "/obj/%{prj.name}")

    files {
        root .. "/src/**.cpp",
        root .. "/src/**.h",
    }
    includedirs {
        root .. "/include"
    }

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
