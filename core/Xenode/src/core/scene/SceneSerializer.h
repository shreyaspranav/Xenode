#pragma once

#include <Core.h>
#include "Components.h"

namespace Xen {
	class Scene;
	// class Scene;

	class SceneSerializer
	{

	public:
		// editorCameraTransform is to serialize the Editor Camera's transform.
		static void Serialize(const Ref<Scene>& scene, const Component::Transform& editorCameraTransform, const std::string& filePath);
		static void SerializeBinary(const Ref<Scene>& scene, const std::string& filePath);

		static Component::Transform Deserialize(const Ref<Scene>& scene, const std::string& filePath);
		static void DeserializeBinary(const Ref<Scene>& scene, const std::string& filePath);
	};
}

