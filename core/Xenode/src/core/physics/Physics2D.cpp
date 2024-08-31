#include <pch>
#include "Physics2D.h"

#include <core/app/Log.h>

#ifdef XEN_INCLUDE_2D_PHYSICS

#include <box2d/box2d.h>
#include <glm/gtc/constants.hpp>

namespace Xen {

	constexpr auto DEGTORAD = 0.0174532925199432957f;
	constexpr auto RADTODEG = 57.295779513082320876f;

	struct PhysicsState
	{
		double fixedTimeStep;

		b2World* physicsWorld;
		std::vector<PhysicsBody2D*> physicsBodies;

	}physicsState;

	void Physics2D::Init(const Vec2& gravity, double fixedTimeStep)
	{
		physicsState.fixedTimeStep = fixedTimeStep;
		physicsState.physicsWorld = new b2World({ gravity.x, gravity.y });
	}

	void Physics2D::End()
	{
		for (PhysicsBody2D* body : physicsState.physicsBodies)
		{
			physicsState.physicsWorld->DestroyBody((b2Body*)(body->runtimeBody));
			delete body;
		}

		physicsState.physicsBodies.clear();
		delete physicsState.physicsWorld;
	}

	void Physics2D::Step(double timestep, uint32_t velocityIterations, uint32_t positionIterations)
	{
		// timestep *= 0.001; // Because the timestep is in milliseconds and box2d expects in seconds

		// uint8_t stepIterationsMinusOne = (timestep / physicsState.fixedTimeStep);
		// double lastIterationTimeStep = timestep - (physicsState.fixedTimeStep * stepIterationsMinusOne);
		// 
		// for (int i = 0; i < stepIterationsMinusOne * 2; i++)
		// 	physicsState.physicsWorld->Step(physicsState.fixedTimeStep, velocityIterations, positionIterations);
		// 
		// physicsState.physicsWorld->Step(lastIterationTimeStep, velocityIterations, positionIterations);
		double timestepInSeconds = physicsState.fixedTimeStep / 1000.0f;
		physicsState.physicsWorld->Step(timestepInSeconds, velocityIterations, positionIterations);

		for (PhysicsBody2D* body : physicsState.physicsBodies)
		{
			b2Body* physicsBody = (b2Body*)body->runtimeBody;

			body->position = { physicsBody->GetPosition().x, physicsBody->GetPosition().y };
			body->rotation = physicsBody->GetAngle() * RADTODEG;
		}
	}

	PhysicsBody2D* Physics2D::CreateBoxBody(const Vec2& position, float rotation, const Vec2& scale, BodyType2D type, const PhysicsMaterial2D& physicsMaterial)
	{
		PhysicsBody2D* body = Physics2D::AddBody(position, rotation, type, physicsMaterial);

		b2Body* physicsBody = (b2Body*)body->runtimeBody;

		b2PolygonShape boxShape;

		// These are HALF Extents:
		boxShape.SetAsBox(scale.x / 2.0f, scale.y / 2.0f);

		b2FixtureDef fixtureDef;

		fixtureDef.shape = &boxShape;
		fixtureDef.density = physicsMaterial.mass / (scale.x * scale.y);
		fixtureDef.friction = physicsMaterial.friction;
		fixtureDef.restitution = physicsMaterial.restitution;
		fixtureDef.restitutionThreshold = physicsMaterial.restitutionThreshold;

		// Filter data to avoid collisions: Basically no collider
		b2Filter noCollisionFilter;
		noCollisionFilter.maskBits = 0x0000;
		noCollisionFilter.categoryBits = 0x0000;

		fixtureDef.filter = noCollisionFilter;

		b2Fixture* bodyFixture = physicsBody->CreateFixture(&fixtureDef);

		body->shape = BodyShape2D::Box;
		body->runtimeBody = physicsBody;
		body->runtimeFixtures.push_back(bodyFixture);
		physicsState.physicsBodies.push_back(body);

		return body;
	}

	PhysicsBody2D* Physics2D::CreateCircleBody(const Vec2& position, float rotation, float radius, BodyType2D type, const PhysicsMaterial2D& physicsMaterial)
	{
		PhysicsBody2D* body = Physics2D::AddBody(position, rotation, type, physicsMaterial);

		b2Body* physicsBody = (b2Body*)body->runtimeBody;

		b2CircleShape circleShape;

		circleShape.m_p.SetZero();
		circleShape.m_radius = radius;

		b2FixtureDef fixtureDef;

		fixtureDef.shape = &circleShape;
		fixtureDef.density = physicsMaterial.mass / (glm::pi<float>() * pow(radius, 2.0));
		fixtureDef.friction = physicsMaterial.friction;
		fixtureDef.restitution = physicsMaterial.restitution;
		fixtureDef.restitutionThreshold = physicsMaterial.restitutionThreshold;

		b2Filter noCollisionFilter;
		noCollisionFilter.maskBits = 0x0000;
		noCollisionFilter.categoryBits = 0x0000;

		fixtureDef.filter = noCollisionFilter;

		b2Fixture* bodyFixture = physicsBody->CreateFixture(&fixtureDef);

		body->shape = BodyShape2D::Circle;
		body->runtimeBody = physicsBody;
		body->runtimeFixtures.push_back(bodyFixture);
		physicsState.physicsBodies.push_back(body);

		return body;
	}

	void Physics2D::SetPhysicsMaterial(PhysicsBody2D* body, const PhysicsMaterial2D& material, const Vec2& scale)
	{
		b2Body* physicsBody = (b2Body*)body->runtimeBody;
		physicsBody->SetFixedRotation(material.fixedRotation);

		b2Fixture* fixture = (b2Fixture*)body->runtimeFixtures[0];

		switch (body->shape)
		{
		case BodyShape2D::Box:
			fixture->SetDensity(material.mass / (scale.x * scale.y));
			// This function actually updates mass and therefore density:
			physicsBody->ResetMassData();
			break;
		case BodyShape2D::Circle:
			fixture->SetDensity(material.mass / (glm::pi<float>() * pow(fixture->GetShape()->m_radius, 2.0)));
			physicsBody->ResetMassData();
			break;
		default:
			break;
		}

		fixture->SetFriction(material.friction);
		fixture->SetRestitution(material.restitution);
		fixture->SetRestitutionThreshold(material.restitutionThreshold);
	}

	void Physics2D::SetBodyType(PhysicsBody2D* body, BodyType2D type)
	{
		b2Body* physicsBody = (b2Body*)body->runtimeBody;

		switch (type)
		{
		case BodyType2D::Static:		physicsBody->SetType(b2_staticBody);		break;
		case BodyType2D::Dynamic:		physicsBody->SetType(b2_dynamicBody);		break;
		case BodyType2D::Kinematic:		physicsBody->SetType(b2_kinematicBody);		break;
		default:
			XEN_ENGINE_LOG_ERROR("Unknown Body Type!");
			break;
		}
	}

	void Physics2D::SetBodyTransform(PhysicsBody2D* body, const Vec2& position, float rotation)
	{
		b2Body* physicsBody = (b2Body*)body->runtimeBody;
		physicsBody->SetTransform({ position.x, position.y }, rotation * DEGTORAD);
	}

	void Physics2D::DeleteBody(PhysicsBody2D* body)
	{
		physicsState.physicsWorld->DestroyBody((b2Body*)body->runtimeBody);

		for (auto it = physicsState.physicsBodies.begin(); it != physicsState.physicsBodies.end(); ++it) 
		{
			if (*it == body) 
			{
				physicsState.physicsBodies.erase(it);
				break;
			}
		}

		delete body;
	}

	void Physics2D::AddCollider(PhysicsBody2D* body, const Vec2& position)
	{
		// TEMPORARY: 
		b2Fixture* fixture = (b2Fixture*)body->runtimeFixtures[0];
		fixture->SetFilterData(b2Filter());

		b2Body* physicsBody = (b2Body*)body->runtimeBody;
		physicsBody->SetTransform({ position.x, position.y }, body->rotation * DEGTORAD);

		body->position = position;
	}

	void Physics2D::ApplyForce(PhysicsBody2D* body, const Vec2& point, const Vec2& force)
	{
		b2Body* physicsBody = (b2Body*)body->runtimeBody;
		physicsBody->ApplyForce({ force.x, force.y }, { point.x, point.y }, true);
	}

	void Physics2D::ApplyForceToCenter(PhysicsBody2D* body, const Vec2& force)
	{
		b2Body* physicsBody = (b2Body*)body->runtimeBody;
		physicsBody->ApplyForceToCenter({ force.x, force.y }, true);
	}

	void Physics2D::SetLinearVelocity(PhysicsBody2D* body, const Vec2& velocity)
	{
		b2Body* physicsBody = (b2Body*)body->runtimeBody;
		physicsBody->SetLinearVelocity({ velocity.x, velocity.y });
	}

	void Physics2D::SetAngularVelocity(PhysicsBody2D* body, float omega)
	{
		b2Body* physicsBody = (b2Body*)body->runtimeBody;
		physicsBody->SetAngularVelocity(omega);

	}

	// Private Methods: --------------------------------------------------------------------------------------------------------------------------------
	PhysicsBody2D* Physics2D::AddBody(const Vec2& position, float rotation, BodyType2D type, const PhysicsMaterial2D& physicsMaterial)
	{
		PhysicsBody2D* body = new PhysicsBody2D();
		b2BodyDef bodyDef;

		switch (type)
		{
		case BodyType2D::Static:		bodyDef.type = b2_staticBody;		break;
		case BodyType2D::Dynamic:		bodyDef.type = b2_dynamicBody;		break;
		case BodyType2D::Kinematic:		bodyDef.type = b2_kinematicBody;	break;
		default:
			XEN_ENGINE_LOG_ERROR("Unknown Body Type!");
			break;
		}

		b2Body* physicsBody = physicsState.physicsWorld->CreateBody(&bodyDef);

		physicsBody->SetTransform({ position.x, position.y }, rotation * DEGTORAD);
		body->position = position;
		body->rotation = rotation;

		physicsBody->SetFixedRotation(physicsMaterial.fixedRotation);

		body->runtimeBody = physicsBody;
		return body;
	}
}

#else

namespace Xen {
	void Physics2D::Init(const Vec2& gravity) {}
	void Physics2D::End() {}
	void Physics2D::Step(double timestep, uint32_t velocityIterations, uint32_t positionIterations) {}
	PhysicsBody2D* Physics2D::CreateBoxBody(const Vec2& position, float rotation, const Vec2& scale, BodyType2D type, const PhysicsMaterial2D& physicsMaterial) {}
	PhysicsBody2D* Physics2D::CreateCircleBody(const Vec2& position, float rotation, float radius, BodyType2D type, const PhysicsMaterial2D& physicsMaterial) {}
	void Physics2D::SetPhysicsMaterial(PhysicsBody2D* body, const PhysicsMaterial2D& material, const Vec2& scale) {}
	void Physics2D::SetBodyType(PhysicsBody2D* body, BodyType2D type) {}
	void Physics2D::SetBodyTransform(PhysicsBody2D* body, const Vec2& position, float rotation) {}
	void Physics2D::DeleteBody(PhysicsBody2D* body) {}
	void Physics2D::AddCollider(PhysicsBody2D* body, const Vec2& position) {}
	void Physics2D::ApplyForce(PhysicsBody2D* body, const Vec2& point, const Vec2& force) {}
	void Physics2D::ApplyForceToCenter(PhysicsBody2D* body, const Vec2& force) {}
	void Physics2D::SetLinearVelocity(PhysicsBody2D* body, const Vec2& velocity) {}
	void Physics2D::SetAngularVelocity(PhysicsBody2D* body, float omega) {}

	// Private Methods: --------------------------------------------------------------------------------------------------------------------------------
	PhysicsBody2D* Physics2D::AddBody(const Vec2& position, float rotation, BodyType2D type, const PhysicsMaterial2D& physicsMaterial) {}
}
#endif