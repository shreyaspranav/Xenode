#include <Core.h>

#ifdef XEN_DEVICE_MOBILE
#include <core/app/MobileApplication.h>
#include <core/app/Surface.h>

#elif XEN_DEVICE_DESKTOP
#include <core/app/desktop/DesktopGameApplication.h>
#endif

#include <core/app/Log.h>

#ifdef XEN_DEVICE_DESKTOP
#include <imgui/ImGuiLayer.h>
#endif

#include <Core.h>

#include <core/app/Log.h>
#include <core/app/Layer.h>

#include <core/app/input/KeyboardInput.h>
#include <core/app/input/MouseInput.h>

#include <core/renderer/Buffer.h>
#include <core/renderer/Shader.h>
#include <core/renderer/Renderer2D.h>
#include <core/renderer/Camera.h>
#include <core/renderer/Texture.h>
#include <core/renderer/FrameBuffer.h>

#include <core/scene/Scene.h>
#include <core/scene/Components.h>
#include <core/scene/SceneSerializer.h>