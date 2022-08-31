#pragma once

#ifdef XEN_PLATFORM_WINDOWS
  #ifdef XEN_BUILD_DLL

    #ifdef _MSC_VER
      #define XEN_API __declspec(dllexport)
    #else
      #error Visual Studio only accepted in Windows!
    #endif

  #endif
  #ifdef XEN_BUILD_EXE

    #ifdef _MSC_VER
      #define XEN_API __declspec(dllimport)
    #else
      #error Visual Studio only accepted in Windows!
    #endif
  #endif
#endif

#ifdef XEN_PLATFORM_LINUX
  #ifdef XEN_BUILD_SL

    #ifdef __GNUG__
      #define XEN_API __attribute__((visibility("default")))
    #else
      #error GCC only accepted in Linux!
    #endif

  #endif
  #ifdef XEN_BUILD_EXEC

    #ifdef __GNUG__
      #define XEN_API
    #else
      #error GCC only accepted in Linux!
    #endif
  #endif
#endif
