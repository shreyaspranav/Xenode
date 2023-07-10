#include "pch"
#include "SceneSerializer.h"
#include "core/app/Log.h"

// Ignore Warnings related to dll linking
#pragma warning(push)

#pragma warning(disable : 4275)
#pragma warning(disable : 4251)
#include <yaml-cpp/yaml.h>

#pragma warning(pop)

#include "Components.h"
#include "core/app/Log.h"

namespace Xen {
	static UUID GetUUID(Entity e)
	{
		if (e.HasAnyComponent<Component::ID>())
			return e.GetComponent<Component::ID>().id;

		XEN_ENGINE_LOG_ERROR_SEVERE("ID Component NOT found");
		TRIGGER_BREAKPOINT;

		return UUID(0);
	}

	YAML::Emitter& operator<<(YAML::Emitter& yamlEmitter, const Color& color)
	{
		yamlEmitter << YAML::Flow;
		yamlEmitter << YAML::BeginSeq << color.r << color.g << color.b << color.a << YAML::EndSeq;
		return yamlEmitter;
	}

	YAML::Emitter& operator<<(YAML::Emitter& yamlEmitter, const Vec4& vector)
	{
		yamlEmitter << YAML::Flow;
		yamlEmitter << YAML::BeginSeq << vector.x << vector.y << vector.z << vector.w << YAML::EndSeq;
		return yamlEmitter;
	}

	YAML::Emitter& operator<<(YAML::Emitter& yamlEmitter, const Vec3& vector)
	{
		yamlEmitter << YAML::Flow;
		yamlEmitter << YAML::BeginSeq << vector.x << vector.y << vector.z  << YAML::EndSeq;
		return yamlEmitter;
	}

	YAML::Emitter& operator<<(YAML::Emitter& yamlEmitter, const Vec2& vector)
	{
		yamlEmitter << YAML::Flow;
		yamlEmitter << YAML::BeginSeq << vector.x << vector.y << YAML::EndSeq;
		return yamlEmitter;
	}


	static void SerializeEntity(YAML::Emitter& yamlEmitter, Entity entity, uint32_t index)
	{
		yamlEmitter << YAML::BeginMap; // Entity
		yamlEmitter << YAML::Key << "Entity";

		if (entity.HasAnyComponent<Component::Tag>())
		{
			Component::Tag& tag = entity.GetComponent<Component::Tag>();
			yamlEmitter << YAML::Flow << YAML::BeginSeq << tag.tag;
		}
		else
			yamlEmitter << YAML::Flow << YAML::BeginSeq << "##TagLess##";

		// UUID:
		yamlEmitter << GetUUID(entity) << YAML::EndSeq;

		if (entity.HasAnyComponent<Component::Transform>())
		{
			Component::Transform& transform = entity.GetComponent<Component::Transform>();

			yamlEmitter << YAML::Key << "Transform";
			yamlEmitter << YAML::BeginMap; // Transform

			yamlEmitter << YAML::Key << "Position" << YAML::Value << transform.position;
			yamlEmitter << YAML::Key << "Rotation" << YAML::Value << transform.rotation;
			yamlEmitter << YAML::Key << "Scale" << YAML::Value << transform.scale;

			yamlEmitter << YAML::EndMap; // Transform
		}

		if (entity.HasAnyComponent<Component::SpriteRenderer>())
		{
			Component::SpriteRenderer& spriteRenderer = entity.GetComponent<Component::SpriteRenderer>();

			yamlEmitter << YAML::Key << "SpriteRenderer";
			yamlEmitter << YAML::BeginMap; // SpriteRenderer

			yamlEmitter << YAML::Key << "SpritePrimitive" << YAML::Value << (int8_t)spriteRenderer.primitive;
			if (spriteRenderer.primitive == Component::SpriteRenderer::Primitive::Polygon)
				yamlEmitter << YAML::Key << "PolygonSegments" << YAML::Value << spriteRenderer.polygon_segment_count;

			yamlEmitter << YAML::Key << "Color" << YAML::Value << spriteRenderer.color;

			yamlEmitter << YAML::Key << "Texture";
			yamlEmitter << YAML::BeginMap; // Texture
			
			yamlEmitter << YAML::Key << "TextureFileRelPath";
			if (spriteRenderer.texture == nullptr)
				yamlEmitter << YAML::Value << "null";
			else 
				yamlEmitter << YAML::Value << spriteRenderer.texture->GetFilePath();
			yamlEmitter << YAML::Key << "TextureTilingFactor" << YAML::Value << spriteRenderer.texture_tile_factor;
			
			yamlEmitter << YAML::EndMap;

			yamlEmitter << YAML::EndMap; // SpriteRenderer
		}

		if (entity.HasAnyComponent<Component::CircleRenderer>())
		{
			Component::CircleRenderer& circleRenderer = entity.GetComponent<Component::CircleRenderer>();
			
			yamlEmitter << YAML::Key << "CircleRenderer";
			yamlEmitter << YAML::BeginMap; // CircleRenderer

			yamlEmitter << YAML::Key << "Color" << YAML::Value << circleRenderer.color;
			yamlEmitter << YAML::Key << "Thickness" << YAML::Value << circleRenderer.thickness;
			yamlEmitter << YAML::Key << "InnerFade" << YAML::Value << circleRenderer.inner_fade;
			yamlEmitter << YAML::Key << "OuterFade" << YAML::Value << circleRenderer.outer_fade;

			yamlEmitter << YAML::EndMap; // CircleRenderer
		}

		if (entity.HasAnyComponent<Component::PointLight>())
		{
			Component::PointLight& pointLight = entity.GetComponent<Component::PointLight>();

			yamlEmitter << YAML::Key << "PointLight";
			yamlEmitter << YAML::BeginMap; // PointLight

			yamlEmitter << YAML::Key << "Color" << YAML::Value << pointLight.lightColor;
			yamlEmitter << YAML::Key << "Radius" << YAML::Value << pointLight.radius;
			yamlEmitter << YAML::Key << "FallofA" << YAML::Value << pointLight.fallofA;
			yamlEmitter << YAML::Key << "FallofB" << YAML::Value << pointLight.fallofB;

			yamlEmitter << YAML::EndMap; // PointLight
		}

		if (entity.HasAnyComponent<Component::CameraComp>())
		{
			Component::CameraComp& camera = entity.GetComponent<Component::CameraComp>();

			yamlEmitter << YAML::Key << "CameraComp";
			yamlEmitter << YAML::BeginMap; // CameraComp

			yamlEmitter << YAML::Key << "Camera";
			yamlEmitter << YAML::BeginMap; // Camera

			if (camera.camera->GetProjectionType() == CameraType::Orthographic)
			{
				yamlEmitter << YAML::Key << "ProjectionType" << YAML::Value << "Orthographic";
				yamlEmitter << YAML::Key << "ZNear" << YAML::Value << camera.camera->GetNearPoint();
				yamlEmitter << YAML::Key << "ZFar" << YAML::Value << camera.camera->GetFarPoint();
			}

			else if (camera.camera->GetProjectionType() == CameraType::Perspective)
			{
				yamlEmitter << YAML::Key << "ProjectionType" << YAML::Value << "Perspective";
				yamlEmitter << YAML::Key << "FovAngle" << YAML::Value << camera.camera->GetFovAngle();
				yamlEmitter << YAML::Key << "ZNear" << YAML::Value << camera.camera->GetNearPoint();
				yamlEmitter << YAML::Key << "ZFar" << YAML::Value << camera.camera->GetFarPoint();
			}
			yamlEmitter << YAML::EndMap; // Camera

			yamlEmitter << YAML::Key << "IsPrimary" << YAML::Value << camera.is_primary_camera;
			yamlEmitter << YAML::Key << "IsResizable" << YAML::Value << camera.is_resizable;

			yamlEmitter << YAML::EndMap; // CameraComp
		}

		// 2D physics stuff:------------------------------------------------------------------------------------
		if (entity.HasAnyComponent<Component::RigidBody2D>())
		{
			Component::RigidBody2D& rigidBody = entity.GetComponent<Component::RigidBody2D>();

			yamlEmitter << YAML::Key << "RigidBody2D" << YAML::BeginMap;

			yamlEmitter << YAML::Key << "FixedRotation" << YAML::Value << rigidBody.fixedRotation;
			yamlEmitter << YAML::Key << "BodyType" << YAML::Value << (int8_t)rigidBody.bodyType;

			yamlEmitter << YAML::EndMap;
		}

		if (entity.HasAnyComponent<Component::BoxCollider2D>())
		{
			Component::BoxCollider2D& boxCollider = entity.GetComponent<Component::BoxCollider2D>();

			yamlEmitter << YAML::Key << "BoxCollider2D" << YAML::BeginMap;

			yamlEmitter << YAML::Key << "Size" << YAML::Value << boxCollider.size;
			yamlEmitter << YAML::Key << "Offset" << YAML::Value << boxCollider.bodyOffset;

			yamlEmitter << YAML::Key << "Density" << YAML::Value << boxCollider.bodyDensity;
			yamlEmitter << YAML::Key << "Friction" << YAML::Value << boxCollider.bodyFriction;
			yamlEmitter << YAML::Key << "Restitution" << YAML::Value << boxCollider.bodyRestitution;
			yamlEmitter << YAML::Key << "RestitutionThreshold" << YAML::Value << boxCollider.bodyRestitutionThreshold;

			yamlEmitter << YAML::EndMap;
		}
		// ----------------------------------------------------------------------------------------------------

		yamlEmitter << YAML::EndMap; // Entity
	}

	void SceneSerializer::Serialize(const Ref<Scene>& scene, const std::string& filePath)
	{
		YAML::Emitter yamlEmitter;

		yamlEmitter << YAML::BeginMap;
		yamlEmitter << YAML::Key << "Scene" << YAML::Value << "Untitled"; // Scene name goes here

		yamlEmitter << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

		uint32_t entity_count = 0;
		scene->m_Registry.each([&](auto& entityID) {

			Entity entity = Entity(entityID, scene.get());
			entity_count++;
			
			SerializeEntity(yamlEmitter, entity, entity_count);  
		});

		yamlEmitter << YAML::EndSeq;
		yamlEmitter << YAML::Key << "Entity Count" << YAML::Value << entity_count;
		yamlEmitter << YAML::EndMap;

		std::ofstream out_stream(filePath);
		out_stream << yamlEmitter.c_str();
		out_stream.close();
	}
	void SceneSerializer::SerializeBinary(const Ref<Scene>& scene, const std::string& filePath)
	{
		XEN_ENGINE_LOG_ERROR("Not Yet Implemented!");
		TRIGGER_BREAKPOINT;
	}
	void SceneSerializer::Deserialize(const Ref<Scene>& scene, const std::string& filePath)
	{
		std::ifstream file_stream(filePath);
		std::stringstream scene_string_data;
		scene_string_data << file_stream.rdbuf();

		YAML::Node scene_data = YAML::Load(scene_string_data);

		if (!scene_data["Scene"])
		{
			XEN_ENGINE_LOG_ERROR("Not a valid scene!");
			TRIGGER_BREAKPOINT;
		}

		//m_Scene = std::make_shared<Scene>();
		//m_Scene->DestroyAllEntities();

		std::string scene_name = scene_data["Scene"].as<std::string>();

		YAML::Node entities = scene_data["Entities"];

		if (entities)
		{
			for (const YAML::Node& entity : entities)
			{
				std::string tag = entity["Entity"][0].as<std::string>();
				uint64_t uuid = entity["Entity"][1].as<uint64_t>();

				//Entity entt = m_Scene->CreateEntityWithUUID(tag, UUID(uuid));
				Entity entt = scene->CreateEntity(tag);

				// Transform Component------------------------------------------------------
				const YAML::Node& transform_component = entity["Transform"];
				if (transform_component)
				{
					Vec3 position = Vec3(
						transform_component["Position"][0].as<float>(),
						transform_component["Position"][1].as<float>(),
						transform_component["Position"][2].as<float>()
					);

					Vec3 rotation = Vec3(
						transform_component["Rotation"][0].as<float>(),
						transform_component["Rotation"][1].as<float>(),
						transform_component["Rotation"][2].as<float>()
					);

					Vec3 scale = Vec3(
						transform_component["Scale"][0].as<float>(),
						transform_component["Scale"][1].as<float>(),
						transform_component["Scale"][2].as<float>()
					);

					Component::Transform& trans = entt.GetComponent<Component::Transform>();
					trans.position = position;
					trans.rotation = rotation;
					trans.scale = scale;
				}
				// SpriteRenderer Component-----------------------------------------------
				const YAML::Node& spriteRenderer_component = entity["SpriteRenderer"];
				if (spriteRenderer_component)
				{
					Color color = Color(
						spriteRenderer_component["Color"][0].as<float>(),
						spriteRenderer_component["Color"][1].as<float>(),
						spriteRenderer_component["Color"][2].as<float>(),
						spriteRenderer_component["Color"][3].as<float>()
					);

					Component::SpriteRenderer::Primitive primitive = (Component::SpriteRenderer::Primitive)spriteRenderer_component["SpritePrimitive"].as<int32_t>();

					const YAML::Node& texture_node = spriteRenderer_component["Texture"];

					Ref<Texture2D> texture = nullptr;

					if (texture_node["TextureFileRelPath"].as<std::string>() != "null")
					{
						texture = Texture2D::CreateTexture2D(texture_node["TextureFileRelPath"].as<std::string>(), true);
						texture->LoadTexture();

						//XEN_ENGINE_LOG_WARN("Texture deserialization not yet implemented");
						//XEN_ENGINE_LOG_WARN(texture_node["TextureFileRelPath"].as<std::string>());
					}

					float texture_tile_factor = texture_node["TextureTilingFactor"].as<float>();

					auto& sprite = entt.AddComponent<Component::SpriteRenderer>(color, primitive, 5);
					sprite.texture = texture;
				}

				// CircleRenderer Component-------------------------------------------------
				const YAML::Node& circleRenderer_component = entity["CircleRenderer"];
				if (circleRenderer_component)
				{
					Color color = Color(
						circleRenderer_component["Color"][0].as<float>(),
						circleRenderer_component["Color"][1].as<float>(),
						circleRenderer_component["Color"][2].as<float>(),
						circleRenderer_component["Color"][3].as<float>()
					);

					float thickness = circleRenderer_component["Thickness"].as<float>();
					float innerfade = circleRenderer_component["InnerFade"].as<float>();
					float outerfade = circleRenderer_component["OuterFade"].as<float>();

					entt.AddComponent<Component::CircleRenderer>(color, thickness, innerfade, outerfade);
				}

				// PointLight Component-------------------------------------------------
				const YAML::Node& pointLight_component = entity["PointLight"];
				if (pointLight_component)
				{
					Color color = Color(
						pointLight_component["Color"][0].as<float>(),
						pointLight_component["Color"][1].as<float>(),
						pointLight_component["Color"][2].as<float>(),
						pointLight_component["Color"][3].as<float>()
					);

					float radius = pointLight_component["Radius"].as<float>();
					float fallofA = pointLight_component["FallofA"].as<float>();
					float fallofB = pointLight_component["FallofB"].as<float>();

					entt.AddComponent<Component::PointLight>(color, radius, fallofA, fallofB);
				}

				//CameraComp Component--------------------------------------------------------
				const YAML::Node& cameraComponent = entity["CameraComp"];
				if (cameraComponent)
				{
					Ref<Camera> camera;

					const YAML::Node& camera_node = cameraComponent["Camera"];
					if (camera_node["ProjectionType"].as<std::string>() == "Orthographic")
					{
						camera = std::make_shared<Camera>(
							CameraType::Orthographic,
							scene->m_FramebufferWidth,
							scene->m_FramebufferHeight
						);
					}

					else if (camera_node["ProjectionType"].as<std::string>() == "Perspective")
					{
						camera = std::make_shared<Camera>(
							CameraType::Perspective,
							scene->m_FramebufferWidth,
							scene->m_FramebufferHeight
							);

						float FovAngle = camera_node["FovAngle"].as<float>();
						camera->SetFovAngle(FovAngle);
					}
					float ZFar = camera_node["ZFar"].as<float>();
					float ZNear = camera_node["ZNear"].as<float>();

					bool isPrimary = cameraComponent["IsPrimary"].as<bool>();
					bool isResizable = cameraComponent["IsResizable"].as<bool>();

					camera->SetNearPoint(ZNear);
					camera->SetFarPoint(ZFar);

					Component::CameraComp& camera_comp = entt.AddComponent<Component::CameraComp>(camera);
					camera_comp.is_primary_camera = isPrimary;
					camera_comp.is_resizable = isResizable;
				}

				// 2D Physics Components:
				// RigidBody2D:-----------------------------------------------------------------------------
				const YAML::Node& rigidBodyComponent = entity["RigidBody2D"];
				if (rigidBodyComponent)
				{
					Component::RigidBody2D::BodyType bodyType = (Component::RigidBody2D::BodyType)rigidBodyComponent["BodyType"].as<int32_t>();
					bool fixedRotation = rigidBodyComponent["FixedRotation"].as<bool>();

					entt.AddComponent<Component::RigidBody2D>(bodyType, fixedRotation);
				}

				// BoxCollider2D:---------------------------------------------------------------------------
				const YAML::Node& boxColliderComponent = entity["BoxCollider2D"];
				if (boxColliderComponent)
				{
					Vec2 size = Vec2(
						boxColliderComponent["Size"][0].as<float>(),
						boxColliderComponent["Size"][1].as<float>()
					);

					Vec2 offset = Vec2(
						boxColliderComponent["Offset"][0].as<float>(),
						boxColliderComponent["Offset"][1].as<float>()
					);

					Component::BoxCollider2D& boxColliderComp = entt.AddComponent<Component::BoxCollider2D>();

					boxColliderComp.size = size;
					boxColliderComp.bodyOffset = offset;
					boxColliderComp.bodyDensity = boxColliderComponent["Density"].as<float>();
					boxColliderComp.bodyFriction = boxColliderComponent["Friction"].as<float>();
					boxColliderComp.bodyRestitution = boxColliderComponent["Restitution"].as<float>();
					boxColliderComp.bodyRestitutionThreshold = boxColliderComponent["RestitutionThreshold"].as<float>();
					
				}
			}
		}
		
	}
	void SceneSerializer::DeserializeBinary(const Ref<Scene>& scene, const std::string& filePath)
	{
		XEN_ENGINE_LOG_ERROR("Not Yet Implemented!");
		TRIGGER_BREAKPOINT;
	}
}