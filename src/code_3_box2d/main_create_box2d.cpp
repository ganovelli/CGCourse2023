#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include "..\common\debugging.h"
#include "..\common\renderable.h"
#include "..\common\shaders.h"

renderable r;

/*	this function  sets up the buffers for rendering a box  made
	of nx by ny quadrilateral, each made of two triangles. 
*/
void create_box2d(int nx, int ny) {
	float positions[] = { -0.1, -0.1,	// 1st vertex
		0.1, -0.1,  // 2nd vertex
		0.1, 0.1,	// 3nd vertex
		-0.1, 0.1    // 4th vertex
	};
	unsigned int ind[] = { 0,1,2,0,2,3 };
	r.create();
	r.add_vertex_attribute<float>(positions, sizeof(float) * 8, 0, 2);
	r.add_indices(ind, sizeof(unsigned int) *6, GL_TRIANGLES);
}

int main(void)
{

	
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1000, 800, "code_3_box2d", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    
    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glewInit();

    printout_opengl_glsl_info();

	create_box2d(1, 1);
	r.bind();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

		/* here the call to render the box --*/
		glDrawElements(GL_TRIANGLES, 4, GL_UNSIGNED_INT, 0);
		/* -------------------------------------*/
		
		/* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
