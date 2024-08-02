#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "transforms.h"
#include "Color.h"
#include "utilsGL.h"
#include "ObjMesh.h"

using Vertex = ObjMesh::Vertex;

GLTexture init_texture(std::string image)
{
    GLTexture texture{GL_TEXTURE_2D};
    texture.load(image);

    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (GLEW_EXT_texture_filter_anisotropic)
    {
        GLfloat fLargest;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);
    }
    return texture;
}

class GLMesh
{
    VAO vao;
    GLBuffer vbo;
    std::vector<MaterialRange> materials;
    std::map<std::string, GLTexture> texture_map;

public:
    mat4 Model;

    GLMesh() = default;

    GLMesh(std::string obj_file, mat4 _Model, std::string default_texture = "")
    {
        ObjMesh mesh{obj_file};
        init_buffers(mesh.getTriangles());

        MaterialInfo std_mat;
        std_mat.map_Kd = default_texture;

        materials = mesh.getMaterials(std_mat);

        for (MaterialRange range : materials)
            load_texture(mesh.path, range.mat.map_Kd);

        Model = _Model;
    }

    void draw() const
    {
        glBindVertexArray(vao);
        for (MaterialRange range : materials)
        {
            Uniform{"has_texture"} = get_texture(range.mat.map_Kd);
            Uniform{"default_color"} = range.mat.Kd;
            glDrawArrays(GL_TRIANGLES, range.first, range.count);
        }
    }

private:
    void init_buffers(const std::vector<Vertex> &vertices)
    {
        vao = VAO{true};
        glBindVertexArray(vao);

        vbo = GLBuffer{GL_ARRAY_BUFFER};
        vbo.data(vertices);

        size_t stride = sizeof(Vertex);
        size_t offset_position = offsetof(Vertex, position);
        size_t offset_texCoords = offsetof(Vertex, texCoords);
        size_t offset_normal = offsetof(Vertex, normal);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)offset_position);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void *)offset_texCoords);

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void *)offset_normal);
    }

    void load_texture(std::string path, std::string file)
    {
        if (file != "" && texture_map.find(file) == texture_map.end())
        {
            std::string img = path + file;
            std::cout << "read image " << img << '\n';
            texture_map[file] = init_texture(img);
        }
    }

    bool get_texture(std::string file) const
    {
        auto it = texture_map.find(file);
        if (it != texture_map.end())
        {
            glBindTexture(GL_TEXTURE_2D, it->second);
            return true;
        }

        return false;
    }
};

ShaderProgram shaderProgram;
std::vector<GLMesh> meshes;
float vangle = 0;
mat4 BaseView = lookAt({0, 1.6, 5}, {0, 1.6, 0}, {0, 1, 0});
unsigned int screen_width = 600;
unsigned int screen_height = 600;

void init_shader()
{
    shaderProgram = ShaderProgram{
        Shader{"TextureShader.vert", GL_VERTEX_SHADER},
        Shader{"TextureShader.frag", GL_FRAGMENT_SHADER}};
    glUseProgram(shaderProgram);
}

void init()
{
    glEnable(GL_DEPTH_TEST);

    init_shader();

    meshes.emplace_back(
        "modelos/floor.obj",
        scale(35, 35, 35),
        "../stone.jpg");

    meshes.emplace_back(
        "modelos/pinguim/pinguim.obj",
        translate(1, 1, -6) * rotate_y(3) * scale(.4, .4, .4));

    meshes.emplace_back(
        "modelos/carro/carro.obj",
        translate(-1, 0.6, 2) * scale(1, 1, 1));

    meshes.emplace_back(
        "modelos/luigi/Luigi.obj",
        translate(1, 0, 0) * scale(0.6, 0.6, 0.6));

    meshes.emplace_back(
        "modelos/House Complex/House Complex.obj",
        translate(4, 0, 0) * rotate_y(0.5 * M_PI) * scale(.15, .15, .15));

    meshes.emplace_back(
        "modelos/mario/Mario.obj",
        translate(-2, 0, -3) * scale(0.6, 0.6, 0.6));
}

void desenha()
{
    glClearColor(0, 165.0 / 255, 220.0 / 255, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float a = screen_width / (float)screen_height;

    mat4 Projection = perspective(45, a, 0.1, 100);
    mat4 View = rotate_x(vangle) * BaseView;

    for (const GLMesh &mesh : meshes)
    {
        mat4 Model = mesh.Model;
        Uniform{"M"} = Projection * View * Model;
        mesh.draw();
    }
}

double last_x, last_y;

void mouse_button_callback(GLFWwindow *window, int button, int action, int /*mods*/)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        last_x = x;
        last_y = y;
    }
}

void cursor_position_callback(GLFWwindow *window, double x, double y)
{
    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    if (state == GLFW_PRESS)
    {
        double dx = x - last_x;
        double dy = y - last_y;

        vangle += 0.01 * dy;
        vangle = clamp(vangle, -1.5, 1.5);
        BaseView = rotate_y(dx * 0.01) * BaseView;

        last_x = x;
        last_y = y;
    }
}

void special_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS && action != GLFW_REPEAT)
        return;

    vec3 dir = {0, 0, 0};
    float zmove = 0;
    float xmove = 0;

    if (key == GLFW_KEY_UP)
        zmove += 0.2;

    if (key == GLFW_KEY_DOWN)
        zmove -= 0.2;

    if (key == GLFW_KEY_LEFT)
        xmove += 0.2;

    if (key == GLFW_KEY_RIGHT)
        xmove -= 0.2;

    BaseView = translate(xmove, 0, zmove) * BaseView;
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

    GLFWwindow *window = glfwCreateWindow(screen_width, screen_height, "CG UFF", NULL, NULL);
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
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetKeyCallback(window, special_callback);

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
