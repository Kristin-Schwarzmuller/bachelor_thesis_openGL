#pragma once

//#include <gl_core_4_4.hpp>
#include <glad.h>
#include <GLFW/glfw3.h>


namespace cgbv
{
	class Renderer
	{
	protected:
		GLFWwindow *window;

		int window_width, window_height;


	public:
		Renderer(GLFWwindow *window);
		virtual ~Renderer(void);

		virtual void destroy() = 0;
		virtual void resize(int width, int height) = 0;
		virtual void input(int key, int scancode, int action, int modifiers) = 0;
		virtual bool setup() = 0;
		virtual void render() = 0;
		virtual void update() = 0;
	};
}
