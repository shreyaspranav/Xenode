#include "pch"
#include "SceneSerializer.h"
#include "Scene.h"

#include <core/app/Log.h>

// Ignore Warnings related to dll linking
#pragma warning(push)

#pragma warning(disable : 4275)
#pragma warning(disable : 4251)
#include <yaml-cpp/yaml.h>

#pragma warning(pop)

#include "Components.h"

namespace Xen 
{
	struct BinSerializerData
	{
		const Size 
			entityIdTransformPair        = sizeof(UUID) + sizeof(Component::Transform),
			entityIdSpriteRendererPair   = sizeof(UUID) + sizeof(Component::SpriteRenderer),
			entityIdCameraCompPair       = sizeof(UUID) + sizeof(CameraType) + sizeof(bool) + (3 * sizeof(Vec3)) + (3 * sizeof(float)),
			entityIdRigidBody2dPair      = sizeof(UUID) + sizeof(BodyType2D) + sizeof(PhysicsMaterial2D),
			entityIdBoxCollider2dPair    = sizeof(UUID) + (2 * sizeof(Vec2)),
			entityIdCircleCollider2dPair = sizeof(UUID) + sizeof(Vec2) + sizeof(float);

		// For CameraComp
		const Size 
			cameraTypeOffset             = 0,
			isPrimaryCameraOffset        = cameraTypeOffset + sizeof(CameraType),
			positionOffset               = isPrimaryCameraOffset + sizeof(bool),
			rotationOffset               = positionOffset + sizeof(Vec3),
			scaleOffset                  = rotationOffset + sizeof(Vec3),
			fovAngleOffset               = scaleOffset + sizeof(Vec3),
			zNearOffset                  = fovAngleOffset + sizeof(float),
			zFarOffset                   = zNearOffset + sizeof(float);

		// For RigidBody2D
		const Size
			bodyTypeOffset               = 0,
			physicsMaterial2dOffset      = bodyTypeOffset + sizeof(BodyType2D);

		// For BoxCollider2D
		const Size
			boxBodyOffsetOffset          = 0,
			sizeScaleOffset              = boxBodyOffsetOffset + sizeof(Vec2);

		// For RigidBody2D
		const Size
			circleBodyOffsetOffset       = 0,
			radiusScaleOffset            = circleBodyOffsetOffset + sizeof(Vec2);
	}binSerializerData;
	 
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
			{
				yamlEmitter << YAML::Key << "PolygonProperties";
				yamlEmitter << YAML::BeginMap; // PolygonProperties

				yamlEmitter << YAML::Key << "Segments" << YAML::Value << spriteRenderer.polygon_properties.segment_count;

				yamlEmitter << YAML::EndMap; // PolygonProperties
			}

			else if (spriteRenderer.primitive == Component::SpriteRenderer::Primitive::Circle) 
			{
				yamlEmitter << YAML::Key << "CircleProperties";
				yamlEmitter << YAML::BeginMap; // CircleProperties

				yamlEmitter << YAML::Key << "Thickness" << YAML::Value << spriteRenderer.circle_properties.thickness;
				yamlEmitter << YAML::Key << "InnerFade" << YAML::Value << spriteRenderer.circle_properties.innerfade;
				yamlEmitter << YAML::Key << "OuterFade" << YAML::Value << spriteRenderer.circle_properties.outerfade;

				yamlEmitter << YAML::EndMap;
			}

			yamlEmitter << YAML::Key << "Color" << YAML::Value << spriteRenderer.color;

			yamlEmitter << YAML::Key << "Texture";
			yamlEmitter << YAML::BeginMap; // Texture
			
			// yamlEmitter << YAML::Key << "TextureFileRelPath";
			// if (spriteRenderer.texture == nullptr)
			// 	yamlEmitter << YAML::Value << "null";
			// else 
			// 	yamlEmitter << YAML::Value << spriteRenderer.texture->GetFilePath();

			yamlEmitter << YAML::Key << "TextureHandle" << YAML::Value << spriteRenderer.textureHandle; // calls operator uint64_t();

			yamlEmitter << YAML::Key << "TextureTilingFactor" << YAML::Value << spriteRenderer.texture_tile_factor;
			
			yamlEmitter << YAML::EndMap;

			yamlEmitter << YAML::EndMap; // SpriteRenderer
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

		if (entity.HasAnyComponent<Component::AmbientLight>())
		{
			Component::AmbientLight& ambientLight = entity.GetComponent<Component::AmbientLight>();

			yamlEmitter << YAML::Key << "AmbientLight";
			yamlEmitter << YAML::BeginMap; // AmbientLight

			yamlEmitter << YAML::Key << "Color" << YAML::Value << ambientLight.color;
			yamlEmitter << YAML::Key << "Intensity" << YAML::Value << ambientLight.intensity;

			yamlEmitter << YAML::EndMap; // AmbientLight
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

			yamlEmitter << YAML::Key << "FixedRotation" << YAML::Value << rigidBody.physicsMaterial.fixedRotation;
			yamlEmitter << YAML::Key << "BodyType" << YAML::Value << (int8_t)rigidBody.bodyType;

			yamlEmitter << YAML::Key << "Density" << YAML::Value << rigidBody.physicsMaterial.mass;
			yamlEmitter << YAML::Key << "Friction" << YAML::Value << rigidBody.physicsMaterial.friction;
			yamlEmitter << YAML::Key << "Restitution" << YAML::Value << rigidBody.physicsMaterial.restitution;
			yamlEmitter << YAML::Key << "RestitutionThreshold" << YAML::Value << rigidBody.physicsMaterial.restitutionThreshold;

			yamlEmitter << YAML::EndMap;
		}

		if (entity.HasAnyComponent<Component::BoxCollider2D>())
		{
			Component::BoxCollider2D& boxCollider = entity.GetComponent<Component::BoxCollider2D>();

			yamlEmitter << YAML::Key << "BoxCollider2D" << YAML::BeginMap;

			yamlEmitter << YAML::Key << "SizeScale" << YAML::Value << boxCollider.sizeScale;
			yamlEmitter << YAML::Key << "Offset" << YAML::Value << boxCollider.bodyOffset;

			yamlEmitter << YAML::EndMap;
		}

		if (entity.HasAnyComponent<Component::CircleCollider2D>())
		{
			Component::CircleCollider2D& circleCollider = entity.GetComponent<Component::CircleCollider2D>();

			yamlEmitter << YAML::Key << "CircleCollider2D" << YAML::BeginMap;

			yamlEmitter << YAML::Key << "RadiusScale" << YAML::Value << circleCollider.radiusScale;
			yamlEmitter << YAML::Key << "Offset" << YAML::Value << circleCollider.bodyOffset;

			yamlEmitter << YAML::EndMap;
		}
		// ----------------------------------------------------------------------------------------------------

		yamlEmitter << YAML::EndMap; // Entity
	}

	void SceneSerializer::Serialize(const Ref<Scene>& scene, const Component::Transform& editorCameraTransform, const std::string& filePath)
	{
		YAML::Emitter yamlEmitter;

		yamlEmitter << YAML::BeginMap;
		yamlEmitter << YAML::Key << "Scene" << YAML::Value << "Untitled"; // TODO: Scene name goes here

		yamlEmitter << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

		uint32_t entity_count = 0;

		scene->m_SceneRegistry.each([&](auto& entityID)
		{
		
			Entity entity = Entity(entityID, scene.get());
			entity_count++;
			
			SerializeEntity(yamlEmitter, entity, entity_count);  
		});

		yamlEmitter << YAML::EndSeq;
		yamlEmitter << YAML::Key << "Entity Count" << YAML::Value << entity_count;

		yamlEmitter << YAML::Key << "EditorCameraTransform" << YAML::Value << YAML::BeginMap;
		yamlEmitter << YAML::Key << "Position" << YAML::Value << editorCameraTransform.position;
		yamlEmitter << YAML::Key << "Rotation" << YAML::Value << editorCameraTransform.rotation;
		yamlEmitter << YAML::Key << "Scale" << YAML::Value << editorCameraTransform.scale;

		yamlEmitter << YAML::EndMap;

		std::ofstream out_stream(filePath);
		out_stream << yamlEmitter.c_str();
		out_stream.close();
	}

	Component::Transform SceneSerializer::Deserialize(const Ref<Scene>& scene, const std::string& filePath)
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

				Entity entt = scene->AddNewEntityWithID(tag, uuid);

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
					auto& sprite = entt.AddComponent<Component::SpriteRenderer>();

					Color color = Color(
						spriteRenderer_component["Color"][0].as<float>(),
						spriteRenderer_component["Color"][1].as<float>(),
						spriteRenderer_component["Color"][2].as<float>(),
						spriteRenderer_component["Color"][3].as<float>()
					);

					sprite.color = color;

					Component::SpriteRenderer::Primitive primitive = (Component::SpriteRenderer::Primitive)spriteRenderer_component["SpritePrimitive"].as<int32_t>();
					sprite.primitive = primitive;

					Component::SpriteRenderer::PolygonProperties polygon_properties;
					Component::SpriteRenderer::CircleProperties circle_properties;

					if (primitive == Component::SpriteRenderer::Primitive::Polygon)
						sprite.polygon_properties.segment_count = spriteRenderer_component["PolygonProperties"]["Segments"].as<uint32_t>();

					else if (primitive == Component::SpriteRenderer::Primitive::Circle)
					{
						sprite.circle_properties.thickness = spriteRenderer_component["CircleProperties"]["Thickness"].as<float>();
						sprite.circle_properties.innerfade = spriteRenderer_component["CircleProperties"]["InnerFade"].as<float>();
						sprite.circle_properties.outerfade = spriteRenderer_component["CircleProperties"]["OuterFade"].as<float>();
					}

					Ref<Texture2D> texture = nullptr;

					// if (spriteRenderer_component["Texture"]["TextureHandle"].as<uint64_t>() != "null")
					// {
					// 	texture = Texture2D::CreateTexture2D(spriteRenderer_component["Texture"]["TextureFileRelPath"].as<std::string>(), true);
					// 	texture->LoadTexture();
					// }

					sprite.textureHandle = spriteRenderer_component["Texture"]["TextureHandle"].as<uint64_t>();
					sprite.texture_tile_factor = spriteRenderer_component["Texture"]["TextureTilingFactor"].as<float>();
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

				// PointLight Component-------------------------------------------------
				const YAML::Node& ambientLight_component = entity["AmbientLight"];
				if (ambientLight_component)
				{
					Color color = Color(
						ambientLight_component["Color"][0].as<float>(),
						ambientLight_component["Color"][1].as<float>(),
						ambientLight_component["Color"][2].as<float>(),
						ambientLight_component["Color"][3].as<float>()
					);

					float intensity = ambientLight_component["Intensity"].as<float>();

					entt.AddComponent<Component::AmbientLight>(color, intensity);
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
							CameraType::Orthographic, 1, 1
							// scene->m_FramebufferWidth,
							// scene->m_FramebufferHeight
						);
					}

					else if (camera_node["ProjectionType"].as<std::string>() == "Perspective")
					{
						camera = std::make_shared<Camera>(
							CameraType::Perspective, 1, 1
							// scene->m_FramebufferWidth,
							// scene->m_FramebufferHeight
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
					BodyType2D bodyType = (BodyType2D)rigidBodyComponent["BodyType"].as<int32_t>();
					bool fixedRotation = rigidBodyComponent["FixedRotation"].as<bool>();

					Component::RigidBody2D& rBody = entt.AddComponent<Component::RigidBody2D>(bodyType, fixedRotation);

					rBody.physicsMaterial.mass = rigidBodyComponent["Density"].as<float>();
					rBody.physicsMaterial.friction = rigidBodyComponent["Friction"].as<float>();
					rBody.physicsMaterial.restitution = rigidBodyComponent["Restitution"].as<float>();
					rBody.physicsMaterial.restitutionThreshold = rigidBodyComponent["RestitutionThreshold"].as<float>();

				}

				// BoxCollider2D:---------------------------------------------------------------------------
				const YAML::Node& boxColliderComponent = entity["BoxCollider2D"];
				if (boxColliderComponent)
				{
					Vec2 sizeScale = Vec2(
						boxColliderComponent["SizeScale"][0].as<float>(),
						boxColliderComponent["SizeScale"][1].as<float>()
					);

					Vec2 offset = Vec2(
						boxColliderComponent["Offset"][0].as<float>(),
						boxColliderComponent["Offset"][1].as<float>()
					);

					Component::BoxCollider2D& boxColliderComp = entt.AddComponent<Component::BoxCollider2D>();

					boxColliderComp.sizeScale = sizeScale;
					boxColliderComp.bodyOffset = offset;
					
				}

				// CircleCollider2D:---------------------------------------------------------------------------
				const YAML::Node& circleColliderComponent = entity["CircleCollider2D"];
				if (circleColliderComponent)
				{
					float radiusScale = circleColliderComponent["RadiusScale"].as<float>();

					Vec2 offset = Vec2(
						circleColliderComponent["Offset"][0].as<float>(),
						circleColliderComponent["Offset"][1].as<float>()
					);

					Component::CircleCollider2D& circleColliderComp = entt.AddComponent<Component::CircleCollider2D>();

					circleColliderComp.radiusScale = radiusScale;
					circleColliderComp.bodyOffset = offset;

				}
			}
		}
		// Getting the Editor Camera Transform:

		Vec3 position = {
			scene_data["EditorCameraTransform"]["Position"][0].as<float>(),
			scene_data["EditorCameraTransform"]["Position"][1].as<float>(),
			scene_data["EditorCameraTransform"]["Position"][2].as<float>(),
		};

		Vec3 rotation = {
			scene_data["EditorCameraTransform"]["Rotation"][0].as<float>(),
			scene_data["EditorCameraTransform"]["Rotation"][1].as<float>(),
			scene_data["EditorCameraTransform"]["Rotation"][2].as<float>(),
		};

		Vec3 scale = {
			scene_data["EditorCameraTransform"]["Scale"][0].as<float>(),
			scene_data["EditorCameraTransform"]["Scale"][1].as<float>(),
			scene_data["EditorCameraTransform"]["Scale"][2].as<float>(),
		};

		Component::Transform transform = { position, rotation, scale };
		return transform;
	}
	void SceneSerializer::SerializeBinary(const Ref<Scene>& scene, Buffer& buffer)
	{
		// TODO: Look into unnecessary copies being done on data and optimize it.
		Vector<uint8_t> binaryData;

		// Lambda function to add binary data:
		auto&& appendToBuffer =
		[&](const void* data, Size size)
		{
			const uint8_t* dataInBytes = reinterpret_cast<const uint8_t*>(data);

			for (int i = 0; i < size; i++)
				binaryData.push_back(dataInBytes[i]);
		};

		// TODO: Look for little endian/big endian portability:
		// Firstly, add "XenScene" string in the beginning to indicate its a scene:
		const std::string headerEntry = "XenScene";
		appendToBuffer(headerEntry.c_str(), headerEntry.size() + 1); // Account the null termination character.

		// Serialize the no. of entities:
		Size entityCount = scene->m_SceneRegistry.size();
		appendToBuffer(&entityCount, sizeof(Size));

		// Serialize the entities UUID, and its Transform component (The tag doesn't make sense in the runtime anyway)
		scene->m_SceneRegistry.each(
		[&](auto&& entt)
		{
			Entity e = Entity(entt, scene.get());

			// ID, Transform
			const Component::ID& IdComponent = e.GetComponent<Component::ID>();
			const Component::Transform& transformComponent = e.GetComponent<Component::Transform>();

			uint64_t id = IdComponent.id; // Call operator uint64_t()
			appendToBuffer(&id, sizeof(uint64_t));

			appendToBuffer(&transformComponent.position, sizeof(Vec3));
			appendToBuffer(&transformComponent.rotation, sizeof(Vec3));
			appendToBuffer(&transformComponent.scale, sizeof(Vec3));
		});

		// As of the time writing this, I've implemented serializing 
		//	SpriteRenderer, CameraComp, RigidBody2D, BoxCollider2D, CircleCollider2D
		auto&& spriteView = scene->m_SceneRegistry.view<Component::SpriteRenderer>();
		
		// Serialize the no. of entities in this view:
		Size entityCountView = spriteView.size();
		if(entityCountView != 0)
			appendToBuffer(&entityCountView, sizeof(Size));

		for (auto&& entt : spriteView)
		{
			Entity e = Entity(entt, scene.get());

			// ID, SpriteRenderer
			const Component::ID& IdComponent = e.GetComponent<Component::ID>();
			const Component::SpriteRenderer& spriteRenderer = e.GetComponent<Component::SpriteRenderer>();

			uint64_t id = IdComponent.id; // Call operator uint64_t()
			appendToBuffer(&id, sizeof(uint64_t));

			appendToBuffer(&spriteRenderer, sizeof(Component::SpriteRenderer));
		}

		auto&& cameraView = scene->m_SceneRegistry.view<Component::CameraComp>();

		// Serialize the no. of entities in this view:
		entityCountView = cameraView.size();
		if (entityCountView != 0)
			appendToBuffer(&entityCountView, sizeof(Size));

		for (auto&& entt : cameraView)
		{
			Entity e = Entity(entt, scene.get());

			// ID, CameraComp
			const Component::ID& IdComponent = e.GetComponent<Component::ID>();
			const Component::CameraComp& cameraComp = e.GetComponent<Component::CameraComp>();

			uint64_t id = IdComponent.id; // Call operator uint64_t()
			appendToBuffer(&id, sizeof(uint64_t));

			// TODO: look into keeping only the camera transform and other properties instead of the camera pointer
			// Order would be: CameraType, isPrimaryCamera, Position, Rotation, Scale, (fovAngle, zNear, zFar if CameraType is Perspective)
			CameraType type = cameraComp.camera->GetProjectionType();
			appendToBuffer(&type, sizeof(CameraType));

			appendToBuffer(&cameraComp.is_primary_camera, sizeof(bool));

			Vec3 position = cameraComp.camera->GetPosition();
			Vec3 rotation = cameraComp.camera->GetRotation();
			Vec3 scale = cameraComp.camera->GetScale();

			appendToBuffer(&position, sizeof(Vec3));
			appendToBuffer(&rotation, sizeof(Vec3));
			appendToBuffer(&scale, sizeof(Vec3));

			// TODO: Serialize event the unnecessary stuff, later optimize to reduce size:
			// if (type == CameraType::Perspective)
			{
				float fov = cameraComp.camera->GetFovAngle();
				float zNear = cameraComp.camera->GetNearPoint();
				float zFar = cameraComp.camera->GetFarPoint();

				appendToBuffer(&fov, sizeof(float));
				appendToBuffer(&zNear, sizeof(float));
				appendToBuffer(&zFar, sizeof(float));
			}
		}

		auto&& rigidBody2DView = scene->m_SceneRegistry.view<Component::RigidBody2D>();

		// Serialize the no. of entities in this view:
		entityCountView = rigidBody2DView.size();
		if (entityCountView != 0)
			appendToBuffer(&entityCountView, sizeof(Size));

		for (auto&& entt : rigidBody2DView)
		{
			Entity e = Entity(entt, scene.get());

			// ID, RigidBody2D
			const Component::ID& IdComponent = e.GetComponent<Component::ID>();
			const Component::RigidBody2D& rigidBody2D = e.GetComponent<Component::RigidBody2D>();

			uint64_t id = IdComponent.id; // Call operator uint64_t()
			appendToBuffer(&id, sizeof(uint64_t));

			appendToBuffer(&rigidBody2D.bodyType, sizeof(BodyType2D));
			appendToBuffer(&rigidBody2D.physicsMaterial, sizeof(PhysicsMaterial2D));
		}

		auto&& boxCollider2DView = scene->m_SceneRegistry.view<Component::BoxCollider2D>();

		// Serialize the no. of entities in this view:
		entityCountView = boxCollider2DView.size();
		if (entityCountView != 0)
			appendToBuffer(&entityCountView, sizeof(Size));

		for (auto&& entt : boxCollider2DView)
		{
			Entity e = Entity(entt, scene.get());

			// ID, RigidBody2D
			const Component::ID& IdComponent = e.GetComponent<Component::ID>();
			const Component::BoxCollider2D& boxCollider2D = e.GetComponent<Component::BoxCollider2D>();

			uint64_t id = IdComponent.id; // Call operator uint64_t()
			appendToBuffer(&id, sizeof(uint64_t));

			appendToBuffer(&boxCollider2D.bodyOffset, sizeof(Vec2));
			appendToBuffer(&boxCollider2D.sizeScale, sizeof(Vec2));
		}
		
		auto&& circleCollider2DView = scene->m_SceneRegistry.view<Component::CircleCollider2D>();

		// Serialize the no. of entities in this view:
		entityCountView = circleCollider2DView.size();
		if (entityCountView != 0)
			appendToBuffer(&entityCountView, sizeof(Size));
		
		for (auto&& entt : circleCollider2DView)
		{
			Entity e = Entity(entt, scene.get());

			// ID, RigidBody2D
			const Component::ID& IdComponent = e.GetComponent<Component::ID>();
			const Component::CircleCollider2D& circleCollider2D = e.GetComponent<Component::CircleCollider2D>();

			uint64_t id = IdComponent.id; // Call operator uint64_t()
			appendToBuffer(&id, sizeof(uint64_t));

			appendToBuffer(&circleCollider2D.bodyOffset, sizeof(Vec2));
			appendToBuffer(&circleCollider2D.radiusScale, sizeof(float));
		}

		// Add 69(nice) to the end of the buffer to indicate that the buffer came to an end.
		uint8_t endTag = 69;
		appendToBuffer(&endTag, sizeof(uint8_t));

		// In the end, copy the data to the buffer
		uint8_t* buf = new uint8_t[binaryData.size()];
		memcpy(buf, binaryData.data(), binaryData.size());

		buffer.size = binaryData.size();
		buffer.buffer = buf;
		buffer.alloc = true;
	}
	void SceneSerializer::DeserializeBinary(const Ref<Scene>& scene, Buffer& buffer)
	{
		uint8_t* bufferBase = reinterpret_cast<uint8_t*>(buffer.buffer);
		uint8_t* bufferPtr = bufferBase;
		Size bufferSize = buffer.size;

		bufferPtr += 9; // Increment 9 bytes to skip past the "XenScene" header.

		Size entityCount;
		memcpy(&entityCount, bufferPtr, sizeof(Size));

		if (entityCount == 0)
		{
			XEN_ENGINE_LOG_WARN("Serializing Empty scene!");
			return;
		}

		bufferPtr += sizeof(Size);

		UnorderedMap<UUID, Entity> entityMap;
		
		for (int i = 0; i < entityCount; i++)
		{
			UUID entityID = 0;
			Component::Transform transform;

			memcpy(&entityID, bufferPtr, sizeof(UUID));
			
			Entity currentEntity = scene->AddNewEntityWithID(std::string("XenodeTag"), entityID);
			Component::Transform& currentEntityTransform = currentEntity.GetComponent<Component::Transform>();

			memcpy(&transform, bufferPtr + sizeof(UUID), sizeof(Component::Transform));

			currentEntityTransform.position = transform.position;
			currentEntityTransform.rotation = transform.rotation;
			currentEntityTransform.scale = transform.scale;

			// For later easy access
			entityMap.insert({ entityID, currentEntity });

			bufferPtr += binSerializerData.entityIdTransformPair;
		}

		// Order of components to read: SpriteRenderer, CameraComp, RigidBody2D, BoxCollider2D, CircleCollider2D

		// Check for buffer overflows
		if (bufferPtr - bufferBase >= bufferSize - 1) return;

		Size spriteRendererCount;
		memcpy(&spriteRendererCount, bufferPtr, sizeof(Size));

		bufferPtr += sizeof(Size);

		for (int i = 0; i < spriteRendererCount; i++)
		{
			UUID currentID = 0;
			Component::SpriteRenderer spriteRenderer;

			memcpy(&currentID, bufferPtr, sizeof(UUID));
			memcpy(&spriteRenderer, bufferPtr + sizeof(UUID), sizeof(Component::SpriteRenderer));

			// Add the component to the required entity
			entityMap[currentID].AddComponent<Component::SpriteRenderer>(spriteRenderer); // Use the copy constructor

			bufferPtr += binSerializerData.entityIdSpriteRendererPair;
		}

		// Check for buffer overflows
		if (bufferPtr - bufferBase >= bufferSize - 1) return;

		Size cameraCompCount;
		memcpy(&cameraCompCount, bufferPtr, sizeof(Size));

		bufferPtr += sizeof(Size);

		for (int i = 0; i < cameraCompCount; i++)
		{
			UUID currentID = 0;
			Component::CameraComp cameraComp;
		
			memcpy(&currentID, bufferPtr, sizeof(UUID));
			bufferPtr += sizeof(UUID);
		
			CameraType cameraType;
			bool isPrimaryCamera;
			Vec3 position, rotation, scale;
			float fovAngle, zNear, zFar;
		
			memcpy(&cameraType, bufferPtr, sizeof(CameraType));    bufferPtr += sizeof(CameraType);
			memcpy(&isPrimaryCamera, bufferPtr, sizeof(bool));     bufferPtr += sizeof(bool);
			memcpy(&position, bufferPtr, sizeof(Vec3));            bufferPtr += sizeof(Vec3);
			memcpy(&rotation, bufferPtr, sizeof(Vec3));            bufferPtr += sizeof(Vec3);
			memcpy(&scale, bufferPtr, sizeof(Vec3));               bufferPtr += sizeof(Vec3);
			memcpy(&fovAngle, bufferPtr, sizeof(float));           bufferPtr += sizeof(float);
			memcpy(&zNear, bufferPtr, sizeof(float));              bufferPtr += sizeof(float);
			memcpy(&zFar, bufferPtr, sizeof(float));               bufferPtr += sizeof(float);

			cameraComp.camera = std::make_shared<Camera>(cameraType, 1, 1);

			cameraComp.is_primary_camera = isPrimaryCamera;
			cameraComp.camera->SetPosition(position);
			cameraComp.camera->SetRotation(rotation);
			cameraComp.camera->SetScale(scale);
			cameraComp.camera->SetFovAngle(fovAngle);
			cameraComp.camera->SetNearPoint(zNear);
			cameraComp.camera->SetFarPoint(zFar);
		
			// Add the component to the required entity
			entityMap[currentID].AddComponent<Component::CameraComp>(cameraComp); // Use the copy constructor
		}

		// Check for buffer overflows
		if (bufferPtr - bufferBase >= bufferSize - 1) return;

		Size rigidBody2dCount;
		memcpy(&rigidBody2dCount, bufferPtr, sizeof(Size));

		bufferPtr += sizeof(Size);

		for (int i = 0; i < rigidBody2dCount; i++)
		{
			UUID currentID = 0;
			Component::RigidBody2D rigidBody;

			memcpy(&currentID, bufferPtr, sizeof(UUID));

			memcpy(&rigidBody.bodyType, bufferPtr + sizeof(UUID) + binSerializerData.bodyTypeOffset, sizeof(BodyType2D));
			memcpy(&rigidBody.physicsMaterial, bufferPtr + sizeof(UUID) + binSerializerData.physicsMaterial2dOffset, sizeof(PhysicsMaterial2D));

			// Add the component to the required entity
			entityMap[currentID].AddComponent<Component::RigidBody2D>(rigidBody); // Use the copy constructor

			bufferPtr += binSerializerData.entityIdRigidBody2dPair;
		}

		// Check for buffer overflows
		if (bufferPtr - bufferBase >= bufferSize - 1) return;

		Size boxCollider2dCount;
		memcpy(&boxCollider2dCount, bufferPtr, sizeof(Size));

		bufferPtr += sizeof(Size);

		for (int i = 0; i < boxCollider2dCount; i++)
		{
			UUID currentID = 0;
			Component::BoxCollider2D boxCollider;

			memcpy(&currentID, bufferPtr, sizeof(UUID));

			memcpy(&boxCollider.bodyOffset, bufferPtr + sizeof(UUID) + binSerializerData.boxBodyOffsetOffset, sizeof(Vec2));
			memcpy(&boxCollider.sizeScale, bufferPtr + sizeof(UUID) + binSerializerData.sizeScaleOffset, sizeof(Vec2));

			// Add the component to the required entity
			entityMap[currentID].AddComponent<Component::BoxCollider2D>(boxCollider); // Use the copy constructor

			bufferPtr += binSerializerData.entityIdBoxCollider2dPair;
		}

		// Check for buffer overflows
		if (bufferPtr - bufferBase >= bufferSize - 1) return;

		Size circleCollider2dCount;
		memcpy(&circleCollider2dCount, bufferPtr, sizeof(Size));

		bufferPtr += sizeof(Size);

		for (int i = 0; i < circleCollider2dCount; i++)
		{
			UUID currentID = 0;
			Component::CircleCollider2D circleCollider;

			memcpy(&currentID, bufferPtr, sizeof(UUID));

			memcpy(&circleCollider.bodyOffset, bufferPtr + sizeof(UUID) + binSerializerData.boxBodyOffsetOffset, sizeof(Vec2));
			memcpy(&circleCollider.radiusScale, bufferPtr + sizeof(UUID) + binSerializerData.sizeScaleOffset, sizeof(float));

			// Add the component to the required entity
			entityMap[currentID].AddComponent<Component::CircleCollider2D>(circleCollider); // Use the copy constructor

			bufferPtr += binSerializerData.entityIdCircleCollider2dPair;
		}

	}
	void SceneSerializer::SerializeBinaryToFile(const Ref<Scene>& scene, const std::string& filePath)
	{
		Buffer b;

		// This function will also allocate the buffer.
		SerializeBinary(scene, b);

		std::ofstream outputStream(filePath, std::ios::binary);
		outputStream.write((const char*)b.buffer, b.size);
		outputStream.close();
	}

	void SceneSerializer::DeserializeBinaryFromFile(const Ref<Scene>& scene, const std::string& filePath)
	{
		std::ifstream inputStream(filePath, std::ios::binary);

		// Check if the binary file is valid or not:
		{
			const char* sceneBinHeader = "XenScene";
			char fileHeader[9] = { 0 };

			inputStream.read(fileHeader, 9);

			if (strcmp(sceneBinHeader, fileHeader) != 0) return;
		}
		
		inputStream.seekg(0, std::ios::end);
		Size s = inputStream.tellg();
		inputStream.seekg(0, std::ios::beg);

		Buffer b;
		b.buffer = malloc(s);
		b.size = s;
		b.alloc = true;

		inputStream.read(reinterpret_cast<char*>(b.buffer), s);
		inputStream.close();

		DeserializeBinary(scene, b);
	}
}