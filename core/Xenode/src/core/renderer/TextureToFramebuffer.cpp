#include "pch"
#include "Renderer2D.h"
#include "RenderCommand.h"

#include "Shader.h"

namespace Xen
{
	// Path of the simple passthrough shader:
	const std::filesystem::path passthroughShaderPath = std::string(COMMON_RESOURCES) + "/shaders/Renderer2D/Passthrough.shader";
	struct Data
	{
		Ref<Shader> defaultPassthroughShader = nullptr;
	}state;

	// RenderTextureToFramebuffer Implementation: ---------------------------------------------------------------------------------------------------
	void Renderer2D::RenderTextureToFramebuffer(const Ref<Texture2D>& texture, const Ref<FrameBuffer>& frameBuffer, const Ref<Shader>& shader)
	{
		if (!state.defaultPassthroughShader)
		{
			state.defaultPassthroughShader = Shader::CreateShader(passthroughShaderPath.string());
			state.defaultPassthroughShader->LoadShader(nullptr);
		}

		state.defaultPassthroughShader->Bind();
		texture->Bind(0);
		frameBuffer->Bind();

		RenderCommand::DrawNonIndexed(PrimitiveType::Triangles, nullptr, 6);

		frameBuffer->Unbind();
	}
}