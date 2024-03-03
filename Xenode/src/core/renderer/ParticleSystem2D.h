#pragma once

#include <Core.h>

#include "ParticleSettings2D.h"

namespace Xen {
	class XEN_API ParticleSystem2D
	{
	public:
		friend class ParticleInstance;

		static void Initialize();
		//static void RenderParticles(ParticleSettings2D* particleSettings, double timestep);

		static void RenderParticles(ParticleInstance2D* particleInstance, double timestep);
	private:
		static void InitializeBuffers(ParticleInstance2D* particleInstance);
	};
}

