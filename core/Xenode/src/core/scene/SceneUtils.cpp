#include "pch"
#include "SceneUtils.h"

#include "Components.h"

namespace Xen 
{
	void SceneUtils::CopyScene(Ref<Scene> destination, Ref<Scene> source)
	{
		auto& srcSceneRegistry = source->m_SceneRegistry;
		auto& dstSceneRegistry = destination->m_SceneRegistry;

		UnorderedMap<UUID, Entity> uuidEntityMap;

		srcSceneRegistry.each([&](auto entt) 
			{
				Entity entity = Entity(entt, source.get());

				Component::ID& id = entity.GetComponent<Component::ID>();
				Component::Tag& tag = entity.GetComponent<Component::Tag>();

				Entity newEntity = destination->AddNewEntityWithID(tag.tag, id.id);

				uuidEntityMap.insert({ id.id, newEntity });
			});

		// TODO: Maybe CopyEntity Itself is enough? Instead of this abomination
		// SceneUtils::CopyComponentAllEntities(Component::All{}, source, destination, idEntityMap);

		CopyComponentAllEntities<Component::Transform>(srcSceneRegistry, dstSceneRegistry, uuidEntityMap);
		CopyComponentAllEntities<Component::CameraComp>(srcSceneRegistry, dstSceneRegistry, uuidEntityMap);
		CopyComponentAllEntities<Component::SpriteRenderer>(srcSceneRegistry, dstSceneRegistry, uuidEntityMap);;
		CopyComponentAllEntities<Component::TextRenderer>(srcSceneRegistry, dstSceneRegistry, uuidEntityMap);
		CopyComponentAllEntities<Component::RigidBody2D>(srcSceneRegistry, dstSceneRegistry, uuidEntityMap);
		CopyComponentAllEntities<Component::BoxCollider2D>(srcSceneRegistry, dstSceneRegistry, uuidEntityMap);
		CopyComponentAllEntities<Component::CircleCollider2D>(srcSceneRegistry, dstSceneRegistry, uuidEntityMap);
		CopyComponentAllEntities<Component::NativeScript>(srcSceneRegistry, dstSceneRegistry, uuidEntityMap);
		CopyComponentAllEntities<Component::ScriptComp>(srcSceneRegistry, dstSceneRegistry, uuidEntityMap);
		CopyComponentAllEntities<Component::PointLight>(srcSceneRegistry, dstSceneRegistry, uuidEntityMap);
		CopyComponentAllEntities<Component::AmbientLight>(srcSceneRegistry, dstSceneRegistry, uuidEntityMap);
		CopyComponentAllEntities<Component::ParticleSystem2DComp>(srcSceneRegistry, dstSceneRegistry, uuidEntityMap);
	}

	Entity SceneUtils::CopyEntity(Entity entity)
	{
		Entity toReturn = entity.GetParentScene()->AddNewEntity(entity.GetComponent<Component::Tag>().tag);
		CopyComponents(Component::All{}, entity, toReturn);

		return toReturn;
	}

	// Private Functions: ------------------------------------------------------------------------------------------------------------------------------------------------

	// template<typename... Comp>
	// void SceneUtils::CopyComponentAllEntities(Component::Group<Comp...>, const Ref<Scene>& srcScene, const Ref<Scene>& dstScene, const UnorderedMap<UUID, Scene::Entity>& uuidMap)
	// {
	// 
	// 	// ([&]() 
	// 	// 	{
	// 	// 		auto& srcRegistry = srcScene->m_SceneRegistry;
	// 	// 		auto& dstRegistry = dstScene->m_SceneRegistry;
	// 	// 
	// 	// 		srcRegistry.each([&](auto entt)
	// 	// 			{
	// 	// 				Scene::Entity srcEntity = Scene::Entity(entt, srcScene.get());
	// 	// 
	// 	// 				UUID uuid = srcEntity.GetComponent<Component::ID>().id;
	// 	// 				Scene::Entity dstEntity = uuidMap.at(uuid);
	// 	// 
	// 	// 				if (srcEntity.HasAnyComponent<Comp>())
	// 	// 				{
	// 	// 					auto& compToBeCopied = srcEntity.GetComponent<Comp>();
	// 	// 					dstEntity.EmplaceOrReplaceComponent<Comp>(compToBeCopied);
	// 	// 				}
	// 	// 			});
	// 	// 	}, ...);
	// 
	// 	SceneUtils::CopyComponentAllEntities<Comp...>(srcScene, dstScene, uuidMap);
	// 
	// }
	
	template<typename Comp>
	void SceneUtils::CopyComponentAllEntities(entt::registry& srcSceneRegistry, entt::registry& dstSceneRegistry, const std::unordered_map<UUID, Entity>& uuidMap)
	{

		auto srcRegView = srcSceneRegistry.view<Component::ID>();
		for (entt::entity src_entt : srcRegView)
		{
			UUID uuid = srcSceneRegistry.get<Component::ID>(src_entt).id;
			entt::entity dst_entt = uuidMap.at(uuid);

			if (srcSceneRegistry.any_of<Comp>(src_entt))
			{
				auto& compToBeCopied = srcSceneRegistry.get<Comp>(src_entt);
				dstSceneRegistry.emplace_or_replace<Comp>(dst_entt, compToBeCopied);
			}
		}
		
	}

	template<typename... Comp>
	void SceneUtils::CopyComponents(Component::Group<Comp...>, Entity& src, Entity& dst)
	{
		SceneUtils::CopyComponents<Comp...>(src, dst);
	}
	
	template<typename... Comp>
	void SceneUtils::CopyComponents(Entity& src, Entity& dst)
	{
		([&]()
			{
				if (src.HasAnyComponent<Comp>())
				{
					auto& compToBeCopied = src.GetComponent<Comp>();
					dst.EmplaceOrReplaceComponent<Comp>(compToBeCopied);
				}
			}(), ...);
	}
}