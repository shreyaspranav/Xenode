#include "pch"
#include "AudioEngine.h"

#include <core/app/Log.h>

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

namespace Xen {

	ma_engine s_Engine;
	bool s_Initialized = false;

	void AudioEngine::Init()
	{
		ma_engine_config config = ma_engine_config_init();
		ma_result result = ma_engine_init(&config, &s_Engine);

		if (result != MA_SUCCESS)
		{
			XEN_ENGINE_LOG_ERROR("Failed to initialize audio engine!");
			return;
		}

		s_Initialized = true;
		XEN_ENGINE_LOG_INFO("Audio Engine initialized successfully.");
	}

	void AudioEngine::Shutdown()
	{
		if (s_Initialized)
		{
			ma_engine_uninit(&s_Engine);
			s_Initialized = false;
		}
	}

	void AudioEngine::SetMasterVolume(float volume)
	{
		if (s_Initialized)
			ma_engine_set_volume(&s_Engine, volume);
	}

	float AudioEngine::GetMasterVolume()
	{
		if (s_Initialized)
			return ma_engine_get_volume(&s_Engine);
		return 0.0f;
	}
}
