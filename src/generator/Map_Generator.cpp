#include "generator\Map_Generator.h"
#include <iostream>

using namespace pugi;

std::map<std::string, Map_Generator::MAP_MODELPROPERTY> Map_Generator::m_mapModel;

void Map_Generator::init()
{
	m_mapModel.clear();
	//init surface
	MAP_MODELPROPERTY model;

	model.vertex.push_back(glm::vec3(-0.5f, 0.0f, -0.5f));
	model.vertex.push_back(glm::vec3(0.5f, 0.0f, -0.5f));
	model.vertex.push_back(glm::vec3(-0.5f, 0.0f, 0.5f));
	model.vertex.push_back(glm::vec3(0.5f, 0.0f, 0.5f));

	model.texCoord.push_back(glm::vec2(0.0f, 1.0f));
	model.texCoord.push_back(glm::vec2(1.0f, 1.0f));
	model.texCoord.push_back(glm::vec2(0.0f, 0.0f));
	model.texCoord.push_back(glm::vec2(1.0f, 0.0f));

	model.indices.push_back(0);
	model.indices.push_back(1);
	model.indices.push_back(3);
	model.indices.push_back(0);
	model.indices.push_back(2);
	model.indices.push_back(3);

	m_mapModel.insert({ "ground" ,model });

	//clear
	model.indices.clear();
	model.texCoord.clear();
	model.vertex.clear();

	//init vegetation
	model.vertex.push_back(glm::vec3(-0.5f, 0.5f, 0.0f));
	model.vertex.push_back(glm::vec3(0.45f, 0.5f, 0.0f));
	model.vertex.push_back(glm::vec3(-0.5f, -0.5f, 0.0f));
	model.vertex.push_back(glm::vec3(0.45f, -0.5f, 0.0f));

	model.texCoord.push_back(glm::vec2(0.0f, 1.0f));
	model.texCoord.push_back(glm::vec2(1.0f, 1.0f));
	model.texCoord.push_back(glm::vec2(0.0f, 0.0f));
	model.texCoord.push_back(glm::vec2(1.0f, 0.0f));

	model.indices.push_back(0);
	model.indices.push_back(1);
	model.indices.push_back(3);
	model.indices.push_back(0);
	model.indices.push_back(2);
	model.indices.push_back(3);

	m_mapModel.insert({ "plant" ,model });
}

Map_Generator::Map_Generator()
{
	try {
		loadConfig();
		loadTexture();
	}
	catch (RPG_STATUS &ret)
	{
		std::cout << "err in Map_Generator construct :" << ret << std::endl;
	}
}

Map_Generator::~Map_Generator()
{
	//free map texture;
	auto tex = m_mapTexture.begin();
	while (tex != m_mapTexture.end())
	{
		if (tex->second.texture.id != 0)
			glDeleteTextures(1,&tex->second.texture.id);
		++tex;
	}
}

RPG_STATUS Map_Generator::loadConfig()
{
	//add config path
	RPG_XMLFILE xmlFile = { "./config/envConfig.xml" ,NULL };
	m_vecConfigFile.insert({ "envConfig",xmlFile });

	//open config file
	RPG_STATUS ret = openConfigFile();
	if (ret != Success)
		throw ret;

	//get map config

	auto mapConfig = m_vecConfigFile.find("envConfig")->second.xml;
	for (xml_node env = mapConfig->child("env"); env; env = env.next_sibling("random"))
	{
		auto size = env.child("size");
		m_nWidth = std::stoi(size.child("width").first_child().value());
		m_nHeight = std::stoi(size.child("height").first_child().value());

		for (auto tex = env.child("texture"); tex; tex = tex.next_sibling("texture"))
			m_mapTexture.insert({tex.attribute("name").value(),{{0,"texture_diffuse" ,aiString(tex.first_child().value())},(unsigned char)tex.attribute("id").as_uint(),tex.attribute("type").value() } });
	}
	return Success;
}

RPG_STATUS Map_Generator::loadTexture()
{
	auto tex = m_mapTexture.begin();
	while (tex != m_mapTexture.end())
	{
		try {
			loadSpecifiedTexture(tex->second.texture);
		}
		catch (RPG_STATUS &ret)
		{
			throw ret;
		}
		++tex;
	}
	return Success;
}

RPG_STATUS Map_Generator::generate(MapObject * map)
{
	if (m_nSeed == 0)
		engine.seed(time(NULL));
	else
		engine.seed(m_nSeed);
	if (m_nHeight < 2 || m_nWidth < 2)
		return ArgsErr;


	//add mesh(model) to obj
	auto tex = m_mapTexture.begin();
	while (tex != m_mapTexture.end())
	{
		vector<Vertex> vertices;
		for (int i = 0; i < m_mapModel[tex->second.type].vertex.size(); i++)
		{
			Vertex ver;
			ver.Position = m_mapModel[tex->second.type].vertex[i];
			ver.TexCoords = m_mapModel[tex->second.type].texCoord[i];
			vertices.push_back(ver);
		}

		vector<Texture> textures;
		textures.push_back(tex->second.texture);
		map->addElement(tex->second.id,vertices,m_mapModel[tex->second.type].indices, textures);

		++tex;
	}

	//generate map in specific size
	RPG_STATUS ret = map->create(m_nWidth, m_nHeight);
	if (ret != Success)
		return ret;
	engine.seed(m_nSeed);
	
	//start generate map
	auto surfaceData = map->data(0);
	auto elementData = map->data(1);
	if (surfaceData == NULL || elementData == NULL)
		return ConfigErr;

	for (int rows = 0; rows < m_nHeight; rows++)
	{
		int lineIndex = rows*m_nWidth;
		for (int cols = 0; cols < m_nWidth; cols++)
			surfaceData[cols + lineIndex] = getRandom(1, 100) < 60 ? 0 : 1;
	}

	for (int rows = 0; rows < m_nHeight; rows++)
	{
		int lineIndex = rows*m_nWidth;
		for (int cols = 0; cols < m_nWidth; cols++)
		{
			if(getRandom(1, 100) < 30&& surfaceData[cols + lineIndex]==0)
			elementData[cols + lineIndex] =  21;
		}
	}

	return Success;
}

int Map_Generator::aroundNum(int cols, int rows, int label, MapObject::MapData* pMap)
{
	int num = 0;
	if (rows == 0)
	{
		if (cols == 0)
		{
			if (isExist(cols + 1, rows, label, pMap))
				num++;
			if (isExist(cols, rows + 1, label, pMap))
				num++;
			if (isExist(cols + 1, rows + 1, label, pMap))
				num++;
		}
		else if (cols == m_nWidth - 1)
		{
			if (isExist(cols - 1, rows, label, pMap))
				num++;
			if (isExist(cols, rows + 1, label, pMap))
				num++;
			if (isExist(cols - 1, rows + 1, label, pMap))
				num++;
		}
		else
		{
			if (isExist(cols - 1, rows, label, pMap))
				num++;
			if (isExist(cols + 1, rows, label, pMap))
				num++;
			if (isExist(cols - 1, rows + 1, label, pMap))
				num++;
			if (isExist(cols, rows + 1, label, pMap))
				num++;
			if (isExist(cols + 1, rows + 1, label, pMap))
				num++;
		}
	}
	else if (rows == m_nHeight - 1)
	{
		if (cols == 0)
		{
			if (isExist(cols + 1, rows, label, pMap))
				num++;
			if (isExist(cols, rows - 1, label, pMap))
				num++;
			if (isExist(cols + 1, rows - 1, label, pMap))
				num++;
		}
		else if (cols == m_nWidth - 1)
		{
			if (isExist(cols - 1, rows, label, pMap))
				num++;
			if (isExist(cols, rows - 1, label, pMap))
				num++;
			if (isExist(cols - 1, rows - 1, label, pMap))
				num++;
		}
		else
		{
			if (isExist(cols - 1, rows - 1, label, pMap))
				num++;
			if (isExist(cols, rows - 1, label, pMap))
				num++;
			if (isExist(cols + 1, rows - 1, label, pMap))
				num++;
			if (isExist(cols - 1, rows, label, pMap))
				num++;
			if (isExist(cols + 1, rows, label, pMap))
				num++;
		}
	}
	else
	{
		if (cols == 0)
		{
			if (isExist(cols, rows - 1, label, pMap))
				num++;
			if (isExist(cols + 1, rows - 1, label, pMap))
				num++;
			if (isExist(cols + 1, rows, label, pMap))
				num++;
			if (isExist(cols, rows + 1, label, pMap))
				num++;
			if (isExist(cols + 1, rows + 1, label, pMap))
				num++;
		}
		else if (cols == m_nWidth - 1)
		{
			if (isExist(cols, rows - 1, label, pMap))
				num++;
			if (isExist(cols - 1, rows - 1, label, pMap))
				num++;
			if (isExist(cols - 1, rows, label, pMap))
				num++;
			if (isExist(cols, rows + 1, label, pMap))
				num++;
			if (isExist(cols - 1, rows + 1, label, pMap))
				num++;
		}
		else
		{
			if (isExist(cols - 1, rows - 1, label, pMap))
				num++;
			if (isExist(cols, rows - 1, label, pMap))
				num++;
			if (isExist(cols + 1, rows - 1, label, pMap))
				num++;
			if (isExist(cols - 1, rows, label, pMap))
				num++;
			if (isExist(cols + 1, rows, label, pMap))
				num++;
			if (isExist(cols - 1, rows + 1, label, pMap))
				num++;
			if (isExist(cols, rows + 1, label, pMap))
				num++;
			if (isExist(cols + 1, rows + 1, label, pMap))
				num++;
		}
	}
	return num;
}

bool Map_Generator::isExist(int cols, int rows, int label, MapObject::MapData * pMap)
{
	if (pMap[cols + rows*m_nWidth] == label)
		return true;
	else
		return false;
}
