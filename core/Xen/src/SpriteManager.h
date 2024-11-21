#pragma once

#include <Core.h>
#include <core/scene/Scene.h>
#include <core/scene/Components.h>
#include <core/asset/Asset.h>
#include <core/renderer/Structs.h>

class SpriteManager
{
public:
	static void AddSprite(Xen::AssetHandle textureHandle, const Xen::Vec2& pos, const Xen::Ref<Xen::Scene>& scene)
	{
		Xen::Entity e = scene->AddNewEntity("Unnamed");
		e.AddComponent<Xen::Component::SpriteRenderer>(Xen::Color(1.0f, 1.0f, 1.0f, 1.0f), textureHandle);

		Xen::Component::Transform& transform = e.GetComponent<Xen::Component::Transform>();
		transform.position = { pos.x, pos.y, 0.0f };
	}
};