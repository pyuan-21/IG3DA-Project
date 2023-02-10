#pragma once
#include <chrono>

namespace IceRender
{
	class TimeManager
	{
	private:
		// Frame related
		double currentTime; // current time in seconds
		double deltaTime; // elapsed time in seconds

		// FPS relevant
		double frameStartTime;
		unsigned int frameCount; // used for showing FPS
		bool isShowFPS;

		// irrelated to frame. Designed for debug
		std::chrono::time_point<std::chrono::high_resolution_clock> recordPoint;

	public:
		TimeManager();
		~TimeManager();

		void Init();
		void Update();

		void SetShowFPS(bool _show);

		// Frame related
		double GetCurrentTime() const;
		double GetDeltaTime() const;

		// record current time point
		void StartRecord();
		// end record and return duration in seconds
		double EndRecord();
	};
}

