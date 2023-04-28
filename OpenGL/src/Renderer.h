#pragma once
#include "Shader.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

class Renderer
{
public:
	void Clear(unsigned int cl) const;
	void ClearColor(float c1, float c2, float c3, float c4) const;
	void DrawElement(const VertexArray& va, const IndexBuffer& ib, Shader& shader) const;
	//void DrawMeshElement(Mesh& mesh, const Shader& shader) const;

    void DrawArray(const VertexArray& va, const Shader& shader) const;
	void DrawQuads(const VertexArray& va, const Shader& shader) const;
    void DrawArray_strip(const VertexArray& va, const Shader& shader) const;

    void DrawArrayInstance(const VertexArray& va, const Shader& shader, unsigned int num) const;
    void DrawPoint(const VertexArray& va, const Shader& shader) const;
	//void DrawMeshArray(Mesh& mesh,  const Shader& shader) const;
	
};

