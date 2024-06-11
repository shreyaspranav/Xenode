#pragma once
#include <Core.h>

#include <core/app/UUID.h>

#include "Scene.h"
#include "Components.h"

namespace Xen 
{
	class XEN_API SceneUtils
	{
	public:
		static void CopyScene(Ref<Scene> destination, Ref<Scene> source);
		static Entity CopyEntity(Entity entity);


		template<typename Comp>
		static void CopyComponentAllEntities(entt::registry& srcSceneRegistry, entt::registry& dstSceneRegistry, const std::unordered_map<UUID, Entity>& uuidMap);

		// template<typename... Comp>
		// static void CopyComponentAllEntities(Component::Group<Comp...>, const Ref<Scene>& srcSceneRegistry, const Ref<Scene>& dstSceneRegistry, const UnorderedMap<UUID, Entity>& uuidMap);

		template<typename... Comp>
		static void CopyComponents(Component::Group<Comp...>, Entity& src, Entity& dst);
		
		template<typename... Comp>
		static void CopyComponents(Entity& src, Entity& dst);
	};
}