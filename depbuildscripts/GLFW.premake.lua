local root = path.getabsolute("../deps/glfw")

project "GLFW"
	kind "StaticLib"
	language "C"
	-- staticruntime "on"

	targetdir ("%{wks.location}/bin/" .. bin_folder .. "/bin/%{prj.name}")
	objdir ("%{wks.location}/bin/" .. bin_folder .. "/obj/%{prj.name}")

	files {
		root .. "/include/GLFW/glfw3.h",
		root .. "/include/GLFW/glfw3native.h",
		root .. "/src/internal.h",
		root .. "/src/platform.h",
		root .. "/src/mappings.h",
		root .. "/src/context.c",
		root .. "/src/init.c",
		root .. "/src/input.c",
		root .. "/src/monitor.c",
		root .. "/src/platform.c",
		root .. "/src/vulkan.c",
		root .. "/src/window.c",
		root .. "/src/egl_context.c",
		root .. "/src/osmesa_context.c",
		root .. "/src/null_platform.h",
		root .. "/src/null_joystick.h",
		root .. "/src/null_init.c",
		root .. "/src/null_monitor.c",
		root .. "/src/null_window.c",
		root .. "/src/null_joystick.c"
	}
	filter "system:linux"
		systemversion "latest"

		files {
			root .. "/src/x11_init.c",
			root .. "/src/x11_monitor.c",
			root .. "/src/x11_window.c",
			root .. "/src/x11_platform.h",
			root .. "/src/xkb_unicode.c",
			root .. "/src/xkb_unicode .h",
			root .. "/src/posix_module.c",
			root .. "/src/posix_poll.c",
			root .. "/src/posix_poll.h",
			root .. "/src/posix_thread.c",
			root .. "/src/posix_thread.h",
			root .. "/src/posix_time.h",
			root .. "/src/posix_time.c",
			root .. "/src/glx_context.c",
			root .. "/src/egl_context.c",
			root .. "/src/osmesa_context.c",
			root .. "/src/linux_joystick.c",
			root .. "/src/linux_joystick.h",
		}

		defines
		{
			"_GLFW_X11"
		}

		-- Link 'pthread' and 'dl' to link properly!!!!

	filter "system:windows"
		systemversion "latest"

		files {
			root .. "/src/win32_init.c",
			root .. "/src/win32_joystick.h",
			root .. "/src/win32_joystick.c",
			root .. "/src/win32_module.c",
			root .. "/src/win32_monitor.c",
			root .. "/src/win32_platform.h",
			root .. "/src/win32_thread.h",
			root .. "/src/win32_thread.c",
			root .. "/src/win32_time.c",
			root .. "/src/win32_time.h",
			root .. "/src/win32_window.c",
			root .. "/src/wgl_context.c",
		}

		defines {
			"_GLFW_WIN32",
			"_CRT_SECURE_NO_WARNINGS"
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
