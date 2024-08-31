#include "pch"
#include "DebugRenderer.h"

#include "Shader.h"
#include "Buffer.h"
#include "BufferObjectBindings.h"

#include "RenderCommand.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#ifdef XEN_ENABLE_DEBUG_RENDERER
namespace Xen 
{
	// Global Settings: -----------------------------------------------------------------------------------------------
	
	// The maximum amount of vertices drawn per batch
	constexpr uint32_t maxVerticesPerBatch = 40000;
	
	// TODO: Change this after building a asset system
	std::string debugRendererShaderPath = "assets/shaders/DebugRenderer.shader";

	//-----------------------------------------------------------------------------------------------------------------

	// Struct that represents the type of the primitive rendered.
	enum class Primitive
	{
		Quad	= 0,
		Circle	= 1,

		Text	= 4
	};

	// Struct that represents a Vertex.
	struct Vertex
	{
		Vec3 position;
		Color color;
		Vec2 worldCoords;

		uint32_t primitiveType;

		// Each of the floats mean different parameters for each of the primitive types
		// 
		// For a Quad: 
		//		P1 -> Thickness
		//		P2 -> Scale X
		//		P3 -> Scale Y
		// 
		// For a Circle:
		//		P1 -> Thickness
		// 		P2 -> Scale X
		// 		P3 -> Scale Y
		//
		// For Text:
		//		P1 -> <unused>
		// 		P2 -> <unused>
		// 		P3 -> <unused>
		// 
		// In case of expanding a feature or something, extra parameters can be useful
		float P1, P2, P3;
	};

	// Struct that owns the data that belongs to the Debug Renderer
	struct DebugRendererData
	{
		// This is an variable sized array of a fixed size array.
		// The index of the inner array specify the batch number and the index of the outer array specify the vertex.
		Vector<Vertex*> vertices;
		uint32_t batchIndex = 0, vertexIndex = 0, vertexCount = 0;

		Ref<Shader> shader;
		Ref<StorageBuffer> storageBuffer;

		void Finalize()
		{
			for (Vertex* vertex : vertices)
				delete[] vertex;
		}

		~DebugRendererData() { Finalize(); }

	}debugRendererState;

	// Private Functions: ----------------------------------------------------------------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------------------------------------------------------------------------------
	
	// Checks if the current batch is full, if full, adds a new batch
	static void CheckBatch()
	{
		if (debugRendererState.vertexIndex >= maxVerticesPerBatch)
		{
			// Allocate a new batch.
			Vertex* newBatch = new Vertex[maxVerticesPerBatch];
			debugRendererState.vertices.push_back(newBatch);

			debugRendererState.batchIndex++;
			debugRendererState.vertexIndex = 0;
		}
	}

	// Returns the current batch.
	static Vertex* GetCurrentBatch()
	{
		return debugRendererState.vertices[debugRendererState.batchIndex];
	}

	// Applies Translation, Rotation and Scale to an array of vertices.
	static void ApplyTRS(Vec3* srcVertices, uint32_t srcVertexCount, const Vec3& position, const Vec3& rotation, const Vec3& scale)
	{
		glm::mat4 transformMat = glm::mat4(1.0f);

		// Translate
		transformMat = glm::translate(transformMat, position.GetVec());
		
		// Rotate
		transformMat = glm::rotate(transformMat, glm::radians(rotation.x), { 1, 0, 0 });
		transformMat = glm::rotate(transformMat, glm::radians(rotation.y), { 0, 1, 0 });
		transformMat = glm::rotate(transformMat, glm::radians(rotation.z), { 0, 0, 1 });
		
		// Scale
		transformMat = glm::scale(transformMat, scale.GetVec());

		for (int i = 0; i < srcVertexCount; i++)
		{
			auto&& vertex = srcVertices[i].GetVec();
			vertex = transformMat * glm::vec4(vertex, 1.0f);
			srcVertices[i] = { vertex.x, vertex.y, vertex.z };
		}
	}

	// 
	static void Draw2DPrimitive(const Vec3& position, float rotation, const Vec2& scale, const Color& color, float thickness, Primitive primitive)
	{
		Vertex* currentBatch = GetCurrentBatch();
		uint32_t index = debugRendererState.vertexIndex;

		Vec3 defaultVertices[4] = 
		{
			{  0.5f,  0.5f,  0.0f },
			{ -0.5f,  0.5f,  0.0f },
			{ -0.5f, -0.5f,  0.0f },
			{  0.5f, -0.5f,  0.0f },
		};

		Vec2 defaultWorldCoords[4] =
		{
			{  scale.x,  scale.y },
			{ -scale.x,  scale.y },
			{ -scale.x, -scale.y },
			{  scale.x, -scale.y },
		};

		ApplyTRS(defaultVertices, 4, position, { 0.0f, 0.0f, rotation }, { scale.x, scale.y, 1.0f });

		for (int i = 0; i < 4; i++)
		{
			currentBatch[index + i].position = defaultVertices[i];
			currentBatch[index + i].worldCoords = defaultWorldCoords[i];
			currentBatch[index + i].color = color;
			currentBatch[index + i].primitiveType = (uint32_t)primitive;
			
			if (primitive == Primitive::Quad || primitive == Primitive::Circle)
			{
				currentBatch[index + i].P1 = thickness;
				currentBatch[index + i].P2 = scale.x;
				currentBatch[index + i].P3 = scale.y;
			}
		}

		debugRendererState.vertexIndex += 4;
	}

	// DebugRenderer Implementation: -----------------------------------------------------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------------------------------------------------------------------------------
	void DebugRenderer::Init() 
	{
		debugRendererState.shader = Shader::CreateShader(debugRendererShaderPath);
		debugRendererState.shader->LoadShader(nullptr);

		VertexBufferLayout bl;
		debugRendererState.storageBuffer = StorageBuffer::CreateStorageBuffer(
			maxVerticesPerBatch * sizeof(Vertex), 
			bl,	// This is not used anyway.
			StorageBufferBinding::DEBUG_RENDERER_SHADER_VERTEX_BUFFERS_DATA);

		Vertex* firstBatch = new Vertex[maxVerticesPerBatch];
		debugRendererState.vertices.push_back(firstBatch);
	}
	void DebugRenderer::Begin(const Ref<Camera>& camera) 
	{
		debugRendererState.vertexIndex = 0;
		debugRendererState.batchIndex = 0;
	}
	
	void DebugRenderer::End() 
	{

	}

	void DebugRenderer::RenderFrame(double timestep)
	{
		debugRendererState.shader->Bind();
		for (int i = 0; i < debugRendererState.vertices.size(); i++)
		{
			Vertex* batch = debugRendererState.vertices[i];
			Size lastBatchIndex = debugRendererState.vertices.size() - 1;
			
			debugRendererState.storageBuffer->Put(0, batch, sizeof(Vertex) * maxVerticesPerBatch);
			
			// If the current "Rendering" batch is not the last batch, you render "maxVerticesPerBatch" no of vertices, 
			// if not, you render the "vertexIndex" no. of vertices
			RenderCommand::DrawNonIndexed(PrimitiveType::Triangles, nullptr, 
				i != lastBatchIndex ? maxVerticesPerBatch : (debugRendererState.vertexIndex / 4) * 6);
		}
	}

	void DebugRenderer::Draw2DQuad(const Vec3& position, float rotation, const Vec2& scale, const Color& color, float thickness) 
	{
		CheckBatch();
		Draw2DPrimitive(position, rotation, scale, color, thickness, Primitive::Quad);
	}
	void DebugRenderer::Draw2DCircle(const Vec3& position, float rotation, const Vec2& scale, const Color& color, float thickness) 
	{
		CheckBatch();
		Draw2DPrimitive(position, rotation, scale, color, thickness, Primitive::Circle);
	}

	void DebugRenderer::DrawString(const std::string& text, const Vec3& position, float bOpacity, float size) 
	{
		CheckBatch();
		// Will be right back!
	}
	
}
#else
namespace Xen 
{
	void DebugRenderer::Init() {}
	void DebugRenderer::Begin(const Ref<Camera>& camera) {}
	void DebugRenderer::End() {}
	void DebugRenderer::Draw2DQuad(const Vec3& position, float rotation, const Vec2& scale, const Color& color, float thickness) {}
	void DebugRenderer::Draw2DCircle(const Vec3& position, float rotation, const Vec2& scale, const Color& color, float thickness) {}
	void DebugRenderer::DrawString(const std::string& text, const Vec3& position, float bOpacity, float size) {}
}
#endif
