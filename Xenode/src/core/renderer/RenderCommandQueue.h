#pragma once

#include <Core.h>
#include "RenderCommand.h"

#include "VertexArray.h"
#include "Buffer.h"
#include "Camera.h"
#include "Shader.h"
#include "Texture.h"

namespace Xen {

	struct SceneData
	{
		Ref<Camera> camera;

		Ref<VertexBuffer> lineVertexBuffer;
		Ref<Shader> lineShader;

		Ref<VertexBuffer> vertexBuffer;
		Ref<ElementBuffer> indexBuffer;
		Ref<Shader> shader;

		Ref<Shader> lightShader;
		Ref<Shader> screenShader;

		SceneData()
		{}
	};

	class XEN_API RenderCommandQueue
	{
	private:
		inline static std::vector<SceneData> m_Batches;
		inline static SceneData m_Data;

	public:
		static void Submit(SceneData data) { m_Data = data; }
		static void RenderFrame()
		{
			//RenderCommand::DrawIndexed(m_Data.vertexArray);
		}
	};
}