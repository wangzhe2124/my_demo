#pragma once
#include <GL/glew.h>
class VertexBuffer
{
private:
	unsigned int m_RendererId;
	unsigned int Number;
public:
	VertexBuffer();
	VertexBuffer(unsigned int size);

    VertexBuffer(const void* data, unsigned int size);
	void BindData(const void* data, unsigned int size);
	VertexBuffer operator=(const VertexBuffer& vb);
	~VertexBuffer();

	void Bind() const;
	void Unbind() const;
	inline unsigned int Get_Number() const { return Number; }
	unsigned int GetId() const { return m_RendererId; }
};



class VertexBuffers
{
public:
	VertexBuffer textVb = VertexBuffer(sizeof(GLfloat) * 6 * 4);
};
//class UniformBuffer
//{
//private:
//	unsigned int UniformBufferId;
//	unsigned int stride;
//public:
//	UniformBuffer(unsigned int size, unsigned int slot);
//	~UniformBuffer();
//
//	template<typename T>
//	void PushUniform(T t)
//	{
//		glBindBuffer(GL_UNIFORM_BUFFER, UniformBufferId);
//		glBufferSubData(GL_UNIFORM_BUFFER, stride, sizeof(t), &t);
//		stride += sizeof(t);
//		glBindBuffer(GL_UNIFORM_BUFFER, 0);
//	}
//	template<>
//	void PushUniform(float f)
//	{
//		glBindBuffer(GL_UNIFORM_BUFFER, UniformBufferId);
//		glBufferSubData(GL_UNIFORM_BUFFER, stride, sizeof(f), &f);
//		stride += sizeof(f);
//		glBindBuffer(GL_UNIFORM_BUFFER, 0);
//	}
//	template<>
//	void PushUniform(int i)
//	{
//		glBindBuffer(GL_UNIFORM_BUFFER, UniformBufferId);
//		glBufferSubData(GL_UNIFORM_BUFFER, stride, sizeof(i), &i);
//		stride += 4;
//		glBindBuffer(GL_UNIFORM_BUFFER, 0);
//	}
//	template<>
//	void PushUniform(glm::vec3 v)
//	{
//		glBindBuffer(GL_UNIFORM_BUFFER, UniformBufferId);
//		glBufferSubData(GL_UNIFORM_BUFFER, stride, sizeof(v), glm::value_ptr(v));
//		stride += 16;
//		glBindBuffer(GL_UNIFORM_BUFFER, 0);
//	}
//	template<>
//	void PushUniform(glm::mat4 m)
//	{
//		glBindBuffer(GL_UNIFORM_BUFFER, UniformBufferId);
//		glBufferSubData(GL_UNIFORM_BUFFER, stride, sizeof(m), glm::value_ptr(m));
//		stride += sizeof(m);
//		glBindBuffer(GL_UNIFORM_BUFFER, 0);
//	}
//};
//UniformBuffer::UniformBuffer(unsigned int size, unsigned int slot) :stride(0)
//{
//    glGenBuffers(1, &UniformBufferId);
//    glBindBuffer(GL_UNIFORM_BUFFER, UniformBufferId);
//    glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW); // 分配size字节的内存
//    
//    glBindBuffer(GL_UNIFORM_BUFFER, 0);
//	glBindBufferRange(GL_UNIFORM_BUFFER, slot, UniformBufferId, 0, size);
//}
//UniformBuffer::~UniformBuffer()
//{
//    glDeleteBuffers(1, &UniformBufferId);
//}