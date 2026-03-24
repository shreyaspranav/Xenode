#pragma once
#include <Core.h>

namespace Xen {
	class XEN_API AudioClip {
	public:
		AudioClip();
		~AudioClip();

		void LoadFromFile(const std::string& filePath);
		void Unload();
		bool IsLoaded() const;
		const std::string& GetFilePath() const;

		void Play();
		void Stop();
		bool IsPlaying() const;

		void SetVolume(float volume);
		void SetPitch(float pitch);
		void SetLooping(bool loop);

	private:
		std::string m_FilePath;
		bool m_Loaded = false;
		void* m_Sound = nullptr; // ma_sound*
	};
}
