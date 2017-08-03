#pragma once
#include "RPG_Item.h"

class RPG_RuneStone :protected RPG_Item
{
public:
	typedef struct
	{

	}RPG_RUNE_PROPERTY;
	RPG_RuneStone(std::string& name, RPG_RUNE_PROPERTY&runestoneProperty,int&level) :RPG_Item(name,level), m_tRuneStoneProperty(runestoneProperty) {};
	~RPG_RuneStone();
protected:
	RPG_RUNE_PROPERTY m_tRuneStoneProperty;
};