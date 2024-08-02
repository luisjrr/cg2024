#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include "utilsGL.h"
#include "VertexUtils.h"
#include "transforms.h"
#include "bezier.h"

VAO vaoControlPoints, vaoBezierCurve;
GLBuffer vboControlPoints, vboBezierCurve;
ShaderProgram shaderProgram;
unsigned int screen_width = 400;
unsigned int screen_height = 400;

std::vector<vec2> P = loadCurve("curvaC.txt");

std::vector<vec2> Q;

void init()
{
    // Compila e linka os shaders
    shaderProgram = ShaderProgram{
        Shader{"ColorShader1.vert", GL_VERTEX_SHADER},
        Shader{"ColorShader1.frag", GL_FRAGMENT_SHADER}};
    glUseProgram(shaderProgram);

    // Amostra a curva de Bézier
    Q = sample_bezier_spline<3>(P, 50);

    // Configura os VAOs e VBOs para os pontos de controle
    vaoControlPoints = VAO{true};
    glBindVertexArray(vaoControlPoints);

    vboControlPoints = GLBuffer{GL_ARRAY_BUFFER};
    vboControlPoints.data(P, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void *)0);

    // Configura os VAOs e VBOs para a curva de Bézier
    vaoBezierCurve = VAO{true};
    glBindVertexArray(vaoBezierCurve);

    vboBezierCurve = GLBuffer{GL_ARRAY_BUFFER};
    vboBezierCurve.data(Q, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void *)0);
}

void desenha()
{
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    Uniform{"M"} = orthogonal(0, screen_width, 0, screen_height, -1, 1);

    // Desenha os pontos de controle
    // glBindVertexArray(vaoControlPoints);
    // Uniform{"color"} = vec4{1, 0, 0, 1}; // Vermelho
    // glDrawArrays(GL_LINE_STRIP, 0, P.size());

    // Desenha a curva de Bézier
    glBindVertexArray(vaoBezierCurve);
    Uniform{"color"} = vec4{1, 0, 0, 1}; // Vermelho
    glDrawArrays(GL_LINE_STRIP, 0, Q.size());
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
