#pragma once

#include "Camera.h"
#include "Texture.h"
#include "Structs.h"
#include "ParticleSettings2D.h"

namespace Xen {

	class Renderer2D
	{
	private:
		// Way of identifing each primitive: 
		enum class Primitive {
			LINE =		1 << 0,
			TRIANGLE =	1 << 1,
			QUAD =		1 << 2,
			POLYGON =	1 << 3,
			CIRCLE =	1 << 4,

			POINT_LIGHT = 1 << 5,
			SHADOW_QUAD = 1 << 6
		};

	public:
		struct Renderer2DStatistics
		{
			uint32_t draw_calls;
			uint32_t quad_count;
			uint32_t circle_count;
			uint32_t texture_count;

			uint32_t batch_count;

			uint32_t quad_vertex_buffer_size;
			uint32_t quad_index_buffer_size;

			uint32_t circle_vertex_buffer_size;
			uint32_t circle_index_buffer_size;

			uint32_t quad_indices_drawn;
			uint32_t circle_indices_drawn;
		};

	public:
		static void Init();
		static void ShutDown();
		
		static void BeginScene(const Ref<Camera>& camera);
		static void EndScene();
		
		static void RenderFrame();
		static void RenderOverlay();

		static void RenderLights();

		static void AddParticles(const ParticleSettings2D* particleSettings);

		// Draw Functions:
		static void DrawClearQuad(const Vec3& position, const Vec3& rotation, const Vec2& scale, const Color& color = Color(), int32_t id = -1);
		static void DrawClearQuad(const Vec3& position, const Vec3& rotation, const Vec2& scale, const Color color[4], int32_t id = -1);
		
		static void DrawTexturedQuad(const Ref<Texture2D>& texture, const Vec3& position, const Vec3& rotation, const Vec2& scale, const Color& tintcolor = Color(1.0f), float tiling_factor = 1.0f, const Vec2 tex_coords[4] = nullptr, int32_t id = -1);
		static void DrawTexturedQuad(const Ref<Texture2D>& texture, const Vec3& position, const Vec3& rotation, const Vec2& scale, const Color tintcolor[4], float tiling_factor = 1.0f, const Vec2 tex_coords[4] = nullptr, int32_t id = -1);

		static void DrawLine(const Vec3& p1, const Vec3& p2, const Color& color = Color(1.0f, 1.0f, 1.0f, 1.0f), float thickness = 1.0f);

		static void DrawClearCircle(const Vec3& position, const Vec3& rotation, const Vec2& scale = 1.0f, const Color& color = Color(), float thickness = 1.0f, float innerfade = 0.0f, float outerfade = 0.0f, int32_t id = -1);

		static void SetLineWidth(float width);

		// Particles:
		static void DrawParticles(const ParticleSettings2D& particleSettings);

		// Outline Only(for Debugging)
		static void DrawQuadOverlay(const Vec3& position, const Vec3& rotation, const Vec2& scale, const Color& color = Color());
		static void DrawCircleOverlay(const Vec3& position, float radius, const Color& color = Color(), float thickness = 0.025f);


		// Stats functions:
		static Renderer2DStatistics& GetStatistics();
	
	private:
		static void AddPrimitive(Primitive primitive, const Vec3& position, const Vec3& rotation, const Vec2& scale, int32_t id);

		static void AddColorStatic(uint8_t vertex_count, const Color& color);
		static void AddColorArray(uint8_t vertex_count, const Color* color);
		static void AddTextureSlot(uint8_t vertex_count, bool is_clear_color, const Ref<Texture2D>& texture = nullptr);
		static void AddDefaultTextureCoords(Primitive p, float tiling_factor = 1.0f);
		static void AddID(uint8_t vertex_count, int32_t id);

		static void JumpDeltaVertexIndex(uint32_t index_delta);
	};

}
