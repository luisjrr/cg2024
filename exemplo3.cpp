#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "utilsGL.h"
#include "VertexUtils.h"
#include "transforms.h"

VAO vao;
GLBuffer vbo;
ShaderProgram shaderProgram;
unsigned int screen_width = 600;
unsigned int screen_height = 600;

void init()
{
	shaderProgram = ShaderProgram{
		Shader{"ColorShader.vert", GL_VERTEX_SHADER},
		Shader{"ColorShader.frag", GL_FRAGMENT_SHADER}};
	glUseProgram(shaderProgram);

	Vec2Col P[] = {
		{{60, 105}, red},
		{{229, 114}, green},
		{{145, 270}, blue},
		{{364, 208}, yellow},
		{{283, 333}, cyan},
		{{471, 298}, orange},
	};

	vao = VAO{true};
	glBindVertexArray(vao);

	vbo = GLBuffer{GL_ARRAY_BUFFER};
	vbo.data(P);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
						  sizeof(Vec2Col), (void *)offsetof(Vec2Col, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_UNSIGNED_BYTE, GL_TRUE,
						  sizeof(Vec2Col), (void *)offsetof(Vec2Col, color));
}

void desenha()
{
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	Uniform{"M"} = orthogonal(0, screen_width, 0, screen_height, -1, 1);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 15);
}

void framebuffer_size_callback(GLFWwindow * /*window*/, int width, int height)
{
	glViewport(0, 0, width, height);
	screen_width = width;
	screen_height = height;
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow *window = glfwCreateWindow(screen_width, screen_height, "OpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glewInit();
	init();

	glViewport(0, 0, screen_width, screen_height);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	std::cout << "GL Version: " << glGetString(GL_VERSION) << '\n';
	std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';

	while (!glfwWindowShouldClose(window))
	{
		desenha();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
}
