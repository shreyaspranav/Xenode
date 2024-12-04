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
		static void Serialize(const Ref<Scene>& scene, const Component::Transform& editorCameraTransform, const std::string& filePath);
		static Component::Transform Deserialize(const Ref<Scene>& scene, const std::string& filePath);

		// Binary serialization, usually only when building asset packs and during runtime -----------------------------------------------
		static void SerializeBinary(const Ref<Scene>& scene, Buffer& buffer);
		static void DeserializeBinary(const Ref<Scene>& scene, Buffer& buffer);
		static void SerializeBinaryToFile(const Ref<Scene>& scene, const std::string& filePath);
		static void DeserializeBinaryFromFile(const Ref<Scene>& scene, const std::string& filePath);
	};
}

