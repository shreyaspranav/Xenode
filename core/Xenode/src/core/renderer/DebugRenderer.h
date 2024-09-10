#pragma once
#include <Core.h>

#include "Structs.h"
#include "Camera.h"

namespace Xen 
{
	// DebugRenderer is common to both 2D and 3D Renderer
	// DebugRenderer can be completely stripped from the build by not defining XEN_ENABLE_DEBUG_RENDERER
	class XEN_API DebugRenderer
	{
	public:
		static void Init(uint32_t width, uint32_t height);
		static void Begin(const Ref<Camera>& camera);
		static void End();

		static void RenderFrame(double timestep);

		static void OnFrameBufferResize(uint32_t width, uint32_t height);
		
		static void Draw2DQuad(const Vec3& position, float rotation, const Vec2& scale, const Color& color, float thickness = 1.0f, bool useScreenCoordinates = false);
		static void Draw2DCircle(const Vec3& position, float rotation, const Vec2& scale, const Color& color, float thickness = 1.0f, bool useScreenCoordinates = false);

		static void DrawString(const std::string& text, const Vec3& position, const Color& color, float bOpacity = 0.2f, float size = 1.0f, bool useScreenCoordinates = false);
	
	};
}