#include "Renderer.h"

void Renderer::Clear(unsigned int cl) const
{
    switch (cl)
    {
    case 0:glClear(GL_COLOR_BUFFER_BIT);
    case 1:glClear(GL_DEPTH_BUFFER_BIT);
    case 2:glClear(GL_STENCIL_BUFFER_BIT);
    }

}

void Renderer::ClearColor(float c1, float c2, float c3, float c4) const
{
    glClearColor(c1, c2, c3, c4);
}

void Renderer::DrawElement(const VertexArray& va, const IndexBuffer& ib, Shader& shader) const
{
    va.Bind();
    ib.Bind();
    shader.Bind();
    glDrawElements(GL_TRIANGLE_STRIP, ib.GetCount(), GL_UNSIGNED_INT, nullptr);
    va.Unbind();
    ib.Unbind();

}


void Renderer::DrawArray(const VertexArray& va, const Shader& shader) const
{
    va.Bind();
    shader.Bind();
    glDrawArrays(GL_TRIANGLES, 0, va.GetNumber());
    va.Unbind();

}

void Renderer::DrawQuads(const VertexArray& va, const Shader& shader) const
{
    va.Bind();
    shader.Bind();
    glDrawArrays(GL_QUADS, 0, va.GetNumber());
    va.Unbind();

}
void Renderer::DrawArray_strip(const VertexArray& va, const Shader& shader) const
{
    va.Bind();
    shader.Bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, va.GetNumber());
    va.Unbind();

}
void Renderer::DrawPoint(const VertexArray& va, const Shader& shader) const
{
    va.Bind();
    shader.Bind();
    glDrawArrays(GL_POINTS, 0, va.GetNumber());
    va.Unbind();

}
void Renderer::DrawArrayInstance(const VertexArray& va, const Shader& shader, unsigned int num) const
{
    va.Bind();
    shader.Bind();
    glDrawArraysInstanced(GL_TRIANGLES, 0, va.GetNumber(), num);
    va.Unbind();

}
