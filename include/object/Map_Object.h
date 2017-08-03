#pragma once
#include "RPG_Object.h"
#include "type\RPG_Type.h"
#include <tools\glm\glm.hpp>
#include <tools\glm\gtc\matrix_transform.hpp>
#include <tools\glm\gtc\type_ptr.hpp>
#include <openGL\Mesh.h>
#include <map>

class MapObject :public RPG_Object
{
public:

	using MapData = unsigned char;
	typedef struct
	{
		glm::vec3 model;
	}Map_Point;

	MapObject(std::string &name) :RPG_Object(name) {
		MapData* data = NULL;
		m_vecMapData.push_back(data);
		m_vecMapData.push_back(data);
	}
	~MapObject() { free(); }
	RPG_STATUS create(int width, int height);
	void free();
	virtual std::string Info() override;
	int size() { return sizeof(MapData)*m_nHeight*m_nWidth; }
	int height() { return m_nHeight; }
	int width() { return m_nWidth; }
	RPG_STATUS setSight(int sight);
	RPG_STATUS setView(int startX, int startY);
	int viewStartX()const { return m_nStartX; }
	int viewStartY()const { return m_nStartY; }
	void addElement(unsigned char id, vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures) {
		Mesh* mesh = new Mesh(vertices, indices, textures);
		m_mapModel.insert({ id,mesh});
	}
	RPG_STATUS Draw(Shader& shader);
	MapData* data(int index) {
		if (index > m_vecMapData.size() - 1)
			return NULL;
		else
			return m_vecMapData[index];
	}
private:
	vector<MapData*> m_vecMapData;
	Map_Point* m_arrSight = NULL;
	std::map<unsigned char, Mesh*> m_mapModel;
	int m_nWidth = 0;
	int m_nHeight = 0;
	int m_nSight = 0;
	int m_nStartX = 0;
	int m_nStartY = 0;
};