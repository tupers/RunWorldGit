#include <engine\EngineFps.h>


EngineFps::EngineFps()
{
	m_fLastTime = glfwGetTime();
	m_nCnt = 0;
}

void EngineFps::counting()
{
	float curTime = glfwGetTime();
	float deltaTime = curTime - m_fLastTime;
	if (deltaTime > 1.0)
	{
		m_fFps = (double)m_nCnt / deltaTime;
		m_nCnt = 0;
		m_fLastTime = glfwGetTime();
	}
	else
		m_nCnt++;
}

std::string EngineFps::getFpsInfo()
{
	std::stringstream info;
	info << "fps:" << getFps();
	return info.str();
}
