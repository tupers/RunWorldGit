#pragma once
#include <tools/GLFW/glfw3.h>
#include <sstream>

class EngineFps
{
public:
	EngineFps();
	void counting();
	auto getFps() { return m_fFps; }
	std::string getFpsInfo();
private:
	float m_fFps;
	float m_fLastTime;
	int m_nCnt=0;
};