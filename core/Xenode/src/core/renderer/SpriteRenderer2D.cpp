#include "pch"
#include "SpriteRenderer2D.h"
#include "Renderer2D.h"

#include "Buffer.h"
#include "Shader.h"
#include "Camera.h"
#include "RenderCommand.h"
#include "BufferObjectBindings.h"

#include <glm/ext/matrix_transform.hpp>

namespace Xen 
{
	// Global Settings: ---------------------------------------------------------------------------------------------------------------
	
	// The maximum amount of vertices drawn per batch:
	constexpr uint32_t maxVerticesPerBatch = 40000;

	// The maximum amount of textures that can be bound at a time:
	constexpr uint8_t maxTextureSlots = 32;

	// The main shader path that is used to render all the sprites:
	// TODO: Refactor the code when the engine uses an actual asset system
	// UPDATE TODO: The "Common Resources" should be an asset pack and the shader should be loaded from that.
	const std::string mainShaderPath = std::string(COMMON_RESOURCES) + "/shaders/Renderer2D/SpriteRenderer2D.shader";

	// --------------------------------------------------------------------------------------------------------------------------------

	// Default vertices. Final vertices are rendered by transforming these vertices:
	glm::vec4 defaultVertices[] = 
	{
		glm::vec4( 0.5f,  0.5f,  0.0f,  1.0f),
		glm::vec4(-0.5f,  0.5f,  0.0f,  1.0f),
		glm::vec4(-0.5f, -0.5f,  0.0f,  1.0f),
		glm::vec4( 0.5f, -0.5f,  0.0f,  1.0f),
	};

	// Enum for identifying primitives in the storage buffer
	enum class Primitive 
	{
		QUAD		= 1 << 2,
		CIRCLE		= 1 << 4,
	};

	// Struct for identifying a vertex in the storage buffer
	struct Vertex
	{
		Vec3 position;
		Color color;

		// Can represent texture coordinates for the quad sprites 
		// as well as world coordinates for the circle sprites
		Vec2 textureWorldCoords;

		// Represents the primitive type of the sprite mentioned in the Primitive enum
		uint32_t primitiveType;

		// Each of the floats mean different parameters for each of the primitive types
		// 
		// For a Quad: 
		//		P1 -> Texture Slot
		//		P2 -> <unused>
		//		P3 -> <unused>
		//		P4 -> <unused>
		//		P5 -> <unused>
		// 
		// For a Circle:
		//		P1 -> Thickness
		// 		P2 -> Inner Fade
		// 		P3 -> Outer Fade
		// 		P4 -> <unused>
		// 		P5 -> <unused>
		// 
		// In case of expanding a feature or something, extra parameters can be useful
		float P1, P2, P3, P4, P5;

		// Editor Purpose only: _vertexID is used to render the 
		// integer framebuffer that is used for mouse picking for now
		int32_t _vertexID;
	};

	// Struct to hold the cameraBuffer Uniform buffer
	struct CameraData { glm::mat4 viewProjectionMat = glm::mat4(1.0f); };

	// Struct that holds the vertices and other data of sprites of one batch.
	// If there are more vertices than the allocated memory space for vertices then
	// a new Batch instance will be created.
	struct SpriteBatch
	{
		Vertex* vertices;
		uint32_t vertexIndex = 0, indexCount = 0;
		
		std::vector<Ref<Texture2D>> textures;
		uint8_t textureSlotIndex = 0;

		SpriteBatch()	{ vertices = new Vertex[maxVerticesPerBatch]; }
		~SpriteBatch()	{ delete[] vertices; }
	};

	// Struct that holds the data that is owned by the 2D Sprite Renderer
	struct SpriteRenderer2DStorage
	{
		// Batches of sprites are stored in here:
		std::vector<Ref<SpriteBatch>> batchStorage;
		uint32_t batchIndex = 0;
		uint32_t batchCount = 0;

		// Buffers, Shaders and whatnot:
		Ref<Camera> camera;
		CameraData cameraData;
		Ref<UniformBuffer> cameraBuffer;
		Ref<StorageBuffer> vertexStorageBuffer;
		Ref<Shader> shader;

		// White texture to be added to the textures vector every time a new batch is created
		Ref<Texture2D> whiteTexture;
	}spriteRendererStorage;


	// Private Functions: ------------------------------------------------------------------------------------------------------------------------------------------------------
	// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	// Functions that are related to querying batch storage and creating a new SpriteBatch --------------------------------------------------------------

	// Returns the current SpriteBatch that is being filled
	inline static const Ref<SpriteBatch>& GetCurrentBatch()
	{
		return spriteRendererStorage.batchStorage[spriteRendererStorage.batchIndex];
	}

	// Returns the SpriteBatch stored in a specific index in the batch_storage
	inline static const Ref<SpriteBatch>& GetBatchByIndex(uint32_t index)
	{
		return spriteRendererStorage.batchStorage[index];
	}

	// Returns true if the current batch's vertices are full or current batch has 'maxTextureSlots' amount of distinct textures 
	inline static bool IsCurrentBatchFull()
	{
		auto& currentBatch = GetCurrentBatch();
		return (currentBatch->vertexIndex >= maxVerticesPerBatch || currentBatch->textures.size() >= maxTextureSlots);
	}

	// Return true there is need to push back a new SpriteBatch to the batch storage
	inline static bool IsNewBatchNeeded()
	{
		return (spriteRendererStorage.batchIndex >= spriteRendererStorage.batchCount);
	}

	// Pushes a new instance of SpriteBatch to batch storage
	inline static void CreateNewBatch()
	{
		spriteRendererStorage.batchStorage.push_back(std::make_shared<SpriteBatch>());
		spriteRendererStorage.batchCount++;

		if(spriteRendererStorage.batchIndex)
			spriteRendererStorage.batchIndex++;
	}

	// ----------------------------------------------------------------------------------------------------------------------------------------------
	// Functions to modify properties of a specific batch -------------------------------------------------------------------------------------------

	static int32_t GetTextureID(const Ref<SpriteBatch>& batch, Ref<Texture2D> texture)
	{
		auto&& it = std::find(batch->textures.begin(), batch->textures.end(), texture);

		if (it != batch->textures.end())
			return (int32_t)(it - batch->textures.begin());
		else
			return -1;
	}

	// Pushes a new Texture Object to the requires batch
	static void PushNewTexture(const Ref<SpriteBatch>& batch, Ref<Texture2D> texture)
	{
		if (batch->textures.size() >= maxTextureSlots)
		{
			XEN_ENGINE_LOG_ERROR("The number of textures in this batch exceeded than the number of texture slots");
			TRIGGER_BREAKPOINT;
		}

		batch->textures.push_back(texture);
	}

	// Function to add vertex positions to the current batch
	static void AddSpriteVertexPositions(const Vec3& position, float rotation, const Vec2& scale)
	{
		auto&& currentBatch = GetCurrentBatch();

		// Create a transform matrix to transform the default vertices to the vertices of specified location, rotation and scale
		glm::mat4 transformMat;

		// Perform transformations in the order: TRS(Translation, Rotation Scale)
		transformMat = glm::translate(glm::mat4(1.0f), position.GetVec());
		transformMat = glm::rotate(transformMat, glm::radians(rotation), glm::vec3(0, 0, 1));
		transformMat = glm::scale(transformMat, glm::vec3(scale.x, scale.y, 1.0f));

		for (auto& vec : defaultVertices)
		{
			// Remember the order!
			auto&& position = transformMat * vec;
			currentBatch->vertices[currentBatch->vertexIndex++].position = { position.x, position.y, position.z };
		}

		// Reset the index after updating
		currentBatch->vertexIndex -= 4;
	}

	// Function to add vertex colors to the current batch
	static void AddSpriteColor(const Color& color)
	{
		auto&& currentBatch = GetCurrentBatch();

		for (int i = 0; i < 4; i++)
			currentBatch->vertices[currentBatch->vertexIndex++].color = color;

		currentBatch->vertexIndex -= 4;
	}

	static void AddSpriteColors(const Color color[4])
	{
		auto&& currentBatch = GetCurrentBatch();

		for (int i = 0; i < 4; i++)
			currentBatch->vertices[currentBatch->vertexIndex++].color = color[i];

		currentBatch->vertexIndex -= 4;
	}

	// Function to add texture coordinates for quads and world coordinates for circles
	static void AddSpriteTextureWorldCoords(const Vec2 textureWorldCoords[4])
	{
		auto&& currentBatch = GetCurrentBatch();

		for (int i = 0; i < 4; i++)
			currentBatch->vertices[currentBatch->vertexIndex++].textureWorldCoords = textureWorldCoords[i];

		currentBatch->vertexIndex -= 4;
	}

	// Function to add the PrimitiveType for each Sprite
	static void AddSpritePrimitiveType(Primitive primitive)
	{
		auto&& currentBatch = GetCurrentBatch();

		for (int i = 0; i < 4; i++)
			currentBatch->vertices[currentBatch->vertexIndex++].primitiveType = (uint32_t)primitive;

		currentBatch->vertexIndex -= 4;
	}

	// Function to add parameters P1, P2, P3, P4, P5f
	static void AddSpriteParameters(float P1, float P2, float P3, float P4, float P5)
	{
		auto&& currentBatch = GetCurrentBatch();

		for (int i = 0; i < 4; i++)
		{
			currentBatch->vertices[currentBatch->vertexIndex].P1 = P1;
			currentBatch->vertices[currentBatch->vertexIndex].P2 = P2;
			currentBatch->vertices[currentBatch->vertexIndex].P3 = P3;
			currentBatch->vertices[currentBatch->vertexIndex].P4 = P4;
			currentBatch->vertices[currentBatch->vertexIndex].P5 = P5;

			currentBatch->vertexIndex++;
		}

		currentBatch->vertexIndex -= 4;
	}

	// Function to Add the vertex ID for each sprite
	static void AddSpriteVertexID(int32_t vertexID)
	{
		auto&& currentBatch = GetCurrentBatch();

		for (int i = 0; i < 4; i++)
			currentBatch->vertices[currentBatch->vertexIndex++]._vertexID = vertexID;

		currentBatch->vertexIndex -= 4;
	}

	// SpriteRenderer2D Implementation: ---------------------------------------------------------------------- -----------------------------------------------------------------
	// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	void SpriteRenderer2D::Init()
	{
		// Loading the shader: 
		spriteRendererStorage.shader = Shader::CreateShader(mainShaderPath);
		spriteRendererStorage.shader->LoadShader(nullptr);

		// Creating and Allocating memory in the Storage Buffer (VertexBufferLayout is not used by the implementation):
		VertexBufferLayout l;
		spriteRendererStorage.vertexStorageBuffer = StorageBuffer::CreateStorageBuffer(maxVerticesPerBatch * sizeof(Vertex), l, StorageBufferBinding::MAIN_SHADER_VERTEX_BUFFER_DATA);

		
		// Creating a white texture that is required to be added as the first texture to every batch
		TextureProperties whiteTextureProperties;
		whiteTextureProperties.width = 1;
		whiteTextureProperties.height = 1;
		whiteTextureProperties.format = TextureFormat::RGBA8;

		uint32_t whiteTextureData = 0xffffffff;
		spriteRendererStorage.whiteTexture = Texture2D::CreateTexture2D(whiteTextureProperties, &whiteTextureData, sizeof(uint32_t));

		// Create a new batch and add a new texture to that batch
		CreateNewBatch();
		PushNewTexture(GetCurrentBatch(), spriteRendererStorage.whiteTexture);

		// Create the cameraData uniform buffer:
		spriteRendererStorage.cameraBuffer = UniformBuffer::CreateUniformBuffer(sizeof(CameraData), l, UniformBufferBinding::MAIN_SHADER_PER_FRAME_DATA);

		// Push integers to the texture sampler object in the shader:
		int samplerInts[32];
		for (int i = 0; i < 32; i++)
			samplerInts[i] = i;

		spriteRendererStorage.shader->SetIntArray("tex", samplerInts, 32);
	}

	void SpriteRenderer2D::BeginScene(const Ref<Camera>& camera)
	{
		// Setting up the camera Uniform Buffer:
		spriteRendererStorage.camera = camera;
		spriteRendererStorage.cameraData.viewProjectionMat = camera->GetViewProjectionMatrix();

		spriteRendererStorage.cameraBuffer->Put(0, &spriteRendererStorage.cameraData, sizeof(CameraData));

		// Set all indexes to 0
		spriteRendererStorage.batchIndex = 0;

		for (int i = 0; i < spriteRendererStorage.batchCount; i++)
		{
			auto& currentBatch = GetBatchByIndex(i);

			currentBatch->vertexIndex = 0;
			currentBatch->indexCount = 0;
			currentBatch->textureSlotIndex = 0;
		}
	}

	void SpriteRenderer2D::EndScene()
	{
		// Nothing For Now!
	}

	void SpriteRenderer2D::RenderFrame(float timestep)
	{
		// Rendering Sprites:
		spriteRendererStorage.shader->Bind();
		spriteRendererStorage.cameraBuffer->Put(0, &spriteRendererStorage.cameraData, sizeof(CameraData));

		for (auto& batch : spriteRendererStorage.batchStorage)
		{
			for (int i = 0; i < batch->textures.size(); i++)
				batch->textures[i]->Bind(i);

			spriteRendererStorage.vertexStorageBuffer->Put(0, batch->vertices, batch->vertexIndex * sizeof(Vertex));
			RenderCommand::DrawNonIndexed(PrimitiveType::Triangles, nullptr, batch->indexCount);
		}
	}

	// Draw Functions Implementation: (Functions from Renderer2D.h) -------------------------------------------------------------------------
	// List of functions implemented: 
	//		Renderer2D::DrawQuadSprite(const QuadSprite& quadSprite) 
	//		Renderer2D::DrawCircleSprite(const CircleSprite& circleSprite)
	// --------------------------------------------------------------------------------------------------------------------------------------
	void Renderer2D::DrawQuadSprite(const QuadSprite& quadSprite)
	{
		AddSpriteVertexPositions(quadSprite.position, quadSprite.rotation, quadSprite.scale);

		if (quadSprite.useSingleColor)
			AddSpriteColor(quadSprite.color[0]);
		else
			AddSpriteColors(quadSprite.color);

		AddSpriteTextureWorldCoords(quadSprite.textureCoords);
		AddSpritePrimitiveType(Xen::Primitive::QUAD);

		float texSlot;
		if (!quadSprite.texture)
			texSlot = 0.0f;
		else
		{
			int32_t textureID = GetTextureID(GetCurrentBatch(), quadSprite.texture);

			if (textureID == -1)
			{
				texSlot = GetCurrentBatch()->textures.size();
				PushNewTexture(GetCurrentBatch(), quadSprite.texture);
			}
			else
				texSlot = (float)textureID;
		}

		AddSpriteParameters(texSlot, 0.0f, 0.0f, 0.0f, 0.0f);
		AddSpriteVertexID(quadSprite.id);

		GetCurrentBatch()->vertexIndex += 4;
		GetCurrentBatch()->indexCount += 6;
	}

	void Renderer2D::DrawCircleSprite(const CircleSprite& circleSprite)
	{
		AddSpriteVertexPositions(circleSprite.position, circleSprite.rotation, circleSprite.scale);
		AddSpriteColor(circleSprite.color);

		AddSpriteTextureWorldCoords(circleSprite.worldCoords);
		AddSpritePrimitiveType(Xen::Primitive::CIRCLE);

		AddSpriteParameters(circleSprite.thickness, circleSprite.innerFade, circleSprite.outerFade, 0.0f, 0.0f);
		AddSpriteVertexID(circleSprite.id);

		GetCurrentBatch()->vertexIndex += 4;
		GetCurrentBatch()->indexCount += 6;
	}

	// ----------------------------------------------------------------------------------------------

	// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------
}