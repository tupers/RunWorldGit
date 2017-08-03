#include "object\RPG_Weapon.h"

RPG_Weapon::~RPG_Weapon()
{
}

std::string RPG_Weapon::Info()
{
	std::stringstream info;
	info << "this weapon name is: [" << getName() << "], it has " << m_vecProperty.size() << " properties with ";
	info << m_vecSlot.size() << " slot\n";
	auto iter = m_vecProperty.begin();
	while (iter != m_vecProperty.end())
	{
		info << "[" + iter->first + "] " << iter->second.fVal << std::endl;
		++iter;
	}
	return info.str();
}

void RPG_Weapon::reset()
{
	m_vecProperty.clear();
	m_vecSlot.clear();
}


