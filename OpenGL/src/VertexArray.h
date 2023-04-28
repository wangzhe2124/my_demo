#pragma once

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
class VertexArray
{
private:
	unsigned int m_RendererId;
	unsigned int Number;
	unsigned int j;
public:
	VertexArray();
	VertexArray(VertexArray& va);
	~VertexArray();
	inline unsigned int GetNumber() const { return Number; }
	void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);
	
	void Bind() const;
	void Unbind() const;
	unsigned int GetId() { return m_RendererId; }
	unsigned int GetNum() { return Number; }
	unsigned int GetJ() { return j; }
};


class VertexArrays
{
public:
	VertexArray quadVa;
	VertexArray cubeVa;
	VertexArray cubeQuadVa;
	VertexArray skyboxVa;
	VertexArray textVa;
	VertexArray billboardVa;
};