#include "pch"
#include "ScenePhysics.h"

#include <core/physics/Physics2D.h>

#include "Components.h"

namespace Xen
{
	// Constants:
	const uint32_t velocityIterations = 6;
	const uint32_t positionIterations = 2;

	struct ScenePhysicsData
	{
		double fixedTimeStep;

		Ref<Scene> currentScene;

	}scenePhysicsState;

	void ScenePhysics::Initialize(float fixedTimeStep)
	{
		scenePhysicsState.fixedTimeStep = fixedTimeStep;
	}

	void ScenePhysics::SetActiveScene(const Ref<Scene>& scene)
	{
		scenePhysicsState.currentScene = scene;
	}

	void ScenePhysics::RuntimeStart(const Vec3& gravity)
	{
		if (scenePhysicsState.currentScene->GetSceneType() == SceneType::_2D)
		{
			Physics2D::Init({ gravity.x, gravity.y }, scenePhysicsState.fixedTimeStep);

			auto rigidBodyView = scenePhysicsState.currentScene->m_SceneRegistry.view<Component::RigidBody2D>();

			auto boxColliderView = scenePhysicsState.currentScene->m_SceneRegistry.view<Component::BoxCollider2D>();
			auto circleColliderView = scenePhysicsState.currentScene->m_SceneRegistry.view<Component::CircleCollider2D>();

			for (auto entt : boxColliderView)
			{
				Entity entity = Entity(entt, scenePhysicsState.currentScene.get());

				Component::Transform& transform = entity.GetComponent<Component::Transform>();
				Component::BoxCollider2D& boxCollider = entity.GetComponent<Component::BoxCollider2D>();

				PhysicsBody2D* body = Physics2D::CreateBoxBody(
					{ transform.position.x + boxCollider.bodyOffset.x, transform.position.y + boxCollider.bodyOffset.y },
					transform.rotation.z,
					{ transform.scale.x * boxCollider.sizeScale.x, transform.scale.y * boxCollider.sizeScale.y },
					BodyType2D::Static,
					PhysicsMaterial2D()
				);

				boxCollider.runtimePhysicsBody = body;

				Physics2D::AddCollider(body, { transform.position.x + boxCollider.bodyOffset.x, transform.position.y + boxCollider.bodyOffset.y });

				if (entity.HasAnyComponent<Component::RigidBody2D>())
				{
					Component::RigidBody2D& rigidBody2d = entity.GetComponent<Component::RigidBody2D>();

					Physics2D::SetBodyType(body, rigidBody2d.bodyType);
					Physics2D::SetPhysicsMaterial(body,
						rigidBody2d.physicsMaterial,
						{ transform.scale.x * boxCollider.sizeScale.x, transform.scale.y * boxCollider.sizeScale.y }
					);

					rigidBody2d.runtimePhysicsBody = body;
				}
			}

			for (auto entt : circleColliderView)
			{
				Entity entity = Entity(entt, scenePhysicsState.currentScene.get());

				Component::Transform& transform = entity.GetComponent<Component::Transform>();
				Component::CircleCollider2D& circleCollider = entity.GetComponent<Component::CircleCollider2D>();

				PhysicsBody2D* body = Physics2D::CreateCircleBody(
					{ transform.position.x + circleCollider.bodyOffset.x, transform.position.y + circleCollider.bodyOffset.y },
					transform.rotation.z,
					transform.scale.x * circleCollider.radiusScale * 0.5f,
					BodyType2D::Static,
					PhysicsMaterial2D()
				);

				circleCollider.runtimePhysicsBody = body;

				Physics2D::AddCollider(body, { transform.position.x + circleCollider.bodyOffset.x, transform.position.y + circleCollider.bodyOffset.y });

				if (entity.HasAnyComponent<Component::RigidBody2D>())
				{
					Component::RigidBody2D& rigidBody2d = entity.GetComponent<Component::RigidBody2D>();

					Physics2D::SetBodyType(body, rigidBody2d.bodyType);
					Physics2D::SetPhysicsMaterial(body,
						rigidBody2d.physicsMaterial,
						{ 0.0f, 0.0f }		// This parameter is not used in case of circle
					);

					rigidBody2d.runtimePhysicsBody = body;
				}
			}

			for (auto entt : rigidBodyView)
			{
				Entity entity = Entity(entt, scenePhysicsState.currentScene.get());
				if (!entity.HasAnyComponent<Component::BoxCollider2D, Component::CircleCollider2D>())
				{
					Component::Transform& transform = entity.GetComponent<Component::Transform>();
					Component::RigidBody2D& rigidBody2d = entity.GetComponent<Component::RigidBody2D>();

					PhysicsBody2D* body = Physics2D::CreateBoxBody(
						{ transform.position.x, transform.position.y },
						transform.rotation.z,
						{ transform.scale.x, transform.scale.y },
						rigidBody2d.bodyType,
						rigidBody2d.physicsMaterial
					);

					rigidBody2d.runtimePhysicsBody = body;
				}
			}
		}

		else
		{
			// Initialize and setup the 3D physics system. 
		}

	}
	void ScenePhysics::Step(float timestep)
	{
		if (scenePhysicsState.currentScene->GetSceneType() == SceneType::_2D)
		{
			auto rigidBodyView = scenePhysicsState.currentScene->m_SceneRegistry.view<Component::RigidBody2D>();

			for (auto& entt : rigidBodyView)
			{
				Entity entity = Entity(entt, scenePhysicsState.currentScene.get());

				Component::Transform& transform = entity.GetComponent<Component::Transform>();
				Component::RigidBody2D& rigidBody2d = entity.GetComponent<Component::RigidBody2D>();

				Vec2 offset;
				if (entity.HasAnyComponent<Component::BoxCollider2D>()) {
					Component::BoxCollider2D& boxCollider2d = entity.GetComponent<Component::BoxCollider2D>();
					offset = boxCollider2d.bodyOffset;
				}
				else if (entity.HasAnyComponent<Component::CircleCollider2D>()) {
					Component::CircleCollider2D& circleCollider2D = entity.GetComponent<Component::CircleCollider2D>();
					offset = circleCollider2D.bodyOffset;
				}

				Vec2 position = rigidBody2d.runtimePhysicsBody->position;

				transform.position.x = position.x - offset.x;
				transform.position.y = position.y - offset.y;

				transform.rotation.z = rigidBody2d.runtimePhysicsBody->rotation;
			}

			Physics2D::Step(timestep, velocityIterations, positionIterations);
		}
		else
		{
			// Step 3D physics
		}
	}
	void ScenePhysics::FixedStepUpdate()
	{

	}
	void ScenePhysics::RuntimeEnd()
	{

	}
}