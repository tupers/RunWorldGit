#include "object\Map_Object.h"

RPG_STATUS MapObject::create(int width, int height)
{
	m_nHeight = height;
	m_nWidth = width;

	//need to confirm enough model to build map
	if (m_mapModel.size() == 0)
		return ConfigErr;

	//create map buffer
	for (int i = 0; i < m_vecMapData.size(); i++)
	{
		m_vecMapData[i] = new MapData[height*width];
		if (m_vecMapData[i] == NULL)
		{
			for (int j = 0; j < i; j++)
				delete m_vecMapData[j];
			return AllocErr;
		}
		memset(m_vecMapData[i], 0, sizeof(MapData)*height*width);
	}

	//default sight 13x13
	auto ret = setSight(15);
	if (ret != Success)
	{
		for (int i = 0; i < m_vecMapData.size(); i++)
			delete m_vecMapData[i];
		return ret;
	}
	//set sight start pos(default 0,0)
	setView(0, 0);

	return Success;
}

void MapObject::free()
{
	//free map buffer
	for each (auto data in m_vecMapData)
		if (data != NULL)
			delete data;

	//free sight buffer
	if (m_arrSight != NULL)
	{
		delete m_arrSight;
		m_arrSight = NULL;
	}

	//free Model
	auto model = m_mapModel.begin();
	while (model != m_mapModel.end())
	{
		if (model->second != NULL)
			delete model->second;
		++model;
	}
}

std::string MapObject::Info()
{
	return std::string();
}

RPG_STATUS MapObject::setSight(int sight)
{
	//fix sight val
	if (sight <= 0)
		return ArgsErr;
	if (sight > m_nHeight || sight > m_nWidth)
		sight = m_nHeight > m_nWidth ? m_nWidth : m_nHeight;
	if (sight % 2 == 0)
		sight += 1;
	m_nSight = sight;

	//alloc sight buffer
	if (m_arrSight != NULL)
	{
		delete m_arrSight;
		m_arrSight = NULL;
	}
	m_arrSight = new Map_Point[m_nSight*m_nSight];
	if (m_arrSight == NULL)
		return AllocErr;

	//init sight buffer
	int offset = m_nSight / 2;
	for (int rows = 0; rows < m_nSight; rows++)
	{
		int lineIndex = rows*m_nSight;
		for (int cols = 0; cols < m_nSight; cols++)
		{
			m_arrSight[lineIndex + cols].model.x = -(float)offset + cols;
			m_arrSight[lineIndex + cols].model.y = 0.0f;
			m_arrSight[lineIndex + cols].model.z = -(float)offset + rows;
		}
	}
	return Success;
}

RPG_STATUS MapObject::setView(int startX, int startY)
{

	if (m_nSight == 0)
		return ConfigErr;

	//fix start pos
	if (startX + m_nSight > m_nWidth)
		startX = m_nWidth - m_nSight;
	else if (startX <= 0)
		startX = 0;
	if (startY + m_nSight > m_nHeight)
		startY = m_nHeight - m_nSight;
	else if (startY <= 0)
		startY = 0;

	m_nStartX = startX;
	m_nStartY = startY;

	return Success;
}

RPG_STATUS MapObject::Draw(Shader& shader)
{
	if (m_mapModel.size() == 0 || m_nSight == 0 || m_arrSight == NULL)
		return ConfigErr;

	//draw model according map
	//1.surface

	for (int rows = 0; rows < m_nSight; rows++)
	{
		for (int cols = 0; cols < m_nSight; cols++)
		{
			//set model for map element
			int sightIndex = rows*m_nSight + cols;
			int mapIndex = (m_nStartY + rows)*m_nWidth + m_nStartX + cols;
			glm::mat4 model;
			model = glm::translate(model, m_arrSight[sightIndex].model);
			
			auto val = m_vecMapData[0][mapIndex];
			auto mesh = m_mapModel.find(val);
			if (mesh != m_mapModel.end())
			{
				shader.setMat4("model", model);
				mesh->second->Draw(shader);
			} 

		}
	}

	for (int rows = 0; rows < m_nSight; rows++)
	{
		for (int cols = 0; cols < m_nSight; cols++)
		{
			//set model for map element
			int sightIndex = rows*m_nSight + cols;
			int mapIndex = (m_nStartY + rows)*m_nWidth + m_nStartX + cols;
			glm::mat4 model;
			model = glm::translate(model, m_arrSight[sightIndex].model);
			auto modelEnv = glm::translate(model, glm::vec3(0.0, 0.5, 0.0));

			auto val = m_vecMapData[1][mapIndex];
			if (val != 0)
			{
				shader.setMat4("model", modelEnv);
				auto mesh = m_mapModel.find(val);
				if (mesh != m_mapModel.end())
					mesh->second->Draw(shader);
			}

		}
	}
}

