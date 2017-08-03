#pragma once
#include "RPG_Generator.h"
#include "object\Map_Object.h"
#include "type\RPG_Type.h"
#include "openGL\Mesh.h"
#include <map>
#include <glm/glm.hpp>
#include <stb_image.h>


class Map_Generator:public RPG_Generator
{
public:
	static void init();
	typedef struct
	{
		int src;
		int check;
		int pos;
		int neg;
		int thres;
	}MAP_RULE;
	typedef struct
	{
		std::string name;
		int percent;
		std::vector<MAP_RULE> rules;
	}MAP_OBJ;

	typedef struct
	{
		std::vector<glm::vec3> vertex;
		std::vector<glm::vec2> texCoord;
		std::vector<unsigned int> indices;
	}MAP_MODELPROPERTY;

	typedef struct
	{
		Texture texture;
		unsigned char id;
		std::string type;
	}MAP_TEXPROPERTY;

	Map_Generator();
	~Map_Generator();
	int seed() { return m_nSeed; }
	void setSeed(int s) { m_nSeed = s; }
	RPG_STATUS loadConfig() override;
	RPG_STATUS loadTexture();
	RPG_STATUS generate(MapObject* map);
private:
	int aroundNum(int cols,int rows, int label, MapObject::MapData* pMap);
	bool isExist(int cols, int rows, int label, MapObject::MapData* pMap);
	int m_nSeed=0;
	int m_nHeight = 0;
	int m_nWidth = 0;
	static std::map<std::string, MAP_MODELPROPERTY> m_mapModel;
	std::map<std::string, MAP_TEXPROPERTY> m_mapTexture;
};