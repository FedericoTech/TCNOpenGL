#define GLEW_STATIC

#include <GL/glew.h>    //to access the OpenGL functiosn in the graphic card driver.
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

static void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] (" << error << "): " 
            << function << " "
            << file << ":"
            << line << std::endl;
        return false;
    }
    return true;
}

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

    GLCall(glShaderSource(
        id,     //id of the shader
        1,      //num of sources in the string ??
        &src,   //address of the pointer that points to the C-Style string.
        nullptr //the length or nullptr to the end.
    ));

    GLCall(glCompileShader(id));

    int result;

    GLCall(glGetShaderiv(
        id,                 //shader id.
        GL_COMPILE_STATUS,  //we query whether the compilation was successful.
        &result             //we store the result here.
    ));

    //if compilation wasn't successful...
    if (result == GL_FALSE) {
        int length;

        GLCall(glGetShaderiv(  //we query the lenght of the message that contains info about the status
            id,                 //shader id.
            GL_INFO_LOG_LENGTH, //we query the length of the message.
            &length             //we store the length here.
        ));

        //we make room in the stack for an array of characters to store the error message.
        char* message = (char*)_malloca(length * sizeof(char)); //it allocates memory in the stack

        //we get the error message
        GLCall(glGetShaderInfoLog(
            id,         //shader id
            length,     //we give the size of the buffer (in case we have another size)
            &length,    //we get the size of the message
            message     //the message is set in the buffer.
        ));

        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cout << message << std::endl;

        GLCall(glDeleteShader(id)); //we delete this faulty shader.
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
    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));

    GLCall(glLinkProgram(program));
    GLCall(glValidateProgram(program));

    //removing the intermediate resources, like Obj files ??
    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));

    return program;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit()) {
        return -1;
    }

    //we create an OpenGL 3.3 Core profile through glfw facilities. (this has to be set before glfwCreateWindow)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    //Here we are initialising glew
    if (glewInit() != GLEW_OK) {
        std::cout << "Error!" << std::endl;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;

    float positions[] = {
        -.5f, -.5f, //vertex 0
        .5f, -.5f,  //vertex 1
        .5f, .5f,   //vertex 2
        -.5f, .5f,  //vertex 3
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };


//in OpenGL 3.0+ Core we need to set up a vertex array.
    unsigned int vao;
    GLCall(glGenVertexArrays(1, &vao));
    GLCall(glBindVertexArray(vao));

//we create an array buffer of vertices
    unsigned int buffer;

    GLCall(glGenBuffers(
        1,      //number of buffers
        &buffer //buffer id
    ));

    GLCall(glBindBuffer(
        GL_ARRAY_BUFFER,
        buffer //we select this buffer to work on it next.
    ));

    GLCall(glBufferData(
        GL_ARRAY_BUFFER,
        6 * 2 * sizeof(float),  //size in bytes
        positions,          //the data
        GL_STATIC_DRAW      //a hint about the use we are going to make of it
    ));

    GLCall(glEnableVertexAttribArray(0));

    //this command links the buffer with the vertex array
    GLCall(glVertexAttribPointer(
        0, //index to the first attribute
        2, //num of elements that represents a vertex.
        GL_FLOAT, //the type of the attributes. it's float in this case
        GL_FALSE, //whether we need to normalize them. They are normalized already.
        2 * sizeof(float), //size of the stride, number of bytes each vertex takes,
        0 //pointer inside the vertex
    ));

    //glBindBuffer(GL_ARRAY_BUFFER, 0); //unselect the buffer

 //we create an array buffer of indices
    unsigned int ibo; //index buffer object. It has to be unsigned

    GLCall(glGenBuffers(
        1,      //number of buffers
        &ibo //buffer id
    ));

    GLCall(glBindBuffer(
        GL_ELEMENT_ARRAY_BUFFER,
        ibo //we select this buffer to work on it next.
    ));

    GLCall(glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        6 * sizeof(unsigned int),   //size in bytes
        indices,                    //the data
        GL_STATIC_DRAW              //a hint about the use we are going to make of it
    ));

    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
   
    //we create the shader by providing the two programs to our function
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);

    //we select the program. this is because we could have some different programs.
    GLCall(glUseProgram(shader));

    //we retrieve the id of the uniform inside the shader program
    GLCall(int location = glGetUniformLocation(shader, "u_Color"));
    ASSERT(location != -1); //if it wasn't present we notify as error.
    GLCall(glUniform4f(location, 0.8f, 0.3f, 0.8f, 1.0f));  //we set this color to send it to the fragment through the uniform.

    //for the purpose of the demostration we unbind everything to do this where it corresponds.
    GLCall(glBindVertexArray(0));
    GLCall(glUseProgram(0));                            //we unbind the program
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));           //we unbind the array buffer
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));   //we unbind the index buffer

    

    float r = 0.0f;
    float increment = 0.05f;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        GLCall(glUseProgram(shader));                       //we bind the program

        GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f)); //we now can set the uniform

        GLCall(glBindVertexArray(vao));                     //we bind vertex array
        
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo)); //we ind the index buffer
        
        
        GLCall(glDrawElements(
            GL_TRIANGLES,       //what we can draw with the data. In this case triangles
            6,                  //number of elements in the index array
            GL_UNSIGNED_INT,         //type of the array
            nullptr             //because it's in memory already we just put nullptr, otherwise the array of elements
        ));

        if (r > 1.0f) {
            increment = -0.05f;
        }
        else if (r <  0.0f) {
            increment = 0.05f;
        }

        r += increment;

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}