#include "pch"
#include "Renderer2D.h"

#include "core/app/Log.h"

namespace Xen {

	SceneData Renderer2D::s_Data;

	uint32_t max_quads = 1000;
	uint8_t max_texture_slots = 32; // TODO: Automate this

	uint32_t default_quad_indices[6] = { 0, 1, 2, 0, 2, 3 };
	uint32_t current_quad_index;

	struct Renderer2DStorage
	{
		float* quad_verts;
		uint32_t* quad_indices;
		uint32_t quad_index;

		int* texture_slots;
		std::vector<Ref<Texture2D>> textures;

		uint8_t texture_slot_index;

		Ref<Texture2D> white_texture;

		Renderer2DStorage()
		{
			uint32_t data = 0xffffffff;
			white_texture = Texture2D::CreateTexture2D(1, 1, &data, sizeof(uint32_t));

			quad_verts = new float[max_quads * 40];
			quad_indices = new uint32_t[max_quads * 6];

			quad_index = 0;

			texture_slots = new int[max_texture_slots];
		}

		~Renderer2DStorage()
		{
			//delete[] quads;

			delete[] quad_verts;
			delete[] quad_indices;
			delete[] texture_slots;
		}
	};

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

		s_Data.vertexBuffer = Xen::FloatBuffer::CreateFloatBuffer(max_quads * 40);
		s_Data.vertexBuffer->SetBufferLayout(bufferLayout);
		s_Data.indexBuffer = Xen::ElementBuffer::CreateElementBuffer(max_quads * 6);

		s_Data.vertexArray->SetVertexBuffer(s_Data.vertexBuffer);
		s_Data.vertexArray->SetElementBuffer(s_Data.indexBuffer);

		s_Data.vertexArray->Load();

		s_Data.vertexArray->Bind();
		s_Data.shader->Bind();

		storage->textures.push_back(storage->white_texture);

		//storage->textures[0]->Bind(0); //Bind White Texture to 0th slot

		// Set all Texture Slots to 0
		for (int i = 0; i < max_texture_slots; i++)
			storage->texture_slots[i] = i;


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
		storage->texture_slot_index = 1;
	}

	void Renderer2D::EndScene()
	{
		for (int i = 0; i < storage->textures.size(); i++)
			storage->textures[i]->Bind(i);

		s_Data.vertexBuffer->Put(storage->quad_verts, storage->quad_index * 40);

		if (current_quad_index != storage->quad_index)
			s_Data.indexBuffer->Put(storage->quad_indices, storage->quad_index * 6);

		current_quad_index = storage->quad_index;
	}
	void Renderer2D::RenderFrame()
	{
		s_Data.shader->SetMat4("u_ViewProjectionMatrix", s_Data.camera->GetViewProjectionMatrix());
		s_Data.shader->SetIntArray("tex", storage->texture_slots, max_texture_slots);

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
	void Renderer2D::DrawClearQuad(const Vec3& position, float rotation, const Vec2& scale, const Color& color)
	{
		// Deal with Vertices and Indices:
		Renderer2D::AddQuad(position, rotation, scale);

		// Texture Coodinates
		storage->quad_verts[(storage->quad_index * 40) + 7] = 1.0f;
		storage->quad_verts[(storage->quad_index * 40) + 8] = 1.0f;

		storage->quad_verts[(storage->quad_index * 40) + 17] = 0.0f;
		storage->quad_verts[(storage->quad_index * 40) + 18] = 1.0f;

		storage->quad_verts[(storage->quad_index * 40) + 27] = 0.0f;
		storage->quad_verts[(storage->quad_index * 40) + 28] = 0.0f;

		storage->quad_verts[(storage->quad_index * 40) + 37] = 1.0f;
		storage->quad_verts[(storage->quad_index * 40) + 38] = 0.0f;
		
		// Color, Z coordinate, and texture ID:
		for (int i = 0; i < 40; i += 10)
		{
			storage->quad_verts[(storage->quad_index * 40) + (i + 2)] = position.z;

			storage->quad_verts[(storage->quad_index * 40) + (i + 3)] = color.r;
			storage->quad_verts[(storage->quad_index * 40) + (i + 4)] = color.g;
			storage->quad_verts[(storage->quad_index * 40) + (i + 5)] = color.b;
			storage->quad_verts[(storage->quad_index * 40) + (i + 6)] = color.a;

			storage->quad_verts[(storage->quad_index * 40) + (i + 9)] = 0.0f;
		}

		storage->quad_index++;
	}

	void Renderer2D::DrawClearQuad(const Vec3& position, float rotation, const Vec2& scale, const Color* color)
	{
		Renderer2D::AddQuad(position, rotation, scale);

		// Texture Coodinates
		storage->quad_verts[(storage->quad_index * 40) + 7] = 1.0f;
		storage->quad_verts[(storage->quad_index * 40) + 8] = 1.0f;

		storage->quad_verts[(storage->quad_index * 40) + 17] = 0.0f;
		storage->quad_verts[(storage->quad_index * 40) + 18] = 1.0f;

		storage->quad_verts[(storage->quad_index * 40) + 27] = 0.0f;
		storage->quad_verts[(storage->quad_index * 40) + 28] = 0.0f;

		storage->quad_verts[(storage->quad_index * 40) + 37] = 1.0f;
		storage->quad_verts[(storage->quad_index * 40) + 38] = 0.0f;

		// Color, Z coordinate, and texture ID:

		if (color == nullptr)
		{
			for (int i = 0; i < 40; i += 10)
			{
				storage->quad_verts[(storage->quad_index * 40) + (i + 2)] = position.z;

				storage->quad_verts[(storage->quad_index * 40) + (i + 3)] = 1.0f;
				storage->quad_verts[(storage->quad_index * 40) + (i + 4)] = 1.0f;
				storage->quad_verts[(storage->quad_index * 40) + (i + 5)] = 1.0f;
				storage->quad_verts[(storage->quad_index * 40) + (i + 6)] = 1.0f;

				storage->quad_verts[(storage->quad_index * 40) + (i + 9)] = 0.0f;
			}
		}
		else
		{
			for (int i = 0; i < 40; i += 10)
			{
				storage->quad_verts[(storage->quad_index * 40) + (i + 2)] = position.z;

				storage->quad_verts[(storage->quad_index * 40) + (i + 3)] = color[i / 10].r;
				storage->quad_verts[(storage->quad_index * 40) + (i + 4)] = color[i / 10].g;
				storage->quad_verts[(storage->quad_index * 40) + (i + 5)] = color[i / 10].b;
				storage->quad_verts[(storage->quad_index * 40) + (i + 6)] = color[i / 10].a;

				storage->quad_verts[(storage->quad_index * 40) + (i + 9)] = 0.0f;
			}
		}
		

		storage->quad_index++;
	}


	void Renderer2D::DrawTexturedQuad(const Ref<Texture2D>& texture, const Vec3& position, float rotation, const Vec2& scale, const Color& tintcolor, float tiling_factor)
	{
		// Deal with Vertices and Indices:
		Renderer2D::AddQuad(position, rotation, scale);

		// Texture Coodinates
		storage->quad_verts[(storage->quad_index * 40) + 7] = 1.0f * tiling_factor;
		storage->quad_verts[(storage->quad_index * 40) + 8] = 1.0f * tiling_factor;

		storage->quad_verts[(storage->quad_index * 40) + 17] = 0.0f;
		storage->quad_verts[(storage->quad_index * 40) + 18] = 1.0f * tiling_factor;

		storage->quad_verts[(storage->quad_index * 40) + 27] = 0.0f;
		storage->quad_verts[(storage->quad_index * 40) + 28] = 0.0f;

		storage->quad_verts[(storage->quad_index * 40) + 37] = 1.0f * tiling_factor;
		storage->quad_verts[(storage->quad_index * 40) + 38] = 0.0f;

		// Check to see if 'texture' is already in the vector
		if (std::find(storage->textures.begin(), storage->textures.end(), texture) == storage->textures.end()) 
			storage->textures.push_back(texture);

		for (int i = 0; i < 40; i += 10)
		{
			storage->quad_verts[(storage->quad_index * 40) + (i + 2)] = position.z;

			storage->quad_verts[(storage->quad_index * 40) + (i + 3)] = tintcolor.r;
			storage->quad_verts[(storage->quad_index * 40) + (i + 4)] = tintcolor.g;
			storage->quad_verts[(storage->quad_index * 40) + (i + 5)] = tintcolor.b;
			storage->quad_verts[(storage->quad_index * 40) + (i + 6)] = tintcolor.a;

			storage->quad_verts[(storage->quad_index * 40) + (i + 9)] = (float)storage->texture_slot_index;
		}
		//texture->Bind(storage->texture_slot_index);
		storage->texture_slot_index++;

		storage->quad_index++;
	}

	void Renderer2D::DrawTexturedQuad(const Ref<Texture2D>& texture, const float* tex_coords, const Vec3& position, float rotation, const Vec2& scale, const Color& tintcolor, float tiling_factor)
	{
		// Deal with Vertices and Indices:
		Renderer2D::AddQuad(position, rotation, scale);

		// Texture Coodinates
		storage->quad_verts[(storage->quad_index * 40) + 7] = tex_coords[0] * tiling_factor;
		storage->quad_verts[(storage->quad_index * 40) + 8] = tex_coords[1] * tiling_factor;

		storage->quad_verts[(storage->quad_index * 40) + 17] = tex_coords[2] * tiling_factor;
		storage->quad_verts[(storage->quad_index * 40) + 18] = tex_coords[3] * tiling_factor;

		storage->quad_verts[(storage->quad_index * 40) + 27] = tex_coords[4] * tiling_factor;
		storage->quad_verts[(storage->quad_index * 40) + 28] = tex_coords[5] * tiling_factor;

		storage->quad_verts[(storage->quad_index * 40) + 37] = tex_coords[6] * tiling_factor;
		storage->quad_verts[(storage->quad_index * 40) + 38] = tex_coords[7] * tiling_factor;

		if (std::find(storage->textures.begin(), storage->textures.end(), texture) == storage->textures.end())
		{
			storage->textures.push_back(texture);
			storage->texture_slot_index++;
		}

		for (int i = 0; i < 40; i += 10)
		{
			storage->quad_verts[(storage->quad_index * 40) + (i + 2)] = position.z;

			storage->quad_verts[(storage->quad_index * 40) + (i + 3)] = tintcolor.r;
			storage->quad_verts[(storage->quad_index * 40) + (i + 4)] = tintcolor.g;
			storage->quad_verts[(storage->quad_index * 40) + (i + 5)] = tintcolor.b;
			storage->quad_verts[(storage->quad_index * 40) + (i + 6)] = tintcolor.a;

			storage->quad_verts[(storage->quad_index * 40) + (i + 9)] = (float)storage->texture_slot_index;
		}
		storage->texture_slot_index++;

		storage->quad_index++;
	}

	// Private methods

	void Renderer2D::AddQuad(const Vec3& position, float rotation, const Vec2& scale)
	{
		for (int i = 0; i < 6; i++)
			storage->quad_indices[(storage->quad_index * 6) + i] = (storage->quad_index * 4) + default_quad_indices[i];

		// Vertices
		storage->quad_verts[(storage->quad_index * 40) + 0] = position.x + (0.5f * scale.x);
		storage->quad_verts[(storage->quad_index * 40) + 1] = position.y + (0.5f * scale.y);

		storage->quad_verts[(storage->quad_index * 40) + 10] = position.x - (0.5f * scale.x);
		storage->quad_verts[(storage->quad_index * 40) + 11] = position.y + (0.5f * scale.y);

		storage->quad_verts[(storage->quad_index * 40) + 20] = position.x - (0.5f * scale.x);
		storage->quad_verts[(storage->quad_index * 40) + 21] = position.y - (0.5f * scale.y);

		storage->quad_verts[(storage->quad_index * 40) + 30] = position.x + (0.5f * scale.x);
		storage->quad_verts[(storage->quad_index * 40) + 31] = position.y - (0.5f * scale.y);
	}
}