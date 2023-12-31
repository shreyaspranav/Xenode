#pragma once

#include <Core.h>
#include "ParticleSettings2D.h"

#include "Structs.h"

namespace Xen {
	class Texture2D;

	class XEN_API ParticleSystem2D
	{
	public:
		static void Initialize(const ParticleSettings2D* settings);
		static void RenderParticles(double timestep);

	private:
		std::vector<ParticleInstance> m_Instances;
	};
}

