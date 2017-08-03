#include <generator\Particle_Generator.h>

using namespace pugi;

Particle_Generator::Particle_Generator(Shader* sha)
{
	if (sha == NULL)
		throw ArgsErr;
	m_hShader = sha;
	m_hShader->use();
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(800), 0.0f, static_cast<GLfloat>(600));
	m_hShader->setMat4("projection", projection);

	try {
		loadConfig();
		loadTexture();
		init();
	}
	catch (RPG_STATUS &ret)
	{
		std::cout << "err in Map_Generator construct :" << ret << std::endl;
	}
}

Particle_Generator::~Particle_Generator()
{
	//clean ParticleList
	auto par = m_mapParticleList.begin();
	while (par != m_mapParticleList.end())
		m_mapParticleList.erase(par++);
	//free map texture;
	for each (auto tex in m_mapTexture)
	{
		if (tex.second.id != 0)
			glDeleteTextures(1, &tex.second.id);
	}
	//free VAO,VBO
	glDeleteVertexArrays(1, &m_nVAO);
	glDeleteBuffers(1, &m_nVBO);
}

RPG_STATUS Particle_Generator::loadConfig()
{
	//add config path
	RPG_XMLFILE xmlFile = { "./config/particleConfig.xml" ,NULL };
	m_vecConfigFile.insert({ "particleConfig",xmlFile });

	//open config file
	RPG_STATUS ret = openConfigFile();
	if (ret != Success)
		throw ret;

	//get map config

	auto config = m_vecConfigFile.find("particleConfig")->second.xml->child("Particles");
	for (xml_node par = config.child("particle"); par; par = par.next_sibling("particle"))
	{
		Texture tex;
		tex.id = 0;
		tex.type = "texture_diffuse";
		tex.path = par.child("texture").first_child().value();
		m_mapTexture.insert({ par.child("name").first_child().value(), tex });
	}
	return Success;
}

RPG_STATUS Particle_Generator::loadTexture()
{
	auto tex = m_mapTexture.begin();
	while(tex!=m_mapTexture.end())
	{
		try {
			loadSpecifiedTexture(tex->second);
		}
		catch (RPG_STATUS &ret)
		{
			throw ret;
		}
		tex++;
	}
	return Success;
}

void Particle_Generator::createParticles(std::string name, unsigned int initNum, std::string texName)
{
	ParticleObj obj;
	Particle par;
	par.Life = 0.0;
	for (int i = 0; i < initNum; i++)
		obj.particles.push_back(par);
	auto tex = m_mapTexture.find(texName);
	if (tex == m_mapTexture.end())
		throw ConfigErr;
	obj.ptex = &m_mapTexture[texName];
	obj.updateNum = 8;
	obj.lastUsedParticle = 0;
	m_mapParticleList.insert({name,obj});

}

void Particle_Generator::deleteParticles(std::string name)
{
	auto par = m_mapParticleList.find(name);
	if (par == m_mapParticleList.end())
		throw ArgsErr;
	m_mapParticleList.erase(par);
}

void Particle_Generator::update()
{
	//update eache particle obj
	auto pObj = m_mapParticleList.begin();
	while(pObj!=m_mapParticleList.end())
	{
		//add new particles;
		ParticleObj& obj = pObj->second;
		for (int i = 0; i < obj.updateNum; i++)
		{
			auto index = FirstUnusedParticle(&obj);
			RespawnParticle(obj.particles[index]);
		}

		//update all particles
		float dt = 0.01;
		for (int i = 0; i < obj.particles.size(); i++)
		{
			Particle& Par = obj.particles[i];
			Par.Life -= dt;
			if (Par.Life > 0.0)
			{
				Par.angle--;
				Par.Position = glm::vec2(Par.Ellipse.x*cos(glm::radians(Par.angle)), Par.Ellipse.y*sin(glm::radians(Par.angle)));
			}
		}
		pObj++;
	}
}

void Particle_Generator::draw()
{
	// Use additive blending to give it a 'glow' effect
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDisable(GL_DEPTH_TEST);
	m_hShader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(m_nVAO);
	for each (auto obj in m_mapParticleList)
	{
		
		glBindTexture(GL_TEXTURE_2D, obj.second.ptex->id);
		
		for each (auto pPar in obj.second.particles)
		{
			if (pPar.Life > 0.0f)
			{
				auto dif = pPar.Position + pPar.offset;
				m_hShader->setVec2("offset", pPar.Position+pPar.offset);
				//m_hShader->setVec2("")
				glDrawArrays(GL_TRIANGLES, 0, 6);
			}
		}
	}
	glEnable(GL_DEPTH_TEST);
	// Don't forget to reset to default blending mode
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Particle_Generator::init()
{

	float particle_quad[] = {
		-0.5f, 0.5f, 0.0f, 1.0f,
		0.5f, -0.5f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f, 0.0f,

		-0.5f, 0.5f, 0.0f, 1.0f,
		0.5f, 0.5f, 1.0f, 1.0f,
		0.5f, -0.5f, 1.0f, 0.0f
	};

	glGenVertexArrays(1, &m_nVAO);
	glGenBuffers(1, &m_nVBO);
	glBindVertexArray(m_nVAO);
	// Fill mesh buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_nVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
	// Set mesh attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);

}

int Particle_Generator::FirstUnusedParticle(ParticleObj* obj)
{
	// Search from last used particle, this will usually return almost instantly
	for(int i= obj->lastUsedParticle;i<obj->particles.size();i++)
	{
		auto par = obj->particles[i];
		if (par.Life <= 0.0f)
		{
			obj->lastUsedParticle = i;
			return i;
		}
	}
	// Otherwise, do a linear search
	for (int i = 0; i < obj->lastUsedParticle; i++)
	{
		auto par = obj->particles[i];
		if (par.Life <= 0.0f)
		{
			obj->lastUsedParticle = i;
			return i;
		}
	}
	// Override first particle if all others are alive
	obj->lastUsedParticle = 0;
	return 0;
}

void Particle_Generator::RespawnParticle(Particle & par)
{
	float randoma = (float)getRandom(-50,50) / 100.0;
	float randomb = (float)getRandom(0, 100) / 100.0;

	par.Ellipse = glm::vec2(randoma, randomb);
	par.angle = 180.0f;
	par.Position = glm::vec2(-randoma, 0);
	par.offset = -par.Position-glm::vec2(0.0,0.5);
	par.Life = 1.0f;
}
