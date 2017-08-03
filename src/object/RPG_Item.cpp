#include "object\RPG_Item.h"

RPG_Item::~RPG_Item()
{
}

bool RPG_Item::addProperty(std::string name, RPG_PROPERTY property)
{
	auto ret = m_vecProperty.insert({ name,property });
	return ret.second;
}

std::string RPG_Item::Info()
{
	std::stringstream info;
	info << "this is only an item name is: " << getName();
	info << ", it has " + m_vecProperty.size();
	info << "properties. \n";
	return info.str();
}

