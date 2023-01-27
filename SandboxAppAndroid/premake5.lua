require "../deps/android_studio"

workspace "SandboxAppAndroid"
    configurations {"Debug", "Release_Debug", "Production"}
    
    gradleversion "com.android.tools.build:gradle:7.0.2"
    androidappid "org.xenode.sandboxappapk"

    gradlewrapper {
        "distributionUrl=https://services.gradle.org/distributions/gradle-7.0.2-all.zip"
    }

    location("SandboxApk")

    assetpacks {
		["pack"] = "install-time"
	}

    XENODE_ENGINE_SRC_DIR = "../Xenode/src"
    
project "SandboxAppAndroid"
    kind "ConsoleApp"
    language "C++"
	targetdir "bin/%{cfg.buildcfg}"
    cppdialect "C++17"
    
    androidsdkversion "28"
    androidminsdkversion "25"

    androidabis {
        "arm64-v8a",    -- 64 bit ARM architecture
        "armeabi-v7a"   -- 32 bit ARM architecture
    } --Will Add x86 and x86_64 later

    files {
        "%{XENODE_ENGINE_SRC_DIR}/core/app/EntryPoint.h",
        "%{XENODE_ENGINE_SRC_DIR}/core/app/EventDispatcher.h",
        "%{XENODE_ENGINE_SRC_DIR}/core/app/Events.h",
        "%{XENODE_ENGINE_SRC_DIR}/core/app/MobileApplication.h",
        "%{XENODE_ENGINE_SRC_DIR}/core/app/KeyCodes.h",
        "%{XENODE_ENGINE_SRC_DIR}/core/app/Layer.h",
        "%{XENODE_ENGINE_SRC_DIR}/core/app/LayerStack.h",
        "%{XENODE_ENGINE_SRC_DIR}/core/app/Log.h",

        "%{XENODE_ENGINE_SRC_DIR}/core/app/MobileApplication.cpp",
        "%{XENODE_ENGINE_SRC_DIR}/core/app/EventDispatcher.cpp",
        "%{XENODE_ENGINE_SRC_DIR}/core/app/LayerStack.cpp",
        "%{XENODE_ENGINE_SRC_DIR}/core/app/Log.cpp",

        "src/cpp/android_native_app_glue.c",
        "src/cpp/android_native_app_glue.h",
        "src/cpp/**.cpp",
        "src/cpp/**.h",

        --Android Manifest
        "src/AndroidManifest.xml"
    }

    removefiles {
        "%{XENODE_ENGINE_SRC_DIR}/core/app/Input.cpp",
        "%{XENODE_ENGINE_SRC_DIR}/core/app/Monitor.cpp",
        "%{XENODE_ENGINE_SRC_DIR}/core/app/Window.cpp",

    }

    includedirs {
        "src/cpp",
        "%{XENODE_ENGINE_SRC_DIR}/core/app",
        "%{XENODE_ENGINE_SRC_DIR}/pch",
        "%{XENODE_ENGINE_SRC_DIR}/",

        "%{XENODE_ENGINE_SRC_DIR}/../deps/glm/include",
        "%{XENODE_ENGINE_SRC_DIR}/../deps/entt/include",
        "%{XENODE_ENGINE_SRC_DIR}/../deps/spdlog/include",
    }

    links {
        "log", "EGL", "GLESv1_CM", "android",
    }

	assetpackdependencies {
		"pack"
	}

    defines {"XEN_PLATFORM_ANDROID", "XEN_DEVICE_MOBILE"}

    configuration "Debug"
		defines { "XEN_DEBUG", "XEN_LOG_ON" }
		symbols "On"

	configuration "Release_Debug"
		defines { "XEN_RELEASE", "XEN_LOG_ON" }
		optimize "On"

    configuration "Release_Debug"
		defines { "XEN_PRODUCTION", "XEN_LOG_OFF" }
		optimize "On"
