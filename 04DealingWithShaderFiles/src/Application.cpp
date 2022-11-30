#define GLEW_STATIC

#include <GL/glew.h>    //to access the OpenGL functiosn in the graphic card driver.
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    enum class ShaderType
    {
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while (std::getline(stream, line)) {

        std::cout << line << std::endl;

        //if we find a new section...
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;
            }
        
        } //if the line isn't a new section...
        else {
            ss[(int)type] << line << '\n';
        }
    }

    return { ss[0].str(), ss[1].str() };
}


static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str(); //we get the text of the shader in C-Style string

    glShaderSource(
        id,     //id of the shader
        1,      //num of sources in the string ??
        &src,   //address of the pointer that points to the C-Style string.
        nullptr //the length or nullptr to the end.
    );

    glCompileShader(id);

    int result;

    glGetShaderiv(
        id,                 //shader id.
        GL_COMPILE_STATUS,  //we query whether the compilation was successful.
        &result             //we store the result here.
    );

    //if compilation wasn't successful...
    if (result == GL_FALSE) {
        int length;

        glGetShaderiv(  //we query the lenght of the message that contains info about the status
            id,                 //shader id.
            GL_INFO_LOG_LENGTH, //we query the length of the message.
            &length             //we store the length here.
        );

        //we make room in the stack for an array of characters to store the error message.
        char* message = (char*)_malloca(length * sizeof(char)); //it allocates memory in the stack

        //we get the error message
        glGetShaderInfoLog(
            id,         //shader id
            length,     //we give the size of the buffer (in case we have another size)
            &length,    //we get the size of the message
            message     //the message is set in the buffer.
        );

        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cout << message << std::endl;

        glDeleteShader(id); //we delete this faulty shader.
        return 0;
    }

    return id;
}

/*
* This function gets the vertex and fragment shaders in text format and compiles and link them together
*/
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();

    //like compiling a C++ program
    GLuint vs = CompileShader(GL_VERTEX_SHADER, vertexShader); //GLuint is a typedef of unsigned int
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    //like linking a C++ program
    glAttachShader(program, vs);
    glAttachShader(program, fs);

    glLinkProgram(program);
    glValidateProgram(program);

    //removing the intermediate resources, like Obj files ??
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

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

    //glBindBuffer(GL_ARRAY_BUFFER, 0); //unselect the buffer

    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
   
    //we create the shader by providing the two programs to our function
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);

    //we select the program. this is because we could have some different programs.
    glUseProgram(shader);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

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

    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}