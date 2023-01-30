#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include "..\common\debugging.h"
#include "..\common\renderable.h"
#include "..\common\shaders.h"

/* if you want you can use the support struct "renderable" */
renderable r;

/*	this function  sets up the buffers for rendering a box  made
	of nx by ny quadrilateral, each made of two triangles. 
*/
void create_box2d(int nx, int ny) {
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
	/* uncomment if you used the support class renderable */
	// r.bind();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

		/* here the call to render the box --*/
		// glDraw ....
		/* -------------------------------------*/
		
		/* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
