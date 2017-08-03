#include "object\RPG_Object.h"

RPG_Object::~RPG_Object()
{
}

void RPG_Object::setName(std::string &name)
{
	m_strName = name;
}

std::string RPG_Object::Info()
{
	std::stringstream info;
	info << "this is only an object name is" << m_strName << "\n";
	return info.str();
}
