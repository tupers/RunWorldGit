#include "generator\RPG_Generator.h"

using namespace pugi;

RPG_Generator::RPG_Generator()
{
	m_vecConfigFile.clear();
}

RPG_Generator::~RPG_Generator()
{
	auto iter = m_vecConfigFile.begin();
	while (iter != m_vecConfigFile.end())
	{
		if(iter->second.xml!=NULL)
		delete iter->second.xml;
		++iter;
	}
}

RPG_STATUS RPG_Generator::openConfigFile()
{
	auto configFile = m_vecConfigFile.begin();
	while (configFile != m_vecConfigFile.end())
	{
		configFile->second.xml = new xml_document;
		if (configFile->second.xml == NULL)
		{
			auto iter = m_vecConfigFile.begin();
			while (iter != m_vecConfigFile.end())
			{
				if (iter != configFile)
				{
					delete iter->second.xml;
					iter->second.xml = NULL;
				}
				else
					break;
				++iter;
			}
			for (auto iter = m_vecConfigFile.begin(); (iter != configFile) || (iter == m_vecConfigFile.end()); ++iter)
				delete iter->second.xml;
			return AllocErr;
		}
		xml_parse_result ret = configFile->second.xml->load_file(configFile->second.path.c_str());
		if (ret.status != xml_parse_status::status_ok)
		{
			auto iter = m_vecConfigFile.begin();
			while (iter != m_vecConfigFile.end())
			{
				if (iter != configFile)
				{
					delete iter->second.xml;
					iter->second.xml = NULL;
				}
				else
					break;
				++iter;
			}
			delete configFile->second.xml;
			iter->second.xml = NULL;
			return PathErr;
		}
		++configFile;
	}
	return Success;

}

int RPG_Generator::getRandom(int start, int end)
{
	std::uniform_int_distribution<int> dis(start,end);
	return dis(engine);
}

void RPG_Generator::loadSpecifiedTexture(Texture& tex)
{
	glGenTextures(1, &tex.id);
	int width, height, nrComponents;
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load(tex.path.C_Str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;
		glBindTexture(GL_TEXTURE_2D, tex.id);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		if (format == GL_RGBA)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << tex.path.C_Str() << std::endl;
		stbi_image_free(data);
		throw PathErr;
	}
}
