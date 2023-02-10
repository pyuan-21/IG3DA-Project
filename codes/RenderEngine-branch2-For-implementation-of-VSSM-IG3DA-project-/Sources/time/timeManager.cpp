#include "timeManager.hpp"
#include <string>
#include "../globals.hpp"
#include "GLFW/glfw3.h"

using namespace IceRender;

TimeManager::TimeManager() { currentTime = 0; deltaTime = 0; frameStartTime = 0; frameCount = 0; isShowFPS = false; }
TimeManager::~TimeManager() {}

void TimeManager::Init()
{
	currentTime = glfwGetTime();
	deltaTime = 0;
}

void TimeManager::Update()
{
	double newCurTime = glfwGetTime();
	deltaTime = newCurTime - currentTime;
	currentTime = newCurTime;

	// FPS
	if (isShowFPS) 
	{
		frameCount++;
		if (frameCount >= 60)
		{
			auto window = glfwGetCurrentContext();
			if (window != nullptr)
			{
				int FPS = 60.0 / (currentTime - frameStartTime);
				std::string titleWithFPS = "IceRender - " + std::to_string(FPS) + "FPS";
				glfwSetWindowTitle(window, titleWithFPS.c_str());
			}
			// reset frame info
			frameCount = 0;
			frameStartTime = currentTime;
		}
	}
}


void TimeManager::SetShowFPS(bool _show) 
{ 
	Print(NULL, "sb", "SetShowFPS: ", _show);

	isShowFPS = _show; 
	frameCount = 0;
	frameStartTime = currentTime;
	if (!isShowFPS)
	{
		auto window = glfwGetCurrentContext();
		if (window != nullptr)
			glfwSetWindowTitle(window, "IceRender");
	}
}

double TimeManager::GetCurrentTime() const { return currentTime; }
double TimeManager::GetDeltaTime() const { return deltaTime; }

void TimeManager::StartRecord() { recordPoint = std::chrono::high_resolution_clock::now(); }
double TimeManager::EndRecord()
{
	auto endPoint = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed_seconds = endPoint - recordPoint;
	return elapsed_seconds.count();
}