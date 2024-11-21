#pragma once

#include <core/app/UUID.h>
#include <core/renderer/Structs.h>
#include <core/renderer/Texture.h>
#include <core/renderer/Camera.h>
#include <core/scripting/Script.h>
#include <core/renderer/ParticleSettings2D.h>

#include <core/asset/Asset.h>

#include "ScriptableEntity.h"
#include <pch/pch>

#include <core/physics/Physics2D.h>

namespace Xen {

	// The Component namespace will contain all the components that can be added to an entity.
	namespace Component {

		// Component that holds the UUID id of an entity
		struct ID 
		{
			UUID id;

			ID() : id(UUID()) {}
			ID(const UUID& id) : id(id) {}
			ID(const ID& tag) = default;
		};

		// Component that holds the string "tag" of an entity
		struct Tag
		{
			std::string tag;

			Tag() : tag("Unnamed") {}
			Tag(const std::string& string) : tag(string) {}
			Tag(const Tag& tag) = default;
		};

		// Component that holds the position, rotation and scale of an entity 
		// TODO: Use a quaternion for rotation
		struct Transform
		{
			Vec3 position;
			Vec3 rotation = 0.0f;
			Vec3 scale = 1.0f;

			Transform() = default;
			Transform(const Transform& transform) = default;

			Transform(const Vec3& position, const Vec3& rotation, const Vec3& scale) 
				:position(position), rotation(rotation), scale(scale) {}
		};

		// Component that represents a sprite that is to be rendered
		// TODO: Add back rendering of triangle and polygons through shaders
		struct SpriteRenderer
		{
			enum class Primitive { Triangle, Quad, Polygon, Circle };

			struct CircleProperties		{ float thickness = 1.0f, innerfade = 0.0f, outerfade = 0.0f; };
			struct PolygonProperties	{ uint32_t segment_count = 5; };

			Color color;

			Primitive primitive;
			CircleProperties circle_properties;
			PolygonProperties polygon_properties;

			// Ref<Texture2D> texture;
			AssetHandle textureHandle;

			float texture_tile_factor;

			SpriteRenderer() = default;
			SpriteRenderer(const SpriteRenderer& transform) = default;

			SpriteRenderer(const Color& color, Primitive primitive = Primitive::Quad) 
				: color(color), 
				textureHandle(0),
				texture_tile_factor(1.0f),
				primitive(primitive) {}

			SpriteRenderer(const Color& color, AssetHandle textureHandle, float tile_factor = 1.0f) 
				: color(color), textureHandle(textureHandle), texture_tile_factor(tile_factor), primitive(Primitive::Quad) {}
		};

		// Component that represents renderable text
		// TODO: This is not implemented, implement this
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

		// Component that represents a scene camera.
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

		// Component that represents native C++ scripts
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

		// Component that represents a script written in Lua or any other scripting language
		struct ScriptComp
		{
			Ref<Script> script_instance = nullptr;
			
			// Temp: Will replace after having a asset manager
			std::string script_file_path = "";

			ScriptComp() = default;
			ScriptComp(const ScriptComp& comp) = default;

			ScriptComp(const Ref<Script>& script)
				: script_instance(script) 
			{
				script_file_path = script->GetFilePath();
			}

			ScriptComp(const std::string& filePath)
				: script_file_path(filePath)
			{
				script_instance = Script::CreateScript(filePath);
			}
		};

		// Represents a 2D point light
		// TODO: This is not implemented, implement this.
		struct PointLight
		{
			float radius = 1.0f;
			float intensity = 1.0f;
			float fallofA = 1.0f;
			float fallofB = 1.0f;

			bool castShadow = true;

			Color lightColor = { 1.0f, 1.0f, 1.0f, 1.0f };

			PointLight() = default;
			PointLight(const Color& color, float radius = 1.0f, float fallofA = 1.0f, float fallofB = 1.0f)
				:lightColor(color), radius(radius), fallofA(fallofA), fallofB(fallofB) {}
		};

		// Represents a 2D ambient light
		// TODO: This is not implemented, implement this.
		struct AmbientLight
		{
			Color color;
			float intensity = 0.1f;

			AmbientLight() = default;
			AmbientLight(const Color& color, float intensity)
				:color(color), intensity(intensity) {}
		};

		// Physics Components: ------------------------------------------------------------------------------------------------------------------------------------
		// --------------------------------------------------------------------------------------------------------------------------------------------------------
		struct RigidBody2D
		{
			BodyType2D bodyType = BodyType2D::Static;
			PhysicsMaterial2D physicsMaterial;
			PhysicsBody2D* runtimePhysicsBody;

			RigidBody2D() = default;
			RigidBody2D(const RigidBody2D& rigidBody) = default;

			RigidBody2D(BodyType2D type, bool fixedRotation = false)
				:bodyType(type), runtimePhysicsBody(nullptr)
			{
				physicsMaterial.fixedRotation = fixedRotation;
			}
		};

		struct BoxCollider2D
		{
			Vec2 bodyOffset = { 0.0f, 0.0f };
			Vec2 sizeScale	= { 1.0f, 1.0f };

			PhysicsBody2D* runtimePhysicsBody;

			BoxCollider2D() = default;
			BoxCollider2D(const BoxCollider2D& boxCollider) = default;

			BoxCollider2D(const Vec2& bodyOffset, const Vec2& sizeScale)
				:bodyOffset(bodyOffset), sizeScale(sizeScale), runtimePhysicsBody(nullptr)
			{}
		};

		struct CircleCollider2D
		{
			Vec2 bodyOffset = { 0.0f, 0.0f };
			float radiusScale = 1.0f;

			PhysicsBody2D* runtimePhysicsBody;

			CircleCollider2D() = default;
			CircleCollider2D(const CircleCollider2D& circleCollider) = default;
			
			CircleCollider2D(const Vec2& bodyOffset, float radiusScale)
				:bodyOffset(bodyOffset), radiusScale(radiusScale) {}
		};

		// Particle System ----
		struct ParticleSystem2DComp
		{
			ParticleInstance2D particleInstance;

			ParticleSystem2DComp() = default;
			ParticleSystem2DComp(const ParticleSystem2DComp& particleSystem2D) = default;

			ParticleSystem2DComp(const ParticleInstance2D& particleInstance)
				:particleInstance(particleInstance) {}
		};

		template<typename... T>
		struct Group{};

		// Here All refers to all components except ID and Transform Component.
		using All =
			Group<Transform, SpriteRenderer, TextRenderer, CameraComp, 
				NativeScript, ScriptComp, PointLight, AmbientLight, RigidBody2D, BoxCollider2D, 
				CircleCollider2D, ParticleSystem2DComp>;
	}
}