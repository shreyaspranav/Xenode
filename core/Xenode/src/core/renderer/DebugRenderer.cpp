#include "pch"
#include "DebugRenderer.h"

#include "Shader.h"
#include "Buffer.h"
#include "BufferObjectBindings.h"

#include "RenderCommand.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Texture.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

#ifdef XEN_ENABLE_DEBUG_RENDERER
namespace Xen 
{
	// Global Settings: -----------------------------------------------------------------------------------------------
	
	// The maximum amount of vertices drawn per batch
	constexpr uint32_t maxVerticesPerBatch = 40000;

	// The pixel height of the debug text.
	constexpr uint32_t debugTextLineHeight = 64;

	// The first ASCII code and number of characters to be 
	// included in the font atlas
	constexpr uint32_t firstCharOnAtlas = 32; // Space(' ')
	constexpr uint32_t charCountOnAtlas = 95; // 95 Charecters

	// Size of the font Atlas texture in pixels
	constexpr uint32_t fontAtlasTextureWidth  = 512;
	constexpr uint32_t fontAtlasTextureHeight = 512;
	
	// TODO: Change this after building a asset system
	// UPDATE TODO: The "Common Resources" should be an asset pack and the shader should be loaded from that.
	std::string debugRendererShaderPath = std::string(COMMON_RESOURCES) + "/shaders/DebugRenderer/DebugRenderer.shader";
	std::string fontPath = std::string(COMMON_RESOURCES) + "/fonts/ProggyClean.ttf";

	//-----------------------------------------------------------------------------------------------------------------

	// Struct that represents the type of the primitive rendered.
	enum class Primitive
	{
		Quad	= 0,
		Circle	= 1,

		Text	= 4
	};

	// Struct that represents a "debug camera"'s view projection
	// this "Debug camera" is a orthographic camera that doesn't move, rotate or scale and 
	// it is mapped with the screen coordinates.
	// Debug graphics can also be rendered using the normal scene camera
	struct DebugPerFrameData { glm::mat4 debugViewProjectionMatrix; };

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

		// If non zero, renders from the "debug camera"
		// else renders from the scene camera.
		uint32_t renderFromDebugCamera;
	};

	// Struct that holds information to render a single glyph.
	struct CharRenderData
	{
		// (x0, y0), (x1, y1) are the pixel coordinates of the top left and bottom right 
		// of the glyph's bounding box respectively in the font texture atlas.
		uint32_t x0, y0, x1, y1;

		uint32_t xAdvance;

		int32_t xOffset, yOffset; 
		Vec2 texCoordTopLeft, texCoordBottomRight;
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
		
		Ref<Camera> debugSceneCamera;
		DebugPerFrameData debugCameraData;
		Ref<UniformBuffer> debugCameraBuffer;

		// Debug text font atlas.
		Ref<Texture2D> fontAtlasTexture;
		std::map<char, CharRenderData> fontRenderData;

		uint32_t frameBufferWidth, frameBufferHeight;

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

	static void Draw2DPrimitive(const Vec3& position, float rotation, const Vec2& scale, const Color& color, float thickness, Primitive primitive, bool useScreenCoordinates)
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
			currentBatch[index + i].primitiveType = static_cast<uint32_t>(primitive);
			currentBatch[index + i].renderFromDebugCamera = static_cast<bool>(useScreenCoordinates);
			
			if (primitive == Primitive::Quad || primitive == Primitive::Circle)
			{
				currentBatch[index + i].P1 = thickness;
				currentBatch[index + i].P2 = scale.x;
				currentBatch[index + i].P3 = scale.y;
			}
		}

		debugRendererState.vertexIndex += 4;
	}

	static void SwapVec3(Vec3* a, Vec3* b)
	{
		Vec3 temp = *a;
		*a = *b;
		*b = temp;
	}

	// DebugRenderer Implementation: -----------------------------------------------------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------------------------------------------------------------------------------
	void DebugRenderer::Init(uint32_t width, uint32_t height)
	{
		// Vertex Buffer and Camera setup: ---------------------------------------------------
		debugRendererState.frameBufferWidth = width;
		debugRendererState.frameBufferHeight = height;

		debugRendererState.shader = Shader::CreateShader(debugRendererShaderPath);
		debugRendererState.shader->LoadShader(nullptr);

		VertexBufferLayout bl;
		debugRendererState.storageBuffer = StorageBuffer::CreateStorageBuffer(
			maxVerticesPerBatch * sizeof(Vertex), 
			bl,	// This is not used anyway.
			StorageBufferBinding::DEBUG_RENDERER_SHADER_VERTEX_BUFFERS_DATA);

		Vertex* firstBatch = new Vertex[maxVerticesPerBatch];
		debugRendererState.vertices.push_back(firstBatch);

		// Initialize debug camera:
		debugRendererState.debugSceneCamera = std::make_shared<Camera>(CameraType::Orthographic, width, height, true);
		debugRendererState.debugCameraBuffer = UniformBuffer::CreateUniformBuffer(
			sizeof(DebugPerFrameData), 
			bl, // This is not used anyway.
			UniformBufferBinding::DEBUG_RENDERER_PER_FRAME_DATA);

		// Setting up the debug font -------------------------------------------------------
		std::ifstream inputStream(fontPath.c_str(), std::ios::binary);
		
		inputStream.seekg(0, std::ios::end);
		auto pos = inputStream.tellg();
		inputStream.seekg(0, std::ios::beg);

		// Read the font data:
		uint8_t* fontData = new uint8_t[static_cast<size_t>(pos)];
		inputStream.read((char*)fontData, pos);
		inputStream.close();

		// Allocate the bitmap buffer
		uint8_t* fontAtlas = new uint8_t[fontAtlasTextureWidth * fontAtlasTextureHeight];
		
		// Rendering a font atlas from ascii 32 to ascii 126.
		stbtt_pack_context ctx;
		stbtt_packedchar charRenderData[charCountOnAtlas];

		stbtt_PackBegin(&ctx, (unsigned char*)fontAtlas, fontAtlasTextureWidth, fontAtlasTextureHeight, 0, 1, nullptr);
		stbtt_PackFontRange(&ctx, fontData, 0, debugTextLineHeight, firstCharOnAtlas, charCountOnAtlas, charRenderData);
		stbtt_PackEnd(&ctx);

		for (int i = 0; i < charCountOnAtlas; i++)
		{
			stbtt_aligned_quad alignedQuad;
			CharRenderData data;

			float unusedX = 0.0f, unusedY = 0.0f;
			stbtt_GetPackedQuad(charRenderData, fontAtlasTextureWidth, fontAtlasTextureHeight, i, &unusedX, &unusedY, &alignedQuad, 0);
			
			data.x0 = charRenderData[i].x0;
			data.y0 = charRenderData[i].y0;
			data.x1 = charRenderData[i].x1;
			data.y1 = charRenderData[i].y1;

			data.xOffset = charRenderData[i].xoff;
			data.yOffset = charRenderData[i].yoff;

			data.xAdvance = charRenderData[i].xadvance;

			data.texCoordTopLeft     = { alignedQuad.s0, alignedQuad.t0 };
			data.texCoordBottomRight = { alignedQuad.s1, alignedQuad.t1 };

			debugRendererState.fontRenderData.insert({ static_cast<char>(i + firstCharOnAtlas),  data});
		}

		// Setup the font atlas texture:
		TextureProperties props;
		props.format = TextureFormat::G8;
		props.width = fontAtlasTextureWidth;
		props.height = fontAtlasTextureHeight;
		debugRendererState.fontAtlasTexture = Texture2D::CreateTexture2D(props, fontAtlas, fontAtlasTextureWidth * fontAtlasTextureHeight);

		// Freeing up resources:
		delete[] fontAtlas;
		delete[] fontData;
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
		debugRendererState.debugCameraData.debugViewProjectionMatrix = debugRendererState.debugSceneCamera->GetViewProjectionMatrix();

		debugRendererState.shader->Bind();
		debugRendererState.debugCameraBuffer->Put(0, &debugRendererState.debugCameraData, sizeof(DebugPerFrameData));

		// Bind the texture atlas to texture slot 1
		debugRendererState.fontAtlasTexture->Bind(1);

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

	void DebugRenderer::OnFrameBufferResize(uint32_t width, uint32_t height)
	{
		debugRendererState.frameBufferWidth = width;
		debugRendererState.frameBufferHeight = height;

		debugRendererState.debugSceneCamera->OnViewportResize(width, height);
		debugRendererState.debugSceneCamera->Update();
	}

	void DebugRenderer::Draw2DQuad(const Vec3& position, float rotation, const Vec2& scale, const Color& color, float thickness, bool useScreenCoordinates)
	{
		CheckBatch();
		Draw2DPrimitive(position, rotation, scale, color, thickness, Primitive::Quad, useScreenCoordinates);
	}
	void DebugRenderer::Draw2DCircle(const Vec3& position, float rotation, const Vec2& scale, const Color& color, float thickness, bool useScreenCoordinates)
	{
		CheckBatch();
		Draw2DPrimitive(position, rotation, scale, color, thickness, Primitive::Circle, useScreenCoordinates);
	}

	void DebugRenderer::DrawString(const std::string& text, const Vec3& position, const Color& color, float bOpacity, float size, bool useScreenCoordinates)
	{
		float pixelSize = useScreenCoordinates ? 1.0f : 2.0f / debugRendererState.frameBufferHeight; // Size of one pixel in the "scene camera" coordinate system.
		Vec3 pos = position;
		
		for (char ch : text)
		{
			CheckBatch();
			Vertex* currentBatch = GetCurrentBatch();
			uint32_t index = debugRendererState.vertexIndex;

			if (ch >= firstCharOnAtlas && ch <= firstCharOnAtlas + charCountOnAtlas) // Do we have the cached data to render a glyph?
			{
				CharRenderData renderData = debugRendererState.fontRenderData[ch];
				
				Vec2 glyphSize =
				{
					(renderData.x1 - renderData.x0) * pixelSize * size,
					(renderData.y1 - renderData.y0) * pixelSize * size
				};

				Vec2 glyphBoundingBoxBottomLeft = 
				{ 
					pos.x + renderData.xOffset * pixelSize * size,
					useScreenCoordinates ? 
					pos.y + renderData.yOffset * pixelSize * size :
					pos.y - (renderData.yOffset + renderData.y1 - renderData.y0) * pixelSize * size
				};

				Vec2 glyphTextureCoordinates[4];

				glyphTextureCoordinates[0] = { renderData.texCoordBottomRight.x, renderData.texCoordTopLeft.y };
				glyphTextureCoordinates[1] = renderData.texCoordTopLeft;
				glyphTextureCoordinates[2] = { renderData.texCoordTopLeft.x, renderData.texCoordBottomRight.y };
				glyphTextureCoordinates[3] = renderData.texCoordBottomRight;

				Vec3 glyphVertices[4] =
				{
					{ glyphBoundingBoxBottomLeft.x + glyphSize.x, glyphBoundingBoxBottomLeft.y + glyphSize.y, position.z },
					{ glyphBoundingBoxBottomLeft.x, glyphBoundingBoxBottomLeft.y + glyphSize.y, position.z },
					{ glyphBoundingBoxBottomLeft.x, glyphBoundingBoxBottomLeft.y, position.z },
					{ glyphBoundingBoxBottomLeft.x + glyphSize.x, glyphBoundingBoxBottomLeft.y, position.z }
				};

				if (useScreenCoordinates)
				{
					SwapVec3(&glyphVertices[0], &glyphVertices[3]);
					SwapVec3(&glyphVertices[1], &glyphVertices[2]);
				}

				for (int i = 0; i < 4; i++)
				{
					currentBatch[index + i].position = glyphVertices[i];
					currentBatch[index + i].worldCoords = glyphTextureCoordinates[i];
					currentBatch[index + i].color = color;

					// P1 -> bOpacity
					currentBatch[index + i].P1 = bOpacity;

					currentBatch[index + i].primitiveType = static_cast<uint32_t>(Primitive::Text);
					currentBatch[index + i].renderFromDebugCamera = static_cast<bool>(useScreenCoordinates);
				}

				debugRendererState.vertexIndex += 4;
				pos.x += renderData.xAdvance * pixelSize * size;
			}
		}
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
