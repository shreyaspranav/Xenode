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

		Ref<VertexArray> quadVertexArray;
		Ref<FloatBuffer> quadVertexBuffer;
		Ref<ElementBuffer> quadIndexBuffer;
		Ref<Shader> quadShader;

		Ref<VertexArray> circleVertexArray;
		Ref<FloatBuffer> circleVertexBuffer;
		Ref<ElementBuffer> circleIndexBuffer;
		Ref<Shader> circleShader;

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