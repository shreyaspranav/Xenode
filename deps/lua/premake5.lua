project "Lua"

	kind "StaticLib"
	language "C"
	pic "on"
	staticruntime "on"

	targetdir ("%{wks.location}/bin/" .. bin_folder .. "/bin/%{prj.name}")
	objdir ("%{wks.location}/bin/" .. bin_folder .. "/obj/%{prj.name}")

	files {
		"src/lapi.c", 
		"src/lcode.c", 
		"src/lctype.c", 
		"src/ldebug.c", 
		"src/ldo.c", 
		"src/ldump.c", 
		"src/lfunc.c", 
		"src/lgc.c", 
		"src/llex.c", 
		"src/lmem.c", 
		"src/lobject.c", 
		"src/lopcodes.c", 
		"src/lparser.c", 
		"src/lstate.c", 
		"src/lstring.c", 
		"src/ltable.c", 
		"src/ltm.c", 
		"src/lundump.c", 
		"src/lvm.c", 
		"src/lzio.c", 
		"src/lauxlib.c", 
		"src/lbaselib.c",
		"src/lcorolib.c", 
		"src/ldblib.c", 
		"src/liolib.c", 
		"src/lmathlib.c", 
		"src/loadlib.c", 
		"src/loslib.c", 
		"src/lstrlib.c", 
		"src/ltablib.c",
		"src/lutf8lib.c", 
		"src/linit.c"
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

project "LuaI"

	kind "ConsoleApp"
	language "C"
	pic "on"
	staticruntime "on"

	targetdir ("%{wks.location}/bin/" .. bin_folder .. "/bin/%{prj.name}")
	objdir ("%{wks.location}/bin/" .. bin_folder .. "/obj/%{prj.name}")

	files {
		"src/lua.c"
	}

	links "Lua"

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

project "LuaC"

	kind "ConsoleApp"
	language "C"
	pic "on"
	staticruntime "on"

	targetdir ("%{wks.location}/bin/" .. bin_folder .. "/bin/%{prj.name}")
	objdir ("%{wks.location}/bin/" .. bin_folder .. "/obj/%{prj.name}")

	files {
		"src/luac.c"
	}

	links "Lua"

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