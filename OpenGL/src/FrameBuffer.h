#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <random>
class CameraDepthFBO
{
private:
	unsigned int frameBufferId;
	unsigned int DepthTextureId;
	unsigned int c_screenWidth;
	unsigned int c_screenHeight;
public:
	CameraDepthFBO(unsigned int scr_width, unsigned int scr_height) :c_screenWidth(scr_width), c_screenHeight(scr_height)
	{
		glGenFramebuffers(1, &frameBufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
		BindTextureBuffer();
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	void BindTextureBuffer()
	{
		glGenTextures(1, &DepthTextureId);
		glBindTexture(GL_TEXTURE_2D, DepthTextureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, c_screenWidth, c_screenHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);//只需要深度
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthTextureId, 0);
	}
	~CameraDepthFBO()
	{
		glDeleteFramebuffers(1, &frameBufferId);
		glDeleteFramebuffers(1, &DepthTextureId);
	};
	void Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
	};
	void UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	void BindTexture(unsigned int slot = 0)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, DepthTextureId);
	};
	void Read()
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBufferId);
	}
	void Write()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferId);
	};
	void SetViewPort()
	{
		glViewport(0, 0, c_screenWidth, c_screenHeight);
	}
};
struct SSAO_DATA
{
	std::vector<glm::vec3> ssaokernel;
	std::vector<glm::vec3> ssaonoise;
};
static SSAO_DATA Get_SSAO_SAMPLE()
{
	std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // 随机浮点数，范围0.0 - 1.0
	std::default_random_engine generator;
	std::vector<glm::vec3> ssaoKernel;
	for (GLuint i = 0; i < 64; ++i)
	{
		glm::vec3 sample(
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator)
		);
		sample = glm::normalize(sample);
		sample *= randomFloats(generator);
		float scale = float(i) / 64.0f;
		scale = 0.1f + 0.9f * scale;
		sample *= scale;
		ssaoKernel.push_back(sample);
	}
	std::vector<glm::vec3> ssaoNoise;
	for (GLuint i = 0; i < 16; i++)
	{
		glm::vec3 noise(
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			0.0f);
		ssaoNoise.push_back(noise);
	}
	SSAO_DATA ssaodata = { ssaoKernel, ssaoNoise };
	return ssaodata;
}
class SSAOFBO
{
private:
	unsigned int frameBufferId;
	unsigned int ssaoTextureId;
	unsigned int noiseTextureId;
	unsigned int c_screenWidth;
	unsigned int c_screenHeight;
	
public:
	SSAO_DATA  ssao_data;
	SSAOFBO(unsigned int scr_width, unsigned int scr_height) :c_screenWidth(scr_width), c_screenHeight(scr_height)
	{
		ssao_data = Get_SSAO_SAMPLE();
		glGenFramebuffers(1, &frameBufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
		BindTextureBuffer();
		BindNoiseBuffer();

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	};
	void BindTextureBuffer()
	{
		glGenTextures(1, &ssaoTextureId);
		glBindTexture(GL_TEXTURE_2D, ssaoTextureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, c_screenWidth, c_screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoTextureId, 0);
	}
	void BindNoiseBuffer()
	{

		glGenTextures(1, &noiseTextureId);
		glBindTexture(GL_TEXTURE_2D, noiseTextureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssao_data.ssaonoise[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, noiseTextureId, 0);
	}

	~SSAOFBO()
	{
		glDeleteFramebuffers(1, &frameBufferId);
		glDeleteFramebuffers(1, &ssaoTextureId);
		glDeleteFramebuffers(1, &noiseTextureId);

	};
	void Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
	};
	void UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	void BindTexture(unsigned int slot = 0)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, ssaoTextureId);
	};
	void BindNoiseTexture(unsigned int slot = 0)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, noiseTextureId);
	};

	void Write()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferId);
	};
	void SetViewPort()
	{
		glViewport(0, 0, c_screenWidth, c_screenHeight);
	}
};
class SSAOBlurFBO
{
private:
	unsigned int frameBufferId;
	unsigned int ssaoBlurTextureId;
	unsigned int c_screenWidth;
	unsigned int c_screenHeight;
public:
	SSAOBlurFBO(unsigned int scr_width, unsigned int scr_height) :c_screenWidth(scr_width), c_screenHeight(scr_height)
	{
		glGenFramebuffers(1, &frameBufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
		BindTextureBuffer();
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	void BindTextureBuffer()
	{
		glGenTextures(1, &ssaoBlurTextureId);
		glBindTexture(GL_TEXTURE_2D, ssaoBlurTextureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, c_screenWidth, c_screenHeight, 0, GL_RGB, GL_FLOAT, NULL);//只需要深度
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoBlurTextureId, 0);
	}


	~SSAOBlurFBO()
	{
		glDeleteFramebuffers(1, &frameBufferId);
		glDeleteFramebuffers(1, &ssaoBlurTextureId);

	};
	void Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
	};
	void UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	void BindTexture(unsigned int slot = 0)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, ssaoBlurTextureId);
	};

	void Write()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferId);
	};
	void SetViewPort()
	{
		glViewport(0, 0, c_screenWidth, c_screenHeight);
	}
};

class Blooming_HighlightFBO
{
private:
	unsigned int frameBufferId;
	unsigned int textureColorId[2];
	unsigned int RenderBufferId;
	unsigned int c_screenWidth;
	unsigned int c_screenHeight;
public:
	Blooming_HighlightFBO(unsigned int scr_width, unsigned int scr_height) :c_screenWidth(scr_width), c_screenHeight(scr_height)
	{
		glGenFramebuffers(1, &frameBufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
		BindTextureBuffer();
		//BindRenderBuffer();
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	void BindTextureBuffer()
	{
		glGenTextures(2, textureColorId);
		for (int i = 0; i < 2; i++)
		{

			glBindTexture(GL_TEXTURE_2D, textureColorId[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, c_screenWidth, c_screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);//使用高精度32F颜色缓冲HDR
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textureColorId[i], 0);
		}
		GLuint colorattachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, colorattachments);
	};
	void BindRenderBuffer()
	{
		glGenRenderbuffers(1, &RenderBufferId);
		glBindRenderbuffer(GL_RENDERBUFFER, RenderBufferId);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, c_screenWidth, c_screenHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RenderBufferId);
	};
	~Blooming_HighlightFBO()
	{
		glDeleteFramebuffers(1, &frameBufferId);
		glDeleteFramebuffers(1, textureColorId);
	};
	void Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
	};
	void UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	void BindTexture(unsigned int slot = 0, int i = 0)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, textureColorId[i]);
	};
	void Write()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferId);
	};
	void SetViewPort()
	{
		glViewport(0, 0, c_screenWidth, c_screenHeight);
	}
};

class basic_FBO
{
private:
	unsigned int frameBufferId;
	unsigned int textureColorId;
	unsigned int RenderBufferId;
	unsigned int c_screenWidth;
	unsigned int c_screenHeight;
	GLenum precision;
	GLenum type;
public:
	basic_FBO(unsigned int scr_width, unsigned int scr_height, int p = 0, int t = 0) :c_screenWidth(scr_width), c_screenHeight(scr_height)
	{
		if (p == 0)
		{
			precision = GL_RGBA16F;
		}
		else
			precision = GL_RG;
		if (t == 0)
			type = GL_FLOAT;
		else
			type = GL_BYTE;
		glGenFramebuffers(1, &frameBufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
		BindTextureBuffer();
		//BindRenderBuffer();
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	void BindTextureBuffer()
	{
		glGenTextures(1, &textureColorId);
		glBindTexture(GL_TEXTURE_2D, textureColorId);
		glTexImage2D(GL_TEXTURE_2D, 0, precision, c_screenWidth, c_screenHeight, 0, GL_RGBA, type, NULL);//使用高精度32F颜色缓冲HDR
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorId, 0);

	};
	void BindRenderBuffer()
	{
		glGenRenderbuffers(1, &RenderBufferId);
		glBindRenderbuffer(GL_RENDERBUFFER, RenderBufferId);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, c_screenWidth, c_screenHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RenderBufferId);
	};
	~basic_FBO()
	{
		glDeleteFramebuffers(1, &frameBufferId);
		glDeleteFramebuffers(1, &textureColorId);
	};
	void Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
	};
	void UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	void BindTexture(unsigned int slot = 0)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, textureColorId);
	};
	void Write()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferId);
	};
	void WriteTexture()
	{
		glBindTexture(GL_TEXTURE_2D, textureColorId);
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, c_screenWidth, c_screenHeight);
	};
	void Read()
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBufferId);
	};

	void SetViewPort()
	{
		glViewport(0, 0, c_screenWidth, c_screenHeight);
	}
};
class GBuffer
{
private:
	unsigned int frameBufferId;
	unsigned int textureColorId[4];
	unsigned int RenderBufferId;
	bool with_RBO;
	unsigned int c_screenWidth;
	unsigned int c_screenHeight;
public:
	GBuffer(unsigned int scr_width, unsigned int scr_height, bool with_rbo = true) :c_screenWidth(scr_width), c_screenHeight(scr_height),with_RBO(with_rbo)
	{
		glGenFramebuffers(1, &frameBufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
		BindTextureBuffer();
		BindRenderBuffer();
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	void BindTextureBuffer()
	{
		glGenTextures(4, textureColorId);
		for (int i = 0; i < 3; i++)
		{
			glBindTexture(GL_TEXTURE_2D, textureColorId[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, c_screenWidth, c_screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);//必须32
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			GLfloat max_TexAni;    //查询允许的各向异性数量
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_TexAni);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_TexAni);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textureColorId[i], 0);
		}
		{
			glBindTexture(GL_TEXTURE_2D, textureColorId[3]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, c_screenWidth, c_screenHeight, 0, GL_RED, GL_FLOAT, NULL);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + 3, GL_TEXTURE_2D, textureColorId[3], 0);
		}
		GLuint colorattachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
		glDrawBuffers(4, colorattachments);
	};
	void BindRenderBuffer()
	{
		glGenRenderbuffers(1, &RenderBufferId);
		glBindRenderbuffer(GL_RENDERBUFFER, RenderBufferId);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, c_screenWidth, c_screenHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RenderBufferId);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	};
	~GBuffer()
	{
		glDeleteFramebuffers(1, &frameBufferId);
		glDeleteFramebuffers(1, textureColorId);
		glDeleteFramebuffers(1, &RenderBufferId);
	};
	void Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
	};
	void UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	void BindTexture(unsigned int slot = 0, int i = 0)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, textureColorId[i]);
	};
	void BindDepth(unsigned int slot = 0, int i = 0)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, RenderBufferId);
	};
	void Write()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferId);
	};
	void WriteTexture(int i = 0)
	{
		glBindTexture(GL_TEXTURE_2D, textureColorId[i]);
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, c_screenWidth, c_screenHeight);

	};
	void unWrite()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	};
	void Read()
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBufferId);
	}
	void unRead()
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	}
	void BlitDepthBuffer()
	{
		glBlitFramebuffer(0, 0, c_screenWidth, c_screenHeight, 0, 0, c_screenWidth, c_screenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	};

	void SetViewPort()
	{
		glViewport(0, 0, c_screenWidth, c_screenHeight);
	}
};
class preShadowBuffer
{
private:
	unsigned int frameBufferId;
	unsigned int textureColorId[2];
	unsigned int c_screenWidth;
	unsigned int c_screenHeight;
public:
	preShadowBuffer(unsigned int scr_width, unsigned int scr_height) :c_screenWidth(scr_width), c_screenHeight(scr_height)
	{
		glGenFramebuffers(1, &frameBufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
		BindTextureBuffer();
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	void BindTextureBuffer()
	{
		//1
		glGenTextures(2, textureColorId);
		glBindTexture(GL_TEXTURE_2D, textureColorId[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, c_screenWidth, c_screenHeight, 0, GL_RGBA, GL_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorId[0], 0);
		//2
		glBindTexture(GL_TEXTURE_2D, textureColorId[1]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, c_screenWidth, c_screenHeight, 0, GL_RGBA, GL_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, textureColorId[1], 0);
		GLuint colorattachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
		glDrawBuffers(2, colorattachments);
	};

	~preShadowBuffer()
	{
		glDeleteFramebuffers(1, &frameBufferId);
		glDeleteFramebuffers(1, textureColorId);

	};
	void Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
	};
	void UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	void BindTexture(unsigned int slot = 0, int i = 0)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, textureColorId[i]);
	};

	void Write()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferId);
	};
	void WriteTexture(int i = 0)
	{
		glBindTexture(GL_TEXTURE_2D, textureColorId[i]);
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, c_screenWidth, c_screenHeight);

	};
	void unWrite()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	};
	void Read()
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBufferId);
	}
	void unRead()
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	}
	void BlitDepthBuffer()
	{
		glBlitFramebuffer(0, 0, c_screenWidth, c_screenHeight, 0, 0, c_screenWidth, c_screenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	};

	void SetViewPort()
	{
		glViewport(0, 0, c_screenWidth, c_screenHeight);
	}
};
class MSAAFrameBuffer
{
private:
	unsigned int frameBufferId;
	unsigned int textureColorId;

	unsigned int renderBufferId;
	unsigned int c_screenWidth;
	unsigned int c_screenHeight;
	unsigned int SampleNumber;
public:
	MSAAFrameBuffer(unsigned int scr_width, unsigned int scr_height, unsigned int SN) : c_screenWidth(scr_width), c_screenHeight(scr_height), SampleNumber(SN)
	{
		glGenFramebuffers(1, &frameBufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
		BindTextureBuffer();
		BindRenderBuffer();
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	void BindTextureBuffer()
	{
		glGenTextures(1, &textureColorId);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureColorId);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, SampleNumber, GL_RGB32F, c_screenWidth, c_screenHeight, GL_TRUE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, textureColorId, 0);

	};
	void BindRenderBuffer()
	{
		glGenRenderbuffers(1, &renderBufferId);
		glBindRenderbuffer(GL_RENDERBUFFER, renderBufferId);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, SampleNumber, GL_DEPTH_COMPONENT, c_screenWidth, c_screenHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBufferId);
	};
	~MSAAFrameBuffer()
	{
		glDeleteFramebuffers(1, &frameBufferId);
		glDeleteFramebuffers(1, &textureColorId);
		glDeleteFramebuffers(1, &renderBufferId);
	};
	void Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
	};
	void UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	void BindTexture()
	{
		glBindTexture(GL_TEXTURE_2D, textureColorId);
	};

	void BlitBuffer()
	{
		glBlitFramebuffer(0, 0, c_screenWidth, c_screenHeight, 0, 0, c_screenWidth, c_screenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	};
	void Read()
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBufferId);
	};
	void Write()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferId);
	};
};
class HDRFBO
{
private:
	unsigned int frameBufferId;
	unsigned int textureColorId;
	unsigned int RenderBufferId;

	unsigned int c_screenWidth;
	unsigned int c_screenHeight;
public:
	HDRFBO(unsigned int scr_width, unsigned int scr_height) :c_screenWidth(scr_width), c_screenHeight(scr_height)
	{
		glGenFramebuffers(1, &frameBufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
		BindTextureBuffer();
		BindRenderBuffer();
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	void BindTextureBuffer()
	{
		glGenTextures(1, &textureColorId);
		for (int i = 0; i < 1; i++)
		{
			glBindTexture(GL_TEXTURE_2D, textureColorId);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, c_screenWidth, c_screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);//使用高精度32F颜色缓冲HDR
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textureColorId, 0);
		}

	};
	void BindRenderBuffer()
	{
		glGenRenderbuffers(1, &RenderBufferId);
		glBindRenderbuffer(GL_RENDERBUFFER, RenderBufferId);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, c_screenWidth, c_screenHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RenderBufferId);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	};
	~HDRFBO()
	{
		glDeleteFramebuffers(1, &frameBufferId);
		glDeleteFramebuffers(1, &textureColorId);
		glDeleteFramebuffers(1, &RenderBufferId);
	};
	void Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
	};
	void UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	void BindTexture(unsigned int slot = 0)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, textureColorId);
	};
	void BindDepth(unsigned int slot = 0, int i = 0)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, RenderBufferId);
	};
	void Write()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferId);
	};
	void unWrite()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	};
	void Read()
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBufferId);
	}
	void unRead()
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	}
	void BlitDepthBuffer()
	{
		glBlitFramebuffer(0, 0, c_screenWidth, c_screenHeight, 0, 0, c_screenWidth, c_screenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	};

	void SetViewPort()
	{
		glViewport(0, 0, c_screenWidth, c_screenHeight);
	}
};


class DirLightDepthMapFBO
{
private:
	unsigned int frameBufferId;
	unsigned int DepthTextureId;
	unsigned int RenderBufferId;
	unsigned int c_screenWidth;//分辨率
	unsigned int c_screenHeight;
public:
	DirLightDepthMapFBO(unsigned int c_screenWidth, unsigned int c_screenHeight) :c_screenWidth(c_screenWidth), c_screenHeight(c_screenHeight)
	{
		glGenFramebuffers(1, &frameBufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
		BindTextureBuffer();
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		//BindRenderBuffer();
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void BindTextureBuffer()
	{
		glGenTextures(1, &DepthTextureId);
		glBindTexture(GL_TEXTURE_2D, DepthTextureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, c_screenWidth, c_screenHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);//只需要深度
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glGenerateMipmap(GL_TEXTURE_2D);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthTextureId, 0);
	}
	void BindRenderBuffer()
	{
		glGenRenderbuffers(1, &RenderBufferId);
		glBindRenderbuffer(GL_RENDERBUFFER, RenderBufferId);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, c_screenWidth, c_screenHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RenderBufferId);
	};
	~DirLightDepthMapFBO()
	{
		glDeleteFramebuffers(1, &frameBufferId);
		glDeleteFramebuffers(1, &DepthTextureId);
	}
	void Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);

	}
	void UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void BindTexture(unsigned int slot = 0)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, DepthTextureId);
	}
	void SetViewPort()
	{
		glViewport(0, 0, c_screenWidth, c_screenHeight);
	}
};


class PointLightDepthMapFBO
{
private:
	unsigned int frameBufferId;
	unsigned int DepthTextureId;
	unsigned int c_screenWidth;//分辨率
	unsigned int c_screenHeight;
public:
	PointLightDepthMapFBO(unsigned int c_screenWidth, unsigned int c_screenHeight) :c_screenWidth(c_screenWidth), c_screenHeight(c_screenHeight)
	{
		glGenFramebuffers(1, &frameBufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
		BindTextureBuffer();
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
	}
	void BindTextureBuffer()
	{
		glGenTextures(1, &DepthTextureId);
		glBindTexture(GL_TEXTURE_CUBE_MAP, DepthTextureId);
		for (GLuint i = 0; i < 6; ++i)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, c_screenWidth, c_screenHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);//只需要深度
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, DepthTextureId, 0);
	}
	~PointLightDepthMapFBO()
	{
		glDeleteFramebuffers(1, &frameBufferId);
		glDeleteFramebuffers(1, &DepthTextureId);
	}
	void Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);

	}
	void UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void BindTexture(unsigned int slot = 0)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_CUBE_MAP, DepthTextureId);
	}
	void SetViewPort()
	{
		glViewport(0, 0, c_screenWidth, c_screenHeight);
	}
};

class SpotLightDepthMapFBO
{
private:
	unsigned int frameBufferId;
	unsigned int DepthTextureId;
	unsigned int RenderBufferId;
	unsigned int c_screenWidth;//分辨率
	unsigned int c_screenHeight;
public:
	SpotLightDepthMapFBO(unsigned int c_screenWidth, unsigned int c_screenHeight) :c_screenWidth(c_screenWidth), c_screenHeight(c_screenHeight)
	{
		glGenFramebuffers(1, &frameBufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
		BindTextureBuffer();
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	}
	void BindTextureBuffer()
	{
		glGenTextures(1, &DepthTextureId);
		glBindTexture(GL_TEXTURE_2D, DepthTextureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, c_screenWidth, c_screenHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);//只需要深度
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthTextureId, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		GLfloat borderColor[] = { 0.0, 0.0, 0.0, 0.0 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	}

	~SpotLightDepthMapFBO()
	{
		glDeleteFramebuffers(1, &frameBufferId);
		glDeleteFramebuffers(1, &DepthTextureId);
	}
	void Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);

	}
	void UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void BindTexture(unsigned int slot = 0)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, DepthTextureId);
	}
	void SetViewPort()
	{
		glViewport(0, 0, c_screenWidth, c_screenHeight);
	}
};

class EnvCubeMapFBO
{
private:
	unsigned int frameBufferId;
	unsigned int TextureId;
	unsigned int RenderBufferId;
	unsigned int c_screenWidth;//分辨率
	unsigned int c_screenHeight;
public:
	EnvCubeMapFBO(unsigned int c_screenWidth, unsigned int c_screenHeight) :c_screenWidth(c_screenWidth), c_screenHeight(c_screenHeight)
	{
		glGenFramebuffers(1, &frameBufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
		BindTextureBuffer();
		//BindRenderbuffer();
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}
	void BindTextureBuffer()
	{
		glGenTextures(1, &TextureId);
		glBindTexture(GL_TEXTURE_CUBE_MAP, TextureId);
		for (GLuint i = 0; i < 6; ++i)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, c_screenWidth, c_screenHeight, 0, GL_RGB, GL_FLOAT, nullptr);//只需要深度

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X , TextureId, 0);

	}
	void BindRenderbuffer()
	{
		glGenRenderbuffers(1, &RenderBufferId);
		glBindRenderbuffer(GL_RENDERBUFFER, RenderBufferId);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, c_screenWidth, c_screenHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, frameBufferId);
	}
	~EnvCubeMapFBO()
	{
		glDeleteFramebuffers(1, &frameBufferId);
		glDeleteFramebuffers(1, &TextureId);
		//glDeleteFramebuffers(1, &RenderBufferId);
	}
	void Bind(int i = 0)
	{	if(i == 0)
			glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, TextureId, 0);

	}
	void UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void BindTexture(unsigned int slot = 0)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_CUBE_MAP, TextureId);
	}
	unsigned int GetTextureId()
	{
		return TextureId;
	}
	void SetViewPort()
	{
		glViewport(0, 0, c_screenWidth, c_screenHeight);
	}
};
class EnvCubeMap_ConvolutionFBO
{
private:
	unsigned int frameBufferId;
	unsigned int TextureId;
	unsigned int RenderBufferId;
	unsigned int c_screenWidth;//分辨率
	unsigned int c_screenHeight;
public:
	EnvCubeMap_ConvolutionFBO(unsigned int c_screenWidth, unsigned int c_screenHeight) :c_screenWidth(c_screenWidth), c_screenHeight(c_screenHeight)
	{
		glGenFramebuffers(1, &frameBufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
		BindTextureBuffer();
		//BindRenderbuffer();
		//if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		//	std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}
	void BindTextureBuffer()
	{
		glGenTextures(1, &TextureId);
		glBindTexture(GL_TEXTURE_CUBE_MAP, TextureId);
		for (GLuint i = 0; i < 6; ++i)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, c_screenWidth, c_screenHeight, 0, GL_RGB, GL_FLOAT, nullptr);//只需要深度

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, TextureId, 0);

	}
	void BindRenderbuffer()
	{
		glGenRenderbuffers(1, &RenderBufferId);
		glBindRenderbuffer(GL_RENDERBUFFER, RenderBufferId);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, c_screenWidth, c_screenHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, frameBufferId);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}
	~EnvCubeMap_ConvolutionFBO()
	{
		glDeleteFramebuffers(1, &frameBufferId);
		glDeleteFramebuffers(1, &TextureId);
		glDeleteFramebuffers(1, &RenderBufferId);
	}
	void Bind(int i = 0)
	{
		if (i == 0)
			glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, TextureId, 0);

	}
	void UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void BindTexture(unsigned int slot = 0)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_CUBE_MAP, TextureId);
	}
	void SetViewPort()
	{
		glViewport(0, 0, c_screenWidth, c_screenHeight);
	}
};
class EnvCubeMap_spec_ConvolutionFBO
{
private:
	unsigned int frameBufferId;
	unsigned int TextureId;
	unsigned int RenderBufferId;
	unsigned int c_screenWidth;//分辨率 128
	unsigned int c_screenHeight;
public:
	EnvCubeMap_spec_ConvolutionFBO(unsigned int c_screenWidth, unsigned int c_screenHeight) :c_screenWidth(c_screenWidth), c_screenHeight(c_screenHeight)
	{
		glGenFramebuffers(1, &frameBufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
		BindTextureBuffer();
		glGenRenderbuffers(1, &RenderBufferId);
		//BindRenderbuffer();
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}
	void BindTextureBuffer()
	{
		glGenTextures(1, &TextureId);
		glBindTexture(GL_TEXTURE_CUBE_MAP, TextureId);
		for (GLuint i = 0; i < 6; ++i)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, c_screenWidth, c_screenHeight, 0, GL_RGB, GL_FLOAT, nullptr);//只需要深度

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, TextureId, 0);//先绑定一个防止报错
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		//glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, TextureId, 0);
	}
	void BindRenderbuffer()
	{
		glGenRenderbuffers(1, &RenderBufferId);
		glBindRenderbuffer(GL_RENDERBUFFER, RenderBufferId);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, c_screenWidth, c_screenHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, frameBufferId);
	}
	void Bindmip_Renderbuffer(unsigned int width, unsigned int height)
	{
		glBindRenderbuffer(GL_RENDERBUFFER, RenderBufferId);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

	}
	~EnvCubeMap_spec_ConvolutionFBO()
	{
		glDeleteFramebuffers(1, &frameBufferId);
		glDeleteFramebuffers(1, &TextureId);
		glDeleteFramebuffers(1, &RenderBufferId);
	}
	void Bind(int i = 0, int j =0)
	{
		if (i == 0)
			glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, TextureId, j);

	}
	void UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void BindTexture(unsigned int slot = 0)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_CUBE_MAP, TextureId);
	}
	void SetViewPort(unsigned int width, unsigned int height)
	{
		glViewport(0, 0, width, height);
	}
};
class EnvCubeMap_spec_BRDF_FBO
{
private:
	unsigned int frameBufferId;
	unsigned int textureColorId[1];

	unsigned int c_screenWidth;
	unsigned int c_screenHeight;
public:
	EnvCubeMap_spec_BRDF_FBO(unsigned int scr_width, unsigned int scr_height) :c_screenWidth(scr_width), c_screenHeight(scr_height)
	{
		glGenFramebuffers(1, &frameBufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
		BindTextureBuffer();
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	void BindTextureBuffer()
	{
		glGenTextures(1, textureColorId);
		for (int i = 0; i < 1; i++)
		{
			glBindTexture(GL_TEXTURE_2D, textureColorId[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, c_screenWidth, c_screenHeight, 0, GL_RG, GL_FLOAT, NULL);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textureColorId[i], 0);
		}
		GLuint colorattachments[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, colorattachments);
	};
	~EnvCubeMap_spec_BRDF_FBO()
	{
		glDeleteFramebuffers(1, &frameBufferId);
		glDeleteFramebuffers(1, textureColorId);
	};
	void Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
	};
	void UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	void BindTexture(unsigned int slot = 0, int i = 0)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, textureColorId[i]);
	};

	void Write()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferId);
	};
	void unWrite()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	};
	void Read()
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBufferId);
	}
	void unRead()
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	}
	void BlitDepthBuffer()
	{
		glBlitFramebuffer(0, 0, c_screenWidth, c_screenHeight, 0, 0, c_screenWidth, c_screenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	};

	void SetViewPort()
	{
		glViewport(0, 0, c_screenWidth, c_screenHeight);
	}
};

class Pre_FrameBuffers
{
public:


	//环境光照
	EnvCubeMapFBO envcubemapFBO = EnvCubeMapFBO(1024, 1024);
	EnvCubeMap_ConvolutionFBO envcubemap_convolutionFBO = EnvCubeMap_ConvolutionFBO(32, 32);
	EnvCubeMap_spec_ConvolutionFBO envcubemap_spec_convolutionFBO = EnvCubeMap_spec_ConvolutionFBO(1024, 1024);
	EnvCubeMap_spec_BRDF_FBO envcubemap_spec_BRDF_FBO = EnvCubeMap_spec_BRDF_FBO(1024, 1024);
};
class FrameBuffers
{
private:
	unsigned int screenWidth;
	unsigned int screenHeight;
public:
	FrameBuffers(unsigned int width = 960, unsigned int height = 640) :screenWidth(width), screenHeight(height)
	{
	}
	
	DirLightDepthMapFBO csm_mapFBO[4] =
	{
	DirLightDepthMapFBO(4096, 4096),
	DirLightDepthMapFBO(4096, 4096),
	DirLightDepthMapFBO(4096, 4096),
	DirLightDepthMapFBO(4096, 4096)

	};
	PointLightDepthMapFBO PointlightMapfbo[4] = {
	 PointLightDepthMapFBO(1024, 1024),
	 PointLightDepthMapFBO(1024, 1024),
	 PointLightDepthMapFBO(1024, 1024),
	 PointLightDepthMapFBO(1024, 1024)
	};
	SpotLightDepthMapFBO SpotlightMapfbo = SpotLightDepthMapFBO(1024, 1024);
	HDRFBO hdrfbo = HDRFBO(screenWidth, screenHeight);
	//MSAAFrameBuffer msaa = MSAAFrameBuffer(screenWidth, screenHeight, 4);
	CameraDepthFBO cameradepthFBO = CameraDepthFBO(screenWidth, screenHeight);
	Blooming_HighlightFBO blooming_hightlightFBO = Blooming_HighlightFBO(screenWidth, screenHeight);
	basic_FBO blooming_blur_horizontalFBO = basic_FBO(screenWidth, screenHeight);
	basic_FBO blooming_blur_verticalFBO = basic_FBO(screenWidth, screenHeight);

	basic_FBO MLAA_FBO = basic_FBO(screenWidth, screenHeight, 1, 1);

	basic_FBO preFrame_FBO = basic_FBO(screenWidth, screenHeight);
	basic_FBO currentFrame_FBO = basic_FBO(screenWidth, screenHeight);

	GBuffer gbuffer = GBuffer(screenWidth, screenHeight);
	preShadowBuffer PreShadowFBO = preShadowBuffer(screenWidth, screenHeight);
	//SSAO采样样本
	SSAOFBO ssaoFBO = SSAOFBO(screenWidth, screenHeight);
	SSAOBlurFBO ssaoblurFBO = SSAOBlurFBO(screenWidth, screenHeight);
	~FrameBuffers()
	{
		for (int i = 0; i < 4; i++)
		{
			csm_mapFBO[i].~DirLightDepthMapFBO();
			PointlightMapfbo[i].~PointLightDepthMapFBO();

		}
		SpotlightMapfbo.~SpotLightDepthMapFBO();
		hdrfbo.~HDRFBO();
		cameradepthFBO.~CameraDepthFBO();
		blooming_hightlightFBO.~Blooming_HighlightFBO();
		blooming_blur_horizontalFBO.~basic_FBO();
		blooming_blur_verticalFBO.~basic_FBO();
		MLAA_FBO.~basic_FBO();

		preFrame_FBO.~basic_FBO();
		currentFrame_FBO.~basic_FBO();
		gbuffer.~GBuffer();
		PreShadowFBO.~preShadowBuffer();

		ssaoFBO.~SSAOFBO();
		ssaoblurFBO.~SSAOBlurFBO();
	}
};
