#pragma once
#include "RPG_Equipment.h"
#include "RPG_RuneStone.h"

#include <vector>

class RPG_Weapon :public RPG_Equipment
{
public:	
	RPG_Weapon(std::string& name,int level) :RPG_Equipment(name,level){};
	~RPG_Weapon();
	virtual std::string Info()override;
	const auto& slot() { return m_vecSlot; }
	void setSlot(std::vector<RPG_RuneStone*>&slot) {m_vecSlot = slot;}
	void reset();
protected:
	std::vector<RPG_RuneStone*> m_vecSlot;
};