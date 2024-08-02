#include <GLFW/glfw3.h>

#include "Render3D.h"
#include "ZBuffer.h"
#include "TextureShader.h"
#include "ObjMesh.h"
#include "transforms.h"
#include "ImageSet.h"

class Mesh
{
	std::vector<ObjMesh::Vertex> tris;
	std::vector<MaterialRange> materials;
	ImageSet image_set;

public:
	mat4 Model;

	Mesh(std::string obj_file, mat4 _Model, std::string default_texture = "")
	{
		ObjMesh mesh{obj_file};
		tris = mesh.getTriangles();

		MaterialInfo std_mat;
		std_mat.map_Kd = default_texture;

		materials = mesh.getMaterials(std_mat);

		for (MaterialRange range : materials)
			image_set.load_texture(mesh.path, range.mat.map_Kd);

		Model = _Model;
	}

	void draw(ImageZBuffer &G, TextureShader &shader) const
	{
		for (MaterialRange range : materials)
		{
			image_set.get_texture(range.mat.map_Kd, shader.texture.img);
			TrianglesRange T{range.first, range.count};
			Render3D(tris, T, shader, G);
		}
	}
};

std::vector<Mesh> meshes;
float vangle = 0;
mat4 BaseView = lookAt({0, 1.6, 5}, {0, 1.6, 0}, {0, 1, 0});
int screen_width = 800;
int screen_height = 600;

void init()
{
	meshes.emplace_back(
		"modelos/floor.obj",
		scale(35, 35, 35),
		"../stone.jpg");

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
	TextureShader shader;
	shader.texture.filter = BILINEAR;
	shader.texture.wrapX = REPEAT;
	shader.texture.wrapY = REPEAT;

	ImageRGB G{screen_width, screen_height};

	float a = screen_width / (float)screen_height;
	mat4 Projection = perspective(45, a, 0.1, 1000);
	mat4 View = rotate_x(vangle) * BaseView;

	G.fill(0x00A5DC_rgb);
	ImageZBuffer I{G};

	for (const Mesh &mesh : meshes)
	{
		shader.M = Projection * View * mesh.Model;
		mesh.draw(I, shader);
	}

	glDrawPixels(screen_width, screen_height, GL_RGB, GL_UNSIGNED_BYTE, G.data());
}

double last_x, last_y;
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
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

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	vec3 dir = {0, 0, 0};
	float zmove = 0;
	float xmove = 0;

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		zmove += 0.2;

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		zmove -= 0.2;

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		xmove += 0.2;

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		xmove -= 0.2;

	BaseView = translate(xmove, 0, zmove) * BaseView;
}

int main(int argc, char *argv[])
{
	glfwInit();
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

	init();

	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetKeyCallback(window, key_callback);

	while (!glfwWindowShouldClose(window))
	{
		desenha();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
}
