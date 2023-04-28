#pragma once
#include "Camera.h"

class Frustum {
private:
    float fov;
    float ratio;//宽高比
    float near;
    float far;
public:
    std::vector<glm::vec3> m_points = std::vector<glm::vec3>(8);//平截头体8个点
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
    int splitNum;//视锥分片数
    float lambda;//划分权重
    std::vector<Frustum> camera_frustums = std::vector<Frustum>(splitNum);
    std::vector<float> z_distance = std::vector<float>(splitNum);
    std::vector<float> xy_distance = std::vector<float>(splitNum);
    float far_plane_distance;
public:
    CSM_Dirlight(Camera& camera, int splitnum = 4, float weight = 0.99f) :splitNum(splitnum), lambda(weight)
    {
        float camera_fov = camera.Zoom;
        float ratio = 1.0;
        //略微添加偏移(这里为0.2)，避免在边框处有伪像
        for (int i = 0; i < splitNum; i++) {
            camera_frustums[i].Setfov(camera_fov);
            camera_frustums[i].Setratio(ratio);
        }
        //计算摄像机视线空间中每个平截头体切片的近距离和远距离
    }
    ~CSM_Dirlight();
    glm::mat4 lightView;//光空间view矩阵
    std::vector<glm::mat4> light_orth_matrix = std::vector<glm::mat4>(splitNum);//分片光空间变换矩阵(投影*观察)
    std::vector<glm::mat4> light_projection_matrix = std::vector<glm::mat4>(splitNum);//光空间投影矩阵
    //生成CSM所需的资源(分片光空间变换矩阵，眼空间到光空间变换矩阵)
    void split_camera_frustum(Camera& camera);
    float Get_frustum_far(int i)
    {
        return camera_frustums[i].Getfar();
    }
    //计算世界空间中的相机视锥切片边界点
    void camera_frustum_points(Camera& camera);
    //生成光视锥切片的光空间变换矩阵
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
