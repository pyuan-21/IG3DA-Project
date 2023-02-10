// ----------------------------------------------
// 
// pyuan-21
// 
// Copyright (C) 2022
// All rights reserved.
// ----------------------------------------------

#include "globals.hpp"
#include "helpers/logger.hpp"
#include "singleton/singleton.hpp"
#include "console.hpp"
#include "input/keyInput.hpp"

using namespace IceRender;

// logic init
void Init()
{
	// init render
	GLOBAL.render->Init();
	
	if (GLOBAL.shaderMgr->LoadShader(GLOBAL.shaderPathPrefix + GLOBAL.defaultShaderName))
		GLOBAL.shaderMgr->PrintActiveShader();

	GLOBAL.sceneMgr->Init();
	GLOBAL.timeMgr->Init();
	GLOBAL.camCtrller->Init();
}

// logic update
void Update()
{
	GLOBAL.timeMgr->Update();
	GLOBAL.camCtrller->Update();
}

// render function
void Display()
{
	GLOBAL.render->Setting();
	GLOBAL.render->Render();
}

int main()
{
	// glfw: initialize and configure
	glfwInit();

	// "Major" and "minor" are two components of a single version number, separated by a dot. Version 4.5 is major version 4, minor version 5.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // in order to use OpenGL 4.5
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5); // in order to use OpenGL 4.5

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 16); // Enable OpenGL multisampling (MSAA)
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); // not allowed to change window size by dragging the edges of window

	Print("glfwInit succeeded.");

	// glfw: create window
	int width = GLOBAL.WIN_WIDTH;
	int height = GLOBAL.WIN_HEIGHT;
	GLFWwindow* window = glfwCreateWindow(width, height, "IceRender", NULL, NULL);
	if (!window)
	{
		Print("Failed to create GLFW window");
		glfwTerminate();
		return -1;
	}
	Print("GLFW window created.");
	glfwMakeContextCurrent(window);

	// TODO: https://www.glfw.org/docs/latest/window_guide.html#window_full_screen
	// look into the above url to figure it out how to create different resolution window-size.

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		Print("Failed to initialize OpenGL context");
		return -1;
	}

	Init();

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		GLOBAL.keyInput->ProcessInput(window);

		Update();

		/* Render here */
		Display();

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}
	GLOBAL.render->Clear();
	glfwTerminate();
	return 0;
}