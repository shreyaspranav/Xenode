#include "pch"
#include "ScreenRenderer.h"

#include "Shader.h"
#include "RenderCommand.h"

#include <glad/gl.h>

namespace Xen {

	Ref<Shader> screenShader;

	void ScreenRenderer2D::Init()
	{
		VertexBufferLayout l;
		screenShader = Shader::CreateShader("assets/shaders/screen_shader.shader");
		screenShader->LoadShader(nullptr);
	}

	void ScreenRenderer2D::RenderFinalSceneToScreen(uint32_t unlitSceneTextureID, uint32_t sceneMaskTextureID, uint32_t lightMapTextureID)
	{
		screenShader->Bind();

		Texture2D::BindTexture(unlitSceneTextureID, 0);

		RenderCommand::DrawTriangles(nullptr, 6);
	}
}