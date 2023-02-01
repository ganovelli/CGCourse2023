#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include "..\common\debugging.h"
#include "..\common\renderable.h"
#include "..\common\shaders.h"
#include "..\common\simple_shapes.h"

#include <glm/glm.hpp>  
#include <glm/ext.hpp>  

void draw_frame() {
	glBegin(GL_LINES);
	glColor3f(1.0, 0.0, 0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(1.0, 0.0, 0.0);

	glColor3f(0.0, 1.0, 0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 1.0, 0.0);

	glColor3f(0.0, 0.0, 1);
	glVertex3f(0.0, 0.0, 1.0);
	glVertex3f(0.0, 0.0, 0.0);
	glEnd();
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
	check_shader(basic_shader.vs);
	check_shader(basic_shader.fs);
    validate_shader_program(basic_shader.pr);

	check_gl_errors(__LINE__, __FILE__);

	renderable r_cube  = shape_maker::cube();
	renderable r_frame = shape_maker::frame(2.0);
	renderable r_cyl   = shape_maker::cylinder(50);

	check_gl_errors(__LINE__, __FILE__);
	glm::mat4 proj = glm::perspective(glm::radians(45.f), 1.33f, 0.1f, 100.f);
	glm::mat4 view = glm::lookAt(glm::vec3(0, 5, 10.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));


	glEnable(GL_DEPTH_TEST);
	int it = 0;
	glm::mat4 R = glm::mat4(1.f);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
		it++;
		R = glm::rotate(R, 0.01f, glm::vec3(0.f, 1.f, 0.f));
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
		glm::mat4 M = view*R;
        glUseProgram(basic_shader.pr);
		glUniformMatrix4fv(basic_shader["uP"], 1, GL_FALSE, &proj[0][0]);
		glUniformMatrix4fv(basic_shader["uV"], 1, GL_FALSE, &M[0][0]);
		check_gl_errors(__LINE__, __FILE__);
		
	 	r_cyl.bind();
		//glDrawArrays(GL_POINTS, 0, 3 * (2 * 50 + 2));
	 	glDrawElements(GL_TRIANGLES, 50*3*4, GL_UNSIGNED_INT, 0);

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
