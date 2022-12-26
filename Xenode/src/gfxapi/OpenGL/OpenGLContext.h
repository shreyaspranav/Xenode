#pragma once

#include <core/renderer/GraphicsContext.h>

struct GLFWwindow;

namespace Xen {
	
	enum class OpenGLVersion
	{
		// OpenGL Versions:
		XEN_OPENGL_API_2_0, // OpenGL 2.0
		XEN_OPENGL_API_3_0, // OpenGL 3.0
		XEN_OPENGL_API_4_3, // OpenGL 4.3
		XEN_OPENGL_API_4_5, // OpenGL 4.5
		XEN_OPENGL_API_4_6, // OpenGL 4.6
	};

	class OpenGLContext : public GraphicsContext
	{
	private:
		GLFWwindow* m_CurrentWindow; 
		inline static OpenGLVersion m_OpenGLVersion;

	public:
		OpenGLContext(GLFWwindow* window) : m_CurrentWindow(window) {}
		virtual ~OpenGLContext() {}

		void Init() override;
		void SwapBuffers() override;

		void DestroyContext() override;

		inline static OpenGLVersion GetOpenGLVersion() { return m_OpenGLVersion; }
	};
}
