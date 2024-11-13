#pragma once
#include <Core.h>

#include <core/renderer/Texture.h>
#include <core/renderer/FrameBuffer.h>
#include <core/renderer/Renderer2D.h>

#include <core/scene/Scene.h>
#include <core/scene/Components.h>

class ThumbnailGenerator
{
public:
	static Xen::Ref<Xen::Texture2D> GenerateSceneThumbnail(
		const Xen::Ref<Xen::Scene>& scene,
		const Xen::Component::Transform& editorCameraTransform,
		uint32_t thumbnailWidth,
		uint32_t thumbnailHeight);
};

