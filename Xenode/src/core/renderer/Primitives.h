#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Structs.h"

namespace Xen {

	struct Drawable
	{
		Vec3 position;
		Vec3 rotation;
		Vec3 scale;

		Drawable(const Vec3& position, const Vec3& rotation, const Vec3& scale)
			:position(position), rotation(rotation), scale(scale)
		{}
	};

	namespace _2D
	{
		struct Quad : public Drawable
		{
			friend class Renderer2D;
			uint32_t ID;

			Vec2 texCoords[4];

			uint8_t texSlot;

		public:
			Quad(const Vec3& position = Vec3(0.0f), const Vec3& rotation = Vec3(0.0f), const Vec3& scale = Vec3(1.0f))
				:Drawable(position, rotation, scale)
			{
				//vertices[0] = Vec3( 0.5f,  0.5f, 0.0f);
				//vertices[1] = Vec3(-0.5f,  0.5f, 0.0f);
				//vertices[2] = Vec3(-0.5f, -0.5f, 0.0f);
				//vertices[3] = Vec3( 0.5f, -0.5f, 0.0f);
				//
				//indices[0] = 0;
				//indices[1] = 1;
				//indices[2] = 2;
				//indices[3] = 0;
				//indices[4] = 2;
				//indices[5] = 3;

				//CalculateVerticesAndIndices();
			}

			void CalculateVerticesAndIndices()
			{
				//vertices[0].x = position.x + (0.5f * scale.x);
				//vertices[0].y = position.y + (0.5f * scale.y);
				//
				//vertices[1].x = position.x - (0.5f * scale.x);
				//vertices[1].y = position.y + (0.5f * scale.y);
				//
				//vertices[2].x = position.x - (0.5f * scale.x);
				//vertices[2].y = position.y - (0.5f * scale.y);
				//
				//vertices[3].x = position.x + (0.5f * scale.x);
				//vertices[3].y = position.y - (0.5f * scale.y);
			}
		};
		struct Circle {};
		struct Line {};
	}
	namespace _3D
	{
		struct Cube {};
		struct Sphere {};
		struct Capsule {};
		struct Torus {};
	}
}