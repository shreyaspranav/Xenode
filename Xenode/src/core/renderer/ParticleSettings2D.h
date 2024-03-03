#pragma once

#include <Core.h>

#include "Structs.h"
#include "Buffer.h"

namespace Xen {
	class Texture2D;

	enum class ParticleSimulationSpace	{ Local = 0, World = 1 };

	enum class ValueType				{ Fixed, Random };
	enum class EmissionType				{ RateOverTime, Burst, };
	enum class AffectColorBy			{ Distance = 0, Lifetime = 1, Random = 2 };

	// This union is represent a float that can be fixed or be a random value between two floats
	union FloatValue
	{
		float value;
		float randomBetween[2];
	};

	union IntValue
	{
		int value;
		int randomBetween[2];
	};

	// TODO: Implement this later
	struct ParticlePhysicsSettings {};

	struct ParticleSettings2D
	{
		// Type of emmision that is used: 
		// 'Burst' is for burst of specified amount of particles in a given time interval
		// 'RateOverTime' is for specified number of particles generated per second
		EmissionType emissionType;

		// Position of the emitter point
		Vec3 position;

		// Angle of the emitter IN DEGREES.
		float emitterAngle;

		// Ranges of size, rotation, and velocity
		Vec2 sizeRandomRange =		{ 0.1f,  0.4f };
		Vec2 rotationRandomRange =	{ 0.0f,  180.0f };
		Vec2 velocityRandomRange =	{ 0.3f,  0.9f };
		Vec2 lifeRandomRange =		{ 10.0f, 2.0f };

		// Used to dampen the velocity of each particle every frame
		float velocityDamping = 0.1f;

		// No. of particles emitted in a second
		uint32_t rate = 10;

		// Color gradient
		ColorGradient colorGradient;
		AffectColorBy affectColorBy = AffectColorBy::Lifetime;
		float colorDistanceScale = 4.0f;

		// Use Local if the particles need to follow the emitter point
		ParticleSimulationSpace particleSimulationSpace;

		bool usePhysics;
		ParticlePhysicsSettings physicsSettings;
	};

	struct ParticleInstance2D
	{
	public:
		friend class ParticleSystem2D;

		ParticleSettings2D particleSettings;
		Vec3 instancePosition;

	private:
		Ref<VertexBuffer> particleReadWriteBuffer[2];

		bool buffersInitialized = false;

		uint32_t numParticlesInBuffer = 1;
		uint32_t currentBuffer = 0;

		// TODO: Maybe add shaders for each particle instance.
	};
}

