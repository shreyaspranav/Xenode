#include "pch"
#include "SceneRenderer.h"

#include <core/app/GameApplication.h>
#include <core/renderer/FrameBuffer.h>
#include <core/renderer/Renderer2D.h>
#include <core/renderer/DebugRenderer.h>
#include <core/renderer/RenderCommand.h>

#include "Components.h"
#include "SceneRuntime.h"


// #define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

namespace Xen 
{

	// Global Settings: ------------------------------------------------------------------------------------------------------------------
	constexpr float fpsUpdateSpeedMS = 500.0f;
	constexpr float fpsLimit = 30.0f;
	// -----------------------------------------------------------------------------------------------------------------------------------

	// A struct that holds the state of the SceneRenderer:
	struct SceneRendererData
	{
		Ref<Scene> currentScene;
		bool isRuntime;

#ifdef XEN_ENABLE_DEBUG_RENDERER
		SceneDebugSettings debugSettings;
#endif
		double timestep;

		// Renderer Specific stuff:
		Ref<FrameBuffer> frameBuffer;
		Ref<Camera> sceneCamera;

		bool renderToGameWindow;
		float elapsed = 0.0f;

		std::stringstream fpsOverlay;
		Color fpsOverlayDisplayColor;
	}sceneRendererState;


	// Implementation: -------------------------------------------------------------------------------------------------------------------------------------
	void SceneRenderer::Initialize(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		GameType gameMode = GetApplicationInstance()->GetGameType();

		// Depending on SceneType, the FrameBufferSpec might be a little different.
		// Only 2D implementation is written for now.
		FrameBufferSpec specifications;
		specifications.width = viewportWidth;
		specifications.height = viewportHeight;

		// Setting up framebuffers:
		{
			// Very simple for now, maybe there will be multiple framebuffers if 2D lighting is implemented.
			// 
			// For now, ther is a single framebuffer that contains 2 attachments:
			//	-> A R11G11B10F color attachment for main rendering.
			//	-> A R32I color attachment for mouse picking.
			// 
			// The R32I layer will be removed in the final build of the game.

			FrameBufferAttachmentSpec mainColorAttachment;
			mainColorAttachment.format = FrameBufferTextureFormat::R11G11B10F; // For HDR Rendering
			mainColorAttachment.filtering = FrameBufferFiltering::Linear;
			mainColorAttachment.clearColor = { 0.1f, 0.1f, 0.1f, 1.0f }; // Slightly grey background color

			specifications.attachments.push_back(mainColorAttachment);

#ifndef XEN_GAME_FINAL_BUILD
			FrameBufferAttachmentSpec mousePickingAttachment;
			mousePickingAttachment.format = FrameBufferTextureFormat::RI;
			mousePickingAttachment.clearColor = { -1.0f, 0.0f, 0.0f, 0.0f };

			specifications.attachments.push_back(mousePickingAttachment);
#endif // !XEN_GAME_FINAL_BUILD
		}

		sceneRendererState.frameBuffer = FrameBuffer::CreateFrameBuffer(specifications);
		
		// Initialize the Debug Renderer First.
		DebugRenderer::Init(viewportWidth, viewportHeight);

		// The 2D Renderer has to be initialized no matter what. Because the sceneType can be either _2D or _2D_AND_3D
		Renderer2D::Init();

		if (gameMode == GameType::_3D)
		{
			// Initialize the 3D Renderer
		}
#if 0
		// Setting up the debug font -------------------------------------------------------
		std::string fontPath = "assets/fonts/Ubuntu.ttf";

		std::ifstream inputStream(fontPath.c_str(), std::ios::binary);

		inputStream.seekg(0, std::ios::end);
		auto pos = inputStream.tellg();
		inputStream.seekg(0, std::ios::beg);

		// Read the font data:
		uint8_t* fontData = new uint8_t[static_cast<size_t>(pos)];
		inputStream.read((char*)fontData, pos);
		inputStream.close();

		// Allocate the bitmap buffer
		const uint32_t bitmapWidth = 1024, bitmapHeight = 1024, lineHeight = 64;
		uint8_t* fontAtlas = new uint8_t[bitmapWidth * bitmapHeight];

		stbtt_fontinfo fontInfo = {};
		if (!stbtt_InitFont(&fontInfo, fontData, 0))
		{
			XEN_ENGINE_LOG_ERROR("stbtt_InitFont(): Failed to initialze font");
			TRIGGER_BREAKPOINT;
		}

		// Rendering a font atlas from ascii 32 to ascii 126.
		stbtt_pack_context ctx;

		stbtt_PackBegin(&ctx, (unsigned char*)fontAtlas, bitmapWidth, bitmapHeight, 0, 1, nullptr);
		stbtt_PackFontRange(&ctx, fontData, 0, lineHeight, 32, 95, charRenderData);
		stbtt_PackEnd(&ctx);

		TextureProperties props;
		props.format = TextureFormat::G8;
		props.width = 1024;
		props.height = 1024;

		atlasTexture = Texture2D::CreateTexture2D(props, fontAtlas, 1024 * 1024);

		// Retrive the font and glyph metrics for each charecter/glyph
		// int32_t ascent, descent, lineGap;
		// struct GlyphMetrics
		// {
		// 	int32_t advanceWidth, leftSideBearing;
		// 
		// 
		// }glyphMetrics[96];
		// 
		// stbtt_GetFontVMetrics(&fontInfo, &ascent, &descent, &lineGap);

#endif

	}

	void SceneRenderer::SetActiveScene(const Ref<Scene>& scene)
	{
		GameType gameMode = GetApplicationInstance()->GetGameType();

		// When the gameMode is _3D, the allowed values of sceneType are _2D_AND_3D and _3D
		// When the gameMode is _2D, the allowed values of sceneType is only _2D
		// It is OK to accept 2D scenes in a 3D game, but won't make sense (just use a 2D project)
		// But it is not OK to accept 3D scenes in a 2D game
		// Don't know if this restriction is good, will see later.
		if (gameMode == GameType::_2D && scene->GetSceneType() == SceneType::_2D_AND_3D)
		{
			// TODO: Do some better error handling:
			XEN_ENGINE_LOG_ERROR("Cannot open a 3D scene in 2D Game Mode!");
			TRIGGER_BREAKPOINT;
		}

		sceneRendererState.currentScene = scene;
	}

	void SceneRenderer::SetActiveCamera(const Ref<Camera>& camera)
	{
		sceneRendererState.sceneCamera = camera;
	}

#ifdef XEN_ENABLE_DEBUG_RENDERER
	void SceneRenderer::SetSceneDebugSettings(const SceneDebugSettings& debugSettings)
	{
		sceneRendererState.debugSettings = debugSettings;
	}
#endif

	void SceneRenderer::Update(double timestep, bool isRuntime)
	{
		sceneRendererState.timestep = timestep;
		sceneRendererState.isRuntime = isRuntime;

		// Update 3D first, then 2D
		if (sceneRendererState.currentScene->GetSceneType() == SceneType::_2D_AND_3D)
			SceneRenderer::Update3D(timestep);

		SceneRenderer::Update2D(timestep);

#ifdef XEN_ENABLE_DEBUG_RENDERER
		SceneRenderer::UpdateDebugGraphics(timestep);
#endif
	}

	void SceneRenderer::Render(bool renderToGameWindow)
	{
		sceneRendererState.renderToGameWindow = renderToGameWindow;

		RenderCommand::Clear();

		// Update 3D first, then 2D
		if (sceneRendererState.currentScene->GetSceneType() == SceneType::_2D_AND_3D)
			SceneRenderer::Render3D();

		SceneRenderer::Render2D();

#ifdef XEN_ENABLE_DEBUG_RENDERER
		SceneRenderer::RenderDebugGraphics();
#endif
	}

	void SceneRenderer::End()
	{
		Renderer2D::EndScene();
	}

	void SceneRenderer::ResizeFrameBuffer(uint32_t width, uint32_t height)
	{
		// TODO: Rename this method to OnViewportResize or something
		RenderCommand::OnWindowResize(width, height);
		sceneRendererState.frameBuffer->Resize(width, height);

		sceneRendererState.sceneCamera->OnViewportResize(width, height);
		DebugRenderer::OnFrameBufferResize(width, height);
	}

	const Ref<FrameBuffer>& SceneRenderer::GetActiveFrameBuffer()
	{
		return sceneRendererState.frameBuffer;
	}

	// Private Functions: ------------------------------------------------------------------------------------------------------------------------------

	void SceneRenderer::Update3D(double timestep)
	{
		// 3D Update Logic
	}

	void SceneRenderer::Update2D(double timestep)
	{
		Renderer2D::BeginScene(sceneRendererState.sceneCamera);

		auto spriteRendererView = sceneRendererState.currentScene->m_SceneRegistry.view<Component::SpriteRenderer>();
		auto particleSystemView = sceneRendererState.currentScene->m_SceneRegistry.view<Component::ParticleSystem2DComp>();

		for (auto entt : spriteRendererView)
		{
			Entity entity = Entity(entt, sceneRendererState.currentScene.get());

			Component::Transform& transform = entity.GetComponent<Component::Transform>();
			Component::SpriteRenderer& spriteRenderer = entity.GetComponent<Component::SpriteRenderer>();

			// Draw* functions:
			if (spriteRenderer.primitive == Component::SpriteRenderer::Primitive::Quad)
			{
				// Create a QuadSprite Object:
				Renderer2D::QuadSprite quadSprite;

				// Set the properties of the object
				quadSprite.position = transform.position;
				quadSprite.rotation = transform.rotation.z;
				quadSprite.scale = { transform.scale.x, transform.scale.y };

				// TODO: Also implement per vertex coloring for quad sprites
				quadSprite.useSingleColor = true;
				quadSprite.color[0] = spriteRenderer.color; // Only the first element is checked if useSingleColor = true
				quadSprite.id = entity;
				quadSprite.texture = spriteRenderer.texture;

				// Add the sprite to the renderer
				Renderer2D::DrawQuadSprite(quadSprite);
			}

			else if (spriteRenderer.primitive == Component::SpriteRenderer::Primitive::Circle)
			{
				// Create a CircleSprite Object:
				Renderer2D::CircleSprite circleSprite;

				// Set the properties of the object
				circleSprite.position = transform.position;
				circleSprite.rotation = transform.rotation.z;
				circleSprite.scale = { transform.scale.x, transform.scale.y };
				circleSprite.color = spriteRenderer.color;

				circleSprite.thickness = spriteRenderer.circle_properties.thickness;
				circleSprite.innerFade = spriteRenderer.circle_properties.innerfade;
				circleSprite.outerFade = spriteRenderer.circle_properties.outerfade;

				circleSprite.id = entity;

				// Add the sprite to the renderer
				Renderer2D::DrawCircleSprite(circleSprite);
			}
		}

		for (auto entt : particleSystemView)
		{
			Entity entity = Entity(entt, sceneRendererState.currentScene.get());
			Component::ParticleSystem2DComp& particleSystem2DComp = entity.GetComponent<Component::ParticleSystem2DComp>();

			Renderer2D::DrawParticles(particleSystem2DComp.particleInstance.particleSettings);
		}

		
#if 0
		// Create a QuadSprite Object:
		Renderer2D::QuadSprite quadSprite;

		// Set the properties of the object
		quadSprite.position = {0.0f, 0.0f, 0.0f};
		quadSprite.rotation = 0.0f;
		quadSprite.scale = { 1.0f, 1.0f };

		quadSprite.useSingleColor = true;
		quadSprite.color[0] = {1.0f, 1.0f, 1.0f, 1.0f};

		// TODO: Also implement per vertex coloring for quad sprites
		quadSprite.id = -1;
		quadSprite.texture = atlasTexture;

		const char* testString = "S hreyas";

		for (int i = 0; i < strlen(testString); i++)
		{
			stbtt_aligned_quad alignedQuad;

			charRenderData[i];

			float x = 0.0f, y = 0.0f;
			stbtt_GetPackedQuad(charRenderData, 1024, 1024, testString[i] - 32, &x, &y, &alignedQuad, 0);

			quadSprite.textureCoords[0] = { alignedQuad.s1, alignedQuad.t0 };
			quadSprite.textureCoords[1] = { alignedQuad.s0, alignedQuad.t0 };
			quadSprite.textureCoords[2] = { alignedQuad.s0, alignedQuad.t1 };
			quadSprite.textureCoords[3] = { alignedQuad.s1, alignedQuad.t1 };
			
			// Add the sprite to the renderer
			Renderer2D::DrawQuadSprite(quadSprite);

			quadSprite.position.x += 1.0f;
		}

#endif
		Renderer2D::EndScene();
	}

	void SceneRenderer::Render3D()
	{
		// 3D Rendering Logic
	}


	void SceneRenderer::Render2D()
	{
		// TODO: Figure out what to do if entities are to be rendered in different framebuffers:
		// Both sets of entities must be updated in the update function and rendered in the render function.
		// Renderer2D::RenderFrame(sceneRendererState.timestep, sceneRendererState.frameBuffer);
		
		// TODO: This is TEMPORARY,
		// Since there is no post processing or any use of HDR, just render to the default framebuffer
		// if running in the runtime.
		
		sceneRendererState.frameBuffer->SetActiveColorAttachments({0, 1});
		Renderer2D::RenderFrame(sceneRendererState.timestep, 
			sceneRendererState.renderToGameWindow ? nullptr : sceneRendererState.frameBuffer);

	}

#ifdef XEN_ENABLE_DEBUG_RENDERER
	void SceneRenderer::UpdateDebugGraphics(double timestep)
	{
		DebugRenderer::Begin(sceneRendererState.sceneCamera);

		DebugRenderer::Draw2DQuad(
			{ 300.0f, 300.0f, 0.0f },
			0.0f,
			{100.0f, 100.0f},
			{1.0f, 1.0f, 1.0f, 1.0f},
			100.0f
		);

		// Updating and Rendering 2D Box Colliders: -----------------------------------------------------------------------------------------------------

		if (sceneRendererState.debugSettings.physicsCollider != DebugRenderTargetFlag::Disabled)
		{
			if ((static_cast<bool>(sceneRendererState.debugSettings.physicsCollider & DebugRenderTargetFlag::Runtime) && sceneRendererState.isRuntime) ||
				(static_cast<bool>(sceneRendererState.debugSettings.physicsCollider & DebugRenderTargetFlag::Editor) && !sceneRendererState.isRuntime))
			{
				auto&& boxCollider2DView = sceneRendererState.currentScene->m_SceneRegistry.view<Component::BoxCollider2D>();
				auto&& circleCollider2DView = sceneRendererState.currentScene->m_SceneRegistry.view<Component::CircleCollider2D>();

				for (auto entt : boxCollider2DView)
				{
					Entity entity = Entity(entt, sceneRendererState.currentScene.get());
					auto& transform = entity.GetComponent<Component::Transform>();

					auto& boxCollider2D = entity.GetComponent<Component::BoxCollider2D>();
					DebugRenderer::Draw2DQuad(
						transform.position + Vec3(boxCollider2D.bodyOffset.x, boxCollider2D.bodyOffset.y, 0.0f),
						transform.rotation.z,
						{ transform.scale.x * boxCollider2D.sizeScale.x, transform.scale.y * boxCollider2D.sizeScale.y },
						sceneRendererState.debugSettings.physicsColliderColor, 2.0f
					);
				}
				for (auto entt : circleCollider2DView)
				{	
					Entity entity = Entity(entt, sceneRendererState.currentScene.get());
					auto& transform = entity.GetComponent<Component::Transform>();

					auto& circleCollider2D = entity.GetComponent<Component::CircleCollider2D>();
					DebugRenderer::Draw2DCircle(
						transform.position + Vec3(circleCollider2D.bodyOffset.x, circleCollider2D.bodyOffset.y, 0.0f),
						transform.rotation.z,
						{ transform.scale.x * circleCollider2D.radiusScale, transform.scale.y },
						sceneRendererState.debugSettings.physicsColliderColor, 2.0f
					);
				
				}
			}
		}
		// ------------------------------------------------------------------------------------------------------------------------------------------------
		// Show FPS: --------------------------------------------------------------------------------------------------------------------------------------

		if (sceneRendererState.debugSettings.showFPSOverlay)
		{
			sceneRendererState.elapsed += timestep;

			if (sceneRendererState.elapsed > fpsUpdateSpeedMS)
			{
				sceneRendererState.fpsOverlay.str("");
				sceneRendererState.fpsOverlay  << "FPS: " << 1000.0f / timestep << " " << "Frame Time: " << timestep << "ms";
				sceneRendererState.elapsed = 0.0f;

				if (1000.0f / timestep < fpsLimit)
					sceneRendererState.fpsOverlayDisplayColor = { 1.0f, 0.0f, 0.0f, 1.0f }; // Display Red
				else
					sceneRendererState.fpsOverlayDisplayColor = { 1.0f, 1.0f, 1.0f, 1.0f }; // Display White
			}

			DebugRenderer::DrawString(
				sceneRendererState.fpsOverlay.str(),
				{ 1.0f, 17.0f, 0.0f },
				sceneRendererState.fpsOverlayDisplayColor,
				0.0f,
				0.4f,
				true
			);
		}
		// ---------------------------------------------------------------------------------------------------------------------------------------------------

		DebugRenderer::End();
	}

	void SceneRenderer::RenderDebugGraphics()
	{
		if(!sceneRendererState.renderToGameWindow)
			sceneRendererState.frameBuffer->Bind();
		
		sceneRendererState.frameBuffer->SetActiveColorAttachments({ 0 });
		DebugRenderer::RenderFrame(sceneRendererState.timestep);
		
		if (!sceneRendererState.renderToGameWindow)
			sceneRendererState.frameBuffer->Unbind();
	}
#endif
}