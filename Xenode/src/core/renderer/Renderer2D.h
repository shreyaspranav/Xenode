#pragma once

#include "RenderCommandQueue.h"
#include "Primitives.h"

namespace Xen {

	class Renderer2D
	{
	private:
		static SceneData s_Data;

	public:
		static void Init();
		static void ShutDown();
		
		static void BeginScene();
		static void BeginScene(const Ref<OrthographicCamera>& camera);
		static void EndScene();
		
		static void RenderFrame();

		static void Submit(Ref<VertexArray> vertexArray);
		static void Submit(_2D::Quad& quad);

		// Draw Functions:
		static void DrawClearQuad(const Vec3& position, float rotation = 0.0f, const Vec2& scale = Vec2(1.0f, 1.0f), const Color& color = Color());
		static void DrawClearQuad(const Vec3& position, float rotation = 0.0f, const Vec2& scale = Vec2(1.0f, 1.0f), const Color* color = nullptr);
		
		static void DrawTexturedQuad(const Ref<Texture2D>& texture, const Vec3& position, float rotation = 0.0f, const Vec2& scale = Vec2(1.0f, 1.0f), const Color& tintcolor = Color(1.0f), float tiling_factor = 1.0f);
		static void DrawTexturedQuad(const Ref<Texture2D>& texture, const float* tex_coords, const Vec3& position, float rotation = 0.0f, const Vec2& scale = Vec2(1.0f, 1.0f), const Color& tintcolor = Color(1.0f), float tiling_factor = 1.0f);
	
	private:
		static void AddQuad(const Vec3& position, float rotation, const Vec2& scale);
	};

}
