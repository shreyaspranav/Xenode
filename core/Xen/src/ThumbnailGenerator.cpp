#include "ThumbnailGenerator.h"

#include <core/renderer/Shader.h>
#include <core/renderer/BufferObjectBindings.h>
#include <core/asset/AssetManagerUtil.h>

Xen::Ref<Xen::Texture2D> ThumbnailGenerator::GenerateSceneThumbnail(
	const Xen::Ref<Xen::Scene>& scene,
	const Xen::Component::Transform& editorCameraTransform,
	uint32_t thumbnailWidth,
	uint32_t thumbnailHeight)
{
	Xen::Ref<Xen::Texture2D> sceneThumbnail;

	Xen::FrameBufferAttachmentSpec fbAttSpec;

	fbAttSpec.clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	fbAttSpec.filtering = Xen::FrameBufferFiltering::Linear;
	fbAttSpec.format = Xen::FrameBufferTextureFormat::RGB8;
	fbAttSpec.resizable = true;

	Xen::FrameBufferSpec fbSpec;
	fbSpec.width = thumbnailWidth;
	fbSpec.height = thumbnailHeight;
	fbSpec.attachments.push_back(fbAttSpec);

	Xen::Ref<Xen::FrameBuffer> fb = Xen::FrameBuffer::CreateFrameBuffer(fbSpec);

	// TODO: Look into what to be rendered inside thumbnails.
	// For now, render only the sprites for the thumbnails
	auto&& spriteView = scene->m_SceneRegistry.view<Xen::Component::SpriteRenderer>();

	// The 2D Renderer should already be initialized.
	// Xen::Renderer2D::Init();

	// Working with only 2D for now, therefore only Orthographic projection.
	Xen::Ref<Xen::Camera> editorCamera = std::make_shared<Xen::Camera>(Xen::CameraType::Orthographic, thumbnailWidth, thumbnailHeight);

	editorCamera->SetPosition(editorCameraTransform.position);
	editorCamera->SetRotation(editorCameraTransform.rotation);
	editorCamera->SetScale(editorCameraTransform.scale);
	editorCamera->Update();

	Xen::Renderer2D::BeginScene(editorCamera);

	for (auto&& entt : spriteView)
	{
		Xen::Entity entity = Xen::Entity(entt, scene.get());

		Xen::Component::Transform& transform = entity.GetComponent<Xen::Component::Transform>();
		Xen::Component::SpriteRenderer& spriteRenderer = entity.GetComponent<Xen::Component::SpriteRenderer>();

		// Draw* functions:
		if (spriteRenderer.primitive == Xen::Component::SpriteRenderer::Primitive::Quad)
		{
			// Create a QuadSprite Object:
			Xen::Renderer2D::QuadSprite quadSprite;

			// Set the properties of the object
			quadSprite.position = transform.position;
			quadSprite.rotation = transform.rotation.z;
			quadSprite.scale = { transform.scale.x, transform.scale.y };

			// TODO: Also implement per vertex coloring for quad sprites
			quadSprite.useSingleColor = true;
			quadSprite.color[0] = spriteRenderer.color; // Only the first element is checked if useSingleColor = true
			quadSprite.id = entity;
			quadSprite.texture = Xen::AssetManagerUtil::GetAsset<Xen::Texture2D>(spriteRenderer.textureHandle);

			// Add the sprite to the renderer
			Xen::Renderer2D::DrawQuadSprite(quadSprite);
		}

		else if (spriteRenderer.primitive == Xen::Component::SpriteRenderer::Primitive::Circle)
		{
			// Create a CircleSprite Object:
			Xen::Renderer2D::CircleSprite circleSprite;

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
			Xen::Renderer2D::DrawCircleSprite(circleSprite);
		}
	}

	Xen::Renderer2D::EndScene();

	// Resize the viewport:
	Xen::RenderCommand::OnWindowResize(thumbnailWidth, thumbnailHeight);
	Xen::Renderer2D::RenderFrame(0.0f, fb);

	Xen::TextureProperties props;
	props.format = Xen::TextureFormat::RGB8;
	props.width = thumbnailWidth;
	props.height = thumbnailHeight;

	sceneThumbnail = Xen::Texture2D::CopyTexture2D(Xen::Texture2D::CreateTexture2D(fb->GetColorAttachmentRendererID(0), props));
	sceneThumbnail->SetTextureWrapMode(Xen::TextureWrapMode::ClampToBorder);

	return sceneThumbnail;
}

Xen::Ref<Xen::Texture2D> ThumbnailGenerator::GenerateTextureThumbnail(const Xen::Ref<Xen::Texture2D>& texture, uint32_t textureHeight)
{
	Xen::Ref<Xen::Texture2D> textureThumbnail;

	Xen::FrameBufferAttachmentSpec fbAttSpec;

	fbAttSpec.clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	fbAttSpec.filtering = Xen::FrameBufferFiltering::Linear;
	fbAttSpec.format = Xen::FrameBufferTextureFormat::RGB8;
	fbAttSpec.resizable = false;

	float aspectRatio = texture->GetTextureProperties().width / texture->GetTextureProperties().height;

	Xen::FrameBufferSpec fbSpec;
	fbSpec.width = static_cast<uint32_t>(textureHeight * aspectRatio);
	fbSpec.height = textureHeight;
	fbSpec.attachments.push_back(fbAttSpec);

	Xen::Ref<Xen::FrameBuffer> fb = Xen::FrameBuffer::CreateFrameBuffer(fbSpec);

	Xen::RenderCommand::OnWindowResize(fbSpec.width, fbSpec.height);
	Xen::Renderer2D::RenderTextureToFramebuffer(texture, fb);

	Xen::TextureProperties props;
	props.format = Xen::TextureFormat::RGB8;
	props.width = fbSpec.width;
	props.height = fbSpec.height;

	textureThumbnail = Xen::Texture2D::CopyTexture2D(Xen::Texture2D::CreateTexture2D(fb->GetColorAttachmentRendererID(0), props));
	textureThumbnail->SetTextureWrapMode(Xen::TextureWrapMode::ClampToBorder);

	return textureThumbnail;
}
