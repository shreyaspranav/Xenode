#include "pch"
#include "ScreenRenderer.h"

#include "Shader.h"
#include "RenderCommand.h"

#include <glad/gl.h>

namespace Xen {

	Ref<Shader> screenShader;

	void ScreenRenderer2D::Init()
	{
		BufferLayout l;
		screenShader = Shader::CreateShader("assets/shaders/screen_shader.shader");
		screenShader->LoadShader(l);
	}

	void ScreenRenderer2D::RenderFinalSceneToScreen(uint32_t unlitSceneTextureID, uint32_t sceneMaskTextureID, uint32_t lightMapTextureID)
	{
		screenShader->Bind();

		Texture2D::BindTexture(unlitSceneTextureID, 0);
		Texture2D::BindTexture(lightMapTextureID, 1);
		//Texture2D::BindTexture(sceneMaskTextureID, 2);

		screenShader->SetInt("u_UnlitSceneTexture", 0);
		screenShader->SetInt("u_LightMapTexture", 1);

		RenderCommand::DrawTriangles(nullptr, 6);
	}
}