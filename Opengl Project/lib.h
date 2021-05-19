#pragma once

#include<iostream>
#include<fstream>
#include<vector>
#include<string.h>

#include<GL/glew.h>
#include<GLFW/glfw3.h>

#include<GLM/glm.hpp>
#include<GLM/vec2.hpp>
#include<GLM/vec3.hpp>
#include<GLM/vec4.hpp>
#include<GLM/mat4x4.hpp>
#include<GLM/gtc/matrix_transform.hpp>
#include<GLM/gtc/type_ptr.hpp>

#include<SOIL2/SOIL2.h>



#pragma once

class Vertex
{
	public:
		glm::vec3 position;
		glm::vec3 color;
		glm::vec2 texture;
};

class vec
{
public:
	double x, y;
	vec()
	{
		x=y=0;
	}
};
