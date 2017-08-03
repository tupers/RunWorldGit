#pragma once
#include <string>
#include <sstream>
class RPG_Object
{
public:
	RPG_Object(std::string &name) :m_strName(name) {};
	~RPG_Object();
	void setName(std::string&);
	const std::string& getName() { return m_strName; }
	virtual std::string Info();
private:
	std::string m_strName = "";
};

