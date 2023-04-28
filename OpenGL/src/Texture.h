#pragma once
#include <iostream>
#include <GL/glew.h>
#include "vender/stb_image/stb_image.h"
#include <vector>
class Texture
{
private:
	unsigned int RenderId;
	unsigned char* Data;
	int width, height;
public:
	Texture(const std::string path, int inChannel, bool gamma = false) :RenderId(0), Data(nullptr)
	{
		glGenTextures(1, &RenderId);
		glBindTexture(GL_TEXTURE_2D, RenderId);
		BindData(path, inChannel,gamma);
	};
	void BindData(const std::string path, int inChannel, bool gamma)
	{	stbi_set_flip_vertically_on_load(1);//旋转
		int outChannel;
		Data = stbi_load(path.c_str(), &width, &height, &outChannel, inChannel);
		if (Data)
		{
			GLenum outformat{};
			if (gamma)
			{
				if (outChannel == 1)
					outformat = GL_RED;
				else if (outChannel == 3)
					outformat = GL_RGB;
				else if (outChannel == 4)
					outformat = GL_RGBA;
			}
			else
			{
				if (outChannel == 1)
					outformat = GL_RED;
				else if (outChannel == 3)
					outformat = GL_SRGB;
				else if (outChannel == 4)
					outformat = GL_SRGB_ALPHA;
			}
			GLenum informat{};
			if (inChannel == 1)
				informat = GL_RED;
			else if (inChannel == 3)
				informat = GL_RGB;
			else if (inChannel == 4 or inChannel ==0)
				informat = GL_RGBA;
			glBindTexture(GL_TEXTURE_2D, RenderId);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexImage2D(GL_TEXTURE_2D, 0, outformat, width, height, 0, informat, GL_UNSIGNED_BYTE, Data);
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//GL_LINEAR,GL_NEAREST_MIPMAP_NEAREST,GL_LINEAR_MIPMAP_NEAREST,GL_NEAREST_MIPMAP_LINEAR,GL_LINEAR_MIPMAP_LINEAR
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);//GL_NEAREST
			GLfloat max_TexAni;    //查询允许的各向异性数量
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_TexAni);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_TexAni);
			glBindTexture(GL_TEXTURE_2D, 0);
			stbi_image_free(Data);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
			stbi_image_free(Data);
		}
		stbi_set_flip_vertically_on_load(0);//旋转

	};
	~Texture()
	{
		glDeleteTextures(1, &RenderId);
	};
	int Get_width() { return width; }
	int Get_height() { return height; }
	void Bind(unsigned int slot = 0) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);//激活纹理单元
		glBindTexture(GL_TEXTURE_2D, RenderId);
	};
	void UnBind() const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	};
	void SetRepeat()
	{
		glBindTexture(GL_TEXTURE_2D, RenderId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	};
};

class HDRTexture
{
private:
	unsigned int RenderId;
	unsigned char* Data;
public:
	HDRTexture(const std::string path, int inChannel) :RenderId(0), Data(nullptr)
	{
		glGenTextures(1, &RenderId);
		glBindTexture(GL_TEXTURE_2D, RenderId);
		BindData(path, inChannel);
		glBindTexture(GL_TEXTURE_2D, 0);
	};
	void BindData(const std::string path, int inChannel)
	{	stbi_set_flip_vertically_on_load(1);//旋转
		int width, height, outChannel;
		Data = stbi_load(path.c_str(), &width, &height, &outChannel, inChannel);
		if (Data)
		{
			GLenum outformat;
			if (outChannel == 1)
				outformat = GL_RED;
			else if (outChannel == 3)
				outformat = GL_SRGB;
			else if (outChannel == 4)
				outformat = GL_SRGB_ALPHA;
			GLenum informat{};
			if (inChannel == 1)
				informat = GL_RED;
			else if (inChannel == 3)
				informat = GL_RGB;
			else if (inChannel == 4)
				informat = GL_RGBA;
			glBindTexture(GL_TEXTURE_2D, RenderId);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, informat, GL_UNSIGNED_BYTE, Data);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//GL_LINEAR,GL_NEAREST_MIPMAP_NEAREST,GL_LINEAR_MIPMAP_NEAREST,GL_NEAREST_MIPMAP_LINEAR,GL_LINEAR_MIPMAP_LINEAR
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);//GL_NEAREST
			GLfloat max_TexAni;    //查询允许的各向异性数量
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_TexAni);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_TexAni);
			glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
			stbi_image_free(Data);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
			stbi_image_free(Data);
		}
		stbi_set_flip_vertically_on_load(0);//旋转
	};
	~HDRTexture()
	{
		glDeleteTextures(1, &RenderId);
	};

	void Bind(unsigned int slot = 0) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);//激活纹理单元
		glBindTexture(GL_TEXTURE_2D, RenderId);
	};
	void UnBind() const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	};
	void SetRepeat()
	{
		glBindTexture(GL_TEXTURE_2D, RenderId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	};
};

class CubeMapTexture
{
private:
	unsigned int CubeMapId;
	unsigned char* Data;
public:
	CubeMapTexture(std::vector<std::string> faces) : CubeMapId(0), Data(nullptr)
	{
		glGenTextures(1, &CubeMapId);
		glBindTexture(GL_TEXTURE_CUBE_MAP, CubeMapId);
		int width, height, nrChannel;
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			Data = stbi_load(faces[i].c_str(), &width, &height, &nrChannel, 0);
			if (Data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, Data);
				stbi_image_free(Data);
			}
			else
			{
				std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
				stbi_image_free(Data);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	};
	CubeMapTexture& operator=(const CubeMapTexture& ct)
	{
		CubeMapId = ct.GetId();
	}
	void CopyId(const unsigned int & i)
	{
		CubeMapId = i;
	}
	unsigned int GetId() const
	{
		return CubeMapId;
	}
	void Bind(unsigned int slot = 0) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);//激活纹理单元
		glBindTexture(GL_TEXTURE_CUBE_MAP, CubeMapId);
	};
	void UnBind() const
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	};
	~CubeMapTexture()
	{
		glDeleteTextures(1, &CubeMapId);
	};
};
class Textures
{
public:
	Texture floor_diffuse = Texture("res/textures/bricks.jpg", 3);
	Texture floor_specular = Texture("res/textures/bricks.jpg", 3);
	Texture floor_normal = Texture("res/textures/bricks_normal.jpg", 3);
	Texture floor_height = Texture("res/textures/bricks_height.jpg", 3);
	HDRTexture equirectangularMap = HDRTexture("res/textures/GCanyon_C_YumaPoint_3k.hdr", 4);
	std::vector<std::string> faces
	{
		"res/textures/storm/stormydays_ft.tga",
		"res/textures/storm/stormydays_bk.tga",
		"res/textures/storm/stormydays_up.tga",
		"res/textures/storm/stormydays_dn.tga",
		"res/textures/storm/stormydays_rt.tga",
		"res/textures/storm/stormydays_lf.tga"
	};
	CubeMapTexture skybox = CubeMapTexture(faces);
	Texture Particle_texture = Texture("res/textures/particle_2.png", 3);
	Texture Terrain_texture = Texture("res/textures/iceland_heightmap.png", 1);
};
#pragma endregion