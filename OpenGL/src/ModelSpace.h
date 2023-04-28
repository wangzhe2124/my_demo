#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
class ModelSpace
{
private:
	glm::mat4 modelspace;
public:
	ModelSpace() :modelspace(glm::mat4(1.0f))
	{};
	ModelSpace operator=(const ModelSpace& ms)
	{ 
		this->modelspace = ms.modelspace; 
		return *this;
	}
	void SetModel(const glm::mat4& mo)
	{
		modelspace = mo;
	};
	void Translate(glm::vec3 tr)
	{
		modelspace = glm::translate(modelspace, tr);
	};
	void Rotate(float angle, const glm::vec3& axis)
	{
		modelspace = glm::rotate(modelspace, (float)glm::radians(angle), axis);
	};
	void Scale(const glm::vec3& sc) 
	{
		modelspace = glm::scale(modelspace, sc);
	};
	void Scale(float sc)
	{
		modelspace = glm::scale(modelspace, glm::vec3(sc));
	};
	inline glm::mat4 GetModelSpace() { return modelspace; }
	float GetVector(int i, int j) { return modelspace[i][j]; }
};


