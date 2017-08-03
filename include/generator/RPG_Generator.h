#pragma once
#include "pugiXML\pugixml.hpp"
#include <stb_image.h>
#include <openGL\Mesh.h>
#include "type\RPG_Type.h"

#include "object\RPG_Object.h"

#include <vector>
#include <string>
#include <map>
#include <stdlib.h>
#include <time.h>
#include <random>

class RPG_Generator
{
public:
	typedef struct
	{
		std::string path;
		pugi::xml_document* xml;
	}RPG_XMLFILE;
	RPG_Generator();
	~RPG_Generator();
	RPG_STATUS openConfigFile();
	virtual RPG_STATUS loadConfig() = 0;
	int getRandom(int start, int end);
protected:
	void loadSpecifiedTexture(Texture& tex);
	std::map<std::string, RPG_XMLFILE> m_vecConfigFile;
	std::default_random_engine engine;
	
};