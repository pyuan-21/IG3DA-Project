#pragma once

#include "GLFW/glfw3.h"
#include <map>


namespace IceRender
{
	class KeyInput
	{
	public:
		KeyInput();
		~KeyInput();

		// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
		void ProcessInput(GLFWwindow* window);

	private:
		std::map<int, bool> keyStateMap; // map for recording last frame pressed or not
	};
}