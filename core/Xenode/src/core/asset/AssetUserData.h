#pragma once
#include <Core.h>

#include <core/scene/Components.h>
#include <core/renderer/Texture.h>

namespace Xen
{
	struct SceneAssetUserData
	{
		Component::Transform editorCameraTransform;
		Ref<Texture2D> thumbnail = nullptr;
	};

	struct TextureAssetUserData
	{
		Ref<Texture2D> thumbnail = nullptr;
	};
}