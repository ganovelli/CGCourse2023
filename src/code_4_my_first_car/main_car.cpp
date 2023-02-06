#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include "..\common\debugging.h"
#include "..\common\renderable.h"
#include "..\common\shaders.h"
#include "..\common\simple_shapes.h"

/* 
GLM library for math  https://github.com/g-truc/glm 
it's a header-only library. You can just copy the folder glm into 3dparty
and set the path properly. 
*/
#include <glm/glm.hpp>  
#include <glm/ext.hpp>  

void out_mat(glm::mat4 m) {
	std::cout << std::endl;
	for (int i=0; i < 4; ++i) {
		for (int j=0; j < 4; ++j)
			std::cout << m[i][j] << " ";
		std::cout << std::endl;
	}

}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1000, 800, "code_4_my_first_car", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    
    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glewInit();

    printout_opengl_glsl_info();

	shader basic_shader;
    basic_shader.create_program("shaders/basic.vert", "shaders/basic.frag");
	basic_shader.bind("uP");
	basic_shader.bind("uV");
	basic_shader.bind("uT");
	basic_shader.bind("uColor");
	check_shader(basic_shader.vs);
	check_shader(basic_shader.fs);
    validate_shader_program(basic_shader.pr);

	/* Set the uT matrix to Identity */
	glUseProgram(basic_shader.pr);
	glUniformMatrix4fv(basic_shader["uT"], 1, GL_FALSE, &glm::mat4(1.0)[0][0]);
	glUseProgram(0);

	check_gl_errors(__LINE__, __FILE__);

	/* create a  cube   centered at the origin with side 2*/
	renderable r_cube	= shape_maker::cube(0.5,0.3,0.0);

	/* create a  cylinder with base on the XZ plane, and height=2*/
	renderable r_cyl	= shape_maker::cylinder(30,0.2,0.1,0.5);

	/* create 3 lines showing the reference frame*/
	renderable r_frame	= shape_maker::frame(4.0);

	check_gl_errors(__LINE__, __FILE__);

	/* Transformation to setup the point of view on the scene */
	glm::mat4 proj = glm::perspective(glm::radians(45.f), 1.33f, 0.1f, 100.f);
	glm::mat4 view = glm::lookAt(glm::vec3(0, 5, 10.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
	glEnable(GL_DEPTH_TEST);

	/*Initialize the matrix to implement the continuos rotation aroun the y axis*/
	glm::mat4 R = glm::mat4(1.f);

	/* define a transformation that will be applyed the the body of the car */
	glm::mat4 rotcar = glm::translate(glm::mat4(1.f), glm::vec3(0.0, 0.0, -2.0));
	rotcar = glm::rotate(rotcar, 0.1f, glm::vec3(-1.0, 0.0, 0.0));
	rotcar = glm::translate(rotcar, glm::vec3(0.0, 0.0, 2.0));
	

	glm::mat4 cube_to_car_body			= glm::scale(rotcar, glm::vec3(1.0, 0.5, 2.0));
	cube_to_car_body = glm::translate(cube_to_car_body, glm::vec3(0.0, 1.0, 0.0));

	glm::mat4 cube_to_spoiler;
	glm::mat4 a1 = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 a2 = glm::shear(glm::mat4(1.0), glm::vec3(0.f), glm::vec2(0.f), glm::vec2(0.f), glm::vec2(0.f, 1.0));
	glm::mat4 a3 = glm::scale(glm::mat4(1.0), glm::vec3(1.0, 0.1, 2.0));
	glm::mat4 a4 = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 1.0, 0.0));
	cube_to_spoiler = rotcar*a1*a2*a3*a4;

	glm::mat4 cyl_to_wheel;
	glm::mat4 w1 = glm::translate(glm::mat4(1.0), glm::vec3(0.0, -1.0, 0.0));
 	glm::mat4 w2 = glm::scale(glm::mat4(1.0), glm::vec3(0.5, 0.1, 0.5));
	glm::mat4 w3 = glm::rotate(glm::mat4(1.0), glm::radians(90.f),glm::vec3(0.0, 0.0, 1.0));
	cyl_to_wheel = w3*w2*w1;
	
	std::vector<glm::mat4> wheels;
	wheels.resize(4);
	wheels[0]= glm::translate(glm::mat4(1.0), glm::vec3(-1.15, 0.0, -1.0));;
	wheels[0] = wheels[0] *cyl_to_wheel;
	 
	wheels[1] = glm::translate(glm::mat4(1.0), glm::vec3( 1.15, 0.0, -1.0));;
	wheels[1] = wheels[1] * cyl_to_wheel;

	wheels[2] = glm::translate(glm::mat4(1.0), glm::vec3(-1.15, 0.0,  1.0));;
	wheels[2] = wheels[2] * cyl_to_wheel;

	wheels[3] = glm::translate(glm::mat4(1.0), glm::vec3( 1.15, 0.0, 1.0));;
	wheels[3] = wheels[3] * cyl_to_wheel;

	int it = 0;
	/* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
		it++;
		/*incremente the rotation by 0.01 radians*/
		R = glm::rotate(R, 0.01f, glm::vec3(0.f, 1.f, 0.f));

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
		glm::mat4 M = view*R;
        glUseProgram(basic_shader.pr);
		glUniformMatrix4fv(basic_shader["uP"], 1, GL_FALSE, &proj[0][0]);
		glUniformMatrix4fv(basic_shader["uV"], 1, GL_FALSE, &M[0][0]);
		check_gl_errors(__LINE__, __FILE__);
		

		/* render box and cylinders so that the look like a car */
		r_cube.bind();

		/*draw the cube tranformed into the car's body*/
		glUniformMatrix4fv(basic_shader["uT"], 1, GL_FALSE, &cube_to_car_body[0][0]);
		glUniform3f(basic_shader["uColor"], 1.0,0.0,0.0);
		glDrawElements(GL_TRIANGLES, r_cube.in, GL_UNSIGNED_INT, 0);

		/*draw the cube tranformed into the car's roof/spoiler */
		glUniformMatrix4fv(basic_shader["uT"], 1, GL_FALSE, &cube_to_spoiler[0][0]);
		glUniform3f(basic_shader["uColor"], 1.0, 1.0, 0.0);
		glDrawElements(GL_TRIANGLES, r_cube.in, GL_UNSIGNED_INT, 0);

		/*draw the wheels */
		r_cyl.bind();
		glUniform3f(basic_shader["uColor"], 0.0, 0.0, 1.0);
		for (int iw = 0; iw < 4; ++iw) {
			glUniformMatrix4fv(basic_shader["uT"], 1, GL_FALSE, &wheels[iw][0][0]);
			glDrawElements(GL_TRIANGLES, r_cyl.in, GL_UNSIGNED_INT, 0);
		}

		glUniformMatrix4fv(basic_shader["uT"], 1, GL_FALSE, &glm::mat4(1.f)[0][0]);

		/* ******************************************************/
		glUniform3f(basic_shader["uColor"], -1.0, 0.0, 1.0);
		r_frame.bind();
		glDrawArrays(GL_LINES, 0, 6);

        check_gl_errors(__LINE__,__FILE__);
        glUseProgram(0);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
