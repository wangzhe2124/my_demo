#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
class View
{
private:
	glm::mat4 view_matrix;
public:
	View();
	~View();
	inline glm::mat4 GetView() { return view_matrix; }
	void ChangeCamera(const glm::vec3& CameraPostion);
};

View::View() :view_matrix(glm::mat4(1.0f))
{

}

View::~View()
{
}

void View::ChangeCamera(const glm::vec3& CameraPostion)
{
	view_matrix = glm::lookAt(CameraPostion, CameraPostion + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0, 1.0, 0.0)); //camera坐标原点也要变化且与视角方向一致
}
