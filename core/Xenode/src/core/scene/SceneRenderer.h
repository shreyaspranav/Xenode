#pragma once
#include <Core.h>

#include <core/renderer/FrameBuffer.h>

namespace Xen {
	class XEN_API SceneRenderer 
	{
	public:
		SceneRenderer();
		~SceneRenderer();
	private:
		Ref<FrameBuffer> m_FrameBuffer;
	};
}