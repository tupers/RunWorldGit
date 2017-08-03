#pragma once
#include "RPG_Object.h"
#include "type\RPG_Type.h"
#include <map>
#include <string>
class RPG_Item : public RPG_Object
{
public:

	RPG_Item(std::string &name,int &level) :RPG_Object(name) ,m_nLevel(level){ m_vecProperty.clear(); };
	~RPG_Item();
	const auto& Property() { return m_vecProperty; }
	bool addProperty(std::string, RPG_PROPERTY);
	const int Level() { return m_nLevel; }
	void setLevel(int level) { m_nLevel = level; }
	virtual std::string Info()override;
protected:
	//basic property for item
	std::map<std::string, RPG_PROPERTY> m_vecProperty;
	int m_nLevel;
	
};

