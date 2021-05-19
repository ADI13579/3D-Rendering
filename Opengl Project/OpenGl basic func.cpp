#pragma once
#include"OpenGl.h"
#include<string>
void OpenGl::setGLFW()
{
	glfwInit();
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4 );
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
}

void OpenGl::getAreaToDraw()
{
	monitor = glfwGetMonitors(new int);
	GLFWvidmode str = *(glfwGetVideoMode(*monitor));
	height = int(str.height * 0.9);
	width = int(str.width * 0.9);
}


string OpenGl::read(std::string filename)
{
	std::string data,temp;
	std::fstream file(filename, std::ios::in);
	if (!file)
	{
		cout << filename << " opening error" << endl;
		return std::string();
	}

	while (std::getline(file,temp))
	{
		data += temp+"\n";
	}
	file.close();
	return data;
}

void OpenGl::shader()
{
	// Vertex Shader source code
	string temp[2];

	temp[0] = read("vertexshader");
	const char* vertexShaderSource = temp[0].c_str();


	temp[1] = read("fragmentshader");
	const char* fragmentShaderSource = temp[1].c_str();

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);


	//Just Seeing if compilation is successful
	int state;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &state);
	cout << "Vetrex Shader Compile Error Status" << state<<endl;

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &state);
	cout << "Fragment Shader Compile Error Status" << state<<endl;

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void OpenGl::renderSettings()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);//Cull=reduce or control
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);


	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}