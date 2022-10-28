#pragma once

#include "RenderCommandQueue.h"
#include "Primitives.h"

namespace Xen {

	class Renderer2D
	{
	private:
		static SceneData s_Data;

	public:
		static void Init();
		static void ShutDown();
		
		static void BeginScene();
		static void BeginScene(const Ref<OrthographicCamera>& camera);
		static void EndScene();
		
		static void RenderFrame();

		static void Submit(Ref<VertexArray> vertexArray);
		static void Submit(_2D::Quad& quad);

		static void DrawClearQuad(const Vec3& position, const Vec2& scale = Vec2(1.0f, 1.0f), float rotation = 0.0f, const Color& color = Color());

	};
}
