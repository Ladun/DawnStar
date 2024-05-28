#include <dspch.hpp>
#include <DawnStar/Renderer/GraphicsContext.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

namespace DawnStar
{
    GraphicsContext::GraphicsContext(GLFWwindow* windowHandle)
        : _windowHandle(windowHandle)
    {
        DS_CORE_ASSERT(windowHandle, "Window handle is null!!");   
    }
    
	void GraphicsContext::Init()
	{
		DS_PROFILE_SCOPE();

		glfwMakeContextCurrent(_windowHandle);
		int status = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
		DS_CORE_ASSERT(status, "Failed to initialize Glad!");

		DS_CORE_INFO("OpenGL Info:");
		DS_CORE_INFO("  Vendor	: {0}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
		DS_CORE_INFO("  Renderer: {0}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
		DS_CORE_INFO("  Version	: {0}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));

	#ifdef DS_ENABLE_ASSERTS
			int versionMajor;
			int versionMinor;
			glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
			glGetIntegerv(GL_MINOR_VERSION, &versionMinor);

			DS_CORE_ASSERT(versionMajor > 4 || (versionMajor == 4 && versionMinor >= 5), "DawnStar requires at least OpenGL version 4.5!");
	#endif
	}

	void GraphicsContext::SwapBuffer()
	{
		DS_PROFILE_SCOPE();

		glfwSwapBuffers(_windowHandle);
	}
} // namespace DawnStar
