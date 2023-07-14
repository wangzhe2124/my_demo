#pragma once
#include <iostream>
#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
   
#include<fstream>
#include <string>
#include <sstream>

#include <GL/glew.h>
struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
    std::string GeometrySource;
    std::string TessContSource;
    std::string TessEvalSource;
};
class Shader
{
private:
	std::string m_FilePath;
	unsigned int m_RendererId;
	std::unordered_map<std::string, int> m_UniformLocationCache;
public:
	Shader(const std::string& filepath);
	~Shader();

	void Bind () const;
	void UnBind() const;
    void SetUniform1i(const std::string& name, int value);
    void SetUniform1b(const std::string& name, bool value);
	void SetUniform1f(const std::string& name, float value);
    void SetUniform2f(const std::string& name, float value1, float value2);
    void SetUniform2f(const std::string& name, glm::vec2 value);
	void SetUniform4f(const std::string& name, float v0, float v1, float f3, float f4);
    void SetUniform4f(const std::string& name, glm::vec4 value);
	void SetUniformmatri4fv(const std::string& name, const glm::mat4& ptr);
	void SetUniform3f(const std::string& name, float v0, float v1, float v2);
	void SetUniform3f(const std::string& name, glm::vec3 vc);
    unsigned int GetId(){return m_RendererId;}
private:
	ShaderProgramSource Parseshader();
	unsigned int CompileShader(unsigned int type,const std::string& source);
	unsigned int CreateShader(const ShaderProgramSource& source);
	int GetUniformLocation(const std::string& name);

};





class Shaders
{
public:
    Shader screenShader = Shader("res/shaders/screen.shader");
    Shader blooming_highlightshader = Shader("res/shaders/blooming_highlight.shader");
    Shader blooming_blurshader = Shader("res/shaders/blooming_blur.shader");
    Shader shadow_blurshader = Shader("res/shaders/shadow_blur.shader");
    Shader DeferedShader = Shader("res/shaders/Defered.shader");
    Shader DeferedPreShadowShader = Shader("res/shaders/DeferedPreShadow.shader");
    Shader SSAOShader = Shader("res/shaders/SSAO.shader");
    Shader SSAOBlurShader = Shader("res/shaders/SSAOblur.shader");
    Shader basic_shader = Shader("res/shaders/basic.shader");
    Shader basicscreen_shader = Shader("res/shaders/basic_screen.shader");
    Shader EnvCubeMapShader = Shader("res/shaders/EnvCubeMap.shader");
    Shader EnvCubeMap_ConvolutionShader = Shader("res/shaders/EnvCubeMap_Convolution.shader");
    Shader EnvCubeMap_spec_ConvolutionShader = Shader("res/shaders/EnvCubeMap_spec_Convolution.shader");
    Shader EnvCubeMap_spec_BRDF_Shader = Shader("res/shaders/EnvCubeMap_spec_BRDF.shader");
    Shader DeferedLighting_shader = Shader("res/shaders/DeferedLighting.shader");
    //Shader model_instance_shader = Shader("res/shaders/model_instance.shader");
    //Shader tencil = Shader("res/shaders/single.shader");
    Shader skyboxShader = Shader("res/shaders/SkyBox.shader");
    Shader DirLightShadowshader = Shader("res/shaders/DirLightShadow.shader");
    Shader PointLightShadowshader = Shader("res/shaders/PointLightShadow.shader");
    Shader SpotLightShadowshader = Shader("res/shaders/SpotLightShadow.shader");
    Shader text_shader = Shader("res/shaders/Text.shader");
    Shader billboardShader = Shader("res/shaders/billboard.shader");
    Shader D3Particle_shader = Shader("res/shaders/D3Particle.shader");
    Shader FXAA_shader = Shader("res/shaders/FXAA.shader");
    Shader MLAA_first_shader = Shader("res/shaders/MLAA_first.shader");
    Shader MLAA_second_shader = Shader("res/shaders/MLAA_second.shader");
    Shader TAA_shader = Shader("res/shaders/TAA.shader");
    Shader Terrain_cpu_shader = Shader("res/shaders/Terrain_cpu.shader");
    Shader Terrain_gpu_shader = Shader("res/shaders/Terrain_gpu.shader");
    Shader Health_bar_shader = Shader("res/shaders/Health_bar.shader");
    Shader Health_bar_enemy_shader = Shader("res/shaders/Health_bar_enemy.shader");
    //Shader realTImePBR_shader = Shader("res/shaders/realtimePBR.shader");
};