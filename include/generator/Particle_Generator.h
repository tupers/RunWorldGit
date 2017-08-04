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
		//glm::vec2 Position;
		//glm::vec2 Velocity;
		glm::vec3 Ellipse;
		glm::vec2 offset;
		float Life;
	}Particle;
	Particle_Generator(Shader* sha);
	~Particle_Generator();
	RPG_STATUS loadConfig() override;
	RPG_STATUS loadTexture();
	void createParticles(std::string name,unsigned int initNum,std::string texName, float size = 1.0, float pointSize = 1.0, glm::vec2 startPos=glm::vec2(0.0,0.0));
	void deleteParticles(std::string name);
	void update();
	void draw();
	void setParticlesPos(std::string parName, glm::vec2 pos);
	void setParticlesSize(std::string parName, float size);
	void setParticlesPointSize(std::string parName, float size);
private:
	typedef struct
	{
		std::vector<Particle> particles;
		Texture* ptex;
		unsigned int updateNum;
		unsigned int lastUsedParticle;
		float dt;
		glm::mat4 model;
		glm::vec2 startPos;
		float scale;
		float pointScale;
	}ParticleObj;
	void init();
	int FirstUnusedParticle(ParticleObj* obj);
	void RespawnParticle(Particle& par);
	std::map<std::string, Texture> m_mapTexture;
	std::map<std::string, ParticleObj> m_mapParticleList;
	Shader* m_hShader = NULL;
	unsigned int m_nVAO, m_nVBO;
};