#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <algorithm>
#include <conio.h>
#include <direct.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include "..\common\debugging.h"
#include "..\common\renderable.h"
#include "..\common\shaders.h"
#include "..\common\simple_shapes.h"
#include "..\common\matrix_stack.h"
#include "..\common\intersection.h"
#include "..\common\trackball.h"
#include "..\common\view_manipulator.h"
#include "..\common\frame_buffer_object.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "..\common\obj_loader.h"

/*
GLM library for math  https://github.com/g-truc/glm
it's a header-only library. You can just copy the folder glm into 3dparty
and set the path properly.
*/
#include <glm/glm.hpp>  
#include <glm/ext.hpp>  
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_access.hpp>

/* light direction in world space*/
glm::vec4 Ldir;

/* projector */
struct projector {
	glm::mat4 view_matrix,proj_matrix;
	texture tex;
	glm::mat4 set_projection(glm::mat4 _view_matrix, box3 box) {
		view_matrix = _view_matrix;
		proj_matrix =  glm::orthoRH(-2.f, 2.f, -2.f, 2.f,0.f,20.f);
		return proj_matrix;
	}
	glm::mat4 light_matrix() {
		return proj_matrix*view_matrix;
	}
	float sm_size_x, sm_size_y;
};


projector Lproj;


/* trackballs for controlloing the scene (0) or the light direction (1) */
trackball tb[2];

/* which trackball is currently used */
int curr_tb;

/* projection matrix*/
glm::mat4 proj;

/* view matrix */
glm::mat4 view ;

/* matrix stack*/
matrix_stack stack;

/* a frame buffer object for the offline rendering*/
frame_buffer_object fbo;

/* object that will be rendered in this scene*/
renderable r_frame, r_plane,r_line,r_torus,r_cube, r_sphere;

/* program shaders used */
shader depth_shader,shadow_shader,flat_shader;

/* implementation of view controller */

/* azimuthal and elevation angle*/
view_manipulator view_man;


/* callback function called when the mouse is moving */
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	if(curr_tb<2)
		tb[curr_tb].mouse_move(proj, view, xpos, ypos);
	else
		view_man.mouse_move(xpos, ypos);
}

/* callback function called when a mouse button is pressed */
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		if (curr_tb < 2)
			tb[curr_tb].mouse_press(proj, view, xpos, ypos);
		else 
			view_man.mouse_press(xpos, ypos);
	}
	else
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
			if(curr_tb<2)
				tb[curr_tb].mouse_release(); 
			else
				view_man.mouse_release();
		}
}

/* callback function called when a mouse wheel is rotated */
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if(curr_tb == 0)
		tb[0].mouse_scroll(xoffset, yoffset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
 /* every time any key is presse it switch from controlling trackball tb[0] to tb[1] and viceversa */
 if(action == GLFW_PRESS)
	 curr_tb = 1 - curr_tb;

}
void print_info() {
}

texture skybox,reflection_map;

static int selected = 0;

void load_textures() {
	std::string path = "../../models/textures/desert_cubemap/";
	skybox.load_cubemap(path + "posx.jpg", path + "negx.jpg", 
						path + "posy.jpg", path + "negy.jpg",
						path + "posz.jpg", path + "negz.jpg",1);

	glActiveTexture(GL_TEXTURE2);
	reflection_map.create_cubemap(2048, 2048,3);
}

void gui_setup() {
	ImGui::BeginMainMenuBar();

	if (ImGui::BeginMenu("Shadow mode"))
	{
	 if (ImGui::Selectable("Basic shadow mapping", selected == 0)) selected = 0;
	 ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Trackball")) {
		if (ImGui::Selectable("control scene", curr_tb == 0)) curr_tb = 0;
		if (ImGui::Selectable("control light", curr_tb == 1)) curr_tb = 1;
		if (ImGui::Selectable("control view", curr_tb == 2)) curr_tb = 2;

		ImGui::EndMenu();
	}
	ImGui::EndMainMenuBar();
}

void draw_torus(  shader & sh) {
	stack.push();
	stack.mult(glm::translate(glm::mat4(1.f), glm::vec3(1.0, 0.5, 0.0)));
	stack.mult(glm::scale(glm::mat4(1.f), glm::vec3(0.2, 0.2, 0.2)));
	glUniformMatrix4fv(sh["uT"], 1, GL_FALSE, &stack.m()[0][0]);
	r_torus.bind();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r_torus.ind);
	glDrawElements(GL_TRIANGLES, r_torus.in, GL_UNSIGNED_INT, 0);
	stack.pop();
}

void draw_plane(  shader & sh) {
	glUniformMatrix4fv(sh["uT"], 1, GL_FALSE, &stack.m()[0][0]);
	r_plane.bind();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r_plane.ind);
	glDrawElements(GL_TRIANGLES, r_plane.in, GL_UNSIGNED_INT, 0);
}



void draw_sphere(  shader & sh) {
	stack.push();
	stack.mult(glm::translate(glm::mat4(1.f), glm::vec3(0.0, 0.5, 0.0)));
	stack.mult(glm::scale(glm::mat4(1.f), glm::vec3(0.5, 0.5, 0.5)));
	glUniformMatrix4fv(sh["uT"], 1, GL_FALSE, &stack.m()[0][0]);
	r_sphere.bind();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r_sphere.ind);
	glDrawElements(GL_TRIANGLES, r_sphere.in, GL_UNSIGNED_INT, 0);
	stack.pop();
}


void draw_scene(  shader & sh) {
	draw_plane(sh);
	draw_sphere(sh);
	draw_torus(sh);
}

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(1000, 800, "code_13_shadows", NULL, NULL);
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

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplOpenGL3_Init();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	
	printout_opengl_glsl_info();

	check_gl_errors(__LINE__, __FILE__, true);
	/* load the shaders */
	std::string shaders_path = "../../src/code_13_shadows/shaders/";
	depth_shader.create_program((shaders_path+"depthmap.vert").c_str(), (shaders_path+"depthmap.frag").c_str());
	depth_shader.bind("uLightMatrix");
	depth_shader.bind("uT");
	depth_shader.bind("uRenderMode");

	check_shader(depth_shader.vs);
	check_shader(depth_shader.fs);
	validate_shader_program(depth_shader.pr);

	shadow_shader.create_program((shaders_path + "shadow_mapping.vert").c_str(), (shaders_path + "shadow_mapping.frag").c_str());
	shadow_shader.bind("uP");
	shadow_shader.bind("uV");
	shadow_shader.bind("uT");
	shadow_shader.bind("uLightMatrix");
	shadow_shader.bind("uRenderMode");

	check_shader(shadow_shader.vs);
	check_shader(shadow_shader.fs);
	validate_shader_program(shadow_shader.pr);
	check_gl_errors(__LINE__, __FILE__, true);

	flat_shader.create_program((shaders_path + "flat.vert").c_str(), (shaders_path + "flat.frag").c_str());
	flat_shader.bind("uP");
	flat_shader.bind("uV");
	flat_shader.bind("uT");
	flat_shader.bind("uColor");
	check_shader(flat_shader.vs);
	check_shader(flat_shader.fs);
	validate_shader_program(flat_shader.pr);
	check_gl_errors(__LINE__, __FILE__, true);
	/* Set the uT matrix to Identity */
	glUseProgram(depth_shader.pr);
	glUniformMatrix4fv(depth_shader["uT"], 1, GL_FALSE, &glm::mat4(1.0)[0][0]);
	glUseProgram(shadow_shader.pr);
	glUniformMatrix4fv(shadow_shader["uT"], 1, GL_FALSE, &glm::mat4(1.0)[0][0]);
	glUseProgram(flat_shader.pr);
	glUniformMatrix4fv(flat_shader["uT"], 1, GL_FALSE, &glm::mat4(1.0)[0][0]);
	glUseProgram(0);
	check_gl_errors(__LINE__, __FILE__, true);
	/* create a  long line*/
	r_line = shape_maker::line(100.f);

	/* create 3 lines showing the reference frame*/
	r_frame = shape_maker::frame(4.0);
	
	/* create a rectangle*/
	shape s_plane;
	shape_maker::rectangle(s_plane, 1, 1);
	s_plane.compute_tangent_space();
	s_plane.to_renderable(r_plane);
	check_gl_errors(__LINE__, __FILE__, true);
	/* create a torus */
	shape  s_torus;
	shape_maker::torus(s_torus, 0.5, 2.0, 50, 50);
	s_torus.compute_tangent_space();
	s_torus.to_renderable(r_torus);
	check_gl_errors(__LINE__, __FILE__, true);
	/* create a torus */
	r_cube = shape_maker::cube();

	/* create a sphere */
	r_sphere = shape_maker::sphere();

	/* initial light direction */
	Ldir = glm::vec4(0.0, 1.0, 0.0, 0.0);

	/* light projection */
	check_gl_errors(__LINE__, __FILE__, true);
	Lproj.sm_size_x = Lproj.sm_size_y = 512;
	Lproj.view_matrix = glm::lookAt(glm::vec3(4, 4, 6.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
	Lproj.tex.load("../../models/textures/batman.png",0);
 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	check_gl_errors(__LINE__, __FILE__, true);

	/* Transformation to setup the point of view on the scene */
	proj = glm::frustum(-1.f, 1.f, -0.8f, 0.8f, 2.f,100.f);
	view = glm::lookAt(glm::vec3(0, 3, 4.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));

	glUseProgram(depth_shader.pr);
	Lproj.view_matrix = glm::lookAt(glm::vec3(3, 3, 4.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
	Lproj.set_projection(Lproj.view_matrix, box3(2.f));
	glUniformMatrix4fv(depth_shader["uLightMatrix"], 1, GL_FALSE, &Lproj.light_matrix()[0][0]);
	glUniformMatrix4fv(depth_shader["uT"], 1, GL_FALSE, &glm::mat4(1.f)[0][0]);
	glUseProgram(0);
	check_gl_errors(__LINE__, __FILE__, true);

	glUseProgram(shadow_shader.pr);
	glUniformMatrix4fv(shadow_shader["uP"], 1, GL_FALSE, &proj[0][0]);
	glUniformMatrix4fv(shadow_shader["uV"], 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(shadow_shader["uLightMatrix"], 1, GL_FALSE, &Lproj.light_matrix()[0][0]);
	glUseProgram(0);
	check_gl_errors(__LINE__, __FILE__, true);

	glUseProgram(flat_shader.pr);
	glUniformMatrix4fv(flat_shader["uP"], 1, GL_FALSE, &proj[0][0]);
	glUniformMatrix4fv(flat_shader["uV"], 1, GL_FALSE, &view[0][0]);
	glUniform3f(flat_shader["uColor"], 1.0, 1.0, 1.0);
	glUseProgram(0);
	glEnable(GL_DEPTH_TEST);
	check_gl_errors(__LINE__, __FILE__, true);

	print_info();

	/* set the trackball position */
	tb[0].set_center_radius(glm::vec3(0, 0, 0), 2.f);
	tb[1].set_center_radius(glm::vec3(0, 0, 0), 2.f);
	view_man.reset();
	curr_tb = 0;

	/* define the viewport  */
	glViewport(0, 0, 1000, 800);

	load_textures();
	
	fbo.create(Lproj.sm_size_x, Lproj.sm_size_y);
	check_gl_errors(__LINE__, __FILE__, true);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClearColor(0.5, 0.5, 0.5, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		gui_setup();

		/* rotate the view accordingly to view_rot*/
		glm::mat4 curr_view = view_man.apply_to_view(view);

		/* light direction transformed by the trackball tb[1]*/
		glm::vec4 curr_Ldir = tb[1].matrix()*Ldir;

		stack.push();
		stack.mult(tb[0].matrix());

		//stack.push();
		glViewport(0, 0, 1000, 800);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

//		glm::mat4 lm = Lproj.proj_matrix*Lproj.view_matrix;

		glm::mat4 lm = glm::orthoRH(-2.f,2.f,-2.f,2.f,0.f,10.f)*Lproj.view_matrix;

		glUseProgram(depth_shader.pr);
		glUniformMatrix4fv(depth_shader["uLightMatrix"], 1, GL_FALSE, &lm[0][0]);
 		glUniformMatrix4fv(depth_shader["uLightMatrix"], 1, GL_FALSE, &Lproj.light_matrix()[0][0]);
		glUniformMatrix4fv(depth_shader["uT"], 1, GL_FALSE, &stack.m()[0][0]);
		 
		draw_scene(depth_shader);
	
		//stack.pop();

		// render the reference frame
		glUseProgram(flat_shader.pr);
		glUniformMatrix4fv(flat_shader["uV"], 1, GL_FALSE, &curr_view[0][0]);
		glUniformMatrix4fv(flat_shader["uT"], 1, GL_FALSE, &stack.m()[0][0]);
		glUniform3f(flat_shader["uColor"], -1.0, 1.0, 1.0);

		r_frame.bind();
		glDrawArrays(GL_LINES, 0, 6);
		glUseProgram(0);

		check_gl_errors( __LINE__,__FILE__,true);
		stack.pop();

		// render the light direction
		stack.push();
		stack.mult(tb[1].matrix());
		 
		glUseProgram(flat_shader.pr);
		glUniformMatrix4fv(flat_shader["uT"], 1, GL_FALSE, &stack.m()[0][0]);
		glUniform3f(flat_shader["uColor"], 1.0,1.0,1.0);
		r_line.bind();
		glDrawArrays(GL_LINES, 0, 2);
		glUseProgram(0);

		stack.pop();


		check_gl_errors(__LINE__, __FILE__);
	
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}