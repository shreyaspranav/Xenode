#pragma once
#include <Core.h>

#include "Project.h"

namespace Xen
{
	class ProjectSerializer
	{
	public:
		static void Serialize(const Ref<Project>& project, const std::filesystem::path& projectPath);
		static void Deserialize(const Ref<Project>& project, const std::filesystem::path& projectFilePath);

		static bool IsValidProjectFile(const std::filesystem::path& projectFilePath);
	};
}