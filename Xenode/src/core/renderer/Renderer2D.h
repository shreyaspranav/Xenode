#pragma once

#include "RenderCommandQueue.h"
#include "Primitives.h"

namespace Xen {

	class Renderer2D
	{
	private:
		static SceneData s_Data;
	public:
		struct Renderer2DStatistics
		{
			uint32_t draw_calls;
			uint32_t quad_count;
			uint32_t texture_count;

			uint32_t predefined_batches;

			uint32_t vertex_buffer_size;
			uint32_t index_buffer_size;

			uint32_t indices_drawn;
		};

	public:
		static void Init();
		static void ShutDown();
		
		static void BeginScene(const Ref<Camera>& camera, const Vec2& viewport_size);
		static void EndScene();
		
		static void RenderFrame();

		// Draw Functions:
		static void DrawClearQuad(const Vec3& position, float rotation = 0.0f, const Vec2& scale = Vec2(1.0f, 1.0f), const Color& color = Color());
		static void DrawClearQuad(const Vec3& position, const Vec3& rotation = 0.0f, const Vec3& scale = Vec3(1.0f), const Color& color = Color());
		static void DrawClearQuad(const Vec3& position, float rotation = 0.0f, const Vec2& scale = Vec2(1.0f, 1.0f), const Color* color = nullptr);
		
		static void DrawTexturedQuad(const Ref<Texture2D>& texture, const Vec3& position, float rotation = 0.0f, const Vec2& scale = Vec2(1.0f, 1.0f), const Color& tintcolor = Color(1.0f), float tiling_factor = 1.0f);
		static void DrawTexturedQuad(const Ref<Texture2D>& texture, const Vec3& position, const Vec3& rotation = 0.0f, const Vec3& scale = Vec3(1.0f), const Color& tintcolor = Color(1.0f), float tiling_factor = 1.0f);
		static void DrawTexturedQuad(const Ref<Texture2D>& texture, const float* tex_coords, const Vec3& position, float rotation = 0.0f, const Vec2& scale = Vec2(1.0f, 1.0f), const Color& tintcolor = Color(1.0f), float tiling_factor = 1.0f);

		//static void DrawClearPolygon(uint32_t segments, const Vec3& position, float rotation = 0.0f, const Vec2& scale = Vec2(1.0f, 1.0f));

		// Stats functions:
		static Renderer2DStatistics& GetStatistics();
	
	private:
		static void AddQuad(const Vec3& position, const Vec3& rotation, const Vec3& scale);
	};

}
