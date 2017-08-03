#pragma once
#include <string>

typedef enum
{
	Major = 0,
	Minor
}RPG_PROPERTY_LEVEL;

typedef struct
{
	float fVal;
	RPG_PROPERTY_LEVEL emlevel;
}RPG_PROPERTY;

typedef enum
{
	Success=0,
	AllocErr,
	PathErr,
	ArgsErr,
	CompileErr,
	ConfigErr
}RPG_STATUS;

typedef struct
{
	float growth;
	float range;
}RPG_MAJOR_PROPPRIN;

typedef struct
{
	float range;
	std::string name;
}RPG_MINOR_PROPPRIN;

typedef struct
{
	std::string path;
	unsigned int texture;
}ENGINE_TEXTURE;