local root = path.getabsolute("../deps/imgui")

project "ImGui"
	language "C++"
	kind "StaticLib"
	cppdialect "C++17"

	targetdir ("%{wks.location}/bin/" .. bin_folder .. "/bin/%{prj.name}")
	objdir ("%{wks.location}/bin/" .. bin_folder .. "/obj/%{prj.name}")

	files {
		root .. "/imconfig.h",
		root .. "/imgui.h",
		root .. "/imgui.cpp",
		root .. "/imgui_draw.cpp",
		root .. "/imgui_internal.h",
		root .. "/imgui_widgets.cpp",
		root .. "/imstb_rectpack.h",
		root .. "/imstb_textedit.h",
		root .. "/imstb_truetype.h",
		root .. "/imgui_demo.cpp",
		root .. "/imgui_tables.cpp",

		root .. "/backends/imgui_impl_glfw.h",
		root .. "/backends/imgui_impl_glfw.cpp",
		root .. "/backends/imgui_impl_opengl3.h",
		root .. "/backends/imgui_impl_opengl3.cpp",
		root .. "/backends/imgui_impl_opengl3_loader.h",
	}

	links { "GLFW" }

	includedirs {
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glad}",
	}

	filter "system:windows"
		systemversion "latest"

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
