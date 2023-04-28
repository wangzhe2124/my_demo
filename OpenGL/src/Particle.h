#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
struct Particle
{
	glm::vec2 Position;
	float Velocity;
	glm::vec4 Color;
	float Life;
	Particle() : Position(0.0f), Velocity(0.0f), Color(0.0f), Life(0.0f)
	{};
};



class Particle_Generator
{
private:
	int Particle_Num = 500;
	int new_Particle_Num = 5;
	int dead_Particle_index = 0;
	float life_reduce;
	float velocity = 2.0f;
	
public:
	std::vector<Particle> particles;
	Particle_Generator(int particle_num = 500,int new_particle_num = 5, float re = 0.0005) : Particle_Num(particle_num), new_Particle_Num(new_particle_num),
		life_reduce(re)
	{
		Initial_Particle();
	}
	void Initial_Particle()
	{
		for (int i = 0; i < Particle_Num; i++)
		{
			particles.push_back(Particle());
		}
	}
	void update_Particle(glm::vec2 position, float offset, int new_number, float vel, float reduce)
	{
		new_Particle_Num = new_number;
		velocity = vel;
		life_reduce = reduce;
		for (int i = 0; i < new_Particle_Num; i++)
		{
			int dead_particle = search_dead_particle();
			respawn_particle(particles[dead_particle], position, offset);
		}
		for (int i = 0; i < Particle_Num; i++)
		{
			Particle& p = particles[i];
			p.Life -= life_reduce;
			if (p.Life > 0.0f)
			{
				p.Position.y += (rand() % 2) * p.Velocity;
				p.Position.x += (rand() % 3 - 1) * p.Velocity;

				p.Color -= life_reduce;
			}
		}
	}
	int search_dead_particle()
	{
		for (int i = dead_Particle_index; i < Particle_Num; i++)
		{
			if (particles[i].Life <= 0)
			{
				dead_Particle_index = i;
				return i;
			}
		}
		for (int i = 0; i < dead_Particle_index; i++)
		{
			if (particles[i].Life <= 0)
			{
				dead_Particle_index = i;
				return i;
			}
		}
		dead_Particle_index = 0;
		return 0;
	}
	void respawn_particle(Particle& particle, glm::vec2 position, float offset)
	{
		float randomx = position.x + ((rand() % 200) - 100) * offset;
		float randomy = position.y + (rand() % 20 - 10) * offset;
		float rcolor =  (rand() % 100) / 100.0f;
		particle.Position = glm::vec2(randomx,randomy);
		particle.Color = glm::vec4(rcolor, rcolor, rcolor, 1.0f);
		particle.Life = 1.0f;
		particle.Velocity = velocity;
	}
};

struct D3Particle
{
	glm::mat4  Position;
	glm::vec3 Velocity;
	glm::vec4 Color;
	float Life;
	D3Particle() :Position(1.0f), Velocity(0.0f), Color(0.0f), Life(0.0f)
	{};
};

class D3Particle_Generator
{
private:
	int Particle_Num = 500;
	int new_Particle_Num = 5;
	int dead_Particle_index = 0;
	float life_reduce;
	unsigned int amount = 10000;
	

public:
	std::vector<D3Particle> particles;
	D3Particle_Generator(int particle_num = 500, int new_particle_num = 5, float re = 0.0005) : Particle_Num(particle_num), new_Particle_Num(new_particle_num),
		life_reduce(re)
	{
		Initial_Particle();
	}
	void Initial_Particle()
	{
		for (int i = 0; i < Particle_Num; i++)
		{
			particles.push_back(D3Particle());
		}
	}
	void update_Particle()
	{
		for (int i = 0; i < new_Particle_Num; i++)
		{
			int dead_particle = search_dead_particle();
			respawn_particle(particles[dead_particle]);
		}
		for (int i = 0; i < Particle_Num; i++)
		{
			D3Particle& p = particles[i];
			p.Life -= life_reduce*0.1f;
			if (p.Life > 0.0f)
			{
				p.Position[3][0] += (rand() % 2) / 400.0f;
				p.Position[3][1] += (rand() % 2) / 400.0f;
				p.Position[3][2] -= (rand() % 2) / 400.0f;

				p.Color -= life_reduce * 0.1f;
			}
		}
	}
	int search_dead_particle()
	{
		for (int i = dead_Particle_index; i < Particle_Num; i++)
		{
			if (particles[i].Life <= 0)
			{
				dead_Particle_index = i;
				return i;
			}
		}
		for (int i = 0; i < dead_Particle_index; i++)
		{
			if (particles[i].Life <= 0)
			{
				dead_Particle_index = i;
				return i;
			}
		}
		dead_Particle_index = 0;
		return 0;
	}
	void respawn_particle(D3Particle& particle)
	{
		float randomx = static_cast<float>((rand() % 10) - 5);
		float randomy = static_cast<float>((rand() % 10) - 5);
		float randomz = static_cast<float>((rand() % 10) - 5);
		float rcolor = 10.0f +(rand() % 500) / 200.0f;
		particle.Position[3][0] = randomx;
		particle.Position[3][1] = randomy;
		particle.Position[3][2] = randomz;
		particle.Color = glm::vec4(rcolor, rcolor, rcolor, 1.0f);
		particle.Life = 1.0f;
	}
};