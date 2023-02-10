#pragma once
#include <string>
#include <map>
#include <functional>

namespace IceRender
{
	// All testing code must be written here.
	// Each Test*() functions can be call by using command. (* is number or name)
	class TestUnit
	{
	private:
		std::map<std::string, std::function<void()>> funcMap;

	public:
		TestUnit();
		~TestUnit();

		void TryCallFunc(std::string funcName);
	};
}