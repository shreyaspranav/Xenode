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

	float line_width = 1.0f;

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

	/*
	
	TODO: Renderer Plan:

	1. Make a struct a like this:
	struct Vertex
	{
		Vec3 position;
		Color color;
		Vec2 textureWorldCoords;
		
		float P1, P2, P3, P4, P5;
		uint32_t primitiveType;

		// Editor purpose only:
		int32_t _vertexID;
	};

	2. Remove all the legacy opengl code and make the renderer multithreaded
	3. Implement SPIR-V shaders
	4. Add other renderer features:
		a. Particle system + particle physics
		b. Soft shadows
		c. Normal and roughness maps
		d. Text rendering
		e. Overlay rendering(debug/UI)
		f. Compute Shader implementation
		g. Post processing effects:
			1. Bloom
			2. Blur
			3. Color grading
	*/

	struct Renderer2DStorage
	{
		float* verts;
		uint32_t* indices;
		uint32_t vertex_index;
		uint32_t index_count;

		float* light_verts;
		uint32_t* light_indices;
		uint32_t light_vertex_index;
		uint32_t light_index_count;

		float* line_verts;
		uint32_t line_index;

		std::vector<Ref<Texture2D>> textures;
		uint8_t texture_slot_index;

		Renderer2DStorage()
		{
			line_verts = new float[max_lines_per_batch * 7];
			line_index = 0;

			verts = new float[max_vertices_per_batch * stride_count];
			indices = new uint32_t[max_quads_per_batch * 6];
			vertex_index = 0;
			index_count = 0;

			light_verts = new float[max_vertices_per_batch * stride_count];
			light_indices = new uint32_t[max_quads_per_batch * 6];
			light_vertex_index = 0;
			light_index_count = 0;

			texture_slot_index = 1;
		}

		~Renderer2DStorage()
		{
			delete[] line_verts;

			delete[] verts;
			delete[] indices;

			delete[] light_verts;
			delete[] light_indices;
		}
	};

	int texture_slots[8];

	static std::vector<Ref<Renderer2DStorage>> batch_storage;

	Renderer2D::Renderer2DStatistics stats;

	VertexBufferLayout bufferLayout;
	VertexBufferLayout lineBufferLayout;

	void Renderer2D::Init()
	{
		XEN_PROFILE_FN();

		bufferLayout = VertexBufferLayout{
			{ "aPosition", VertexBufferDataType::Float3, 0 },
			{ "aColor", VertexBufferDataType::Float4, 1 },
			{ "aTextureWorldCoords", VertexBufferDataType::Float2, 2 },
			{ "aP1", VertexBufferDataType::Float, 3 },
			{ "aP2", VertexBufferDataType::Float, 4 },
			{ "aP3", VertexBufferDataType::Float, 5 },
			{ "aP4", VertexBufferDataType::Float, 6 },
			{ "aP5", VertexBufferDataType::Float, 7 },
			{ "aPrimitiveType", VertexBufferDataType::Float, 8 }
		};

		lineBufferLayout = {
			{ "aLinePosition", VertexBufferDataType::Float3, 0 },
			{ "aLineColor", VertexBufferDataType::Float4, 1 }
		};

		white_texture =  Texture2D::CreateTexture2D(1, 1, &white_texture_data, sizeof(uint32_t));

		// Main Vertex Buffer, Element buffer and shader:---------------------------------------
		s_Data.vertexBuffer = Xen::VertexBuffer::CreateVertexBuffer(max_vertices_per_batch * stride_count * sizeof(float), bufferLayout);
		s_Data.indexBuffer = Xen::ElementBuffer::CreateElementBuffer(max_quads_per_batch * 6 * sizeof(int), ElementBufferDataType::Unsigned32Bit);
		s_Data.vertexBuffer->SetElementBuffer(s_Data.indexBuffer);

		s_Data.shader = Shader::CreateShader("assets/shaders/main_shader.shader");
		s_Data.shader->LoadShader(bufferLayout);
		// -------------------------------------------------------------------------------------

		// Line Shader and Vertex Buffer: -------------------------------------------------------------------------
		s_Data.lineVertexBuffer = VertexBuffer::CreateVertexBuffer(1000 * sizeof(float), lineBufferLayout);

		s_Data.lineShader = Shader::CreateShader("assets/shaders/line_shader.shader");
		s_Data.lineShader->LoadShader(lineBufferLayout);
		// ---------------------------------------------------------------------------------------------------------

		// Light shader(The Main Vertex and Element Buffer is reused)-----------------------------------------------
		s_Data.lightShader = Shader::CreateShader("assets/shaders/light_shader.shader");
		s_Data.lightShader->LoadShader(bufferLayout);
		// ---------------------------------------------------------------------------------------------------------

		ShaderLib::AddShader("LineShader", s_Data.lineShader);
		ShaderLib::AddShader("MainShader", s_Data.shader);
		ShaderLib::AddShader("LightShader", s_Data.lightShader);

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
			batch_storage[i]->light_vertex_index = 0;

			batch_storage[i]->texture_slot_index = 1;
			batch_storage[i]->index_count = 0;
			batch_storage[i]->light_index_count = 0;
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
			
			s_Data.lineVertexBuffer->Bind();
			s_Data.lineShader->Bind();
			
			s_Data.lineVertexBuffer->Put(batch_storage[i]->line_verts, batch_storage[i]->line_index * 14 * sizeof(float));
			s_Data.lineShader->SetMat4("u_ViewProjectionMatrix", s_Data.camera->GetViewProjectionMatrix());
			
			RenderCommand::DrawLines(s_Data.lineVertexBuffer, batch_storage[i]->line_index * 14);

			//s_Data.vertexArray->Bind();
			s_Data.vertexBuffer->Bind();
			s_Data.shader->Bind();
			
			s_Data.vertexBuffer->Put(batch_storage[i]->verts, batch_storage[i]->vertex_index * stride_count * sizeof(float));
			
			s_Data.indexBuffer->Put(batch_storage[i]->indices, batch_storage[i]->index_count * sizeof(int));
			s_Data.shader->SetIntArray("tex", texture_slots, max_texture_slots);
			
			s_Data.shader->SetMat4("u_ViewProjectionMatrix", s_Data.camera->GetViewProjectionMatrix());
			RenderCommand::DrawIndexed(s_Data.vertexBuffer, batch_storage[i]->index_count);
		}
	}

	void Renderer2D::RenderLights()
	{
		for (int i = 0; i <= batch_index; i++)
		{
			//s_Data.vertexArray->Bind();
			s_Data.vertexBuffer->Bind();
			s_Data.lightShader->Bind();
		
			s_Data.vertexBuffer->Put(batch_storage[i]->light_verts, batch_storage[i]->light_vertex_index * stride_count * sizeof(float));
			s_Data.indexBuffer->Put(batch_storage[i]->light_indices, batch_storage[i]->light_index_count * sizeof(int));
		
			s_Data.lightShader->SetMat4("u_ViewProjectionMatrix", s_Data.camera->GetViewProjectionMatrix());
			RenderCommand::DrawIndexed(s_Data.vertexBuffer, batch_storage[i]->light_index_count);
		}
	}

	void Renderer2D::DrawClearTriangle(const Vec3& position, const Vec3& rotation, const Vec2& scale, const Color& color, int32_t id)
	{
		XEN_PROFILE_FN();

		Renderer2D::AddTriangle(position, rotation, scale, id);

		// Texture Coords
		AddDefaultTextureCoords(Primitive::TRIANGLE);

		JumpDeltaVertexIndex(-3);

		//Color
		AddColorStatic(3, color);
		JumpDeltaVertexIndex(-3);

		// Texture ID:
		AddTextureSlot(3, true);
	}

	void Renderer2D::DrawClearTriangle(const Vec3& position, const Vec3& rotation, const Vec2& scale, const Color color[3], int32_t id)
	{
		XEN_PROFILE_FN();

		Renderer2D::AddTriangle(position, rotation, scale, id);

		// Texture Coords
		AddDefaultTextureCoords(Primitive::TRIANGLE);

		JumpDeltaVertexIndex(-3);

		//Color
		AddColorArray(3, color);
		JumpDeltaVertexIndex(-3);

		// Texture ID:
		AddTextureSlot(3, true);
	}

	void Renderer2D::DrawClearQuad(const Vec3& position, const Vec3& rotation, const Vec2& scale, const Color& color, int32_t id)
	{
		XEN_PROFILE_FN();

		// Deal with Vertices and Indices:
		Renderer2D::AddQuad(position, rotation, scale, id);

		// Texture Coords
		AddDefaultTextureCoords(Primitive::QUAD);

		JumpDeltaVertexIndex(-4);

		//Color
		AddColorStatic(4, color);
		JumpDeltaVertexIndex(-4);

		// Texture ID:
		AddTextureSlot(4, true);
	}

	void Renderer2D::DrawClearQuad(const Vec3& position, const Vec3& rotation, const Vec2& scale, const Color color[4], int32_t id)
	{
		XEN_PROFILE_FN();

		Renderer2D::AddQuad(position, rotation, scale, id);

		// Texture Coords
		AddDefaultTextureCoords(Primitive::QUAD);

		JumpDeltaVertexIndex(-4);

		// Color: 
		AddColorArray(4, color);
		JumpDeltaVertexIndex(-4);

		// Texture:
		AddTextureSlot(4, true);
	}


	void Renderer2D::DrawTexturedQuad(const Ref<Texture2D>& texture, const Vec3& position, const Vec3& rotation, const Vec2& scale, const Color& tintcolor, float tiling_factor, const float texture_coords[4], int32_t id)
	{
		XEN_PROFILE_FN();

		// Deal with Vertices and Indices:
		Renderer2D::AddQuad(position, rotation, scale, id);

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
		AddColorStatic(4, tintcolor);
		JumpDeltaVertexIndex(-4);

		AddTextureSlot(4, false, texture);
	}

	void Renderer2D::DrawTexturedQuad(const Ref<Texture2D>& texture, const Vec3& position, const Vec3& rotation, const Vec2& scale, const Color tintcolor[4], float tiling_factor, const float texture_coords[4], int32_t id)
	{
		XEN_PROFILE_FN();

		Renderer2D::AddQuad(position, rotation, scale, id);

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
		AddColorArray(4, tintcolor);
		JumpDeltaVertexIndex(-4);

		AddTextureSlot(4, false, texture);
	}

	void Renderer2D::DrawPolygon(const Vec3& position, const Vec3& rotation, const Vec2& scale, uint32_t segments, const Color& color, int32_t id)
	{
		XEN_PROFILE_FN();

		Renderer2D::AddPolygon(position, rotation, scale, segments, id);

		AddColorStatic(segments + 1, color);
		JumpDeltaVertexIndex(-(segments + 1));

		AddTextureSlot(segments + 1, true);
	}
	void Renderer2D::DrawPolygon(const Vec3& position, const Vec3& rotation, const Vec2& scale, uint32_t segments, const std::vector<Color>& color, int32_t id)
	{
		XEN_PROFILE_FN();

		Renderer2D::AddPolygon(position, rotation, scale, segments, id);

		AddTextureSlot(segments + 1, true);
	}

	void Renderer2D::DrawClearCircle(const Vec3& position, const Vec3& rotation, const Vec2& scale, const Color& color, float thickness, float innerfade, float outerfade, int32_t id)
	{
		XEN_PROFILE_FN();

		Renderer2D::AddCircleQuad(position, rotation, scale, id);

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

	void Renderer2D::PointLight(const Vec3& position, const Color& color, float radius, float intensity, float fallofA, float fallofB, int32_t id)
	{
		if (batch_storage[batch_index]->light_vertex_index > max_vertices_per_batch - 4)
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

#if 1
		batch_storage[0]->light_vertex_index = 0;
		batch_storage[0]->light_index_count = 0;

		// For Shadows:
		std::vector<Vec3> shadow_src_lines;

		for (int i = 0; i < batch_storage.size(); i++)
		{
			for (int j = 0; j < batch_storage[i]->vertex_index; j++)
			{
				if (batch_storage[batch_index]->verts[(j * stride_count) + 14] == static_cast<float>(Primitive::QUAD))
				{
					float point_distances[4] = {};
					Vec2 vertices[4] = {};

					for (int k = 0; k < 4; k++)
					{
						Vec2 vertex = {
							batch_storage[batch_index]->verts[((j + k) * stride_count) + 0],
							batch_storage[batch_index]->verts[((j + k) * stride_count) + 1]
						};
						vertices[k] = vertex;
						point_distances[k] = sqrtf(powf(vertex.x - position.x, 2) + powf(vertex.y - position.y, 2));
					}
					// Check whether point light lies inside a quad:
					{
						Vec2 AB = vertices[0] - vertices[1];
						Vec2 AD = vertices[2] - vertices[1];
						Vec2 AP = Vec2(position.x, position.y) - vertices[1];

						if ((glm::dot(AP.GetVec(), AB.GetVec()) > 0.0f &&	
							glm::dot(AP.GetVec(), AB.GetVec()) < glm::dot(AB.GetVec(), AB.GetVec())) &&
							(glm::dot(AP.GetVec(), AD.GetVec()) > 0.0f &&
								glm::dot(AP.GetVec(), AD.GetVec()) < glm::dot(AD.GetVec(), AD.GetVec())))
						{
							// No need to render shadow if the light is inside the quad:
							j += 3;
							continue;
						}
					}

					float min_dist = point_distances[0];
					uint32_t min_dist_index = 0;

					for (int k = 0; k < 4; k++)
					{
						if (point_distances[k] > min_dist)
						{
							min_dist = point_distances[k];
							min_dist_index = k;
						}
					}

					uint32_t shadow_indices[3];
					//{
					//	for (int k = 0; k < 4; k++)
					//	{
					//		if(min_dist_index + k < 4)
					//			shadow_indices[k] = min_dist_index + k;
					//		else
					//			shadow_indices[k] = min_dist_index + (k - 4);
					//	}
					//}

					if (min_dist_index == 0 || min_dist_index == 3)
					{
						if (min_dist_index == 0)
						{
							shadow_indices[0] = 3;
							shadow_indices[1] = 0;
							shadow_indices[2] = 1;
						}
						if (min_dist_index == 3)
						{
							shadow_indices[0] = 2;
							shadow_indices[1] = 3;
							shadow_indices[2] = 0;
						}
					}

					else {
						for (int k = 0; k < 3; k++)
							shadow_indices[k] = min_dist_index + (k - 1);
					}

					// The Shadow lines are :vertices[shadow_indices[0]] and vertices[shadow_indices[1]]
					//                       vertices[shadow_indices[1]] and vertices[shadow_indices[2]]

					for (int l = 0; l < 2; l++)
					{
						for (int k = 0; k < 6; k++)
							batch_storage[batch_index]->light_indices[batch_storage[batch_index]->light_index_count + k] = batch_storage[batch_index]->light_vertex_index + default_quad_indices[k] + (4 * l);
						batch_storage[batch_index]->light_index_count += 6;
					}

					for (int k = 0; k < 2; k++)
					{
						batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index  ) * stride_count + 0] = vertices[shadow_indices[k]].x;
						batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index  ) * stride_count + 1] = vertices[shadow_indices[k]].y;
						batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index  ) * stride_count + 2] = position.z;
						batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index++) * stride_count + 12] = 1.0f;

						batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index  ) * stride_count + 0] = vertices[shadow_indices[k + 1]].x;
						batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index  ) * stride_count + 1] = vertices[shadow_indices[k + 1]].y;
						batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index  ) * stride_count + 2] = position.z;
						batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index++) * stride_count + 12] = 1.0f;

						batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index  ) * stride_count + 0] = vertices[shadow_indices[k + 1]].x + 100 * (vertices[shadow_indices[k + 1]].x - position.x);
						batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index  ) * stride_count + 1] = vertices[shadow_indices[k + 1]].y + 100 * (vertices[shadow_indices[k + 1]].y - position.y);
						batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index  ) * stride_count + 2] = position.z;
						batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index++) * stride_count + 12] = 0.0f;

						batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index  ) * stride_count + 0] = vertices[shadow_indices[k]].x + 100 * (vertices[shadow_indices[k]].x - position.x);
						batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index  ) * stride_count + 1] = vertices[shadow_indices[k]].y + 100 * (vertices[shadow_indices[k]].y - position.y);
						batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index  ) * stride_count + 2] = position.z;
						batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index++) * stride_count + 12] = 0.0f;
					}

					batch_storage[batch_index]->light_vertex_index -= 8;

					for(int k = 0; k < 8; k++)
						batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index++) * stride_count + 14] = (float)Primitive::SHADOW_QUAD;

					
#if 0
					for (int i = 0; i < 3; i++)
						XEN_ENGINE_LOG_INFO("Vertices for shadow: ({0}, {1}), ({2}, {3}), ({4}, {5}),", 
							vertices[shadow_indices[0]].x, vertices[shadow_indices[0]].y,
							vertices[shadow_indices[1]].x, vertices[shadow_indices[1]].y,
							vertices[shadow_indices[2]].x, vertices[shadow_indices[2]].y);
#endif
					// Adding 3 so that the loop increased one more time to make up to 4
					j += 3;
					continue;
				}

				else if (batch_storage[batch_index]->verts[(j * stride_count) + 14] == static_cast<float>(Primitive::TRIANGLE))
				{ 
					// Adding 2 so that the loop increased one more time to make up to 3
					j += 2;
					continue;
				}

				else if (batch_storage[batch_index]->verts[(j * stride_count) + 14] == static_cast<float>(Primitive::POLYGON))
					continue;
				else if (batch_storage[batch_index]->verts[(j * stride_count) + 14] == static_cast<float>(Primitive::CIRCLE))
				{
					j += 3;
					continue;
				}
			}
		}
		
		//RenderCommand::SetAdditiveBlendMode(false);
		//glBlendFuncSeparate(GL_ZERO, GL_ONE, GL_ZERO, GL_ZERO);
		//glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);

		RenderCommand::SetBlendMode(
			{ BlendFactor::Zero, BlendFactor::One, BlendOperation::Add },
			{ BlendFactor::Zero, BlendFactor::Zero, BlendOperation::Add }
		);

		for (int i = 0; i <= batch_index; i++)
		{
			s_Data.vertexBuffer->Bind();
			s_Data.lightShader->Bind();

			s_Data.vertexBuffer->Put(batch_storage[i]->light_verts, batch_storage[i]->light_vertex_index * stride_count * sizeof(float));
			s_Data.indexBuffer->Put(batch_storage[i]->light_indices, batch_storage[i]->light_index_count * sizeof(int));

			s_Data.lightShader->SetMat4("u_ViewProjectionMatrix", s_Data.camera->GetViewProjectionMatrix());
			RenderCommand::DrawIndexed(s_Data.vertexBuffer, batch_storage[i]->light_index_count);
		}
#endif

		batch_storage[0]->light_vertex_index = 0;
		batch_storage[0]->light_index_count = 0;

		for (int i = 0; i < 6; i++)
			batch_storage[batch_index]->light_indices[batch_storage[batch_index]->light_index_count + i] = batch_storage[batch_index]->light_vertex_index + default_quad_indices[i];

		batch_storage[batch_index]->light_index_count += 6;



		batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index  ) * stride_count + 0] = position.x + 50.0f * radius;
		batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index  ) * stride_count + 1] = position.y + 50.0f * radius;
		batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index  ) * stride_count + 2] = position.z;
		batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index++) * stride_count + 12] = 1.0f;

		batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index  ) * stride_count + 0] = position.x - 50.0f * radius;
		batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index  ) * stride_count + 1] = position.y + 50.0f * radius;
		batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index  ) * stride_count + 2] = position.z;
		batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index++) * stride_count + 12] = 1.0f;

		batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index  ) * stride_count + 0] = position.x - 50.0f * radius;
		batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index  ) * stride_count + 1] = position.y - 50.0f * radius;
		batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index  ) * stride_count + 2] = position.z;
		batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index++) * stride_count + 12] = 1.0f;


		batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index  ) * stride_count + 0] = position.x + 50.0f * radius;
		batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index  ) * stride_count + 1] = position.y - 50.0f * radius;
		batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index  ) * stride_count + 2] = position.z;
		batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index++) * stride_count + 12] = 1.0f;

		batch_storage[batch_index]->light_vertex_index -= 4;

		batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index  ) * stride_count + 7] = 1.0f;
		batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index++) * stride_count + 8] = 1.0f;

		batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index  ) * stride_count + 7] = -1.0f;
		batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index++) * stride_count + 8] = 1.0f;

		batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index  ) * stride_count + 7] = -1.0f;
		batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index++) * stride_count + 8] = -1.0f;

		batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index  ) * stride_count + 7] = 1.0f;
		batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index++) * stride_count + 8] = -1.0f;

		batch_storage[batch_index]->light_vertex_index -= 4;

		for (int i = 0; i < 4; i++)
		{
			batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index) * stride_count + 3] = color.r;
			batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index) * stride_count + 4] = color.g;
			batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index) * stride_count + 5] = color.b;
			batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index) * stride_count + 6] = color.a;

			batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index) * stride_count + 9] = fallofA;
			batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index) * stride_count + 10] = fallofB;
			batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index) * stride_count + 11] = intensity;

			batch_storage[batch_index]->light_verts[(batch_storage[batch_index]->light_vertex_index++) * stride_count + 14] = static_cast<float>(Primitive::POINT_LIGHT);
		}

		//RenderCommand::SetAdditiveBlendMode(true);

		//glBlendFuncSeparate(GL_DST_ALPHA, GL_ONE, GL_ONE, GL_ZERO);
		//glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
		
		RenderCommand::SetBlendMode(
			{ BlendFactor::DstAlpha, BlendFactor::One, BlendOperation::Add },
			{ BlendFactor::One, BlendFactor::Zero, BlendOperation::Add }
		);

		for (int i = 0; i <= batch_index; i++)
		{
			s_Data.vertexBuffer->Bind();
			s_Data.lightShader->Bind();

			s_Data.vertexBuffer->Put(batch_storage[i]->light_verts, batch_storage[i]->light_vertex_index * stride_count * sizeof(float));
			s_Data.indexBuffer->Put(batch_storage[i]->light_indices, batch_storage[i]->light_index_count * sizeof(int));

			s_Data.lightShader->SetMat4("u_ViewProjectionMatrix", s_Data.camera->GetViewProjectionMatrix());
			RenderCommand::DrawIndexed(s_Data.vertexBuffer, batch_storage[i]->light_index_count);
		}
	}

	void Renderer2D::DrawQuadOutline(const Vec3& position, const Vec3& rotation, const Vec2& scale, const Color& color)
	{
		Vec3 p1, p2, p3, p4;

		if (rotation.x == 0.0f && rotation.y == 0.0f && rotation.z == 0.0f)
		{
			p1 = {
				position.x + (0.5f * scale.x),
				position.y + (0.5f * scale.y),
				position.z
			};

			p2 = {
				position.x - (0.5f * scale.x),
				position.y + (0.5f * scale.y),
				position.z
			};

			p3 = {
				position.x - (0.5f * scale.x),
				position.y - (0.5f * scale.y),
				position.z
			};

			p4 = {
				position.x + (0.5f * scale.x),
				position.y - (0.5f * scale.y),
				position.z
			};
		}

		else {
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), position.GetVec())
				* glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1, 0, 0))
				* glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0, 1, 0))
				* glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0, 0, 1))
				* glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, 1.0f));

			p1 = Vec3(transform * temp_vert[0]);
			p2 = Vec3(transform * temp_vert[1]);
			p3 = Vec3(transform * temp_vert[2]);
			p4 = Vec3(transform * temp_vert[3]);
		}

		Renderer2D::DrawLine(p1, p2, color);
		Renderer2D::DrawLine(p2, p3, color);
		Renderer2D::DrawLine(p3, p4, color);
		Renderer2D::DrawLine(p4, p1, color);
	}

	void Renderer2D::DrawCircleOutline(const Vec3& position, const Color& color, float thickness)
	{
		Renderer2D::DrawClearCircle(position, 0.0f, 1.0f, color, thickness, 0.001f, 0.001f);
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

	void Renderer2D::AddColorStatic(uint8_t vertex_count, const Color& color)
	{
		for (int i = 0; i < vertex_count; i++)
		{
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 3] = color.r;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 4] = color.g;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 5] = color.b;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 6] = color.a;
		}
	}

	void Renderer2D::AddColorArray(uint8_t vertex_count, const Color* color)
	{
		for (int i = 0; i < vertex_count; i++)
		{
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 3] = color[i].r;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 4] = color[i].g;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 5] = color[i].b;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 6] = color[i].a;
		}
	}

	void Renderer2D::AddTextureSlot(uint8_t vertex_count, bool is_clear_color, const Ref<Texture2D>& texture)
	{
		if (!is_clear_color)
		{
			// Check to see if 'texture' is NOT in the vector
			std::vector<Ref<Texture2D>>::iterator itr = std::find(batch_storage[batch_index]->textures.begin(), batch_storage[batch_index]->textures.end(), texture);

			if (itr == batch_storage[batch_index]->textures.end())
			{
				batch_storage[batch_index]->textures.push_back(texture);
				//stats.texture_count++;

				for (int i = 0; i < vertex_count; i++)
					batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 9] = batch_storage[batch_index]->textures.size() - 1;
			}

			else
			{
				for (int i = 0; i < vertex_count; i++)
					batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 9] = (float)std::distance(batch_storage[batch_index]->textures.begin(), itr);
			}
		}

		else {
			for (int i = 0; i < vertex_count; i++)
				batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 9] = 0.0f;
		}
	}

	void Renderer2D::AddID(uint8_t vertex_count, int32_t id)
	{
		for (int i = 0; i < vertex_count; i++)
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 13] = (float)id;
	}

	void Renderer2D::AddQuad(const Vec3& position, const Vec3& rotation, const Vec2& scale, int32_t id)
	{

		//XEN_ENGINE_LOG_ERROR("QUAD_ADDED");
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
			{
				batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 14] = (float)Primitive::QUAD;
				batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 13] = (float)id;
			}

			JumpDeltaVertexIndex(-4);

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
				batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 13] = (float)id;

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

	void Renderer2D::AddCircleQuad(const Vec3& position, const Vec3& rotation, const Vec2& scale, int32_t id)
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

		Renderer2D::AddID(4, id);

		JumpDeltaVertexIndex(-4);
		// -------------------------------------------------------------

		stats.circle_count++;
	}

	void Renderer2D::AddTriangle(const Vec3& position, const Vec3& rotation, const Vec2& scale, int32_t id)
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

		float one_over_root_two = sqrt(2.0) / 2.0;

		for (int32_t angle : angles)
		{
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 0] = position.x + cos(glm::radians(angle + rotation.z)) * one_over_root_two;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 1] = position.y + sin(glm::radians(angle + rotation.z)) * one_over_root_two;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 2] = position.z;
		}

		JumpDeltaVertexIndex(-3);

		glm::mat4 view_mat = glm::mat4(1.0f) *
			glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1, 0, 0)) *
			glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0, 1, 0)) *
			glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, 1.0f));

		glm::vec4 vertex;

		for (int i = 0; i < 3; i++)
		{
			vertex = glm::vec4(
				batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index) * stride_count + 0],
				batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index) * stride_count + 1],
				batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index) * stride_count + 2],
				1.0f
			);

			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 0] = (view_mat * vertex).x;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 1] = (view_mat * vertex).y;
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 2] = (view_mat * vertex).z;
		}

		JumpDeltaVertexIndex(-3);

		for (int i = 0; i < 3; i++)
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 14] = (float)Primitive::TRIANGLE;

		JumpDeltaVertexIndex(-3);

		Renderer2D::AddID(3, id);

		JumpDeltaVertexIndex(-3);

	}

	void Renderer2D::AddPolygon(const Vec3& position, const Vec3& rotation, const Vec2& scale, uint32_t segments, int32_t id)
	{
		if (batch_storage[batch_index]->vertex_index > max_vertices_per_batch - segments)
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

		// Polygon Renderering:
		// No of Vertices: No of sides + 1
		// No of Vertices: No of sides * 3
		// Rendering like a triangle fan but with triangles

		for (int i = 0; i < segments; i++)
		{
			batch_storage[batch_index]->indices[batch_storage[batch_index]->index_count + (i * 3) + 0] = batch_storage[batch_index]->vertex_index;
			batch_storage[batch_index]->indices[batch_storage[batch_index]->index_count + (i * 3) + 1] = batch_storage[batch_index]->vertex_index + i + 1;
			batch_storage[batch_index]->indices[batch_storage[batch_index]->index_count + (i * 3) + 2] = batch_storage[batch_index]->vertex_index + i + 2;
		}

		batch_storage[batch_index]->indices[batch_storage[batch_index]->index_count + ((segments - 1) * 3) + 2] = batch_storage[batch_index]->vertex_index + 1;

		batch_storage[batch_index]->index_count += segments * 3;

		std::vector<float> angles;
		float one_over_root_two = sqrt(2.0) / 2.0;

		for (int i = 1; i <= segments; i++)
			angles.push_back((360.0f / segments) * i);

		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 0] = position.x;
		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 1] = position.y;
		batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 2] = position.z;

		for (float angle : angles)
		{
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index) * stride_count + 0] = position.x + (cos(glm::radians(angle + rotation.z)) * one_over_root_two);
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index) * stride_count + 1] = position.y + (sin(glm::radians(angle + rotation.z)) * one_over_root_two);
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index) * stride_count + 2] = position.z;


			// Temp Tex Coords:
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 7] = position.x + (cos(glm::radians(angle + rotation.z)) * scale.x + 1.0f);
			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index  ) * stride_count + 8] = position.y + (sin(glm::radians(angle + rotation.z)) * scale.y + 1.0f);

			batch_storage[batch_index]->verts[(batch_storage[batch_index]->vertex_index++) * stride_count + 14] = (float)Primitive::POLYGON;
		}

		JumpDeltaVertexIndex(-(segments + 1));

		Renderer2D::AddID(segments + 1, id);

		JumpDeltaVertexIndex(-(segments + 1));
	}

	void Renderer2D::JumpDeltaVertexIndex(uint32_t index_delta) { batch_storage[batch_index]->vertex_index += index_delta; }
}