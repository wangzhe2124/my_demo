#pragma once
class IndexBuffer
{
private:
	unsigned int m_RendererId;
	unsigned int m_Count;
public:
	IndexBuffer();
	IndexBuffer(const unsigned int* data, unsigned int count);
	void BindData(const unsigned int* data, unsigned int count);
	IndexBuffer operator=(const IndexBuffer& vb);

	~IndexBuffer();

	void Bind() const;
	void Unbind() const;
	inline unsigned int GetCount() const { return m_Count; }
};


