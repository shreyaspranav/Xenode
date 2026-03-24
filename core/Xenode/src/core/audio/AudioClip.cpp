#include "pch"
#include "AudioClip.h"

#include <core/app/Log.h>
#include <miniaudio.h>

namespace Xen {
	// The ma_engine is defined in AudioEngine.cpp
	extern ma_engine s_Engine;
	extern bool s_Initialized;

	AudioClip::AudioClip() {}

	AudioClip::~AudioClip()
	{
		Unload();
	}

	void AudioClip::LoadFromFile(const std::string& filePath)
	{
		if (!s_Initialized)
		{
			XEN_ENGINE_LOG_ERROR("Audio engine not initialized. Cannot load audio clip: {0}", filePath);
			return;
		}

		Unload();

		m_FilePath = filePath;

		ma_sound* sound = new ma_sound();
		ma_result result = ma_sound_init_from_file(&s_Engine, filePath.c_str(), 0, nullptr, nullptr, sound);

		if (result != MA_SUCCESS)
		{
			XEN_ENGINE_LOG_ERROR("Failed to load audio clip: {0}", filePath);
			delete sound;
			return;
		}

		m_Sound = sound;
		m_Loaded = true;
	}

	void AudioClip::Unload()
	{
		if (m_Loaded && m_Sound)
		{
			ma_sound* sound = static_cast<ma_sound*>(m_Sound);
			ma_sound_uninit(sound);
			delete sound;
			m_Sound = nullptr;
			m_Loaded = false;
		}
	}

	bool AudioClip::IsLoaded() const { return m_Loaded; }
	const std::string& AudioClip::GetFilePath() const { return m_FilePath; }

	void AudioClip::Play()
	{
		if (m_Loaded && m_Sound)
		{
			ma_sound* sound = static_cast<ma_sound*>(m_Sound);
			ma_sound_seek_to_pcm_frame(sound, 0);
			ma_sound_start(sound);
		}
	}

	void AudioClip::Stop()
	{
		if (m_Loaded && m_Sound)
		{
			ma_sound* sound = static_cast<ma_sound*>(m_Sound);
			ma_sound_stop(sound);
		}
	}

	bool AudioClip::IsPlaying() const
	{
		if (m_Loaded && m_Sound)
		{
			ma_sound* sound = static_cast<ma_sound*>(m_Sound);
			return ma_sound_is_playing(sound);
		}
		return false;
	}

	void AudioClip::SetVolume(float volume)
	{
		if (m_Loaded && m_Sound)
		{
			ma_sound* sound = static_cast<ma_sound*>(m_Sound);
			ma_sound_set_volume(sound, volume);
		}
	}

	void AudioClip::SetPitch(float pitch)
	{
		if (m_Loaded && m_Sound)
		{
			ma_sound* sound = static_cast<ma_sound*>(m_Sound);
			ma_sound_set_pitch(sound, pitch);
		}
	}

	void AudioClip::SetLooping(bool loop)
	{
		if (m_Loaded && m_Sound)
		{
			ma_sound* sound = static_cast<ma_sound*>(m_Sound);
			ma_sound_set_looping(sound, loop);
		}
	}
}
