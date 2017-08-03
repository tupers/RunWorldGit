#pragma once
#include "RPG_Item.h"

class RPG_Equipment :public RPG_Item
{
public:
	RPG_Equipment(std::string &name,int &level) :RPG_Item(name,level){};
	~RPG_Equipment();
	virtual std::string Info()override;
protected:
};