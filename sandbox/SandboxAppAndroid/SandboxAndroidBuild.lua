project "SandboxAndroidBuild"
    kind "Makefile"

    ANDROID_NDK_DIR = os.getenv("NDK_ROOT")
    XENODE_ENGINE_SRC_DIR = "../Xenode/src"

    files {
        "src/cpp/**.c",
        "src/cpp/**.cpp",
        "src/cpp/**.h",
    }

    includedirs {
        "src/cpp/",
        "%{ANDROID_NDK_DIR}/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/include",
        "%{XENODE_ENGINE_SRC_DIR}/core/app",
        "%{XENODE_ENGINE_SRC_DIR}/",

    }

    defines {"XEN_PLATFORM_ANDROID", "XEN_DEVICE_MOBILE", "XEN_LOG_ON"}

    filter "system:windows"
        buildcommands {
            "..\\premake-windows android-studio",
            "cd SandboxApk",
            "gradle build"
        }
    filter "system:linux"
        buildcommands {
            "../premake-linux android-studio",
            "cd SandboxApk",
            "gradle build"
        }