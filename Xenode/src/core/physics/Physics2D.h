#pragma once

#include <Core.h>
#include <core/renderer/Structs.h>

namespace Xen {

	enum BodyShape2D { Box, Circle };
	enum BodyType2D  { Static, Dynamic, Kinematic };

	struct PhysicsBody2D
	{
		BodyShape2D shape;
		BodyType2D type;

		Vec2 position;
		float rotation;

		void* runtimeBody;
		std::vector<void*> runtimeFixtures;
	};

	struct PhysicsMaterial2D
	{
		float friction = 0.2f;
		float restitution = 0.5f;
		float restitutionThreshold;
		float mass = 1.0f;

		bool fixedRotation = false;
	};

	// If Body has only RigidBody2D: Reacts to gravity, doesn't collide with anything.
	// If Body has only BoxCollider2D: collides with objects but a static body.

	class XEN_API Physics2D
	{
	public:
		static void Init(const Vec2& gravity);
		static void End();

		static void Step(double timestep, uint32_t physicsStepIterations, uint32_t velocityIterations, uint32_t positionIterations);

		// Create static bodies with one fixture
		static PhysicsBody2D* CreateBoxBody(const Vec2& position, float rotation, const Vec2& scale, BodyType2D type, const PhysicsMaterial2D& physicsMaterial);
		static PhysicsBody2D* CreateCircleBody(const Vec2& position, float rotation, float radius, BodyType2D type, const PhysicsMaterial2D& physicsMaterial);

		static void AddCollider(PhysicsBody2D* body, const Vec2& position, const Vec2& scale);

	private:
		static PhysicsBody2D* AddBody(const Vec2& position, float rotation, BodyType2D type, const PhysicsMaterial2D& physicsMaterial);
	};
}