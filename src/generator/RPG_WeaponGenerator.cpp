#include "generator\RPG_WeaponGenerator.h"

using namespace pugi;

RPG_WeaponGenerator::RPG_WeaponGenerator()
{
	engine.seed(time(NULL));
	loadConfig();
}

RPG_WeaponGenerator::~RPG_WeaponGenerator()
{
}

RPG_STATUS RPG_WeaponGenerator::loadConfig()
{
	//add config path
	RPG_XMLFILE xmlFile = { "./config/dropConfig.xml" ,NULL };
	m_vecConfigFile.insert({ "dropConfig",xmlFile });

	//open config file
	RPG_STATUS ret = openConfigFile();
	if (ret != Success)
		return ret;

	//get drop config

	auto dropConfig = m_vecConfigFile.find("dropConfig")->second.xml;
	for (xml_node random = dropConfig->child("random"); random; random=random.next_sibling("random"))
	{
		std::string type = random.attribute("type").as_string();
		if (type == "weapon")
		{
			auto slot = random.child("slot");
			for (auto percent = slot.child("percent"); percent; percent = percent.next_sibling("percent"))
				slotRandom.insert({ percent.attribute("num").as_int(),std::stoi(percent.first_child().value()) });

			auto addition = random.child("addition");
			for (auto percent = addition.child("percent"); percent; percent = percent.next_sibling("percent"))
				additionRandom.insert({ percent.attribute("num").as_int(),std::stoi(percent.first_child().value()) });

			auto property = random.child("property");
			for (auto major = property.child("major"); major; major = major.next_sibling("major"))
				majorRandom.insert({ major.first_child().value(),{ major.attribute("growth").as_float(),major.attribute("range").as_float() } });

			for (auto minor = property.child("minor"); minor; minor = minor.next_sibling("minor"))
				minorRandom.push_back({ minor.attribute("range").as_float() ,minor.first_child().value() });

			break;
		}
	}
	return Success;
}

RPG_STATUS RPG_WeaponGenerator::generate_random(RPG_Object * obj)
{
	//reset obj
	RPG_Weapon* handle = (RPG_Weapon*)obj;
	handle->reset();

	//generate obj according config
	int randVal = 0;
	float randValF = 0.0;
	std::vector<RPG_RuneStone*> vecSlot;
	int level = handle->Level();
	int minorNum = 0;
	int slotNum = 0;

	//generate slot
	randVal = getRandom(1, 100);
	for (auto i = slotRandom.begin(); i != slotRandom.end(); i++)
	{
		if (randVal < i->second)
			break;
		else
			slotNum = i->first;
	}
	for (int i = 0; i<slotNum; i++)
		vecSlot.push_back(NULL);
	handle->setSlot(vecSlot);

	//generate major property
	for (auto i = majorRandom.begin(); i != majorRandom.end(); i++)
	{
		randVal = getRandom(-100, 100);
		randValF = (float)randVal*0.01;
		float valF = level*i->second.growth*(1.0 + randValF*i->second.range);
		handle->addProperty(i->first, { valF,Major });
	}
	
	//generate minor property
	if (minorRandom.size() >= 1)
	{
		randVal = getRandom(1, 100);
		for (auto i = additionRandom.begin(); i != additionRandom.end(); i++)
		{
			if (randVal < i->second)
				break;
			else
				minorNum = i->first;
		}
		minorNum = minorNum > minorRandom.size() ? minorRandom.size() : minorNum;

		for (int i = minorNum; i != 0; i--)
		{
			int choosed = getRandom(0, minorRandom.size() - 1);
			randVal = getRandom(1, 100);
			randValF = (float)randVal*0.01;
			float valF = randValF*minorRandom[choosed].range;
			handle->addProperty(minorRandom[choosed].name, { valF,Minor });
			auto iter = minorRandom.begin() + choosed;
			minorRandom.erase(iter);
		}
	}

	return Success;
}

RPG_STATUS RPG_WeaponGenerator::generate_static(std::string path, RPG_Object * obj)
{
	return RPG_STATUS();
}