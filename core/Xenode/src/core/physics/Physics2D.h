#pragma once

#include <Core.h>
#include <core/renderer/Structs.h>

// For test purposes, must be set in the build system.
// If this flag is set, the build includes box2d or else it is not.
#define XEN_INCLUDE_2D_PHYSICS

namespace Xen {

	enum BodyShape2D : int8_t { Box, Circle };
	enum BodyType2D : int8_t { Static, Dynamic, Kinematic };

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
		static void Init(const Vec2& gravity, double fixedTimeStep);
		static void End();

		static void Step(double timestep, uint32_t velocityIterations, uint32_t positionIterations);

		// Create static bodies with one fixture
		static PhysicsBody2D* CreateBoxBody(const Vec2& position, float rotation, const Vec2& scale, BodyType2D type, const PhysicsMaterial2D& physicsMaterial);
		static PhysicsBody2D* CreateCircleBody(const Vec2& position, float rotation, float radius, BodyType2D type, const PhysicsMaterial2D& physicsMaterial);

		// Set properties of a physics body or a fixture
		static void SetPhysicsMaterial(PhysicsBody2D* body, const PhysicsMaterial2D& material, const Vec2& scale);
		static void SetBodyType(PhysicsBody2D* body, BodyType2D type);
		static void SetBodyTransform(PhysicsBody2D* body, const Vec2& position, float rotation);

		static void DeleteBody(PhysicsBody2D* body);

		// Adds a collider object to a Physics Body
		static void AddCollider(PhysicsBody2D* body, const Vec2& position);

		// Add forces, impulses and velocity to the body
		static void ApplyForce(PhysicsBody2D* body, const Vec2& point, const Vec2& force);
		static void ApplyForceToCenter(PhysicsBody2D* body, const Vec2& force);

		static void SetLinearVelocity(PhysicsBody2D* body, const Vec2& velocity);
		static void SetAngularVelocity(PhysicsBody2D* body, float omega);

	private:
		static PhysicsBody2D* AddBody(const Vec2& position, float rotation, BodyType2D type, const PhysicsMaterial2D& physicsMaterial);
	};
}