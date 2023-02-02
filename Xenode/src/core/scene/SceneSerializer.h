#pragma once

#include <Core.h>
#include "Scene.h"

namespace Xen {
	class SceneSerializer
	{
	public:
		SceneSerializer(const Ref<Scene>& scene);
		~SceneSerializer();

		void Serialize(const std::string& filePath);
		void SerializeBinary(const std::string& filePath);

		void Deserialize(const std::string& filePath);
		void DeserializeBinary(const std::string& filePath);

	private:
		Ref<Scene> m_Scene;
	};
}

