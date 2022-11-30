#include "IndexBuffer.h"
#include "Renderer.h"

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count)
    : m_Count(count)
{
    ASSERT(sizeof(unsigned int) == sizeof(GLuint));

    GLCall(glGenBuffers(
        1,      //number of buffers
        &m_RendererID //buffer id
    ));

    GLCall(glBindBuffer(
        GL_ELEMENT_ARRAY_BUFFER,
        m_RendererID //we select this buffer to work on it next.
    ));

    GLCall(glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        count * sizeof(unsigned int),  //size in bytes
        data,          //the data
        GL_STATIC_DRAW      //a hint about the use we are going to make of it
    ));
}

IndexBuffer::~IndexBuffer()
{
    GLCall(glDeleteBuffers(1, &m_RendererID));
}

void IndexBuffer::Bind() const
{
    GLCall(glBindBuffer(
        GL_ELEMENT_ARRAY_BUFFER,
        m_RendererID //we select this buffer to work on it next.
    ));
}

void IndexBuffer::UnBind() const
{
    GLCall(glBindBuffer(
        GL_ELEMENT_ARRAY_BUFFER,
        0 //to unbind
    ));
}
