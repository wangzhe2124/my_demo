#pragma once
#include "VertexArray.h"

VertexArray::VertexArray() :Number(0), j(0)
{
	glGenVertexArrays(1, &m_RendererId);
	glBindVertexArray(m_RendererId);

}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &m_RendererId);
}

VertexArray::VertexArray(VertexArray& va)
{
	m_RendererId = va.GetId();
	Number = va.GetNum();
	j = va.GetJ();
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
	Bind();
	vb.Bind();
	const auto& elements = layout.GetElements();
	unsigned int offset = 0;
	for (unsigned int i = 0; i < elements.size(); i++)
	{
		const auto &element = elements[i];
		glEnableVertexAttribArray(i+j);
		glVertexAttribPointer(i + j, element.count, element.type, element.normalize, layout.GetStride(), (const void*)offset);
		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
	}
	j += (unsigned int)elements.size();
	Number += vb.Get_Number()/layout.GetStride();
	vb.Unbind();
	Unbind();
}

void VertexArray::Bind() const
{
	glBindVertexArray(m_RendererId);
}

void VertexArray::Unbind() const
{
	glBindVertexArray(0);

}