#include "VertexBuffer.h"
#include "Renderer.h"

VertexBuffer::VertexBuffer(const void* data, unsigned int size)
{
    GLCall(glGenBuffers(
        1,      //number of buffers
        &m_RendererID //buffer id
    ));

    GLCall(glBindBuffer(
        GL_ARRAY_BUFFER,
        m_RendererID //we select this buffer to work on it next.
    ));

    GLCall(glBufferData(
        GL_ARRAY_BUFFER,
        size,  //size in bytes
        data,          //the data
        GL_STATIC_DRAW      //a hint about the use we are going to make of it
    ));
}

VertexBuffer::~VertexBuffer()
{
    GLCall(glDeleteBuffers(1, &m_RendererID));
}

void VertexBuffer::Bind() const
{
    GLCall(glBindBuffer(
        GL_ARRAY_BUFFER,
        m_RendererID //we select this buffer to work on it next.
    ));
}

void VertexBuffer::UnBind() const
{
    GLCall(glBindBuffer(
        GL_ARRAY_BUFFER,
        0 //to unbind
    ));
}
