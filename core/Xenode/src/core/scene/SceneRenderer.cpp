#include "pch"
#include "SceneRenderer.h"

#include <core/app/DesktopApplication.h>
#include <core/renderer/FrameBuffer.h>
#include <core/renderer/Renderer2D.h>
#include <core/renderer/RenderCommand.h>

#include "Components.h"

namespace Xen {

	// A struct that holds the state of the SceneRenderer:
	struct SceneRendererData
	{
		Ref<Scene> currentScene;
		double timestep;

		// Renderer Specific stuff:
		Ref<FrameBuffer> frameBuffer;
		Ref<Camera> sceneCamera;

	}sceneRendererState;


	// Implementation: -------------------------------------------------------------------------------------------------------------------------------------
	void SceneRenderer::Initialize(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		GameMode gameMode = DesktopApplication::GetGameMode();

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

		// The 2D Renderer has to be initialized no matter what. Because the sceneType can be either _2D or _2D_AND_3D
		Renderer2D::Init();

		if (gameMode == GameMode::_3D)
		{
			// Initialize the 3D Renderer
		}
	}

	void SceneRenderer::SetActiveScene(const Ref<Scene>& scene)
	{
		GameMode gameMode = DesktopApplication::GetGameMode();

		// When the gameMode is _3D, the allowed values of sceneType are _2D_AND_3D and _3D
		// When the gameMode is _2D, the allowed values of sceneType is only _2D
		// It is OK to accept 2D scenes in a 3D game, but won't make sense (just use a 2D project)
		// But it is not OK to accept 3D scenes in a 2D game
		// Don't know if this restriction is good, will see later.
		if (gameMode == GameMode::_2D && scene->GetSceneType() == SceneType::_2D_AND_3D)
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

	void SceneRenderer::Update(double timestep)
	{
		sceneRendererState.timestep = timestep;

		// Update 3D first, then 2D
		if (sceneRendererState.currentScene->GetSceneType() == SceneType::_2D_AND_3D)
			SceneRenderer::Update3D(timestep);

		SceneRenderer::Update2D(timestep);
	}

	void SceneRenderer::Render()
	{
		// Update 3D first, then 2D
		if (sceneRendererState.currentScene->GetSceneType() == SceneType::_2D_AND_3D)
			SceneRenderer::Render3D();

		SceneRenderer::Render2D();
	}

	void SceneRenderer::End()
	{
		Renderer2D::EndScene();
	}

	void SceneRenderer::ResizeFrameBuffer(uint32_t width, uint32_t height)
	{
		RenderCommand::OnWindowResize(width, height);
		sceneRendererState.frameBuffer->Resize(width, height);

		sceneRendererState.sceneCamera->OnViewportResize(width, height);
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

		Renderer2D::EndScene();
	}

	void SceneRenderer::Render3D()
	{
		// 3D Rendering Logic
	}

	void SceneRenderer::Render2D()
	{
		RenderCommand::Clear();

		// TODO: Figure out what to do if entities are to be rendered in different framebuffers:
		// Both sets of entities must be updated in the update function and rendered in the render function.
		Renderer2D::RenderFrame(sceneRendererState.timestep, sceneRendererState.frameBuffer);
	}
}