#pragma once

#include <chrono>
#include <GLFW/glfw3.h>

namespace statistics
{

	class FrameCounter
	{
		long long ellapsedTime;
		int frames;

		std::chrono::high_resolution_clock::time_point start;
		std::chrono::high_resolution_clock::time_point stop;

	public:
		FrameCounter();
		~FrameCounter();

		void tick(GLFWwindow* window);
	};

}
