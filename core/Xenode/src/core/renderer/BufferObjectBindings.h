#pragma once
#include<Core.h>

namespace Xen 
{
	namespace UniformBufferBinding 
	{
		constexpr uint8_t MAIN_SHADER_PER_FRAME_DATA			= 1;

		constexpr uint8_t PARTICLE_EMITTER_PER_FRAME_DATA		= 2;
		constexpr uint8_t PARTICLE_EMITTER_SETTINGS				= 3;
	}

	namespace StorageBufferBinding 
	{
		constexpr uint8_t MAIN_SHADER_VERTEX_BUFFER_DATA				= 1;
		constexpr uint8_t DEBUG_RENDERER_SHADER_VERTEX_BUFFERS_DATA		= 2;
	}
}
