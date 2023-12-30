#pragma once

#include <Core.h>

#include "Structs.h"

namespace Xen {
	class Texture2D;

	enum class ParticleSimulationSpace	{ Local = 0, World = 1 };

	enum class ValueType				{ Fixed, Random };
	enum class EmissionType				{ RateOverTime, Burst, };

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

		// Particle Size, Rotation and Velocity
		ValueType sizeQuantityType;
		ValueType rotationQuantityType;
		ValueType velocityQuantityType;

		FloatValue size;
		FloatValue rotation;
		FloatValue velocity;

		// No. of particles emitted in a second
		uint16_t rate;

		// Color gradient
		// TODO: Use an actual color gradient type in future
		Color startColor;
		Color endColor;

		// Use Local if the particles need to follow the emitter point
		ParticleSimulationSpace particleSimulationSpace;

		bool usePhysics;
		ParticlePhysicsSettings physicsSettings;
	};

	struct ParticleInstance
	{
		ParticleSettings2D* settings;
	};
}

