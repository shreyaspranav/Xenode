// Documentation: ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
// Xenode.h is a header file that needs to be included in the runtime application or any test application to include the basic files required to
// setup a "game application". This header includes the headers which is in the application layer of the engine.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Core.h>

// The heart of the engine - the GameApplication class
// the runtime or the "game" is the instance of the GameApplication
#include <core/app/GameApplication.h>

// Include GameApplication implementations as well and its support classes
#ifdef XEN_DEVICE_DESKTOP
#include <core/app/desktop/DesktopGameApplication.h>
#include <core/app/desktop/Monitor.h>
#include <core/app/desktop/Window.h>

#elif XEN_DEVICE_MOBILE
#include <core/app/mobile/MobileApplication.h>
#include <core/app/mobile/Surface.h>
#endif

// These headers deal with input from various input devices
#include <core/app/input/KeyboardInput.h>
#include <core/app/input/MouseInput.h>

// This is the header where the main "entry point" of the runtime is located(i.e., the main function)
#ifdef XEN_INCLUDE_ENTRY_POINT
#include <core/app/EntryPoint.h>
#endif

// These headers deals with various events that occur in the engine.
#include <core/app/EventDispatcher.h>
#include <core/app/Events.h>

// This header deals with logging, will be stripped in a Production build 
#include <core/app/Log.h>

// This header has the implementation of the Layer class, 
// the GameApplication can have any number of "layers" and each layer can represent a "layer" of a game
// during runtime, all layers are initialized, updated and deinitialized one after the ohter
#include <core/app/Layer.h>

// This header includes the profiler, will be stripped in a Production build
#include <core/app/Profiler.h>

// This header is used to calculate time on a piece of code
#include <core/app/Timer.h>

// TODO: For now, ImGui Rendering only supports desktop platforms,
// Implement mobile implementations as well
#ifdef XEN_DEVICE_DESKTOP
#include <imgui/ImGuiLayer.h>
#endif
