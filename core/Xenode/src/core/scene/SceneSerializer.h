#pragma once

#include <Core.h>
#include "Components.h"

namespace Xen 
{
	class Scene;

	class SceneSerializer
	{
	public:
		// Text based(YAML) serialization: usually not used in runtime -------------------------------------------------------------------
		// editorCameraTransform is to serialize the Editor Camera's transform.
		static std::string SerializeYAML(const Ref<Scene>& scene, const Component::Transform& editorCameraTransform);
		static void DeserializeYAML(const Ref<Scene>& scene, const std::string& yaml);
		static Component::Transform GetEditorCameraTransform(const std::string& yaml);

		// Binary serialization, usually only when building asset packs and during runtime -----------------------------------------------
		static Vector<std::byte> SerializeBinary(const Ref<Scene>& scene);
		static void DeserializeBinary(const Ref<Scene>& scene, const Vector<std::byte>& buffer);
		static void SerializeBinaryToFile(const Ref<Scene>& scene, const std::filesystem::path& filePath);
		static void DeserializeBinaryFromFile(const Ref<Scene>& scene, const std::filesystem::path& filePath);
	};
}

