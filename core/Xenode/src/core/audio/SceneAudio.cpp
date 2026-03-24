#include "pch"
#include "SceneAudio.h"
#include "AudioEngine.h"
#include "AudioClip.h"

#include <core/scene/Components.h>
#include <core/app/Log.h>

namespace Xen {

	struct SceneAudioData
	{
		Ref<Scene> currentScene;
	} sceneAudioState;

	void SceneAudio::Initialize()
	{
		AudioEngine::Init();
	}

	void SceneAudio::SetActiveScene(const Ref<Scene>& scene)
	{
		sceneAudioState.currentScene = scene;
	}

	void SceneAudio::RuntimeStart()
	{
		auto audioView = sceneAudioState.currentScene->m_SceneRegistry.view<Component::AudioSource>();

		for (auto entt : audioView)
		{
			Entity entity = Entity(entt, sceneAudioState.currentScene.get());
			Component::AudioSource& audioSource = entity.GetComponent<Component::AudioSource>();

			if (audioSource.audioFilePath.empty())
				continue;

			AudioClip* clip = new AudioClip();
			clip->LoadFromFile(audioSource.audioFilePath);

			if (clip->IsLoaded())
			{
				clip->SetVolume(audioSource.volume);
				clip->SetPitch(audioSource.pitch);
				clip->SetLooping(audioSource.loop);

				if (audioSource.playOnStart)
					clip->Play();
			}

			audioSource.runtimeSound = clip;
		}
	}

	void SceneAudio::RuntimeUpdate()
	{
		// Currently no per-frame audio updates needed.
		// Future: update spatial audio positions from transforms.
	}

	void SceneAudio::RuntimeEnd()
	{
		auto audioView = sceneAudioState.currentScene->m_SceneRegistry.view<Component::AudioSource>();

		for (auto entt : audioView)
		{
			Entity entity = Entity(entt, sceneAudioState.currentScene.get());
			Component::AudioSource& audioSource = entity.GetComponent<Component::AudioSource>();

			if (audioSource.runtimeSound)
			{
				AudioClip* clip = static_cast<AudioClip*>(audioSource.runtimeSound);
				clip->Stop();
				delete clip;
				audioSource.runtimeSound = nullptr;
			}
		}
	}
}
