#include "pch"
#include "DebugRenderer.h"


#ifdef XEN_ENABLE_DEBUG_RENDERER
namespace Xen 
{
	void DebugRenderer::Init() 
	{

	}
	void DebugRenderer::Begin(const Ref<Camera>& camera) 
	{

	}
	void DebugRenderer::End() 
	{

	}

	void DebugRenderer::Draw2DBox(const Vec3& position, float rotation, const Vec2& scale, const Color& color, float thickness) 
	{

	}
	void DebugRenderer::Draw2DCircle(const Vec3& position, float rotation, const Vec2& scale, const Color& color, float thickness) 
	{

	}

	void DebugRenderer::DrawString(const std::string& text, const Vec3& position, float bOpacity, float size) 
	{

	}
}
#else
namespace Xen 
{
	void DebugRenderer::Init() {}
	void DebugRenderer::Begin(const Ref<Camera>& camera) {}
	void DebugRenderer::End() {}
	void DebugRenderer::Draw2DBox(const Vec3& position, float rotation, const Vec2& scale, const Color& color, float thickness) {}
	void DebugRenderer::Draw2DCircle(const Vec3& position, float rotation, const Vec2& scale, const Color& color, float thickness) {}
	void DebugRenderer::DrawString(const std::string& text, const Vec3& position, float bOpacity = 0.2f, float size) {}
}
#endif
