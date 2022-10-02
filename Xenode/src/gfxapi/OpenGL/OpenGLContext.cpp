#include "pch"
#include "OpenGLContext.h"
#include "gfxapi/window/glfw/GLFW_window.h"

#include <core/app/Log.h>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#include <GLFW/glfw3.h>

namespace Xen {
	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_CurrentWindow);

		UserPointer p = *(UserPointer*)glfwGetWindowUserPointer(m_CurrentWindow);
		bool vsync = p.props.vsync;
		glfwSwapInterval(vsync);

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

		int version = gladLoadGL(glfwGetProcAddress);
		
		if (!version) { XEN_ENGINE_LOG_ERROR("glad failed to load!"); TRIGGER_BREAKPOINT; }

		XEN_ENGINE_LOG_INFO("Loaded OpenGL {0}.{1}", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

		XEN_ENGINE_LOG_INFO("Vendor: {0}", glGetString(GL_VENDOR));
		XEN_ENGINE_LOG_INFO("Renderer: {0}", glGetString(GL_RENDERER));
		XEN_ENGINE_LOG_INFO("OpenGL: {0}", glGetString(GL_VERSION));
		XEN_ENGINE_LOG_INFO("GLSL: {0}", glGetString(GL_SHADING_LANGUAGE_VERSION));

	}

	void OpenGLContext::DestroyContext()
	{

	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_CurrentWindow);
	}
}