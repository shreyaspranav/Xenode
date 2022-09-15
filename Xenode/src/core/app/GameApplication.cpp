#include "pch"
#include "GameApplication.h"
#include "Log.h"
#include "LayerStack.h"

#define GLAD_GL_IMPLEMENTATION

#include <glad/gl.h>
#include <GLFW/glfw3.h>

namespace Xen {

	void GameApplication::Run()
	{
		int a = glfwInit();

		GLFWwindow* window = glfwCreateWindow(600, 600, "Xenode", NULL, NULL);
		glfwMakeContextCurrent(window);

		XEN_APP_LOG_INFO("{0}", a);
		//TRIGGER_BREAKPOINT;

		a = gladLoadGL(glfwGetProcAddress);
		XEN_APP_LOG_INFO("{0}", a);

		while (!glfwWindowShouldClose(window)) {
			glfwSwapBuffers(window);
			glfwPollEvents();

			glClearColor(0.7f, 0.9f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
		}

	}
}
