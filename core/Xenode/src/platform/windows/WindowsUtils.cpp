#include <pch>
#include "core/app/Utils.h"

#ifdef XEN_PLATFORM_WINDOWS
#include <commdlg.h>

#include <core/app/desktop/DesktopGameApplication.h>

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

namespace Xen 
{
	namespace Utils 
	{
		std::string OpenFileDialogOpen(const char* filter)
		{
			DesktopGameApplication* currentApplication = (DesktopGameApplication*)GetApplicationInstance();
			GLFWwindow* glfwWindow = (GLFWwindow*)currentApplication->GetGameWindow()->GetNativeWindow();

			HWND windowHandle = glfwGetWin32Window(glfwWindow);

			OPENFILENAMEA open_file;
			CHAR file_path_to_return[260] = { 0 };

			ZeroMemory(&open_file, sizeof(OPENFILENAMEA));

			open_file.lStructSize = sizeof(OPENFILENAMEA);
			open_file.hwndOwner = windowHandle;
			open_file.lpstrFile = file_path_to_return;
			open_file.nMaxFile = sizeof(file_path_to_return);
			open_file.lpstrFilter = filter;
			open_file.nFilterIndex = 1;
			open_file.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

			if (GetOpenFileNameA(&open_file) == TRUE)
				return std::string(open_file.lpstrFile);

			return std::string();

		}

		std::string OpenFileDialogSave(const char* filter)
		{
			DesktopGameApplication* currentApplication = (DesktopGameApplication*)GetApplicationInstance();
			GLFWwindow* glfwWindow = (GLFWwindow*)currentApplication->GetGameWindow()->GetNativeWindow();

			HWND windowHandle = glfwGetWin32Window(glfwWindow);

			OPENFILENAMEA open_file;
			CHAR file_path_to_return[260] = { 0 };

			ZeroMemory(&open_file, sizeof(OPENFILENAMEA));

			open_file.lStructSize = sizeof(OPENFILENAMEA);
			open_file.hwndOwner = windowHandle;
			open_file.lpstrFile = file_path_to_return;
			open_file.nMaxFile = sizeof(file_path_to_return);
			open_file.lpstrFilter = filter;
			open_file.nFilterIndex = 1;
			open_file.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

			if (GetSaveFileNameA(&open_file) == TRUE)
				return std::string(open_file.lpstrFile);

			return std::string();
		}
	}
}

#endif // XEN_PLATFORM_WINDOWS
