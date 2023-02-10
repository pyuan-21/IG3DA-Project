#include "../globals.hpp"
#include "../helpers/logger.hpp"
#include "testunit.hpp"
#include "testFunctions.hpp"

using namespace IceRender;

TestUnit::TestUnit()
{
	// Each time add/remove functions, we need to modify here
	funcMap["Test1"] = std::function<void()>(TestFunctions::Test1);
	funcMap["Test2"] = std::function<void()>(TestFunctions::Test2);
	funcMap["Test3"] = std::function<void()>(TestFunctions::Test3);
	funcMap["Test4"] = std::function<void()>(TestFunctions::Test4);

	funcMap["TestPhong"] = std::function<void()>(TestFunctions::TestPhong);
	funcMap["TestSAT"] = std::function<void()>(TestFunctions::TestSAT);
}
TestUnit::~TestUnit() { funcMap.clear(); }


void TestUnit::TryCallFunc(std::string funcName)
{
	if (funcMap.find(funcName) != funcMap.end())
		funcMap[funcName]();
	else
		Print("No such function: " + funcName);
}