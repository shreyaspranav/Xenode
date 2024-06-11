#pragma once

#include <Core.h>

namespace Xen {
	class Scene;
	// class Scene;

	class SceneSerializer
	{

	public:
		static void Serialize(const Ref<Scene>& scene, const std::string& filePath);
		static void SerializeBinary(const Ref<Scene>& scene, const std::string& filePath);

		static void Deserialize(const Ref<Scene>& scene, const std::string& filePath);
		static void DeserializeBinary(const Ref<Scene>& scene, const std::string& filePath);
	};
}

