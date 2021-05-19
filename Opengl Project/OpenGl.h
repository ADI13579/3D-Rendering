#pragma once
#include"lib.h"

using namespace std;
class OpenGl
{
private:
	void setGLFW();
	void getAreaToDraw();
	GLFWwindow* createwindow(const char* WINDOW_MSG);
	string read(std::string filename);
	void shader();
	void renderSettings();
public:
	vec cursor;
	int key;
	void update();

	int height, width;
	GLFWwindow* window;
	GLFWmonitor** monitor;
	GLuint shaderProgram;
	OpenGl(const char* WINDOW_MSG)
	{
		setGLFW();
		renderSettings();

		getAreaToDraw();
		window = createwindow(WINDOW_MSG);
		if (glewInit() != GLEW_OK)
		{
			glfwTerminate();
			cout << "error in glewinit()";
		}
		shader();
	}

	~OpenGl()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
		glDeleteShader(shaderProgram);
	}
};

