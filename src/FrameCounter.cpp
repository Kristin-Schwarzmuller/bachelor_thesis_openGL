#include <FrameCounter.h>

#include <sstream>


namespace statistics
{

	FrameCounter::FrameCounter()
	{
		stop = std::chrono::high_resolution_clock::now();
		ellapsedTime = 0;
		frames = 0;
	}


	FrameCounter::~FrameCounter()
	{
	}

	void FrameCounter::tick(GLFWwindow* window)
	{
		start = std::chrono::high_resolution_clock::now();
		long long delta = std::chrono::duration_cast<std::chrono::microseconds>(start - stop).count();

		ellapsedTime += delta;
		frames++;

		if(ellapsedTime >= 1000000) // µseconds
		{
			std::stringstream s;
			s << frames << " fps";
			glfwSetWindowTitle(window, s.str().c_str());
			frames = 0;
			ellapsedTime = 0;
		}

		stop = start;
	}

}
