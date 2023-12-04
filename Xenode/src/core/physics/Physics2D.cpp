#include <pch>
#include "Physics2D.h"

#include <core/app/Log.h>

#include <box2d/box2d.h>
#include <glm/gtc/constants.hpp>

namespace Xen {

	constexpr auto DEGTORAD = 0.0174532925199432957f;
	constexpr auto RADTODEG = 57.295779513082320876f;

	b2World* physicsWorld;
	std::vector<PhysicsBody2D*> physicsBodies;

	void Physics2D::Init(const Vec2& gravity)
	{
		physicsWorld = new b2World({ gravity.x, gravity.y });
	}

	void Physics2D::End()
	{
		for (PhysicsBody2D* body : physicsBodies) {
			physicsWorld->DestroyBody((b2Body*)(body->runtimeBody));
			delete body;
		}

		delete physicsWorld;
	}
	void Physics2D::Step(double timestep, uint32_t physicsStepIterations, uint32_t velocityIterations, uint32_t positionIterations)
	{
		for (int i = 0; i < physicsStepIterations; i++)
			physicsWorld->Step(timestep, velocityIterations, positionIterations);

		for (PhysicsBody2D* body : physicsBodies)
		{
			b2Body* physicsBody = (b2Body*)body->runtimeBody;

			Vec2 position = { physicsBody->GetPosition().x, physicsBody->GetPosition().y };
			float rotation = physicsBody->GetAngle() * RADTODEG;
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

		b2Filter noCollisionFilter;
		noCollisionFilter.maskBits = 0x0000;
		noCollisionFilter.categoryBits = 0x0000;

		fixtureDef.filter = noCollisionFilter;

		b2Fixture* bodyFixture = physicsBody->CreateFixture(&fixtureDef);
		
		body->runtimeBody = physicsBody;
		body->runtimeFixtures.push_back(bodyFixture);

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

		body->runtimeBody = physicsBody;
		body->runtimeFixtures.push_back(bodyFixture);

		return body;
	}

	void Physics2D::AddBoxCollider(PhysicsBody2D* body, const Vec2& position, const Vec2& scale)
	{
		// TEMPORARY: 
		b2Fixture* fixture = (b2Fixture*)body->runtimeFixtures[0];
		fixture->SetFilterData(b2Filter());

		b2Body* physicsBody = (b2Body*)body->runtimeBody;
		physicsBody->SetTransform({ position.x, position.y }, body->rotation * DEGTORAD);
		
		body->position = position;
	}

	void Physics2D::AddCircleCollider(PhysicsBody2D* body, const Vec2& position, float radius)
	{
		// TEMPORARY: 
		b2Fixture* fixture = (b2Fixture*)body->runtimeFixtures[0];
		fixture->SetFilterData(b2Filter());

		b2Body* physicsBody = (b2Body*)body->runtimeBody;
		physicsBody->SetTransform({ position.x, position.y }, body->rotation * DEGTORAD);
	}

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

		b2Body* physicsBody = physicsWorld->CreateBody(&bodyDef);

		physicsBody->SetTransform({ position.x, position.y }, rotation * DEGTORAD);
		body->position = position;
		body->rotation = rotation;

		physicsBody->SetFixedRotation(physicsMaterial.fixedRotation);

		body->runtimeBody = physicsBody;
		return body;
	}
}