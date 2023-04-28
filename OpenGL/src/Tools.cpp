#include "Tools.h"
#include <iostream>
#include<algorithm>
#include <functional>
float generateHaltonNum(int index, int base)
{
	float f = 1;
	float r = 0;
	int current = index;
	while (current > 0)
	{
		f = f / base;
		r = r + f * (current % base);
		current = glm::floor(current / base);
	}
	return r;
}

std::vector<glm::vec2> generateHaltonSquence(int base1, int base2, int num)
{
	std::vector<glm::vec2> t;
	for (int i = 1; i <= num; i++)
	{
		t.push_back(glm::vec2(generateHaltonNum(i, base1), generateHaltonNum(i, base2)));
	}
	return t;
}
//collision
glm::vec3 Collisions::support(std::vector<glm::vec3>& vertices, glm::vec3 dir)
{
	float maxDistance = -FLT_MAX;
	int count = vertices.size();
	int maxVertex = 0;
	for (int i = 0; i < count; i++)
	{
		float distance = glm::dot(vertices[i], dir);
		if (distance > maxDistance)
		{
			maxDistance = distance;
			maxVertex = i;
		}
	}
	return vertices[maxVertex];
}
float Collisions::distancePoint2Triangle(glm::vec3 p, std::vector<glm::vec3>& vertices)
{
	glm::vec3 a = vertices[0];
	glm::vec3 b = vertices[1];
	glm::vec3 c = vertices[2];
	glm::vec3 normal = glm::normalize(glm::cross(a - c, b - c));
	return abs(glm::dot(p - c, normal));
}
float Collisions::distancePoint2Triangle(glm::vec3 p, glm::vec3 pt, glm::vec3 normal)
{
	return abs(glm::dot(p - pt, normal));
}
glm::vec3 Collisions::normalPoint2Triangle(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
	glm::vec3 normal = glm::normalize(glm::cross(a - c, b - c));
	float distance = glm::dot(p - c, normal);
	if (distance > 0)
	{
		return normal;
	}
	else
		return -normal;
}
bool Collisions::isInSimplex(std::vector<glm::vec3>& vertices)
{
	glm::vec3 Origin = glm::vec3(0);
	glm::vec3 a = vertices[0];
	glm::vec3 b = vertices[1];
	glm::vec3 c = vertices[2];
	glm::vec3 d = vertices[3];
	return (glm::dot(Origin - b, normalPoint2Triangle(a, b, c, d)) >= 0) &&
		(glm::dot(Origin - c, normalPoint2Triangle(b, a, c, d)) >= 0) &&
		(glm::dot(Origin - d, normalPoint2Triangle(c, a, b, d)) >= 0) && 
		(glm::dot(Origin - a, normalPoint2Triangle(d, a, b, c)) >= 0);
}
void Collisions::updateSimplex(std::vector<glm::vec3>& simplex)
{
	float distance = -FLT_MAX;
	std::vector<glm::vec3>::iterator maxP = simplex.begin();
	int count = simplex.size();
	for (std::vector<glm::vec3>::iterator it = simplex.begin(); it != simplex.end(); it++)
	{
		float length = glm::length(*it);
		if (length > distance)
		{
			maxP = it;
			distance = length;
		}
	}
	simplex.erase(maxP);
}
bool Collisions::GJK(std::vector<glm::vec3>& verticesA, std::vector<glm::vec3>& verticesB)
{
	glm::vec3 initDir = glm::vec3(1, 0, 0);

	glm::vec3 Origin = glm::vec3(0);
	std::vector<glm::vec3> simplex;
	glm::vec3 supportVertex = support(verticesA, initDir) - support(verticesB, -initDir);
	simplex.push_back(supportVertex);
	glm::vec3 dir = Origin - supportVertex;
	simplex.push_back(support(verticesA, dir) - support(verticesB, -dir));
	//0,1两点垂直指向原点方向
	dir = glm::cross(glm::cross(simplex[0] - simplex[1], Origin - simplex[1]), simplex[0] - simplex[1]);
	simplex.push_back(support(verticesA, dir) - support(verticesB, -dir));
	//if (distancePoint2Triangle(Origin, simplex) == 0)
	//	return true;
	int condition = 0;
	glm::vec3 newDir;
	while (condition < 8)
	{
		condition++;
		newDir = normalPoint2Triangle(Origin, simplex[0], simplex[1], simplex[2]);
		glm::vec3 suportVertex = support(verticesA, newDir) - support(verticesB, -newDir);
		
		if (distancePoint2Triangle(suportVertex, simplex) == 0)
			return false;
		simplex.push_back(suportVertex);

		if (glm::dot(suportVertex, newDir) < 0)
			return false;
		if (isInSimplex(simplex))
			return true;

		else
		{
			updateSimplex(simplex);
		}
	}
}

std::string myTools::clickDialogBox(myTools::dialogBox& box, float cursorX, float cursorY)
{
	if (cursorX > box.minX && cursorX < box.maxX && cursorY > box.minY && cursorY < box.maxY)
	{
		return box.words;
	}
	else
		return std::string("");
}