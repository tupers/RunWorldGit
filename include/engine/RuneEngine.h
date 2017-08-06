#pragma once
#include <tools/glad/glad.h>
#include <tools/GLFW/glfw3.h>

#include "type\RPG_Type.h"
#include "generator\Map_Generator.h"
#include "generator\Particle_Generator.h"
#include "object\Map_Object.h"
#include <io.h>
#include <direct.h>
#include <tools\openGL\Shader.h>
#include <tools\stb_image.h>
#include <openGL\Text.h>
#include <irrklang\irrKlang.h>

#include <tools\glm\glm.hpp>
#include <tools\glm\gtc\matrix_transform.hpp>
#include <tools\glm\gtc\type_ptr.hpp>

#include <engine\EngineFps.h>

typedef struct
{
	GLFWwindow *window;
	int width;
	int height;
	std::string name;
}ENGINE_WINDOW;

typedef struct
{
	std::string vsPath;
	std::string fsPath;
	Shader* shader = NULL;
}ENGINE_SHADER;

typedef struct
{
	glm::mat4 view;
	glm::mat4 projection;
}ENGINE_CAMERA;

typedef struct
{
	irrklang::ISoundEngine* engine;
	bool isPaused;
	irrklang::ISound* snd;
}ENGINE_AUDIO;
using ENGINE_STATUS = RPG_STATUS;

class RuneEngine:public RPG_Generator
{
public:

	RuneEngine() {}
	~RuneEngine();
	ENGINE_STATUS init();
	RPG_STATUS loadConfig()override;
	ENGINE_STATUS load();
	void exec();
private:
	unsigned char scene = 1;
	void playMainTheme();

	void mapLoop();
	void textLoop();
	void particleLoop();

	void processInput();
	ENGINE_STATUS build();
	ENGINE_STATUS loadMap();
	ENGINE_STATUS loadParticle();
	ENGINE_STATUS loadText();
	ENGINE_STATUS loadAudio();

	ENGINE_STATUS windowInit();
	void cameraInit();

	//util
	EngineFps m_CFps;

	ENGINE_WINDOW m_tMainWindow;
	std::map<std::string, ENGINE_SHADER> m_mapShader;

	MapObject* m_hMainMap = NULL;
	Map_Generator* m_hMapGen = NULL;

	Particle_Generator* m_hParticleGen = NULL;

	ENGINE_CAMERA m_tCamera;

	Text* m_hTextRender;

	ENGINE_AUDIO* m_pAudio=NULL;
};