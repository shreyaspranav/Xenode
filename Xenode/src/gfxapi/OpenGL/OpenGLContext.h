#pragma once

#include <glad/gl.h>

#include <core/renderer/GraphicsContext.h>
#include <GLFW/glfw3.h>

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
	};
}
