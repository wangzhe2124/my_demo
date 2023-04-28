#pragma once
#include "Camera.h"

class Frustum {
private:
    float fov;
    float ratio;//��߱�
    float near;
    float far;
public:
    std::vector<glm::vec3> m_points = std::vector<glm::vec3>(8);//ƽ��ͷ��8����
    Frustum();
    ~Frustum();
    void set(float t_fov, float t_ratio, float t_near, float t_far);
    float Getfov() const { return fov; }
    float Getratio() const { return ratio; }
    float Getnear() const { return near; }
    float Getfar() const { return far; }

    void Setnear(float t_near);
    void Setfar(float t_far);
    void Setfov(float t_fov);
    void Setratio(float t_ratio);
};

class CSM_Dirlight {
private:
    int splitNum;//��׶��Ƭ��
    float lambda;//����Ȩ��
    std::vector<Frustum> camera_frustums = std::vector<Frustum>(splitNum);
    std::vector<float> z_distance = std::vector<float>(splitNum);
    std::vector<float> xy_distance = std::vector<float>(splitNum);
    float far_plane_distance;
public:
    CSM_Dirlight(Camera& camera, int splitnum = 4, float weight = 0.99f) :splitNum(splitnum), lambda(weight)
    {
        float camera_fov = camera.Zoom;
        float ratio = 1.0;
        //��΢���ƫ��(����Ϊ0.2)�������ڱ߿���α��
        for (int i = 0; i < splitNum; i++) {
            camera_frustums[i].Setfov(camera_fov);
            camera_frustums[i].Setratio(ratio);
        }
        //������������߿ռ���ÿ��ƽ��ͷ����Ƭ�Ľ������Զ����
    }
    ~CSM_Dirlight();
    glm::mat4 lightView;//��ռ�view����
    std::vector<glm::mat4> light_orth_matrix = std::vector<glm::mat4>(splitNum);//��Ƭ��ռ�任����(ͶӰ*�۲�)
    std::vector<glm::mat4> light_projection_matrix = std::vector<glm::mat4>(splitNum);//��ռ�ͶӰ����
    //����CSM�������Դ(��Ƭ��ռ�任�����ۿռ䵽��ռ�任����)
    void split_camera_frustum(Camera& camera);
    float Get_frustum_far(int i)
    {
        return camera_frustums[i].Getfar();
    }
    //��������ռ��е������׶��Ƭ�߽��
    void camera_frustum_points(Camera& camera);
    //���ɹ���׶��Ƭ�Ĺ�ռ�任����
    void light_projection();
    void Get_light_projection(Camera& camera, const glm::vec3& DirlightPosition);
    float Get_z_distance(int i) { return z_distance[i]; }
    float Get_xy_distance(int i) { return xy_distance[i]; }
    void Set_far_plane(float fp) { far_plane_distance = fp; }
    float Get_far_plane() { return far_plane_distance; }
    std::vector<glm::vec3> GetCameraFrustum(int i)
    {
        return camera_frustums[i].m_points;
    }
};
