#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <algorithm>
#include <conio.h>
#include <direct.h>



#define TINYGLTF_IMPLEMENTATION

#include <stb_image.h>
#include <stb_image_write.h>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "..\common\gltf_loader.h"
#include "..\common\debugging.h"
#include "..\common\renderable.h"
#include "..\common\shaders.h"
#include "..\common\simple_shapes.h"
#include "..\common\matrix_stack.h"
#include "..\common\intersection.h"
#include "..\common\trackball.h"


/*
GLM library for math  https://github.com/g-truc/glm
it's a header-only library. You can just copy the folder glm into 3dparty
and set the path properly.
*/
#include <glm/glm.hpp>  
#include <glm/ext.hpp>  
#include <glm/gtx/string_cast.hpp>

/* light direction in world space*/
glm::vec4 Ldir;

trackball tb[2];
int curr_tb;

/* projection matrix*/
glm::mat4 proj;

/* view matrix */
glm::mat4 view;


/* object that will be rendered in this scene*/
renderable  r_plane;

/* program shaders used */
shader tex_shader;

gltf_model model;


void draw_line(glm::vec4 l) {
	glColor3f(1, 1, 1);
	glBegin(GL_LINES);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(100 * l.x, 100 * l.y, 100 * l.z);
	glEnd();
}
/* callback function called when the mouse is moving */
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	tb[curr_tb].mouse_move(proj, view, xpos, ypos);
}

/* callback function called when a mouse button is pressed */
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		tb[curr_tb].mouse_press(proj, view, xpos, ypos);
	}
	else
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
			tb[curr_tb].mouse_release();
		}
}

/* callback function called when a mouse wheel is rotated */
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (curr_tb == 0)
		tb[0].mouse_scroll(xoffset, yoffset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	/* every time any key is presse it switch from controlling trackball tb[0] to tb[1] and viceversa */
	if (action == GLFW_PRESS)
		curr_tb = 1 - curr_tb;

}
void print_info() {
}
unsigned int texture, inputmeshPos,inputmeshId;
const unsigned int TEXTURE_WIDTH = 1024, TEXTURE_HEIGHT = 1024;
void create_image() {
	// texture size

	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGBA,
		GL_FLOAT, NULL);
	glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	check_gl_errors(__LINE__, __FILE__);
	
	glGenTextures(1, &inputmeshPos);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, inputmeshPos);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	 
	float triangle[16] = { 0.0,0.0,-3.0,0, 1.0,0.0,-3.0,0, 0.0,0.5,-3.0 ,0,
						1.0,0.5,-3.0,0};
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 16, 1, 0, GL_RGBA,GL_FLOAT, triangle);
	 
	glBindImageTexture(1, inputmeshPos, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	 

	glGenTextures(1, &inputmeshId);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, inputmeshId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	int triangleId[8] = { 0,1,2,0,1,2,3,0 };
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32I, 2, 1, 0, GL_RGBA_INTEGER, GL_INT, triangleId);
	check_gl_errors(__LINE__, __FILE__);
	glBindImageTexture(3, inputmeshId, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32I);
	check_gl_errors(__LINE__, __FILE__);
}

std::string shaders_path = "../../src/code_XX_compute_shader/shaders/";

unsigned int compute;
GLint ID;
int iTime_loc, uWidth_loc, uNTriangles_loc, uBbox_loc;
void init_compute_shader() {

	std::string source = shader::textFileRead((shaders_path + "raytracing_octree.comp").c_str());
	// compute shader
	const GLchar* d = source.c_str();
	compute = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(compute, 1, &d, NULL);
	glCompileShader(compute);
	check_shader(compute);

	// shader Program
	ID = glCreateProgram();
	glAttachShader(ID, compute);
	glLinkProgram(ID);
	validate_shader_program(ID);
	iTime_loc = glGetUniformLocation(ID, "iTime");
	uWidth_loc = glGetUniformLocation(ID, "uWidth");
	uNTriangles_loc = glGetUniformLocation(ID, "uNTriangles");
	uBbox_loc = glGetUniformLocation(ID, "uBbox");
}


int main(int argc, char ** argv)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

		/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(TEXTURE_WIDTH, TEXTURE_HEIGHT, "code_XX_compute_shader", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}
	/* declare the callback functions on mouse events */
	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	glewInit();

	printout_opengl_glsl_info();



	check_gl_errors(__LINE__, __FILE__);
	init_compute_shader();
	check_gl_errors(__LINE__, __FILE__);

	create_image();
	
	model.load(argv[1]);
	model.create_buffers();

	glUseProgram(ID);
	glUniform1i(iTime_loc, 0 * clock());
	glUniform1i(uWidth_loc, 2048);
	glUniform1i(uNTriangles_loc, std::max(model.n_tri, 2) );

	float box4[4];
	box4[0] = model.o.bbox.getMin().x;
	box4[1] = model.o.bbox.getMin().y;
	box4[2] = model.o.bbox.getMin().z;
	box4[3] = model.o.bbox.getLongestEdge();

	glUniform4fv(uBbox_loc, 1, box4);
	
	glDispatchCompute((unsigned int)TEXTURE_WIDTH, (unsigned int)TEXTURE_HEIGHT, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	 

	check_gl_errors(__LINE__, __FILE__);
	/* load the shaders */


	tex_shader.create_program((shaders_path + "tex.vert").c_str(), (shaders_path + "tex.frag").c_str());
	tex_shader.bind("tex");
	tex_shader.bind("uT");
	check_shader(tex_shader.vs);
	check_shader(tex_shader.fs);
	validate_shader_program(tex_shader.pr);
	check_gl_errors(__LINE__, __FILE__);

	/* crete a rectangle*/
	shape s_plane;
	shape_maker::rectangle(s_plane, 10, 10);
	s_plane.to_renderable(r_plane);




	print_info();
	/* define the viewport  */
	glViewport(0, 0, TEXTURE_WIDTH, TEXTURE_HEIGHT);

	/* avoid rendering back faces */
	// uncomment to see the plane disappear when rotating it
	glDisable(GL_CULL_FACE);

	int _ = true;
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		check_gl_errors(__LINE__, __FILE__);
		if (_) {
			// _ = false;
			glUseProgram(ID);
			glUniform1i(iTime_loc, clock());
			glDispatchCompute((unsigned int)TEXTURE_WIDTH, (unsigned int)TEXTURE_HEIGHT, 1);
		}
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		glUseProgram(tex_shader.pr);
		glUniform1i(tex_shader["tex"], 0);
		glUniformMatrix4fv(tex_shader["uT"], 1, GL_FALSE, &glm::rotate(glm::mat4(1.f), glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f))[0][0]);
		r_plane.bind();
		glDrawElements(GL_TRIANGLES, r_plane.inds[0].count, GL_UNSIGNED_INT, 0);
		glUseProgram(0);


		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}