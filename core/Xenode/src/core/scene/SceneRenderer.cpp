#include "pch"
#include "SceneRenderer.h"

#include <core/app/GameApplication.h>
#include <core/renderer/FrameBuffer.h>
#include <core/renderer/Renderer2D.h>
#include <core/renderer/DebugRenderer.h>
#include <core/renderer/RenderCommand.h>

#include <core/asset/AssetManagerUtil.h>

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
		float viewportAspectRatio;

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

		sceneRendererState.viewportAspectRatio = static_cast<float>(viewportWidth) / static_cast<float>(viewportHeight);
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

	void SceneRenderer::Update(float timestep, bool isRuntime)
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

		sceneRendererState.viewportAspectRatio = static_cast<float>(width) / static_cast<float>(height);
	}

	const Ref<FrameBuffer>& SceneRenderer::GetActiveFrameBuffer()
	{
		return sceneRendererState.frameBuffer;
	}

	// Private Functions: ------------------------------------------------------------------------------------------------------------------------------

	void SceneRenderer::Update3D(float timestep)
	{
		// 3D Update Logic
	}

	void SceneRenderer::Update2D(float timestep)
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
				quadSprite.texture = AssetManagerUtil::GetAsset<Texture2D>(spriteRenderer.textureHandle);
				
				constexpr bool flipTextureVertically = false;
				if (flipTextureVertically)
					for (int i = 0; i < 4; i++)
						quadSprite.textureCoords[i].y = 1.0f - quadSprite.textureCoords[i].y;

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
	void SceneRenderer::UpdateDebugGraphics(float timestep)
	{
		if (sceneRendererState.debugSettings.showDebugGraphics)
		{
			DebugRenderer::Begin(sceneRendererState.sceneCamera);

			// Updating and Rendering 2D Box Colliders: -----------------------------------------------------------------------------------------------------
			if (sceneRendererState.debugSettings.physicsColliderTargetFlag != DebugRenderTargetFlag::Disabled)
			{
				if ((static_cast<bool>(sceneRendererState.debugSettings.physicsColliderTargetFlag & DebugRenderTargetFlag::Runtime) && sceneRendererState.isRuntime) ||
					(static_cast<bool>(sceneRendererState.debugSettings.physicsColliderTargetFlag & DebugRenderTargetFlag::Editor) && !sceneRendererState.isRuntime))
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
							sceneRendererState.debugSettings.physicsColliderColor, 
							2.0f
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
							{ transform.scale.x * circleCollider2D.radiusScale, transform.scale.y * circleCollider2D.radiusScale },
							sceneRendererState.debugSettings.physicsColliderColor, 
							2.0f
						);
					
					}
				}
			}

			// Render Bounding boxes for display entities: ----------------------------------------------------------------------------------------------------------------
			if (sceneRendererState.debugSettings.displayEntitiesTargetFlag != DebugRenderTargetFlag::Disabled)
			{
				if ((static_cast<bool>(sceneRendererState.debugSettings.displayEntitiesTargetFlag & DebugRenderTargetFlag::Runtime) && sceneRendererState.isRuntime) ||
					(static_cast<bool>(sceneRendererState.debugSettings.displayEntitiesTargetFlag & DebugRenderTargetFlag::Editor) && !sceneRendererState.isRuntime))
				{
					for (Entity entity : sceneRendererState.debugSettings.displayEntities)
					{
						// Check if the entity is valid or not, just to be safe.
						if (!entity.IsNull() && entity.IsValid())
						{
							auto& transform = entity.GetComponent<Component::Transform>();

							// If the entity is a camera entity, multiply the x scale by the aspect ratio of the viewport.
							bool isCameraEntity = entity.HasAnyComponent<Component::CameraComp>();
							Vec2 scale = 
							{
								isCameraEntity ? transform.scale.x * sceneRendererState.viewportAspectRatio : transform.scale.x,
								transform.scale.y
							};

							if (isCameraEntity)
							{
								auto& cameraComp = entity.GetComponent<Component::CameraComp>();
								Ref<Camera> camera = cameraComp.camera;

								// Because the top is 1.0f, bottom is -1.0f, 
								// multiplying with 2 gives the size that fits to the vireport
								if(camera->GetProjectionType() == CameraType::Orthographic)
									scale = scale * 2.0f;
							}

							DebugRenderer::Draw2DQuad(
								transform.position,
								transform.rotation.z,
								scale,
								sceneRendererState.debugSettings.displayEntitiesColor, 
								3.0f
							);
						}
					}
				}
			}

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
					{ 1.0f, 19.0f, 0.0f },
					sceneRendererState.fpsOverlayDisplayColor,
					0.0f,
					0.4f,
					true
				);
			}
			// ---------------------------------------------------------------------------------------------------------------------------------------------------

			DebugRenderer::End();
		}
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