#include <Renderer.h>



namespace cgbv
{
	Renderer::Renderer(GLFWwindow *window)
	{
        this->window = window;
        glfwGetWindowSize(this->window, &window_width, &window_height);
	}


	Renderer::~Renderer()
	{
	}
}