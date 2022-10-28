#include "pch"
#include "Renderer2D.h"

#include "core/app/Log.h"

namespace Xen {

	SceneData Renderer2D::s_Data;
	bool index_buffer_put = 0;

	uint32_t max_quads = 1024;

	struct Renderer2DStorage
	{
		//_2D::Quad* quads;
		//_2D::Circle* circles;
		//_2D::Line* lines;
		float* quad_verts;
		uint32_t* quad_indices;

		uint32_t quad_index;

		Renderer2DStorage()
		{
			//quads = new _2D::Quad[10000];
			//circles = nullptr;
			//lines = nullptr;

			quad_verts = new float[max_quads * 40];
			quad_indices = new uint32_t[max_quads * 6];

			quad_index = 0;
		}

		~Renderer2DStorage()
		{
			//delete[] quads;

			delete[] quad_verts;
			delete[] quad_indices;
		}
	};


	uint32_t default_quad_indices[6] = {0, 1, 2, 0, 2, 3};
	uint32_t current_quad_index;

	static Scope<Renderer2DStorage> storage;

	void Renderer2D::Init()
	{
		storage = std::make_unique<Renderer2DStorage>();

		s_Data.shader = Xen::Shader::CreateShader("assets/shaders/Shader.s");
		s_Data.shader->LoadShader();

		ShaderLib::AddShader("SimpleShader", s_Data.shader);

		s_Data.vertexArray = Xen::VertexArray::GetVertexArray();
		s_Data.vertexArray->Bind();

		BufferLayout bufferLayout;
		bufferLayout.AddBufferElement(BufferElement("vertex_positions", 0, 3, 0, BufferDataType::Float, false));
		bufferLayout.AddBufferElement(BufferElement("vertex_colors", 1, 4, 3, BufferDataType::Float, false));
		bufferLayout.AddBufferElement(BufferElement("texture_coordinates", 2, 2, 7, BufferDataType::Float, false));
		bufferLayout.AddBufferElement(BufferElement("texture_ID", 3, 1, 9, BufferDataType::Float, false));

		s_Data.vertexBuffer = Xen::FloatBuffer::CreateFloatBuffer(max_quads * 40); // 10000 Quads!
		s_Data.vertexBuffer->SetBufferLayout(bufferLayout);
		s_Data.indexBuffer = Xen::ElementBuffer::CreateElementBuffer(max_quads * 6);

		s_Data.vertexArray->SetVertexBuffer(s_Data.vertexBuffer);
		s_Data.vertexArray->SetElementBuffer(s_Data.indexBuffer);

		s_Data.vertexArray->Load();

		s_Data.vertexArray->Bind();
		s_Data.shader->Bind();
	}

	void Renderer2D::ShutDown()
	{

	}
	void Renderer2D::BeginScene()
	{
	}

	void Renderer2D::BeginScene(const Ref<OrthographicCamera>& camera)
	{
		s_Data.camera = camera;
		storage->quad_index = 0;
	}

	void Renderer2D::EndScene()
	{
		s_Data.vertexBuffer->Put(storage->quad_verts, storage->quad_index * 40);
		if (current_quad_index != storage->quad_index)
		{
			s_Data.indexBuffer->Put(storage->quad_indices, storage->quad_index * 6);
			XEN_ENGINE_LOG_INFO("Updating Index Buffer");
		}

		current_quad_index = storage->quad_index;
	}
	void Renderer2D::RenderFrame()
	{
		s_Data.shader->SetMat4("u_ViewProjectionMatrix", s_Data.camera->GetViewProjectionMatrix());

		RenderCommand::DrawIndexed(s_Data.vertexArray, (storage->quad_index) * 6);
	}
	void Renderer2D::Submit(Ref<VertexArray> vertexArray)
	{
		//s_Data.vertexArray = vertexArray;
		//RenderCommandQueue::Submit(s_Data);
	}
	void Renderer2D::Submit(_2D::Quad& quad)
	{
		//for (int i = 0; i < 6; i++)
		//	quad.indices[i] += (active_quads * 4);
		//
		//float quad_vertices[40];
		//
		//s_Data.vertexBuffer->Put(4 * 10 * active_quads, quad_vertices, 40);
		//s_Data.indexBuffer->Put(6 * active_quads, quad.indices, 6);
		//RenderCommandQueue::Submit(s_Data);
	}
	void Renderer2D::DrawClearQuad(const Vec3& position, const Vec2& scale, float rotation, const Color& color)
	{
		for (int i = 0; i < 6; i++)
			storage->quad_indices[(storage->quad_index * 6) + i] = (storage->quad_index * 4) + default_quad_indices[i];

		storage->quad_verts[(storage->quad_index * 40) + 0] = position.x + (0.5f * scale.x);
		storage->quad_verts[(storage->quad_index * 40) + 1] = position.y + (0.5f * scale.y);
		
		storage->quad_verts[(storage->quad_index * 40) + 10] = position.x - (0.5f * scale.x);
		storage->quad_verts[(storage->quad_index * 40) + 11] = position.y + (0.5f * scale.y);
		
		storage->quad_verts[(storage->quad_index * 40) + 20] = position.x - (0.5f * scale.x);
		storage->quad_verts[(storage->quad_index * 40) + 21] = position.y - (0.5f * scale.y);
		
		storage->quad_verts[(storage->quad_index * 40) + 30] = position.x + (0.5f * scale.x);
		storage->quad_verts[(storage->quad_index * 40) + 31] = position.y - (0.5f * scale.y);



		for (int i = 0; i < 40; i += 10)
		{
			storage->quad_verts[(storage->quad_index * 40) + (i + 2)] = position.z;

			storage->quad_verts[(storage->quad_index * 40) + (i + 3)] = color.r;
			storage->quad_verts[(storage->quad_index * 40) + (i + 4)] = color.g;
			storage->quad_verts[(storage->quad_index * 40) + (i + 5)] = color.b;
			storage->quad_verts[(storage->quad_index * 40) + (i + 6)] = color.a;
		}

		//for (int i = 0; i < 4; i++)
		//{
		//	int u = (storage->quad_index * 4) + i;
		//
		//	//quad_verts[(u * 10) + 2] = storage->quads[storage->quad_index].vertices[i].z;
		//
		//	// Texture Coords
		//	//quad_verts[(u * 10) + 7] = storage->quads[storage->quad_index].texCoords[i].x;
		//	//quad_verts[(u * 10) + 8] = storage->quads[storage->quad_index].texCoords[i].y;
		//
		//	// Texture ID
		//	//quad_verts[(u * 10) + 9] = storage->quads[storage->quad_index].texSlot;
		//}

		//storage->quads[storage->quad_index] = quad;
		storage->quad_index++;
	}
}