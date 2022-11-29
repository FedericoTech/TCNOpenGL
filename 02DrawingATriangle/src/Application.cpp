#define GLEW_STATIC

#include <GL/glew.h>    //to access the OpenGL functiosn in the graphic card driver.
#include <GLFW/glfw3.h>

#include <iostream>

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    //Here we are initialising glew
    if (glewInit() != GLEW_OK)
        std::cout << "Error!" << std::endl;

    std::cout << glGetString(GL_VERSION) << std::endl;

    float positions[6] = {
        -0.5f, -.5f,    //vertex 1
        0.0f, .5f,      //vertex 2
        0.5f, -.5f,     //vertex 3
    };

    unsigned int buffer;

    glGenBuffers(
        1,      //number of buffers
        &buffer //buffer id
    );

    glBindBuffer(
        GL_ARRAY_BUFFER,
        buffer //we select this buffer to work on it next.
    );

    glBufferData(
        GL_ARRAY_BUFFER,
        6 * sizeof(float),  //size in bytes
        positions,          //the data
        GL_STATIC_DRAW      //a hint about the use we are going to make of it
    );

    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        0, //index to the first attribute
        2, //num of elements that represents a vertex.
        GL_FLOAT, //the type of the attributes. it's float in this case
        GL_FALSE, //whether we need to normalize them. They are normalized already.
        2 * sizeof(float), //size of the stride, number of bytes each vertex takes,
        0 //pointer inside the vertex
    );

    glBindBuffer(GL_ARRAY_BUFFER, 0); //unselect the buffer

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        /*
        glBegin(GL_TRIANGLES);
        glVertex2f(-0.5f, -.5f);
        glVertex2f(0.0f, .5f);
        glVertex2f(0.5f, -.5f);
        glEnd();
        */

        glDrawArrays(
            GL_TRIANGLES,   //what we can draw with the data. In this case triangles
            0,              //position in the array to start drawing the triangle
            3               //number of vertices to be rendered
        );

        

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}