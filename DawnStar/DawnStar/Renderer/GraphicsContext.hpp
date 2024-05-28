#pragma once

struct GLFWwindow;

namespace DawnStar
{
    class GraphicsContext
    {
    public:
        GraphicsContext(GLFWwindow* windowHandle);
		~GraphicsContext() = default;
        
		void Init() ;
		void SwapBuffer();
	private:
		GLFWwindow* _windowHandle;
    };
} // namespace DawnStar
