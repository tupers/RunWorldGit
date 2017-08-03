#include "object\RPG_Equipment.h"

RPG_Equipment::~RPG_Equipment()
{
}

std::string RPG_Equipment::Info()
{
	std::stringstream info;
	info << "this is an equipment name is: " << getName();
	info << ", it has " << m_vecProperty.size();
	info << "properties. \n";
	return info.str();
}
