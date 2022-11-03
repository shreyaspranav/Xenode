#include "pch"
#include "OpenGLContext.h"
#include "gfxapi/window/glfw/GLFW_window.h"

#include <core/app/Log.h>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#include <GLFW/glfw3.h>

namespace Xen {

	void DebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
	{
		XEN_ENGINE_LOG_WARN("OpenGL Debug Output: [{0}]: {1} ", id, message);

		switch (source)
		{
			case GL_DEBUG_SOURCE_API:                XEN_ENGINE_LOG_WARN("Source: OpenGL API"); break;
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM:      XEN_ENGINE_LOG_WARN("Source: Window System"); break;
			case GL_DEBUG_SOURCE_SHADER_COMPILER:    XEN_ENGINE_LOG_WARN("Source: OpenGL Shader Compiler"); break;
			case GL_DEBUG_SOURCE_THIRD_PARTY:        XEN_ENGINE_LOG_WARN("Source: Third Party Application"); break;
			case GL_DEBUG_SOURCE_APPLICATION:        XEN_ENGINE_LOG_WARN("Source: Current Application"); break;
			case GL_DEBUG_SOURCE_OTHER:              XEN_ENGINE_LOG_WARN("Source: Other"); break;
		}

		switch (type)
		{
			case GL_DEBUG_TYPE_ERROR:                XEN_ENGINE_LOG_WARN("Type: Error"); break;
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:  XEN_ENGINE_LOG_WARN("Type: Deprecated Behaviour"); break;
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:   XEN_ENGINE_LOG_WARN("Type: Undefined Behaviour"); break;
			case GL_DEBUG_TYPE_PORTABILITY:          XEN_ENGINE_LOG_WARN("Type: Portability"); break;
			case GL_DEBUG_TYPE_PERFORMANCE:          XEN_ENGINE_LOG_WARN("Type: Performance"); break;
			case GL_DEBUG_TYPE_MARKER:               XEN_ENGINE_LOG_WARN("Type: Marker"); break;
			case GL_DEBUG_TYPE_PUSH_GROUP:           XEN_ENGINE_LOG_WARN("Type: Push Group"); break;
			case GL_DEBUG_TYPE_POP_GROUP:            XEN_ENGINE_LOG_WARN("Type: Pop Group"); break;
			case GL_DEBUG_TYPE_OTHER:                XEN_ENGINE_LOG_WARN("Type: Other"); break;
		}

		switch (severity)
		{
			case GL_DEBUG_SEVERITY_HIGH:             XEN_ENGINE_LOG_ERROR_SEVERE("Severity: high"); break;
			case GL_DEBUG_SEVERITY_MEDIUM:           XEN_ENGINE_LOG_WARN("Severity: medium"); break;
			case GL_DEBUG_SEVERITY_LOW:              XEN_ENGINE_LOG_WARN("Severity: low"); break;
			case GL_DEBUG_SEVERITY_NOTIFICATION:     XEN_ENGINE_LOG_WARN("Severity: notification"); break;
		}
	}

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

		#ifdef XEN_DEBUG
		{
			int flags;
			glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

			if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
			{
				XEN_ENGINE_LOG_WARN("Debug OpengGL Context Initialised!");

				glEnable(GL_DEBUG_OUTPUT);
				glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
				glDebugMessageCallback(&DebugOutput, nullptr);

				glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
			}

		}
		#endif // XEN_DEBUG
		XEN_ENGINE_LOG_INFO("Loaded OpenGL {0}.{1}", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

		XEN_ENGINE_LOG_INFO("Vendor: {0}", glGetString(GL_VENDOR));
		XEN_ENGINE_LOG_INFO("Renderer: {0}", glGetString(GL_RENDERER));
		XEN_ENGINE_LOG_INFO("OpenGL: {0}", glGetString(GL_VERSION));
		XEN_ENGINE_LOG_INFO("GLSL: {0}", glGetString(GL_SHADING_LANGUAGE_VERSION));

		int data;
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &data);
		XEN_ENGINE_LOG_INFO("Texture Slots: {0}", data);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
	}

	void OpenGLContext::DestroyContext()
	{

	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_CurrentWindow);
	}

	
}