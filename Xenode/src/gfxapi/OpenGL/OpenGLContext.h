#pragma once

#include <core/renderer/GraphicsContext.h>

struct GLFWwindow;

namespace Xen {
	class OpenGLContext : public GraphicsContext
	{
	private:
		GLFWwindow* m_CurrentWindow; 

	public:
		OpenGLContext(GLFWwindow* window) : m_CurrentWindow(window) {}
		virtual ~OpenGLContext() {}

		void Init() override;
		void SwapBuffers() override;

		void DestroyContext() override;
	};
}
