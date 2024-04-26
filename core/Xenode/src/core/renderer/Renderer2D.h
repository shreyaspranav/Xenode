#pragma once

#include <Core.h>

#include "Camera.h"
#include "Texture.h"
#include "Structs.h"
#include "ParticleSettings2D.h"

#include "SpriteRenderer2D.h"
#include "ParticlesRenderer2D.h"


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

		struct QuadSprite 
		{
			Vec3 position;
			float rotation;
			Vec2 scale;

			Color color[4];
			bool useSingleColor = true;
			
			// TODO: Change this to AssetHandle:
			Ref<Texture2D> texture;
			Vec2 textureCoords[4] = {
				{ 1.0f, 1.0f },
				{ 0.0f, 1.0f },
				{ 0.0f, 0.0f },
				{ 1.0f, 0.0f }
			};

			int32_t id = -1;
		};

		struct CircleSprite
		{
			Vec3 position;
			float rotation;
			Vec2 scale;

			Color color;

			Vec2 worldCoords[4] = {
				{  1.0f,  1.0f },
				{ -1.0f,  1.0f },
				{ -1.0f, -1.0f },
				{  1.0f, -1.0f }
			};

			float thickness = 1.0f, innerFade = 0.0f, outerFade = 0.0f;
			int32_t id = -1;
		};

	public:
		static void Init()
		{
			SpriteRenderer2D::Init();
			ParticlesRenderer2D::Init();
		}

		static void BeginScene(const Ref<Camera>& camera)
		{
			SpriteRenderer2D::BeginScene(camera);
			ParticlesRenderer2D::BeginScene(camera);
		}

		static void EndScene()
		{
			SpriteRenderer2D::EndScene();
			ParticlesRenderer2D::EndScene();
		}

		static void RenderFrame(double timestep)
		{
			SpriteRenderer2D::RenderFrame(timestep);
			ParticlesRenderer2D::RenderFrame(timestep);
		}
		
		// These functions do not draw, but adds them to a buffer and then renders them in RenderFrame()
		// 
		// Implemented in SpriteRenderer2D.cpp ---------------------------------------------------------
		static void DrawQuadSprite(const QuadSprite& quadSprite);
		static void DrawCircleSprite(const CircleSprite& circleSprite);

		// Implemented in ParticlesRenderer2D.cpp ------------------------------------------------------
		static void DrawParticles(const ParticleSettings2D& particles);
	};

}
