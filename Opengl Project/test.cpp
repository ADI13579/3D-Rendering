//#include<iostream>
//#include<string.h>
//
//#include<GL/glew.h>
//#include <GLFW/glfw3.h>
//
//using namespace std;
//const GLint WIDTH = 800, HEIGHT = 600;
//
//int main()
//{
//	if (!glfwInit())
//	{
//		cout << "GLFW initiation failed";
//		glfwTerminate();
//		return -1;
//	}
//
//	//Set Up Screen
//	//Opengl version
//	//Prints version
//	cout << glfwGetVersionString();
//	//==========================================================================
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //sets the version to 3
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //sets the version to 3.3 combined with above
//	//==========================================================================
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//throws error for deprecated codes
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);//makes forward compatibility
//	//==========================================================================
//	//creating window
//	GLFWwindow  *mainwindow = glfwCreateWindow(WIDTH, HEIGHT, "My Computer", NULL, NULL);
//	if (!mainwindow)
//	{
//		cout << "GLFW winddow error";
//		glfwTerminate();
//		return -1;
//	}
//	//==========================================================================
//	//Get buffer size information
//	int b_width, b_height;
//	glfwGetFramebufferSize(mainwindow, &b_width, &b_height);
//	//==========================================================================
//	//say glew that what gets drawn is on this window
//	glfwMakeContextCurrent(mainwindow);
//	//==========================================================================
//	//Not understood why is it used?
//	if (!glfwInit())
//	{
//		cout << "GLFW initiation failed";
//		glfwDestroyWindow(mainwindow);
//		glfwTerminate();
//		return -1;
//	}
//	//===========================================================================
//	//setp viewport
//	glViewport(0, 0, b_width, b_height);
//	//===========================================================================
//	//loop untill window closed
//	while (!glfwWindowShouldClose(mainwindow))
//	{
//		//get + handle user input events
//		glfwPollEvents();
//
//		glClearColor(1.0f,0.0f, 0.0f, 1.0f);
//		glClear(GL_COLOR_BUFFER_BIT);
//
//		glfwSwapBuffers(mainwindow);
//	}
//	//glfwTerminate();
//}
