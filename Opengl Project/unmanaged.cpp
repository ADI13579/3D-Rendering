#pragma once
#include"OpenGl.h"
namespace events
{
	int keyPressed;
	void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (action == 1)
			keyPressed = key;
		else
			keyPressed = -1;
	};
}

GLFWwindow* OpenGl::createwindow(const char* WINDOW_MSG)
{
	window = glfwCreateWindow(width, height, WINDOW_MSG, NULL, NULL);
	glViewport(50, 50, width, height);
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, events::keyCallBack);//Problem Encountered
	return window;
}

void OpenGl::update()
{
	glfwGetCursorPos(window, &cursor.x, &cursor.y);
	key = events::keyPressed;
	if (key == GLFW_KEY_ESCAPE)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}