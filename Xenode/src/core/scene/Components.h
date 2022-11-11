#pragma once

#include <core/renderer/Structs.h>
#include <core/renderer/Texture.h>
#include <pch/pch>

namespace Xen {
	namespace Component {

		struct Tag
		{
			std::string tag;

			Tag() : tag("Unnamed") {}
			Tag(const std::string& string) : tag(string) {}
			Tag(const Tag& tag) = default;
		};

		struct Transform
		{
			Vec3 position;
			float rotation = 0.0f;
			Vec2 scale = Vec2(1.0f, 1.0f);

			Transform() = default;
			Transform(const Transform& transform) = default;

			Transform(const Vec3& position, float rotation, const Vec2& scale) : position(position), rotation(rotation), scale(scale) {}
		};

		struct SpriteRenderer
		{
			Color color;
			Ref<Texture2D> texture;

			SpriteRenderer() = default;
			SpriteRenderer(const SpriteRenderer& transform) = default;

			SpriteRenderer(const Color& color, Ref<Texture2D> texture) : color(color), texture(texture) {}
		};
	}
}