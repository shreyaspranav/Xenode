#include "pch"
#include "OpenGLContext.h"
#include "gfxapi/window/glfw/GLFW_window.h"

#include <core/app/Log.h>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <core/app/Profiler.h>

namespace Xen {

	void DebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
	{
		
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:             XEN_ENGINE_LOG_ERROR("OpenGL Debug Output: [{0}]: {1} ---------------------", id, message); break;
		case GL_DEBUG_SEVERITY_MEDIUM:           XEN_ENGINE_LOG_WARN("OpenGL Debug Output: [{0}]: {1} ----------------------", id, message); break;
		case GL_DEBUG_SEVERITY_LOW:              XEN_ENGINE_LOG_WARN("OpenGL Debug Output: [{0}]: {1} ----------------------", id, message); break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:     XEN_ENGINE_LOG_WARN("OpenGL Debug Output: [{0}]: {1} ----------------------", id, message); break;
		}

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

		XEN_ENGINE_LOG_WARN("End of OpenGL Debug Output --------------------------------------------------\n");
	}

	void OpenGLContext::Init()
	{
		XEN_PROFILE_FN();

		glfwMakeContextCurrent(m_CurrentWindow);

		UserPointer p = *(UserPointer*)glfwGetWindowUserPointer(m_CurrentWindow);
		bool vsync = p.props.vsync;
		glfwSwapInterval(vsync);

		int version = gladLoadGL(glfwGetProcAddress);

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GLAD_VERSION_MAJOR(version));
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GLAD_VERSION_MINOR(version));

		// Anti Aliasing?
		glfwWindowHint(GLFW_SAMPLES, 8);

		
		if (!version) { XEN_ENGINE_LOG_ERROR("glad failed to load!"); TRIGGER_BREAKPOINT; }

		XEN_ENGINE_LOG_INFO("Loaded OpenGL {0}.{1}", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

		XEN_ENGINE_LOG_INFO("Vendor: {0}", glGetString(GL_VENDOR));
		XEN_ENGINE_LOG_INFO("Renderer: {0}", glGetString(GL_RENDERER));
		XEN_ENGINE_LOG_INFO("OpenGL: {0}", glGetString(GL_VERSION));
		XEN_ENGINE_LOG_INFO("GLSL: {0}", glGetString(GL_SHADING_LANGUAGE_VERSION));

		if (GLAD_VERSION_MAJOR(version) < 4 && GLAD_VERSION_MINOR(version) < 5)
		{
			XEN_ENGINE_LOG_ERROR_SEVERE("OpenGL Version Not Supported");
			TRIGGER_BREAKPOINT;
		}

		m_OpenGLVersion = OpenGLVersion::XEN_OPENGL_API_4_5;

#if 0
		if (GLAD_VERSION_MAJOR(version) == 2)
			m_OpenGLVersion = OpenGLVersion::XEN_OPENGL_API_2_0;
		else if (GLAD_VERSION_MAJOR(version) == 3)
			m_OpenGLVersion = OpenGLVersion::XEN_OPENGL_API_3_0;

		else if (GLAD_VERSION_MAJOR(version) == 4)
		{
			if (GLAD_VERSION_MINOR(version) == 6)
				m_OpenGLVersion = OpenGLVersion::XEN_OPENGL_API_4_6;
			else if (GLAD_VERSION_MINOR(version) == 5)
				m_OpenGLVersion = OpenGLVersion::XEN_OPENGL_API_4_5;
			else if (GLAD_VERSION_MINOR(version) == 3)
				m_OpenGLVersion = OpenGLVersion::XEN_OPENGL_API_4_3;

			else
				m_OpenGLVersion = OpenGLVersion::XEN_OPENGL_API_3_0;
		}

		if (GLAD_VERSION_MAJOR(version) >= 3 && GLAD_VERSION_MINOR(version) >= 2)
		{
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		}
#endif

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

		int data;
		XEN_ENGINE_LOG_INFO("GPU Limits:-----------------------------------------");

		glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &data);
		XEN_ENGINE_LOG_INFO("Texture Slots: {0}", data);

		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &data);
		XEN_ENGINE_LOG_INFO("Max 4-component generic vertex attributes: {0}", data);

		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &data);
		XEN_ENGINE_LOG_INFO("Max Texture Size: {0}", data);

		glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &data);
		XEN_ENGINE_LOG_INFO("Max No. Of Uniform Variables: {0}", data);

		XEN_ENGINE_LOG_INFO("----------------------------------------------------");

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glBlendEquation(GL_FUNC_ADD);
		

		
		//glEnable(GL_DEPTH_TEST);
		glEnable(GL_LINE_SMOOTH);
	}

	void OpenGLContext::DestroyContext()
	{

	}

	void OpenGLContext::SwapBuffers()
	{
		XEN_PROFILE_FN();
		glfwSwapBuffers(m_CurrentWindow);
	}

	
}