#include"OpenGl.h"

Vertex vertices[] =
		{
			//Position								//Color							//Texcoords					
			glm::vec3(0.f, 0.5f, 0.f),				glm::vec3(1.f, 0.f, 0.f),		glm::vec2(0.5f, 1.f),		
			glm::vec3(-0.5f, -0.5f, 0.f),			glm::vec3(0.f, 1.f, 0.f),		glm::vec2(0.f, 0.f),		
			glm::vec3(0.5f, -0.5f, 0.f),			glm::vec3(0.f, 0.f, 1.f),		glm::vec2(1.f, 0.f)
		};

unsigned nrOfVertices = sizeof(vertices) / sizeof(Vertex);

GLuint indices[] =
{
	0, 1, 2
};
unsigned nrOfIndices = sizeof(indices) / sizeof(GLuint);

int main()
{
	bool a=1;
	OpenGl *wrap=new OpenGl("Flicker Flicker!!!");
	GLFWwindow *window= wrap->window;

	GLuint VAO;
	glCreateVertexArrays(1,&VAO);
	glBindVertexArray(VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),vertices,GL_STATIC_DRAW);

	GLuint EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex,position));
	glEnableVertexAttribArray(0);


	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex,color));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2 , GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex,texture));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);


	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		wrap->update();
		
		glClearColor(0, 0, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glUseProgram(wrap->shaderProgram);

		glBindVertexArray(VAO);

		glDrawArrays(GL_TRIANGLES, 0, nrOfIndices);

		glfwSwapBuffers(window);
	}
	delete(wrap);
}