#include "engine\RuneEngine.h"

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

using namespace pugi;


RuneEngine::~RuneEngine()
{
	//delete audio
	if (m_pAudio != NULL)
	{
		m_pAudio->engine->drop();
		delete m_pAudio;
	}
	//delete map object
	if (m_hMainMap != NULL)
		delete m_hMainMap;
	//delete generator
	if (m_hMapGen != NULL)
		delete m_hMapGen;
	if (m_hParticleGen != NULL)
		delete m_hParticleGen;
	//delete shader
	auto shader = m_mapShader.begin();
	while (shader != m_mapShader.end())
	{
		if (shader->second.shader != NULL)
			delete shader->second.shader;
		++shader;
	}
	m_mapShader.clear();
	//free glfw resource
	glfwTerminate();
}

ENGINE_STATUS RuneEngine::init()
{
	ENGINE_STATUS ret = Success;
	//create environment folder
	std::string dir = "./config";
	if (_access(dir.c_str(), 0) == -1)
		int flag = _mkdir(dir.c_str());

	dir = "./shader";
	if (_access(dir.c_str(), 0) == -1)
		int flag = _mkdir(dir.c_str());

	dir = "./texture";
	if (_access(dir.c_str(), 0) == -1)
		int flag = _mkdir(dir.c_str());

	//load config
	ret = loadConfig();
	if (ret != Success)
		return ret;

	//init window
	ret = windowInit();
	if (ret != Success)
		return ret;

	//build shader
	ret = build();
	if (ret != Success)
		return ret;

	//camera init
	cameraInit();

	//generator params init
	Map_Generator::init();

	return Success;
}

ENGINE_STATUS RuneEngine::windowInit()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	m_tMainWindow.window = glfwCreateWindow(m_tMainWindow.width, m_tMainWindow.height, m_tMainWindow.name.c_str(), NULL, NULL);
	if (m_tMainWindow.window == NULL)
	{
		glfwTerminate();
		return AllocErr;
	}

	glfwMakeContextCurrent(m_tMainWindow.window);
	glfwSetFramebufferSizeCallback(m_tMainWindow.window, framebuffer_size_callback);



	// glad: load all OpenGL function pointers

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		return AllocErr;

	// configure global opengl state
	glEnable(GL_DEPTH_TEST);

	//ENABLE BLENDING
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return Success;
}

ENGINE_STATUS RuneEngine::loadText()
{
	auto shader = m_mapShader.find("character");
	if (shader == m_mapShader.end())
		return ConfigErr;

	m_hTextRender = new Text(shader->second.shader,m_tMainWindow.width,m_tMainWindow.height);

	//add ctx
	Text::TEXT_CONTEXT ctx;
	ctx.color = glm::vec3(1.0, 0.2, 0.0);
	ctx.coord.x = 20;
	ctx.coord.y = m_tMainWindow.height - 20;
	ctx.data = "fps:00";
	ctx.scale = 0.25;
	m_hTextRender->addContext("fps", ctx);

	return Success;
}

ENGINE_STATUS RuneEngine::loadAudio()
{
	m_pAudio = new ENGINE_AUDIO;
	m_pAudio->engine = irrklang::createIrrKlangDevice();
	m_pAudio->isPaused = true;
	return Success;
}

void RuneEngine::cameraInit()
{
	m_tCamera.projection = glm::perspective(glm::radians(45.0f), (float)m_tMainWindow.width / (float)m_tMainWindow.height, 0.1f, 100.0f);
	m_tCamera.view = glm::lookAt(glm::vec3(0.0, 5.0f, 5.0f), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
}

RPG_STATUS RuneEngine::loadConfig()
{
	//add config path
	RPG_XMLFILE xmlFile = { "./config/engineConfig.xml" ,NULL };
	m_vecConfigFile.insert({ "engineConfig",xmlFile });

	//open config file
	RPG_STATUS ret = openConfigFile();
	if (ret != Success)
		return ret;

	//get config
	//first window config
	auto engine = m_vecConfigFile.find("engineConfig")->second.xml->child("engine");

	auto window = engine.child("window");
	auto height = window.child("height");
	m_tMainWindow.height = std::stoi(height.first_child().value());
	auto width = window.child("width");
	m_tMainWindow.width = std::stoi(width.first_child().value());
	auto name = window.child("name");
	m_tMainWindow.name = name.first_child().value();

	for (auto shader = engine.child("shader"); shader; shader = shader.next_sibling("shader"))
	{
		ENGINE_SHADER sdr;
		sdr.vsPath = shader.child("vs").first_child().value();
		sdr.fsPath = shader.child("fs").first_child().value();
		sdr.shader = NULL;
		m_mapShader.insert({ shader.attribute("tar").value(),sdr });
	}
	return Success;
}

ENGINE_STATUS RuneEngine::build()
{
	auto shader = m_mapShader.begin();
	while (shader != m_mapShader.end())
	{
		shader->second.shader = new Shader(shader->second.vsPath.c_str(), shader->second.fsPath.c_str());

		++shader;
	}
	return Success;
}

ENGINE_STATUS RuneEngine::load()
{
	//load map
	ENGINE_STATUS ret = loadMap();
	if (ret != Success)
		return ret;
	//load particle system
	try {
		loadParticle();
	}
	catch (RPG_STATUS &ret)
	{
		std::cout << "load Particle: " << ret << std::endl;
	}

	//load text render
	ret = loadText();
	if (ret != Success)
		return ret;

	//load audio
	ret = loadAudio();
	if (ret != Success)
		return ret;

	return Success;

}

void RuneEngine::exec()
{
	playMainTheme();
	while (!glfwWindowShouldClose(m_tMainWindow.window))
	{
		processInput();

		//clear whole frame
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		mapLoop();
		particleLoop();
		textLoop();

		//others
		glfwSwapBuffers(m_tMainWindow.window);
		glfwPollEvents();
		//counting fps
		m_CFps.counting();
	}
}

void RuneEngine::playMainTheme()
{
	m_pAudio->snd = m_pAudio->engine->play2D("./audio/breakout.mp3", true,false,true, irrklang::ESM_AUTO_DETECT,true);
}

ENGINE_STATUS RuneEngine::loadMap()
{
	//set camera
	auto shader = m_mapShader.find("map")->second.shader;
	shader->use();

	shader->setMat4("projection", m_tCamera.projection);
	shader->setMat4("view", m_tCamera.view);

	m_hMainMap = new MapObject(std::string("main"));
	m_hMapGen = new Map_Generator;
	m_hMapGen->setSeed(100);
	ENGINE_STATUS ret = m_hMapGen->generate(m_hMainMap);
	if (ret != Success)
		return ret;

	return Success;
}

ENGINE_STATUS RuneEngine::loadParticle()
{
	auto shader = m_mapShader.find("particle");
	if (shader == m_mapShader.end())
		throw ConfigErr;

	m_hParticleGen = new Particle_Generator(shader->second.shader);
	try {
		m_hParticleGen->createParticles("firetest", 100, "particle_fire",5.0,1.0,glm::vec2(400,300));
		m_hParticleGen->createParticles("firetest", 100, "particle_fire", 5.0, 1.0, glm::vec2(400, 300));
		m_hParticleGen->createParticles("firetest", 100, "particle_fire", 5.0, 1.0, glm::vec2(400, 300));
		m_hParticleGen->createParticles("firetest", 100, "particle_fire", 5.0, 1.0, glm::vec2(400, 300));
		}	    
	catch(RPG_STATUS &ret){
		std::cout << "create failed: " << ret << std::endl;
		throw;
	}

	return Success;
}

void RuneEngine::mapLoop()
{

	auto shader = m_mapShader.find("map")->second.shader;

	shader->use();

	m_hMainMap->Draw(*shader);
}

void RuneEngine::textLoop()
{
	//do something...
	auto fps = m_hTextRender->Context("fps");
	fps->data = m_CFps.getFpsInfo();

	//render
	m_hTextRender->RenderTotalText();
}

void RuneEngine::particleLoop()
{
	m_hParticleGen->draw();
	m_hParticleGen->update();
}

void RuneEngine::processInput()
{
	auto window = m_tMainWindow.window;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
	{
		m_pAudio->snd->setIsPaused(m_pAudio->isPaused);
		m_pAudio->isPaused = !m_pAudio->isPaused;
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		m_hMainMap->setView(m_hMainMap->viewStartX(), m_hMainMap->viewStartY() - 1);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		m_hMainMap->setView(m_hMainMap->viewStartX(), m_hMainMap->viewStartY() + 1);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		m_hMainMap->setView(m_hMainMap->viewStartX() - 1, m_hMainMap->viewStartY());
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		m_hMainMap->setView(m_hMainMap->viewStartX() + 1, m_hMainMap->viewStartY());
}

