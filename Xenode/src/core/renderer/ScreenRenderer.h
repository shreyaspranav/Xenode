#pragma once
#include <Core.h>
#include <core/renderer/Texture.h>

namespace Xen {
	class ScreenRenderer2D
	{
	public:
		static void Init();
		static void RenderTextureToScreen(const Ref<Texture2D>& texture);
		static void RenderFinalSceneToScreen(uint32_t unlitSceneTextureID, uint32_t sceneMaskTextureID, uint32_t lightMapTextureID);

		static void SetAmbientLightIntensity(float intensity);
	};
}