#pragma once
#include <iostream>

#include <tools\glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

#include <generator\RPG_Generator.h>
#include <openGL\Mesh.h>
#include <openGL\Shader.h>

class Particle_Generator :public RPG_Generator
{
public:
	typedef struct
	{
		glm::vec2 Position;
		//glm::vec2 Velocity;
		glm::vec2 Ellipse;
		glm::vec2 offset;
		float angle;
		float Life;
	}Particle;
	Particle_Generator(Shader* sha);
	~Particle_Generator();
	RPG_STATUS loadConfig() override;
	RPG_STATUS loadTexture();
	void createParticles(std::string name,unsigned int initNum,std::string texName);
	void deleteParticles(std::string name);
	void update();
	void draw();
private:
	typedef struct
	{
		std::vector<Particle> particles;
		Texture* ptex;
		unsigned int updateNum;
		unsigned int lastUsedParticle;
	}ParticleObj;
	void init();
	int FirstUnusedParticle(ParticleObj* obj);
	void RespawnParticle(Particle& par);
	std::map<std::string, Texture> m_mapTexture;
	std::map<std::string, ParticleObj> m_mapParticleList;
	Shader* m_hShader = NULL;
	unsigned int m_nVAO, m_nVBO;
};