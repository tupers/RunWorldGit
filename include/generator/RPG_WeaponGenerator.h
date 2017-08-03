#pragma once
#include "RPG_Generator.h"
#include "type\RPG_Type.h"
#include "object\RPG_RuneStone.h"
#include "object\RPG_Weapon.h"

class RPG_WeaponGenerator :public RPG_Generator
{
public:
	RPG_WeaponGenerator();
	~RPG_WeaponGenerator();
	virtual RPG_STATUS loadConfig()override;
	RPG_STATUS generate_random(RPG_Object* obj);
	RPG_STATUS generate_static(std::string path, RPG_Object* obj);
private:
	std::map<int, int> slotRandom;
	std::map<int, int> additionRandom;
	std::map<std::string, RPG_MAJOR_PROPPRIN> majorRandom;
	std::vector<RPG_MINOR_PROPPRIN> minorRandom;
};