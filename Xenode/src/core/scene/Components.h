#pragma once

#include <core/app/UUID.h>
#include <core/renderer/Structs.h>
#include <core/renderer/Texture.h>
#include <core/renderer/Camera.h>
#include <scripting/Script.h>
#include "ScriptableEntity.h"
#include <pch/pch>

namespace Xen {

	enum class SpriteRendererPrimitive {
		Triangle, Quad, Polygon
	};

	namespace Component {

		struct ID 
		{
			UUID id;

			ID() : id(UUID()) {}
			ID(const UUID& id) : id(id) {}
			ID(const ID& tag) = default;
		};

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
			Vec3 rotation = 0.0f;
			Vec3 scale = 1.0f;

			Transform() = default;
			Transform(const Transform& transform) = default;

			Transform(const Vec3& position, const Vec3& rotation, const Vec3& scale) 
				: position(position), rotation(rotation), scale(scale) {}
		};

		struct SpriteRenderer
		{
			Color color;

			SpriteRendererPrimitive primitive;
			uint32_t polygon_segment_count = 5;

			Ref<Texture2D> texture;

			float texture_tile_factor;

			SpriteRenderer() = default;
			SpriteRenderer(const SpriteRenderer& transform) = default;

			SpriteRenderer(const Color& color, SpriteRendererPrimitive primitive = SpriteRendererPrimitive::Quad, uint32_t polygon_segments = 4) 
				: color(color), 
				texture(nullptr), 
				texture_tile_factor(1.0f),
				polygon_segment_count(polygon_segments),
				primitive(primitive) {}

			SpriteRenderer(const Color& color, Ref<Texture2D> texture, float tile_factor = 1.0f) 
				: color(color), texture(texture), texture_tile_factor(tile_factor), primitive(SpriteRendererPrimitive::Quad) {}
		};

		struct CircleRenderer
		{
			Color color = Color(1.0f);
			float thickness = 1.0f;
			float inner_fade = 0.0f;
			float outer_fade = 0.0f;

			CircleRenderer() = default;
			CircleRenderer(const CircleRenderer& renderer) = default;

			CircleRenderer(const Color& color, float thickness, float innerfade, float outerfade) 
				: color(color), thickness(thickness), inner_fade(innerfade), outer_fade(outerfade) {}
		};

		struct TextRenderer
		{
			std::string text = "Text";
			Color color = Color(1.0f, 1.0f, 1.0f, 1.0f);
			bool bold = 0, italic = 0;

			TextRenderer() = default;
			TextRenderer(const TextRenderer& textRenderer) = default;

			TextRenderer(const std::string& text, bool bold, bool italic) 
				: text(text), italic(italic), bold(bold) {}
		};

		struct CameraComp
		{
			Ref<Camera> camera;
			bool is_primary_camera = 1;
			bool is_resizable = 1;

			CameraComp() = default;
			CameraComp(const CameraComp& transform) = default;

			CameraComp(Xen::CameraType type, float viewport_width, float viewport_height) 
				: camera(std::make_shared<Camera>(type, viewport_width, viewport_height)) {}

			CameraComp(const Ref<Camera>& camera) : camera(camera) {}
		};

		struct NativeScript
		{
			ScriptableEntity* scriptable_entity_instance = nullptr;

			ScriptableEntity* (*InstantiateScript)();
			void (*DestroyScript)(NativeScript*);

			Entity entity;

			template<typename T>
			void Bind(Entity e)
			{
				entity = e;
				InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
				DestroyScript = [](NativeScript* script) 
				{ 
					delete script->scriptable_entity_instance; 
					script->scriptable_entity_instance = nullptr; 
				};
			}
		};

		struct ScriptComp
		{
			Ref<Script> script_instance = nullptr;

			ScriptComp() = default;
			ScriptComp(const ScriptComp& comp) = default;

			ScriptComp(const Ref<Script>& script)
				: script_instance(script) {}
		};

		// Physics Components:
		struct RigidBody2D
		{
			enum class BodyType : int8_t { Static, Dynamic, Kinematic };

			BodyType bodyType = BodyType::Static;
			bool fixedRotation = false;

			// Storage for the runtime object
			void* runtimeBody = nullptr;

			RigidBody2D() = default;
			RigidBody2D(const RigidBody2D& rigidBody) = default;

			RigidBody2D(BodyType type, bool fixedRotation = false)
				:bodyType(type), fixedRotation(fixedRotation) {}

		};

		struct BoxCollider2D
		{
			Vec2 bodyOffset = { 0.0f, 0.0f };
			Vec2 size		= { 0.5f, 0.5f }; // This is a half-extents

			float bodyDensity = 1.0f;
			float bodyFriction = 0.1f;
			float bodyRestitution = 0.4f;
			float bodyRestitutionThreshold = 0.5f;

			// Storage for the runtime object
			void* runtimeBody = nullptr;

			BoxCollider2D() = default;
			BoxCollider2D(const BoxCollider2D& rigidBody) = default;

			BoxCollider2D(const Vec2& bodyOffset, const Vec2& size)
				:bodyOffset(bodyOffset), size(size) {}

		};
	}
}