#include "pch"
#include "Renderer2D.h"

#include "core/app/Log.h"
#include "glm/glm.hpp"
#include <glm/ext/matrix_transform.hpp>

#include "core/app/Profiler.h"

namespace Xen {

	// Way of identifing each primitive: 
	enum class Primitive {
		LINE	 = 1 << 0,
		TRIANGLE = 1 << 1,
		QUAD	 = 1 << 2,
		POLYGON	 = 1 << 3,
		CIRCLE	 = 1 << 4,
	};

	SceneData Renderer2D::s_Data;

	// The maximum amount of quads or circles drawn per batch:
	uint32_t max_quads_per_batch = 10000;

	// The maximum amount of textures that can be bound at a time:
	uint8_t max_texture_slots = 8;

	// The maximum amount of lines drawn per batch:
	uint32_t max_lines_per_batch = 10000;

	uint32_t current_quad_index;
	uint32_t current_circle_index;
	uint32_t batch_index = 0;
	uint32_t batches_allocated = 1;

	float line_width = 2.0f;

	uint32_t white_texture_data = 0xffffffff;
	Ref<Texture2D> white_texture;

	uint32_t default_quad_indices[6] = { 0, 1, 2, 0, 2, 3 };
	glm::vec4 temp_vert[4] =
	{
		glm::vec4(0.5f,  0.5f, 0.0f, 1.0f),
		glm::vec4(-0.5f,  0.5f, 0.0f, 1.0f),
		glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f),
		glm::vec4(0.5f, -0.5f, 0.0f, 1.0f),
	};

	struct Renderer2DStorage
	{
		float* quad_verts;
		uint32_t* quad_indices;
		uint32_t quad_index;

		float* circle_quad_verts;
		uint32_t* circle_quad_indices;
		uint32_t circle_quad_index;

		float* verts;
		uint32_t* indices;
		uint32_t vertex_index;
		uint32_t index_count;

		float* line_verts;
		uint32_t line_index;

		std::vector<Ref<Texture2D>> textures;
		uint8_t texture_slot_index;

		Renderer2DStorage()
		{
			quad_verts = new float[max_quads_per_batch * 40];
			quad_indices = new uint32_t[max_quads_per_batch * 6];

			circle_quad_verts = new float[max_quads_per_batch * 48];
			circle_quad_indices = new uint32_t[max_quads_per_batch * 6];

			line_verts = new float[max_lines_per_batch * 7];

			verts = new float[max_quads_per_batch * 4 * 15];
			indices = new uint32_t[max_quads_per_batch * 6];

			quad_index = 0;
			circle_quad_index = 0;
			line_index = 0;
			vertex_index = 0;

			index_count = 0;

			texture_slot_index = 1;
		}

		~Renderer2DStorage()
		{
			delete[] quad_verts;
			delete[] quad_indices;

			delete[] circle_quad_verts;
			delete[] circle_quad_indices;

			delete[] line_verts;

			delete[] verts;
			delete[] indices;
		}
	};

	int texture_slots[8];

	static std::vector<Ref<Renderer2DStorage>> batch_storage;

	Renderer2D::Renderer2DStatistics stats;

	BufferLayout quadBufferLayout;
	BufferLayout circleBufferLayout;
	BufferLayout lineBufferLayout;

	BufferLayout bufferLayout;

	uint32_t stride_count = 15;

	void Renderer2D::Init()
	{
		XEN_PROFILE_FN();

		quadBufferLayout.AddBufferElement(BufferElement("aQuadPosition", 0, 3, 0, BufferDataType::Float, false));
		quadBufferLayout.AddBufferElement(BufferElement("aQuadColor", 1, 4, 3, BufferDataType::Float, false));
		quadBufferLayout.AddBufferElement(BufferElement("aQuadTextureCoords", 2, 2, 7, BufferDataType::Float, false));
		quadBufferLayout.AddBufferElement(BufferElement("aQuadTexSlot", 3, 1, 9, BufferDataType::Float, false));

		circleBufferLayout.AddBufferElement(BufferElement("aCircleQuadWorldCoords", 5, 2, 0, BufferDataType::Float, false));
		circleBufferLayout.AddBufferElement(BufferElement("aCirclePosition", 6, 3, 2, BufferDataType::Float, false));
		circleBufferLayout.AddBufferElement(BufferElement("aCircleColor", 7, 4, 5, BufferDataType::Float, false));
		circleBufferLayout.AddBufferElement(BufferElement("aCircleThickness", 8, 1, 9, BufferDataType::Float, false));
		circleBufferLayout.AddBufferElement(BufferElement("aCircleInnerFade", 9, 1, 10, BufferDataType::Float, false));
		circleBufferLayout.AddBufferElement(BufferElement("aCircleOuterFade", 10, 1, 11, BufferDataType::Float, false));

		lineBufferLayout.AddBufferElement(BufferElement("aLinePosition", 5, 3, 0, BufferDataType::Float, false));
		lineBufferLayout.AddBufferElement(BufferElement("aLineColor", 10, 4, 3, BufferDataType::Float, false));

		bufferLayout.AddBufferElement(BufferElement("aPosition", 1, 3, 0, BufferDataType::Float, false));
		bufferLayout.AddBufferElement(BufferElement("aColor", 2, 4, 3, BufferDataType::Float, false));
		bufferLayout.AddBufferElement(BufferElement("aTextureWorldCoords", 3, 2, 7, BufferDataType::Float, false));
		bufferLayout.AddBufferElement(BufferElement("aP1", 4, 1, 9, BufferDataType::Float, false));
		bufferLayout.AddBufferElement(BufferElement("aP2", 5, 1, 10, BufferDataType::Float, false));
		bufferLayout.AddBufferElement(BufferElement("aP3", 6, 1, 11, BufferDataType::Float, false));
		bufferLayout.AddBufferElement(BufferElement("aP4", 7, 1, 12, BufferDataType::Float, false));
		bufferLayout.AddBufferElement(BufferElement("aP5", 8, 1, 13, BufferDataType::Float, false));
		bufferLayout.AddBufferElement(BufferElement("aPrimitiveType", 9, 1, 14, BufferDataType::Float, false));

		white_texture =  Texture2D::CreateTexture2D(1, 1, &white_texture_data, sizeof(uint32_t));

		s_Data.vertexArray = VertexArray::CreateVertexArray();
		s_Data.vertexArray->Bind();

		s_Data.vertexBuffer = Xen::FloatBuffer::CreateFloatBuffer(max_quads_per_batch * 40);
		s_Data.vertexBuffer->SetBufferLayout(bufferLayout);
		s_Data.indexBuffer = Xen::ElementBuffer::CreateElementBuffer(max_quads_per_batch * 6);

		s_Data.vertexArray->SetVertexBuffer(s_Data.vertexBuffer);
		s_Data.vertexArray->SetElementBuffer(s_Data.indexBuffer);

		s_Data.vertexArray->Load(true);

		// Quad-------------------------------------------------------------------------------------
		s_Data.quadVertexArray = VertexArray::CreateVertexArray();
		s_Data.quadVertexArray->Bind();

		s_Data.quadVertexBuffer = Xen::FloatBuffer::CreateFloatBuffer(max_quads_per_batch * 40);
		s_Data.quadVertexBuffer->SetBufferLayout(quadBufferLayout);
		s_Data.quadIndexBuffer = Xen::ElementBuffer::CreateElementBuffer(max_quads_per_batch * 6);

		s_Data.quadVertexArray->SetVertexBuffer(s_Data.quadVertexBuffer);
		s_Data.quadVertexArray->SetElementBuffer(s_Data.quadIndexBuffer);

		s_Data.quadVertexArray->Load(true);
		//-------------------------------------------------------------------------------------------
		// Circle------------------------------------------------------------------------------------
		s_Data.circleVertexArray = VertexArray::CreateVertexArray();
		s_Data.circleVertexArray->Bind();

		s_Data.circleVertexBuffer = Xen::FloatBuffer::CreateFloatBuffer(max_quads_per_batch * 48);
		s_Data.circleVertexBuffer->SetBufferLayout(circleBufferLayout);
		s_Data.circleIndexBuffer = Xen::ElementBuffer::CreateElementBuffer(max_quads_per_batch * 6);

		s_Data.circleVertexArray->SetVertexBuffer(s_Data.circleVertexBuffer);
		s_Data.circleVertexArray->SetElementBuffer(s_Data.circleIndexBuffer);

		s_Data.circleVertexArray->Load(true);
		//-------------------------------------------------------------------------------------------

		//-------------------------------------------------------------------------------------------
		// Line--------------------------------------------------------------------------------------
		s_Data.lineVertexArray = VertexArray::CreateVertexArray();
		s_Data.lineVertexArray->Bind();

		s_Data.lineVertexBuffer = FloatBuffer::CreateFloatBuffer(1000);
		s_Data.lineVertexBuffer->SetBufferLayout(lineBufferLayout);

		s_Data.lineVertexArray->SetVertexBuffer(s_Data.lineVertexBuffer);

		s_Data.lineVertexArray->Load(false);
		//-------------------------------------------------------------------------------------------

		s_Data.circleShader = Shader::CreateShader("assets/shaders/circle_shader.shader");
		s_Data.circleShader->LoadShader(circleBufferLayout);

		s_Data.quadShader = Shader::CreateShader("assets/shaders/quad_shader.shader");
		s_Data.quadShader->LoadShader(quadBufferLayout);

		s_Data.lineShader = Shader::CreateShader("assets/shaders/line_shader.shader");
		s_Data.lineShader->LoadShader(lineBufferLayout);

		s_Data.shader = Shader::CreateShader("assets/shaders/main_shader.shader");
		s_Data.shader->LoadShader(bufferLayout);

		ShaderLib::AddShader("LineShader", s_Data.lineShader);

		batch_storage.push_back(std::make_shared<Renderer2DStorage>());
		batch_storage[0]->textures.push_back(white_texture);

		for (int i = 0; i < max_texture_slots; i++)
			texture_slots[i] = i;

		stats.quad_vertex_buffer_size = s_Data.quadVertexBuffer->GetSize();
		stats.quad_index_buffer_size = s_Data.quadIndexBuffer->GetSize();

		stats.circle_vertex_buffer_size = s_Data.circleVertexBuffer->GetSize();
		stats.circle_index_buffer_size = s_Data.circleIndexBuffer->GetSize();
	}

	void Renderer2D::ShutDown()
	{

	}

	void Renderer2D::BeginScene(const Ref<Camera>& camera)
	{
		XEN_PROFILE_FN();

		s_Data.camera = camera;

		for (int i = 0; i <= batch_index; i++)
		{
			batch_storage[i]->quad_index = 0;
			batch_storage[i]->circle_quad_index = 0;
			batch_storage[i]->line_index = 0;
			
			batch_storage[i]->vertex_index = 0;

			batch_storage[i]->texture_slot_index = 1;
			batch_storage[i]->index_count = 0;

		}
		batch_index = 0;
		memset(&stats, 0, sizeof(Renderer2D::Renderer2DStatistics));

		stats.quad_vertex_buffer_size = s_Data.quadVertexBuffer->GetSize();
		stats.quad_index_buffer_size = s_Data.quadIndexBuffer->GetSize();

		stats.circle_vertex_buffer_size = s_Data.circleVertexBuffer->GetSize();
		stats.circle_index_buffer_size = s_Data.circleIndexBuffer->GetSize();

		stats.batch_count = batch_index + 1;

		stats.quad_count = 0;
		stats.circle_count = 0;

		stats.texture_count = 0;

		for (const Ref<Renderer2DStorage>& storage : batch_storage)
			stats.texture_count += storage->texture_slot_index;
	}

	void Renderer2D::EndScene()
	{
		// Nothing for now!
	}
	void Renderer2D::RenderFrame()
	{
		XEN_PROFILE_FN();

		stats.draw_calls = 0;

		for (int i = 0; i < max_texture_slots; i++)
			texture_slots[i] = i;

		for (int i = 0; i <= batch_index; i++)
		{
			for (int j = 0; j < batch_storage[i]->textures.size(); j++)
				batch_storage[i]->textures[j]->Bind(j);
		}

		for (int i = 0; i <= batch_index; i++)
		{
			//for (int j = 0; j < batch_storage[i]->textures.size(); j++)
			//	batch_storage[i]->textures[j]->Bind(j);
			//
			//s_Data.lineVertexArray->Bind();
			//s_Data.lineShader->Bind();
			//
			//s_Data.lineVertexBuffer->Put(batch_storage[i]->line_verts, batch_storage[i]->line_index * 14);
			//s_Data.lineShader->SetMat4("u_ViewProjectionMatrix", s_Data.camera->GetViewProjectionMatrix());
			//
			//RenderCommand::DrawLines(s_Data.lineVertexArray, batch_storage[i]->line_index * 14);
			//
			//s_Data.quadVertexArray->Bind();
			//s_Data.quadShader->Bind();
			//
			//s_Data.quadShader->SetMat4("u_ViewProjectionMatrix", s_Data.camera->GetViewProjectionMatrix());
			//s_Data.quadShader->SetIntArray("tex", texture_slots, max_texture_slots);
			//
			//s_Data.quadVertexBuffer->Put(batch_storage[i]->quad_verts, batch_storage[i]->quad_index * 40);
			//
			//if (current_quad_index != batch_storage[i]->quad_index)
			//	s_Data.quadIndexBuffer->Put(batch_storage[i]->quad_indices, batch_storage[i]->quad_index * 6);
			//
			//current_quad_index = batch_storage[i]->quad_index;
			//stats.quad_indices_drawn = batch_storage[i]->quad_index * 6;
			//
			//RenderCommand::DrawIndexed(s_Data.quadVertexArray, (batch_storage[i]->quad_index) * 6);
			//
			//s_Data.circleVertexArray->Bind();
			//s_Data.circleShader->Bind();
			//
			//s_Data.circleVertexBuffer->Put(batch_storage[i]->circle_quad_verts, batch_storage[i]->circle_quad_index * 48);
			//
			//if (current_circle_index != batch_storage[i]->circle_quad_index)
			//	s_Data.circleIndexBuffer->Put(batch_storage[i]->circle_quad_indices, batch_storage[i]->circle_quad_index * 6);
			//
			//current_circle_index = batch_storage[i]->circle_quad_index;
			//stats.circle_indices_drawn = batch_storage[i]->circle_quad_index * 6;
			//
			//s_Data.circleShader->SetMat4("u_ViewProjectionMatrix", s_Data.camera->GetViewProjectionMatrix());
			//
			//RenderCommand::DrawIndexed(s_Data.circleVertexArray, (batch_storage[i]->circle_quad_index) * 6);

			s_Data.vertexArray->Bind();
			s_Data.shader->Bind();
			
			s_Data.vertexBuffer->Put(batch_storage[i]->verts, batch_storage[i]->vertex_index * 15);
			
			//if (current_circle_index != batch_storage[i]->circle_quad_index)
			s_Data.indexBuffer->Put(batch_storage[i]->indices, batch_storage[i]->index_count);
			s_Data.shader->SetIntArray("tex", texture_slots, max_texture_slots);

			//current_circle_index = batch_storage[i]->circle_quad_index;
			//stats.circle_indices_drawn = batch_storage[i]->circle_quad_index * 6;
			
			s_Data.shader->SetMat4("u_ViewProjectionMatrix", s_Data.camera->GetViewProjectionMatrix());
			RenderCommand::DrawIndexed(s_Data.vertexArray, batch_storage[i]->index_count);

			//std::cout << "======================================================================================\n";

			//for (int j = 0; j < batch_storage[i]->index_count; j++)
			//{
			//	if (j % 6 == 0)
			//		std::cout << "\n";
			//	std::cout << batch_storage[i]->indices[j] << "\t";
			//}

			//stats.draw_calls += 3;
		}
	}

	void Renderer2D::DrawClearQuad(const Vec3& position, const Vec3& rotation, const Vec2& scale, const Color& color)
	{
		XEN_PROFILE_FN();

		// Deal with Vertices and Indices:
		Renderer2D::AddQuad(position, rotation, scale);

		// Texture Coodinates
		batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 7] = 1.0f;
		batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 8] = 1.0f;

		batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 17] = 0.0f;
		batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 18] = 1.0f;

		batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 27] = 0.0f;
		batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 28] = 0.0f;

		batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 37] = 1.0f;
		batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 38] = 0.0f;

		//New Code:----------

		// Texture Coords
		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 7] = 1.0f;
		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 8] = 1.0f;

		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 7] = 0.0f;
		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 8] = 1.0f;

		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 7] = 0.0f;
		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 8] = 0.0f;

		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 7] = 1.0f;
		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 8] = 0.0f;

		batch_storage[batch_index]->vertex_index -= 4;

		//Color and Texture ID:
		for (int i = 0; i < 4; i++)
		{
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 3] = color.r;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 4] = color.g;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 5] = color.b;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 6] = color.a;
		}
		batch_storage[batch_index]->vertex_index -= 4;

		for (int i = 0; i < 4; i++)
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 9] = 0.0f;

		//-----------------

		// Color and texture ID:
		for (int i = 0; i < 40; i += 10)
		{
			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 3)] = color.r;
			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 4)] = color.g;
			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 5)] = color.b;
			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 6)] = color.a;

			// Setting the texture slot for out default white texture as 0:
			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 9)] = 0.0f;
		}

		batch_storage[batch_index]->quad_index++;
	}

	void Renderer2D::DrawClearQuad(const Vec3& position, const Vec3& rotation, const Vec2& scale, const Color color[4])
	{
		XEN_PROFILE_FN();

		Renderer2D::AddQuad(position, rotation, scale);

		// Texture Coodinates
		batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 7] = 1.0f;
		batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 8] = 1.0f;

		batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 17] = 0.0f;
		batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 18] = 1.0f;

		batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 27] = 0.0f;
		batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 28] = 0.0f;

		batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 37] = 1.0f;
		batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 38] = 0.0f;

		//New Code:----------

		// Texture Coords
		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 7] = 1.0f;
		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 8] = 1.0f;

		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 7] = 0.0f;
		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 8] = 1.0f;

		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 7] = 0.0f;
		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 8] = 0.0f;

		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 7] = 1.0f;
		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 8] = 0.0f;

		batch_storage[batch_index]->vertex_index -= 4;

		//Color and Texture ID:
		for (int i = 0; i < 4; i++)
		{
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 3] = color[i].r;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 4] = color[i].g;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 5] = color[i].b;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 6] = color[i].a;
		}
		batch_storage[batch_index]->vertex_index -= 4;

		for (int i = 0; i < 4; i++)
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 9] = 0.0f;

		//-----------------


		// Color and texture ID:

		if (color == nullptr)
		{
			for (int i = 0; i < 40; i += 10)
			{
				batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 3)] = 1.0f;
				batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 4)] = 1.0f;
				batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 5)] = 1.0f;
				batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 6)] = 1.0f;

				// Setting the texture slot for out default white texture as 0:
				batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 9)] = 0.0f;
			}
		}
		else
		{
			for (int i = 0; i < 40; i += 10)
			{
				batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 3)] = color[i / 10].r;
				batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 4)] = color[i / 10].g;
				batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 5)] = color[i / 10].b;
				batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 6)] = color[i / 10].a;

				// Setting the texture slot for out default white texture as 0:
				batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 9)] = 0.0f;
			}
		}
		

		batch_storage[batch_index]->quad_index++;
	}


	void Renderer2D::DrawTexturedQuad(const Ref<Texture2D>& texture, const Vec3& position, const Vec3& rotation, const Vec2& scale, const Color& tintcolor, float tiling_factor, const float texture_coords[4])
	{
		XEN_PROFILE_FN();

		// Deal with Vertices and Indices:
		Renderer2D::AddQuad(position, rotation, scale);

		// Texture Coodinates
		batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 7] = 1.0f * tiling_factor;
		batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 8] = 1.0f * tiling_factor;

		batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 17] = 0.0f;
		batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 18] = 1.0f * tiling_factor;

		batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 27] = 0.0f;
		batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 28] = 0.0f;

		batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 37] = 1.0f * tiling_factor;
		batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 38] = 0.0f;

		//New Code:----------

		// Texture Coords
		if (texture_coords == nullptr)
		{
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 7] = 1.0f * tiling_factor;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 8] = 1.0f * tiling_factor;

			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 7] = 0.0f;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 8] = 1.0f * tiling_factor;

			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 7] = 0.0f;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 8] = 0.0f;

			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 7] = 1.0f * tiling_factor;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 8] = 0.0f;
		}

		else {

			for (int i = 0; i < 4; i++)
			{
				batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 7] = texture_coords[i] * tiling_factor;
				batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 8] = texture_coords[i] * tiling_factor;
			}
		}

		batch_storage[batch_index]->vertex_index -= 4;

		//Color:
		for (int i = 0; i < 4; i++)
		{
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 3] = tintcolor.r;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 4] = tintcolor.g;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 5] = tintcolor.b;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 6] = tintcolor.a;
		}
		batch_storage[batch_index]->vertex_index -= 4;

		//-----------------


		for (int i = 0; i < 40; i += 10)
		{
			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 3)] = tintcolor.r;
			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 4)] = tintcolor.g;
			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 5)] = tintcolor.b;
			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 6)] = tintcolor.a;
		}

		// Check to see if 'texture' is NOT in the vector
		std::vector<Ref<Texture2D>>::iterator itr = std::find(batch_storage[batch_index]->textures.begin(), batch_storage[batch_index]->textures.end(), texture);

		if (itr == batch_storage[batch_index]->textures.end())
		{
			batch_storage[batch_index]->textures.push_back(texture);
			//stats.texture_count++;
			for (int i = 0; i < 40; i += 10)
				batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 9)] = batch_storage[batch_index]->textures.size() - 1;
		}

		else
		{
			for (int i = 0; i < 40; i += 10)
				batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 9)] = (float)std::distance(batch_storage[batch_index]->textures.begin(), itr);
		}

		// New Code:---------------

		// Check to see if 'texture' is NOT in the vector
		//std::vector<Ref<Texture2D>>::iterator itr = std::find(batch_storage[batch_index]->textures.begin(), batch_storage[batch_index]->textures.end(), texture);
		itr = std::find(batch_storage[batch_index]->textures.begin(), batch_storage[batch_index]->textures.end(), texture);

		if (itr == batch_storage[batch_index]->textures.end())
		{
			batch_storage[batch_index]->textures.push_back(texture);
			//stats.texture_count++;

			for (int i = 0; i < 4; i++)
				batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 9] = batch_storage[batch_index]->textures.size() - 1;
		}

		else
		{
			for (int i = 0; i < 4; i++)
				batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 9] = (float)std::distance(batch_storage[batch_index]->textures.begin(), itr);
		}

		// -------------------------------------

		batch_storage[batch_index]->quad_index++;
	}

	void Renderer2D::DrawTexturedQuad(const Ref<Texture2D>& texture, const Vec3& position, const Vec3& rotation, const Vec2& scale, const Color tintcolor[4], float tiling_factor, const float texture_coords[4])
	{
		XEN_PROFILE_FN();

		Renderer2D::AddQuad(position, rotation, scale);

		// Texture Coodinates
		batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 7] = 1.0f * tiling_factor;
		batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 8] = 1.0f * tiling_factor;

		batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 17] = 0.0f;
		batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 18] = 1.0f * tiling_factor;

		batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 27] = 0.0f;
		batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 28] = 0.0f;

		batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 37] = 1.0f * tiling_factor;
		batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 38] = 0.0f;


		for (int i = 0; i < 40; i += 10)
		{
			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 3)] = tintcolor[i].r;
			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 4)] = tintcolor[i].g;
			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 5)] = tintcolor[i].b;
			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 6)] = tintcolor[i].a;
		}

		// New Code: --------------------------------------------
		// Texture Coords
		if (texture_coords == nullptr)
		{
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 7] = 1.0f * tiling_factor;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 8] = 1.0f * tiling_factor;

			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 7] = 0.0f;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 8] = 1.0f * tiling_factor;

			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 7] = 0.0f;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 8] = 0.0f;

			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 7] = 1.0f * tiling_factor;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 8] = 0.0f;
		}

		else {

			for (int i = 0; i < 4; i++)
			{
				batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 7] = texture_coords[i] * tiling_factor;
				batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 8] = texture_coords[i] * tiling_factor;
			}
		}

		batch_storage[batch_index]->vertex_index -= 4;

		//Color:
		for (int i = 0; i < 4; i++)
		{
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 3] = tintcolor[i].r;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 4] = tintcolor[i].g;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 5] = tintcolor[i].b;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 6] = tintcolor[i].a;
		}
		batch_storage[batch_index]->vertex_index -= 4;

		//-----------------

		// Check to see if 'texture' is NOT in the vector
		std::vector<Ref<Texture2D>>::iterator itr = std::find(batch_storage[batch_index]->textures.begin(), batch_storage[batch_index]->textures.end(), texture);

		if (itr == batch_storage[batch_index]->textures.end())
		{
			batch_storage[batch_index]->textures.push_back(texture);
			//stats.texture_count++;
			for (int i = 0; i < 40; i += 10)
				batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 9)] = batch_storage[batch_index]->textures.size() - 1;
		}

		else
		{
			for (int i = 0; i < 40; i += 10)
				batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 9)] = (float)std::distance(batch_storage[batch_index]->textures.begin(), itr);
		}
		batch_storage[batch_index]->quad_index++;
	}

	//void Renderer2D::DrawTexturedQuad(const Ref<Texture2D>& texture, const Vec3& position, const Vec3& rotation, const Vec2& scale, const Color& tintcolor, float tiling_factor, const float tex_coords[4])
	//{
	//	XEN_PROFILE_FN();
	//
	//	// Deal with Vertices and Indices:
	//	Renderer2D::AddQuad(position, rotation, scale);
	//
	//	// Texture Coodinates
	//	batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 7] = tex_coords[0] * tiling_factor;
	//	batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 8] = tex_coords[1] * tiling_factor;
	//
	//	batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 17] = tex_coords[2] * tiling_factor;
	//	batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 18] = tex_coords[3] * tiling_factor;
	//
	//	batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 27] = tex_coords[4] * tiling_factor;
	//	batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 28] = tex_coords[5] * tiling_factor;
	//
	//	batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 37] = tex_coords[6] * tiling_factor;
	//	batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 38] = tex_coords[7] * tiling_factor;
	//
	//
	//	for (int i = 0; i < 40; i += 10)
	//	{
	//		//batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 2)] = position.z;
	//
	//		batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 3)] = tintcolor.r;
	//		batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 4)] = tintcolor.g;
	//		batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 5)] = tintcolor.b;
	//		batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 6)] = tintcolor.a;
	//
	//		//batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 9)] = (float)batch_storage[batch_index]->texture_slot_index;
	//	}
	//
	//	std::vector<Ref<Texture2D>>::iterator itr = std::find(batch_storage[batch_index]->textures.begin(), batch_storage[batch_index]->textures.end(), texture);
	//
	//	if (itr == batch_storage[batch_index]->textures.end())
	//	{
	//		batch_storage[batch_index]->textures.push_back(texture);
	//		//stats.texture_count++;
	//		for (int i = 0; i < 40; i += 10)
	//			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 9)] = batch_storage[batch_index]->textures.size() - 1;
	//	}
	//
	//	else
	//	{
	//		for (int i = 0; i < 40; i += 10)
	//			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 9)] = (float)std::distance(batch_storage[batch_index]->textures.begin(), itr);
	//	}
	//
	//	batch_storage[batch_index]->quad_index++;
	//}

	void Renderer2D::DrawClearCircle(const Vec3& position, const Vec3& rotation, const Vec2& scale, const Color& color, float thickness, float innerfade, float outerfade)
	{
		XEN_PROFILE_FN();

		Renderer2D::AddCircleQuad(position, rotation, scale);

		for (int i = 0; i < 48; i += 12)
		{
			// Color
			batch_storage[batch_index]->circle_quad_verts[(batch_storage[batch_index]->circle_quad_index * 48) + (i + 5)] = color.r;
			batch_storage[batch_index]->circle_quad_verts[(batch_storage[batch_index]->circle_quad_index * 48) + (i + 6)] = color.g;
			batch_storage[batch_index]->circle_quad_verts[(batch_storage[batch_index]->circle_quad_index * 48) + (i + 7)] = color.b;
			batch_storage[batch_index]->circle_quad_verts[(batch_storage[batch_index]->circle_quad_index * 48) + (i + 8)] = color.a;

			// Thickness
			batch_storage[batch_index]->circle_quad_verts[(batch_storage[batch_index]->circle_quad_index * 48) + (i + 9)] = thickness;

			// Inner Fade
			batch_storage[batch_index]->circle_quad_verts[(batch_storage[batch_index]->circle_quad_index * 48) + (i + 10)] = innerfade;

			// Outer Fade
			batch_storage[batch_index]->circle_quad_verts[(batch_storage[batch_index]->circle_quad_index * 48) + (i + 11)] = outerfade;

		}

		// New Code----------------------------------------
		for (int i = 0; i < 4; i++)
		{
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 3] = color.r;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 4] = color.g;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 5] = color.b;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 6] = color.a;

			// Thickness
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 9 ] = thickness;

			// Inner Fade
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 10] = innerfade;

			// Outer Fade
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 11] = outerfade;

		}


		//-------------------------------------------------

		batch_storage[batch_index]->circle_quad_index++;
	}

	void Renderer2D::SetLineWidth(float width)
	{
		line_width = width;
	}

	Renderer2D::Renderer2DStatistics& Renderer2D::GetStatistics() { return stats; }

	void Renderer2D::DrawLine(const Vec3& p1, const Vec3& p2, const Color& color, float thickness)
	{
		XEN_PROFILE_FN();

		if (batch_storage[batch_index]->line_index > max_lines_per_batch)
		{
			batch_index++;
			
			// Increase the size of the vector if needed
			if (batch_index >= batches_allocated)
			{
				batch_storage.push_back(std::make_shared<Renderer2DStorage>());
				batch_storage[batch_index]->textures.push_back(white_texture);
				batches_allocated = batch_index + 1;
			}
		}
		batch_storage[batch_index]->line_verts[(batch_storage[batch_index]->line_index * 14) + 0] = p1.x;
		batch_storage[batch_index]->line_verts[(batch_storage[batch_index]->line_index * 14) + 1] = p1.y;
		batch_storage[batch_index]->line_verts[(batch_storage[batch_index]->line_index * 14) + 2] = p1.z;

		batch_storage[batch_index]->line_verts[(batch_storage[batch_index]->line_index * 14) + 3] = color.r;
		batch_storage[batch_index]->line_verts[(batch_storage[batch_index]->line_index * 14) + 4] = color.g;
		batch_storage[batch_index]->line_verts[(batch_storage[batch_index]->line_index * 14) + 5] = color.b;
		batch_storage[batch_index]->line_verts[(batch_storage[batch_index]->line_index * 14) + 6] = color.a;

		batch_storage[batch_index]->line_verts[(batch_storage[batch_index]->line_index * 14) + 7] = p2.x;
		batch_storage[batch_index]->line_verts[(batch_storage[batch_index]->line_index * 14) + 8] = p2.y;
		batch_storage[batch_index]->line_verts[(batch_storage[batch_index]->line_index * 14) + 9] = p2.z;

		batch_storage[batch_index]->line_verts[(batch_storage[batch_index]->line_index * 14) + 10] = color.r;
		batch_storage[batch_index]->line_verts[(batch_storage[batch_index]->line_index * 14) + 11] = color.g;
		batch_storage[batch_index]->line_verts[(batch_storage[batch_index]->line_index * 14) + 12] = color.b;
		batch_storage[batch_index]->line_verts[(batch_storage[batch_index]->line_index * 14) + 13] = color.a;

		batch_storage[batch_index]->line_index++;
	}
	
	// Private methods -----------------------------------------------------------------------------------------------------------------------
	/*
	* Quad Vertex Order: 
	*		2		1
	*		--------
	*		|      |
	*		|      |
	*		--------
	*		3		4
	*/
	void Renderer2D::AddQuad(const Vec3& position, const Vec3& rotation, const Vec2& scale)
	{
		XEN_PROFILE_FN();

		if (batch_storage[batch_index]->texture_slot_index >= max_texture_slots || batch_storage[batch_index]->quad_index >= max_quads_per_batch - 1)
		{
			batch_index++;
			
			// Increase the size of the vector if needed
			if (batch_index >= batches_allocated)
			{
				batch_storage.push_back(std::make_shared<Renderer2DStorage>());
				batch_storage[batch_index]->textures.push_back(white_texture);
				batches_allocated = batch_index + 1;
			}
		}

		for (int i = 0; i < 6; i++)
		{
			batch_storage[batch_index]->quad_indices[(batch_storage[batch_index]->quad_index * 6) + i] = (batch_storage[batch_index]->quad_index * 4) + default_quad_indices[i];
			// New code:
			batch_storage[batch_index]->indices[batch_storage[batch_index]->index_count + i] = batch_storage[batch_index]->vertex_index + default_quad_indices[i];
		}
			batch_storage[batch_index]->index_count += 6;

		// Vertices

		if (rotation.x == 0.0f && rotation.y == 0.0f && rotation.z == 0.0f)
		{
			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 0] = position.x + (0.5f * scale.x);
			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 1] = position.y + (0.5f * scale.y);
			
			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 10] = position.x - (0.5f * scale.x);
			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 11] = position.y + (0.5f * scale.y);
			
			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 20] = position.x - (0.5f * scale.x);
			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 21] = position.y - (0.5f * scale.y);
			
			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 30] = position.x + (0.5f * scale.x);
			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 31] = position.y - (0.5f * scale.y);

			// New Code------
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 0] = position.x + (0.5f * scale.x);
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 1] = position.y + (0.5f * scale.y);
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 2] = position.z;

			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 0] = position.x - (0.5f * scale.x);
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 1] = position.y + (0.5f * scale.y);
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 2] = position.z;

			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 0] = position.x - (0.5f * scale.x);
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 1] = position.y - (0.5f * scale.y);
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 2] = position.z;

			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 0] = position.x + (0.5f * scale.x);
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 1] = position.y - (0.5f * scale.y);
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 2] = position.z;

			batch_storage[batch_index]->vertex_index -= 4;

			for (int i = 0; i < 4; i++)
				batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 14] = (float)Primitive::QUAD;

			batch_storage[batch_index]->vertex_index -= 4;

			//----------------


			for (int i = 0; i < 4; i++)
				batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i * 10) + 2] = position.z;
		}

		else {
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), position.GetVec())
				* glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1, 0, 0))
				* glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0, 1, 0))
				* glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0, 0, 1))
				* glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, 1.0f));

			for (int i = 0; i < 4; i++)
			{
				batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i * 10) + 0] = (transform * temp_vert[i]).x;
				batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i * 10) + 1] = (transform * temp_vert[i]).y;
				batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i * 10) + 2] = (transform * temp_vert[i]).z;

				// New code------
				batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 0] = (transform * temp_vert[i]).x;
				batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 1] = (transform * temp_vert[i]).y;
				batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 2] = (transform * temp_vert[i]).z;

				batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 14] = (float)Primitive::QUAD;

				// --------------

			}
			batch_storage[batch_index]->vertex_index -= 4;
		}
		stats.quad_count++;
	}

	void Renderer2D::AddCircleQuad(const Vec3& position, const Vec3& rotation, const Vec2& scale)
	{
		XEN_PROFILE_FN();

		if (batch_storage[batch_index]->circle_quad_index >= max_quads_per_batch - 1)
		{
			batch_index++;

			// Increase the size of the vector if needed
			if (batch_index >= batches_allocated)
			{
				batch_storage.push_back(std::make_shared<Renderer2DStorage>());
				batch_storage[batch_index]->textures.push_back(white_texture);
				batches_allocated = batch_index + 1;
			}
		}

		for (int i = 0; i < 6; i++)
		{
			batch_storage[batch_index]->circle_quad_indices[(batch_storage[batch_index]->circle_quad_index * 6) + i] = (batch_storage[batch_index]->circle_quad_index * 4) + default_quad_indices[i];
			// New code:
			batch_storage[batch_index]->indices[batch_storage[batch_index]->index_count + i] = batch_storage[batch_index]->vertex_index + default_quad_indices[i];
		}

		batch_storage[batch_index]->index_count += 6;

		// Vertices
		batch_storage[batch_index]->circle_quad_verts[(batch_storage[batch_index]->circle_quad_index * 48) + 2] = position.x + (0.5f * scale.x);
		batch_storage[batch_index]->circle_quad_verts[(batch_storage[batch_index]->circle_quad_index * 48) + 3] = position.y + (0.5f * scale.y);

		batch_storage[batch_index]->circle_quad_verts[(batch_storage[batch_index]->circle_quad_index * 48) + 14] = position.x - (0.5f * scale.x);
		batch_storage[batch_index]->circle_quad_verts[(batch_storage[batch_index]->circle_quad_index * 48) + 15] = position.y + (0.5f * scale.y);

		batch_storage[batch_index]->circle_quad_verts[(batch_storage[batch_index]->circle_quad_index * 48) + 26] = position.x - (0.5f * scale.x);
		batch_storage[batch_index]->circle_quad_verts[(batch_storage[batch_index]->circle_quad_index * 48) + 27] = position.y - (0.5f * scale.y);

		batch_storage[batch_index]->circle_quad_verts[(batch_storage[batch_index]->circle_quad_index * 48) + 38] = position.x + (0.5f * scale.x);
		batch_storage[batch_index]->circle_quad_verts[(batch_storage[batch_index]->circle_quad_index * 48) + 39] = position.y - (0.5f * scale.y);

		for (int i = 0; i < 4; i++)
			batch_storage[batch_index]->circle_quad_verts[(batch_storage[batch_index]->circle_quad_index * 48) + (i * 12) + 4] = position.z;

		// New Code------
		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 0] = position.x + (0.5f * scale.x);
		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 1] = position.y + (0.5f * scale.y);
		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 2] = position.z;

		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 0] = position.x - (0.5f * scale.x);
		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 1] = position.y + (0.5f * scale.y);
		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 2] = position.z;

		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 0] = position.x - (0.5f * scale.x);
		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 1] = position.y - (0.5f * scale.y);
		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 2] = position.z;

		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 0] = position.x + (0.5f * scale.x);
		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 1] = position.y - (0.5f * scale.y);
		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 2] = position.z;

		batch_storage[batch_index]->vertex_index -= 4;

		for (int i = 0; i < 4; i++)
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 14] = (float)Primitive::CIRCLE;

		batch_storage[batch_index]->vertex_index -= 4;

		// ---------------------------------------------------------
		
		// Circle World coordinates
		batch_storage[batch_index]->circle_quad_verts[(batch_storage[batch_index]->circle_quad_index * 48) +  0] = 1.0f;
		batch_storage[batch_index]->circle_quad_verts[(batch_storage[batch_index]->circle_quad_index * 48) +  1] = 1.0f;

		batch_storage[batch_index]->circle_quad_verts[(batch_storage[batch_index]->circle_quad_index * 48) + 12] = -1.0f;
		batch_storage[batch_index]->circle_quad_verts[(batch_storage[batch_index]->circle_quad_index * 48) + 13] = 1.0f;

		batch_storage[batch_index]->circle_quad_verts[(batch_storage[batch_index]->circle_quad_index * 48) + 24] = -1.0f;
		batch_storage[batch_index]->circle_quad_verts[(batch_storage[batch_index]->circle_quad_index * 48) + 25] = -1.0f;

		batch_storage[batch_index]->circle_quad_verts[(batch_storage[batch_index]->circle_quad_index * 48) + 36] = 1.0f;
		batch_storage[batch_index]->circle_quad_verts[(batch_storage[batch_index]->circle_quad_index * 48) + 37] = -1.0f;

		// New Code:------------------------------------------------

		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 7] = 1.0f;
		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 8] = 1.0f;

		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 7] = -1.0f;
		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 8] = 1.0f;

		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 7] = -1.0f;
		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 8] = -1.0f;

		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 7] = 1.0f;
		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 8] = -1.0f;

		batch_storage[batch_index]->vertex_index -= 4;
		// -------------------------------------------------------------

		stats.circle_count++;
	}
}