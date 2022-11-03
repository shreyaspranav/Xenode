#include "pch"
#include "Renderer2D.h"

#include "core/app/Log.h"

namespace Xen {

	SceneData Renderer2D::s_Data;

	uint32_t max_quads_per_batch = 10000;
	uint8_t max_texture_slots = 32; // TODO: Automate this

	uint32_t default_quad_indices[6] = { 0, 1, 2, 0, 2, 3 };
	uint32_t current_quad_index;

	uint32_t batch_index = 0;

	uint32_t data = 0xffffffff;
	Ref<Texture2D> white_texture;

	uint32_t batches_allocated = 1;

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

		std::vector<Ref<Texture2D>> textures;
		uint8_t texture_slot_index;

		Renderer2DStorage()
		{
			quad_verts = new float[max_quads_per_batch * 40];
			quad_indices = new uint32_t[max_quads_per_batch * 6];

			quad_index = 0;

			texture_slot_index = 1;
		}

		~Renderer2DStorage()
		{
			delete[] quad_verts;
			delete[] quad_indices;
		}
	};

	int texture_slots[32];

	static std::vector<Ref<Renderer2DStorage>> batch_storage;

	Renderer2D::Renderer2DStatistics stats;

	void Renderer2D::Init()
	{
		white_texture =  Texture2D::CreateTexture2D(1, 1, &data, sizeof(uint32_t));

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

		s_Data.vertexBuffer = Xen::FloatBuffer::CreateFloatBuffer(max_quads_per_batch * 40);
		s_Data.vertexBuffer->SetBufferLayout(bufferLayout);
		s_Data.indexBuffer = Xen::ElementBuffer::CreateElementBuffer(max_quads_per_batch * 6);

		s_Data.vertexArray->SetVertexBuffer(s_Data.vertexBuffer);
		s_Data.vertexArray->SetElementBuffer(s_Data.indexBuffer);

		s_Data.vertexArray->Load();

		s_Data.vertexArray->Bind();
		s_Data.shader->Bind();

		batch_storage.push_back(std::make_shared<Renderer2DStorage>());
		batch_storage[0]->textures.push_back(white_texture);

		for (int i = 0; i < max_texture_slots; i++)
			texture_slots[i] = i;

		stats.vertex_buffer_size = s_Data.vertexBuffer->GetSize();
		stats.index_buffer_size = s_Data.indexBuffer->GetSize();

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

		for (int i = 0; i < batch_index + 1; i++)
		{
			batch_storage[i]->quad_index = 0;
			batch_storage[i]->texture_slot_index = 1;
		}
		batch_index = 0;
		memset(&stats, 0, sizeof(Renderer2D::Renderer2DStatistics));

		stats.vertex_buffer_size = s_Data.vertexBuffer->GetSize();
		stats.index_buffer_size = s_Data.indexBuffer->GetSize();
	}

	void Renderer2D::EndScene()
	{
		//for (int i = 0; i < storage->texture_slot_index; i++)
		//	storage->textures[i]->Bind(i);
		//
		//s_Data.vertexBuffer->Put(storage->quad_verts, storage->quad_index * 40);
		//
		//if (current_quad_index != storage->quad_index)
		//	s_Data.indexBuffer->Put(storage->quad_indices, storage->quad_index * 6);
		//
		//current_quad_index = storage->quad_index;
	}
	void Renderer2D::RenderFrame()
	{
		stats.draw_calls = 0;

		for (int i = 0; i < max_texture_slots; i++)
			texture_slots[i] = i;

		s_Data.shader->SetMat4("u_ViewProjectionMatrix", s_Data.camera->GetViewProjectionMatrix());
		s_Data.shader->SetIntArray("tex", texture_slots, max_texture_slots);

		for (int i = 0; i <= batch_index; i++)
		{
			for (int j = 0; j < batch_storage[i]->texture_slot_index; j++)
				batch_storage[i]->textures[j]->Bind(j);

			s_Data.vertexBuffer->Put(batch_storage[i]->quad_verts, batch_storage[i]->quad_index * 40);

			if (current_quad_index != batch_storage[i]->quad_index)
				s_Data.indexBuffer->Put(batch_storage[i]->quad_indices, batch_storage[i]->quad_index * 6);

			current_quad_index = batch_storage[i]->quad_index;

			RenderCommand::DrawIndexed(s_Data.vertexArray, (batch_storage[i]->quad_index) * 6);

			stats.draw_calls++;
		}

	}

	void Renderer2D::DrawClearQuad(const Vec3& position, float rotation, const Vec2& scale, const Color& color)
	{
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
		
		// Color, Z coordinate, and texture ID:
		for (int i = 0; i < 40; i += 10)
		{
			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 2)] = position.z;

			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 3)] = color.r;
			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 4)] = color.g;
			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 5)] = color.b;
			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 6)] = color.a;

			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 9)] = 0.0f;
		}

		batch_storage[batch_index]->quad_index++;
	}

	void Renderer2D::DrawClearQuad(const Vec3& position, float rotation, const Vec2& scale, const Color* color)
	{
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

		// Color, Z coordinate, and texture ID:

		if (color == nullptr)
		{
			for (int i = 0; i < 40; i += 10)
			{
				batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 2)] = position.z;

				batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 3)] = 1.0f;
				batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 4)] = 1.0f;
				batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 5)] = 1.0f;
				batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 6)] = 1.0f;

				batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 9)] = 0.0f;
			}
		}
		else
		{
			for (int i = 0; i < 40; i += 10)
			{
				batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 2)] = position.z;

				batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 3)] = color[i / 10].r;
				batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 4)] = color[i / 10].g;
				batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 5)] = color[i / 10].b;
				batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 6)] = color[i / 10].a;

				batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 9)] = 0.0f;
			}
		}
		

		batch_storage[batch_index]->quad_index++;
	}


	void Renderer2D::DrawTexturedQuad(const Ref<Texture2D>& texture, const Vec3& position, float rotation, const Vec2& scale, const Color& tintcolor, float tiling_factor)
	{
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


		for (int i = 0; i < 40; i += 10)
		{
			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 2)] = position.z;

			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 3)] = tintcolor.r;
			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 4)] = tintcolor.g;
			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 5)] = tintcolor.b;
			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 6)] = tintcolor.a;

			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 9)] = (float)batch_storage[batch_index]->texture_slot_index;
		}
		//texture->Bind(batch_storage[batch_index]->texture_slot_index);

		// Check to see if 'texture' is NOT in the vector
		if (std::find(batch_storage[batch_index]->textures.begin(), batch_storage[batch_index]->textures.end(), texture) == batch_storage[batch_index]->textures.end())
		{
			batch_storage[batch_index]->textures.push_back(texture);
			stats.texture_count++;
			batch_storage[batch_index]->texture_slot_index++;
		}
		batch_storage[batch_index]->quad_index++;
	}

	void Renderer2D::DrawTexturedQuad(const Ref<Texture2D>& texture, const float* tex_coords, const Vec3& position, float rotation, const Vec2& scale, const Color& tintcolor, float tiling_factor)
	{
		// Deal with Vertices and Indices:
		Renderer2D::AddQuad(position, rotation, scale);

		// Texture Coodinates
		batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 7] = tex_coords[0] * tiling_factor;
		batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 8] = tex_coords[1] * tiling_factor;

		batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 17] = tex_coords[2] * tiling_factor;
		batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 18] = tex_coords[3] * tiling_factor;

		batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 27] = tex_coords[4] * tiling_factor;
		batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 28] = tex_coords[5] * tiling_factor;

		batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 37] = tex_coords[6] * tiling_factor;
		batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 38] = tex_coords[7] * tiling_factor;


		for (int i = 0; i < 40; i += 10)
		{
			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 2)] = position.z;

			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 3)] = tintcolor.r;
			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 4)] = tintcolor.g;
			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 5)] = tintcolor.b;
			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 6)] = tintcolor.a;

			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i + 9)] = (float)batch_storage[batch_index]->texture_slot_index;
		}

		if (std::find(batch_storage[batch_index]->textures.begin(), batch_storage[batch_index]->textures.end(), texture) == batch_storage[batch_index]->textures.end())
		{
			batch_storage[batch_index]->textures.push_back(texture);
			batch_storage[batch_index]->texture_slot_index++;
			batch_storage[batch_index]->texture_slot_index++;
		}

		batch_storage[batch_index]->quad_index++;
	}

	Renderer2D::Renderer2DStatistics& Renderer2D::GetStatistics()
	{
		return stats;
	}

	// Private methods

	void Renderer2D::AddQuad(const Vec3& position, float rotation, const Vec2& scale)
	{
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
			batch_storage[batch_index]->quad_indices[(batch_storage[batch_index]->quad_index * 6) + i] = (batch_storage[batch_index]->quad_index * 4) + default_quad_indices[i];

		// Vertices

		if (rotation == 0.0f)
		{
			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 0] = position.x + (0.5f * scale.x);
			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 1] = position.y + (0.5f * scale.y);

			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 10] = position.x - (0.5f * scale.x);
			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 11] = position.y + (0.5f * scale.y);

			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 20] = position.x - (0.5f * scale.x);
			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 21] = position.y - (0.5f * scale.y);

			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 30] = position.x + (0.5f * scale.x);
			batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + 31] = position.y - (0.5f * scale.y);
		}

		else {
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), position.GetVec())
				* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0, 0, 1))
				* glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, 1.0f));


			for (int i = 0; i < 4; i++)
			{
				batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i * 10) + 0] = (transform * temp_vert[i]).x;
				batch_storage[batch_index]->quad_verts[(batch_storage[batch_index]->quad_index * 40) + (i * 10) + 1] = (transform * temp_vert[i]).y;
			}
		}
		stats.quad_count++;
	}
}