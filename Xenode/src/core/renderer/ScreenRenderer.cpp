#include "pch"
#include "ScreenRenderer.h"

#include "Shader.h"
#include "RenderCommand.h"

#include <glad/gl.h>

namespace Xen {

	Ref<Shader> screenShader;
	float ambientLightIntensity = 0.3f;

	void ScreenRenderer2D::Init()
	{
		BufferLayout l;
		screenShader = Shader::CreateShader("assets/shaders/screen_shader.shader");
		screenShader->LoadShader(l);
	}
	void ScreenRenderer2D::RenderTextureToScreen(const Ref<Texture2D>& texture)
	{
		screenShader->Bind();
		//texture->Bind(0);
		RenderCommand::DrawTriangles(nullptr, 6);
	}
	void ScreenRenderer2D::RenderFinalSceneToScreen(uint32_t unlitSceneTextureID, uint32_t sceneMaskTextureID, uint32_t lightMapTextureID)
	{
		screenShader->Bind();

		//unlit_scene->Bind(0);
		//light_map->Bind(1);

		Texture2D::BindTexture(unlitSceneTextureID, 0);
		Texture2D::BindTexture(lightMapTextureID, 1);
		Texture2D::BindTexture(sceneMaskTextureID, 2);

		screenShader->SetInt("u_UnlitSceneTexture", 0);
		screenShader->SetInt("u_LightMapTexture", 1);
		screenShader->SetInt("u_SceneMaskTexture", 2);

		screenShader->SetFloat("u_AmbientLightIntensity", ambientLightIntensity);

		RenderCommand::DrawTriangles(nullptr, 6);
	}
	void ScreenRenderer2D::SetAmbientLightIntensity(float intensity)
	{
		ambientLightIntensity = intensity;
	}

	float ScreenRenderer2D::GetAmbientLightIntensity() { return ambientLightIntensity; }
}