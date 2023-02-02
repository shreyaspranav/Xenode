#include "pch"
#include "SceneSerializer.h"
#include "core/app/Log.h"

#include <yaml-cpp/yaml.h>

#include "Components.h"
#include "core/app/Log.h"

namespace Xen {
	SceneSerializer::SceneSerializer(const Ref<Scene>& scene) : m_Scene(scene)
	{}
	SceneSerializer::~SceneSerializer()
	{
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
			yamlEmitter << YAML::Flow << YAML::BeginSeq << tag.tag << "473828532" << YAML::EndSeq;
		}
		else
			yamlEmitter << YAML::Flow << YAML::BeginSeq << "##TagLess##" << "473828532" << YAML::EndSeq;

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

			yamlEmitter << YAML::Key << "Color" << YAML::Value << spriteRenderer.color;

			if (spriteRenderer.texture == nullptr)
				yamlEmitter << YAML::Key << "Texture" << YAML::Value << "null";
			else {
				yamlEmitter << YAML::Key << "Texture";
				yamlEmitter << YAML::BeginMap; // Texture

				yamlEmitter << YAML::Key << "TextureFileRelPath" << YAML::Value << spriteRenderer.texture->GetFilePath();

				yamlEmitter << YAML::EndMap;
			}

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
				yamlEmitter << YAML::Key << "ProjectionType" << YAML::Value << "Orthographic";
				yamlEmitter << YAML::Key << "FovAngle" << YAML::Value << camera.camera->GetFovAngle();
				yamlEmitter << YAML::Key << "ZNear" << YAML::Value << camera.camera->GetNearPoint();
				yamlEmitter << YAML::Key << "ZFar" << YAML::Value << camera.camera->GetFarPoint();
			}
			yamlEmitter << YAML::EndMap; // Camera

			yamlEmitter << YAML::Key << "IsPrimary" << YAML::Value << camera.is_primary_camera;
			yamlEmitter << YAML::Key << "IsResizable" << YAML::Value << camera.is_resizable;

			yamlEmitter << YAML::EndMap; // CameraComp
		}

		yamlEmitter << YAML::EndMap; // Entity
	}

	void SceneSerializer::Serialize(const std::string& filePath)
	{
		YAML::Emitter yamlEmitter;

		yamlEmitter << YAML::BeginMap;
		yamlEmitter << YAML::Key << "Scene" << YAML::Value << "Untitled"; // Scene name goes here

		yamlEmitter << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

		uint32_t entity_count = 0;
		m_Scene->m_Registry.each([&](auto& entityID) {

			Entity entity = Entity(entityID, m_Scene.get());
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
	void SceneSerializer::SerializeBinary(const std::string& filePath)
	{
		XEN_ENGINE_LOG_ERROR("Not Yet Implemented!");
		TRIGGER_BREAKPOINT;
	}
	void SceneSerializer::Deserialize(const std::string& filePath)
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

		std::string name = scene_data["Scene"].as<std::string>();

		YAML::Node entities = scene_data["Entities"];

		if (entities)
		{
			for (const YAML::Node& entity : entities)
			{
				std::string tag = entity["Entity"][1].as<std::string>();
				XEN_ENGINE_LOG_TRACE(tag);
			}
		}
		
	}
	void SceneSerializer::DeserializeBinary(const std::string& filePath)
	{
		XEN_ENGINE_LOG_ERROR("Not Yet Implemented!");
		TRIGGER_BREAKPOINT;
	}
}