#include "pch"
#include "Renderer2D.h"

#include "core/app/Log.h"
#include "glm/glm.hpp"
#include <glm/ext/matrix_transform.hpp>

#include "core/app/Profiler.h"

namespace Xen {

	SceneData Renderer2D::s_Data;

	// The maximum amount of quads or circles drawn per batch:
	uint32_t max_quads_per_batch = 10000;

	// The maximum amount of vertices drawn per batch:
	uint32_t max_vertices_per_batch = 40000;

	// The maximum amount of textures that can be bound at a time:
	uint8_t max_texture_slots = 8;

	// The maximum amount of lines drawn per batch:
	uint32_t max_lines_per_batch = 10000;

	uint32_t batch_index = 0;
	uint32_t batches_allocated = 1;

	float line_width = 2.0f;

	uint32_t stride_count = 15;

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
			line_verts = new float[max_lines_per_batch * 7];

			verts = new float[max_vertices_per_batch * stride_count];
			indices = new uint32_t[max_quads_per_batch * 6];

			line_index = 0;
			vertex_index = 0;

			index_count = 0;

			texture_slot_index = 1;
		}

		~Renderer2DStorage()
		{
			delete[] line_verts;

			delete[] verts;
			delete[] indices;
		}
	};

	int texture_slots[8];

	static std::vector<Ref<Renderer2DStorage>> batch_storage;

	Renderer2D::Renderer2DStatistics stats;


	BufferLayout bufferLayout;
	BufferLayout lineBufferLayout;

	void Renderer2D::Init()
	{
		XEN_PROFILE_FN();

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

		s_Data.vertexBuffer = Xen::FloatBuffer::CreateFloatBuffer(max_vertices_per_batch * stride_count);
		s_Data.vertexBuffer->SetBufferLayout(bufferLayout);
		s_Data.indexBuffer = Xen::ElementBuffer::CreateElementBuffer(max_quads_per_batch * 6);

		s_Data.vertexArray->SetVertexBuffer(s_Data.vertexBuffer);
		s_Data.vertexArray->SetElementBuffer(s_Data.indexBuffer);

		s_Data.vertexArray->Load(true);

		// Line--------------------------------------------------------------------------------------
		s_Data.lineVertexArray = VertexArray::CreateVertexArray();
		s_Data.lineVertexArray->Bind();

		s_Data.lineVertexBuffer = FloatBuffer::CreateFloatBuffer(1000);
		s_Data.lineVertexBuffer->SetBufferLayout(lineBufferLayout);

		s_Data.lineVertexArray->SetVertexBuffer(s_Data.lineVertexBuffer);

		s_Data.lineVertexArray->Load(false);
		//-------------------------------------------------------------------------------------------

		s_Data.lineShader = Shader::CreateShader("assets/shaders/line_shader.shader");
		s_Data.lineShader->LoadShader(lineBufferLayout);

		s_Data.shader = Shader::CreateShader("assets/shaders/main_shader.shader");
		s_Data.shader->LoadShader(bufferLayout);

		ShaderLib::AddShader("LineShader", s_Data.lineShader);
		ShaderLib::AddShader("MainShader", s_Data.shader);

		batch_storage.push_back(std::make_shared<Renderer2DStorage>());
		batch_storage[0]->textures.push_back(white_texture);

		for (int i = 0; i < max_texture_slots; i++)
			texture_slots[i] = i;
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
			batch_storage[i]->line_index = 0;
			
			batch_storage[i]->vertex_index = 0;

			batch_storage[i]->texture_slot_index = 1;
			batch_storage[i]->index_count = 0;

		}
		batch_index = 0;
		memset(&stats, 0, sizeof(Renderer2D::Renderer2DStatistics));

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
			for (int j = 0; j < batch_storage[i]->textures.size(); j++)
				batch_storage[i]->textures[j]->Bind(j);
			
			s_Data.lineVertexArray->Bind();
			s_Data.lineShader->Bind();
			
			s_Data.lineVertexBuffer->Put(batch_storage[i]->line_verts, batch_storage[i]->line_index * 14);
			s_Data.lineShader->SetMat4("u_ViewProjectionMatrix", s_Data.camera->GetViewProjectionMatrix());
			
			RenderCommand::DrawLines(s_Data.lineVertexArray, batch_storage[i]->line_index * 14);

			s_Data.vertexArray->Bind();
			s_Data.shader->Bind();
			
			s_Data.vertexBuffer->Put(batch_storage[i]->verts, batch_storage[i]->vertex_index * 15);
			
			s_Data.indexBuffer->Put(batch_storage[i]->indices, batch_storage[i]->index_count);
			s_Data.shader->SetIntArray("tex", texture_slots, max_texture_slots);
			
			s_Data.shader->SetMat4("u_ViewProjectionMatrix", s_Data.camera->GetViewProjectionMatrix());
			RenderCommand::DrawIndexed(s_Data.vertexArray, batch_storage[i]->index_count);

			//std::cout << "=================================================================================================================================================\n";
			//for (int j = 0; i < batch_storage[i]->vertex_index; i++)
			//{
			//	for (int k = 0; k < 15; k++)
			//		std::cout << batch_storage[(j * 15) + k] << "\t";
			//
			//	std::cout << "\n";
			//}
			//std::cout << "=================================================================================================================================================\n";

		}
	}

	void Renderer2D::DrawClearTriangle(const Vec3& position, const Vec3& rotation, const Vec2& scale, const Color& color)
	{
		XEN_PROFILE_FN();

		Renderer2D::AddTriangle(position, rotation, scale);

		// Texture Coords
		AddDefaultTextureCoords(Primitive::TRIANGLE);

		JumpDeltaVertexIndex(-3);

		//Color
		AddColorStatic(Primitive::TRIANGLE, color);
		JumpDeltaVertexIndex(-3);

		// Texture ID:
		AddTextureSlot(Primitive::TRIANGLE, true);
	}

	void Renderer2D::DrawClearTriangle(const Vec3& position, const Vec3& rotation, const Vec2& scale, const Color color[4])
	{
		XEN_PROFILE_FN();

		Renderer2D::AddTriangle(position, rotation, scale);

		// Texture Coords
		AddDefaultTextureCoords(Primitive::TRIANGLE);

		JumpDeltaVertexIndex(-3);

		//Color
		AddColorArray(Primitive::TRIANGLE, color);
		JumpDeltaVertexIndex(-3);

		// Texture ID:
		AddTextureSlot(Primitive::TRIANGLE, true);
	}

	void Renderer2D::DrawClearQuad(const Vec3& position, const Vec3& rotation, const Vec2& scale, const Color& color)
	{
		XEN_PROFILE_FN();

		// Deal with Vertices and Indices:
		Renderer2D::AddQuad(position, rotation, scale);

		// Texture Coords
		AddDefaultTextureCoords(Primitive::QUAD);

		JumpDeltaVertexIndex(-4);

		//Color
		AddColorStatic(Primitive::QUAD, color);
		JumpDeltaVertexIndex(-4);

		// Texture ID:
		AddTextureSlot(Primitive::QUAD, true);
	}

	void Renderer2D::DrawClearQuad(const Vec3& position, const Vec3& rotation, const Vec2& scale, const Color color[3])
	{
		XEN_PROFILE_FN();

		Renderer2D::AddQuad(position, rotation, scale);

		// Texture Coords
		AddDefaultTextureCoords(Primitive::QUAD);

		JumpDeltaVertexIndex(-4);

		// Color: 
		AddColorArray(Primitive::QUAD, color);
		JumpDeltaVertexIndex(-4);

		// Texture:
		AddTextureSlot(Primitive::QUAD, true);
	}


	void Renderer2D::DrawTexturedQuad(const Ref<Texture2D>& texture, const Vec3& position, const Vec3& rotation, const Vec2& scale, const Color& tintcolor, float tiling_factor, const float texture_coords[4])
	{
		XEN_PROFILE_FN();

		// Deal with Vertices and Indices:
		Renderer2D::AddQuad(position, rotation, scale);

		// Texture Coords
		if (texture_coords == nullptr)
			AddDefaultTextureCoords(Primitive::QUAD, tiling_factor);

		else {

			for (int i = 0; i < 4; i++)
			{
				batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 7] = texture_coords[i] * tiling_factor;
				batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 8] = texture_coords[i] * tiling_factor;
			}
		}

		JumpDeltaVertexIndex(-4);

		// Color:
		AddColorStatic(Primitive::QUAD, tintcolor);
		JumpDeltaVertexIndex(-4);

		AddTextureSlot(Primitive::QUAD, false, texture);
	}

	void Renderer2D::DrawTexturedQuad(const Ref<Texture2D>& texture, const Vec3& position, const Vec3& rotation, const Vec2& scale, const Color tintcolor[4], float tiling_factor, const float texture_coords[4])
	{
		XEN_PROFILE_FN();

		Renderer2D::AddQuad(position, rotation, scale);

		// Texture Coords
		if(texture_coords == nullptr)
			AddDefaultTextureCoords(Primitive::QUAD, tiling_factor);

		else {
			for (int i = 0; i < 4; i++)
			{
				batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 7] = texture_coords[i] * tiling_factor;
				batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 8] = texture_coords[i] * tiling_factor;
			}
		}

		JumpDeltaVertexIndex(-4);

		//Color:
		AddColorArray(Primitive::QUAD, tintcolor);
		JumpDeltaVertexIndex(-4);

		AddTextureSlot(Primitive::QUAD, false, texture);
	}

	void Renderer2D::DrawClearCircle(const Vec3& position, const Vec3& rotation, const Vec2& scale, const Color& color, float thickness, float innerfade, float outerfade)
	{
		XEN_PROFILE_FN();

		Renderer2D::AddCircleQuad(position, rotation, scale);

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

	void Renderer2D::AddDefaultTextureCoords(Primitive primitive_type, float tiling_factor)
	{
		switch (primitive_type)
		{
		case Xen::Renderer2D::Primitive::TRIANGLE:
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 7] = 0.5f * tiling_factor;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 8] = 1.0f * tiling_factor;

			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 7] = 0.0f;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 8] = 0.0f;

			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 7] = 1.0f;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 8] = 0.0f;

			break;
		case Xen::Renderer2D::Primitive::QUAD:
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 7] = 1.0f * tiling_factor;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 8] = 1.0f * tiling_factor;

			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 7] = 0.0f;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 8] = 1.0f * tiling_factor;

			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 7] = 0.0f;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 8] = 0.0f;

			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 7] = 1.0f * tiling_factor;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 8] = 0.0f;

			break;
		case Xen::Renderer2D::Primitive::POLYGON:
			break;
		default:
			break;
		}
	}

	void Renderer2D::AddColorStatic(Primitive primitive_type, const Color& color)
	{
		uint8_t iteration_count = 0;
		switch (primitive_type)
		{
		case Xen::Renderer2D::Primitive::TRIANGLE:
			iteration_count = 3;
			break;
		case Xen::Renderer2D::Primitive::QUAD:
			iteration_count = 4;
			break;
		case Xen::Renderer2D::Primitive::POLYGON:
			break;
		default:
			break;
		}

		for (int i = 0; i < iteration_count; i++)
		{
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index) * stride_count + 3] = color.r;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index) * stride_count + 4] = color.g;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index) * stride_count + 5] = color.b;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 6] = color.a;
		}
	}

	void Renderer2D::AddColorArray(Primitive primitive_type, const Color* color)
	{
		uint8_t iteration_count = 0;
		switch (primitive_type)
		{
		case Xen::Renderer2D::Primitive::TRIANGLE:
			iteration_count = 3;
			break;
		case Xen::Renderer2D::Primitive::QUAD:
			iteration_count = 4;
			break;
		case Xen::Renderer2D::Primitive::POLYGON:
			break;
		default:
			break;
		}

		for (int i = 0; i < iteration_count; i++)
		{
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 3] = color[i].r;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 4] = color[i].g;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 5] = color[i].b;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 6] = color[i].a;
		}
	}

	void Renderer2D::AddTextureSlot(Primitive primitive_type, bool is_clear_color, const Ref<Texture2D>& texture)
	{
		if (!is_clear_color)
		{
			// Check to see if 'texture' is NOT in the vector
			std::vector<Ref<Texture2D>>::iterator itr = std::find(batch_storage[batch_index]->textures.begin(), batch_storage[batch_index]->textures.end(), texture);

			if (itr == batch_storage[batch_index]->textures.end())
			{
				batch_storage[batch_index]->textures.push_back(texture);
				//stats.texture_count++;

				switch (primitive_type)
				{
				case Xen::Renderer2D::Primitive::TRIANGLE:
					break;
				case Xen::Renderer2D::Primitive::QUAD:
					for (int i = 0; i < 4; i++)
						batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 9] = batch_storage[batch_index]->textures.size() - 1;
					break;
				case Xen::Renderer2D::Primitive::POLYGON:
					break;
				default:
					break;
				}
			}

			else
			{
				switch (primitive_type)
				{
				case Xen::Renderer2D::Primitive::TRIANGLE:
					break;
				case Xen::Renderer2D::Primitive::QUAD:
					for (int i = 0; i < 4; i++)
						batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 9] = (float)std::distance(batch_storage[batch_index]->textures.begin(), itr);
					break;
				case Xen::Renderer2D::Primitive::POLYGON:
					break;
				default:
					break;
				}
			}
		}

		else {
			switch (primitive_type)
			{
			case Xen::Renderer2D::Primitive::TRIANGLE:
				for (int i = 0; i < 3; i++)
					batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 9] = 0.0f;
				break;
			case Xen::Renderer2D::Primitive::QUAD:
				for (int i = 0; i < 4; i++)
					batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 9] = 0.0f;
				break;
			case Xen::Renderer2D::Primitive::POLYGON:
				break;
			default:
				break;
			}
		}
	}

	void Renderer2D::AddQuad(const Vec3& position, const Vec3& rotation, const Vec2& scale)
	{
		XEN_PROFILE_FN();

		if (batch_storage[batch_index]->texture_slot_index >= max_texture_slots || batch_storage[batch_index]->vertex_index > max_vertices_per_batch - 4)
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
			batch_storage[batch_index]->indices[batch_storage[batch_index]->index_count + i] = batch_storage[batch_index]->vertex_index + default_quad_indices[i];

		batch_storage[batch_index]->index_count += 6;

		// Vertices

		if (rotation.x == 0.0f && rotation.y == 0.0f && rotation.z == 0.0f)
		{
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

			JumpDeltaVertexIndex(-4);

			for (int i = 0; i < 4; i++)
				batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 14] = (float)Primitive::QUAD;

			JumpDeltaVertexIndex(-4);

			//----------------

		}

		else {
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), position.GetVec())
				* glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1, 0, 0))
				* glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0, 1, 0))
				* glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0, 0, 1))
				* glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, 1.0f));

			for (int i = 0; i < 4; i++)
			{
				// New code------
				batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 0] = (transform * temp_vert[i]).x;
				batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 1] = (transform * temp_vert[i]).y;
				batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 2] = (transform * temp_vert[i]).z;

				batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 14] = (float)Primitive::QUAD;
				// --------------

			}
			JumpDeltaVertexIndex(-4);
		}
		stats.quad_count++;
	}

	void Renderer2D::AddCircleQuad(const Vec3& position, const Vec3& rotation, const Vec2& scale)
	{
		XEN_PROFILE_FN();

		if (batch_storage[batch_index]->vertex_index > max_vertices_per_batch - 4)
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
			batch_storage[batch_index]->indices[batch_storage[batch_index]->index_count + i] = batch_storage[batch_index]->vertex_index + default_quad_indices[i];

		batch_storage[batch_index]->index_count += 6;

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

		JumpDeltaVertexIndex(-4);

		for (int i = 0; i < 4; i++)
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 14] = (float)Primitive::CIRCLE;

		JumpDeltaVertexIndex(-4);

		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 7] = 1.0f;
		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 8] = 1.0f;

		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 7] = -1.0f;
		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 8] = 1.0f;

		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 7] = -1.0f;
		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 8] = -1.0f;

		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 7] = 1.0f;
		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 8] = -1.0f;

		JumpDeltaVertexIndex(-4);
		// -------------------------------------------------------------

		stats.circle_count++;
	}

	void Renderer2D::AddTriangle(const Vec3& position, const Vec3& rotation, const Vec2& scale)
	{
		XEN_PROFILE_FN();

		if (batch_storage[batch_index]->vertex_index > max_vertices_per_batch - 3)
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

		for (int i = 0; i < 3; i++)
			batch_storage[batch_index]->indices[batch_storage[batch_index]->index_count + i] = batch_storage[batch_index]->vertex_index + i;

		batch_storage[batch_index]->index_count += 3;

		std::vector<int32_t> angles;
		angles.push_back(90);
		angles.push_back(-150);
		angles.push_back(-30);

		for (int32_t angle : angles)
		{
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 0] = position.x + (cos(glm::radians(angle + rotation.z)) * scale.x);
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 1] = position.y + (sin(glm::radians(angle + rotation.z)) * scale.y);
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 2] = position.z;
		}

		JumpDeltaVertexIndex(-3);

		for (int i = 0; i < 3; i++)
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 14] = (float)Primitive::TRIANGLE;

		JumpDeltaVertexIndex(-3);

	}

	void Renderer2D::JumpDeltaVertexIndex(uint32_t index_delta) { batch_storage[batch_index]->vertex_index += index_delta; }
}