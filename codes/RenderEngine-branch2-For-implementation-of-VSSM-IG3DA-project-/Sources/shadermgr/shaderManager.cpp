#include "shaderManager.hpp"
#include "../globals.hpp"
#include "../helpers/logger.hpp"
#include "../helpers/utility.hpp"

using namespace IceRender;

bool ShaderManager::FindShaderProgram(const string& _shaderName, shared_ptr<ShaderProgram>& result) const
{
	// Check existing shadername first
	auto iter = shaderMap.find(_shaderName);
	bool isExisted = iter != shaderMap.end();
	if (!isExisted)
		return false;
	result = iter->second;
	return true;
}

void ShaderManager::StopShaderProgram()
{
	// Desactivate the current program
	glUseProgram(0);
	activeShader.clear();
}

bool ShaderManager::ActiveShaderProgram(const string& _shaderName)
{
	shared_ptr<ShaderProgram> target;
	if (FindShaderProgram(_shaderName, target))
	{
		target->Active();
		activeShader = _shaderName;
		return true;
	}
	else
		Print("[Error] No such \'" + _shaderName + "\' shader in ShaderManager.");
	return false;
}

bool ShaderManager::CreateShaderProgram(const string& _shaderName)
{
	// shaderName should be the prefix of vertex/fragment shaders. E.g. "simple" for "simple.vs" and "simple.fs"
	shared_ptr<ShaderProgram> pShaderPro = make_shared<ShaderProgram>();
	bool result = pShaderPro->LoadShader(_shaderName + ".vs", _shaderName + ".fs");
	if (!result)
		return false;

	shaderMap[_shaderName] = pShaderPro;
	return true;
}

void ShaderManager::Clear()
{
	activeShader.clear();
	shaderMap.clear();
}

void ShaderManager::PrintActiveShader()
{
	Print("Current Active Shader Program: " + activeShader);
	shaderMap[activeShader]->PrintShader();
}

string ShaderManager::GetActiveShader() const { return activeShader; }

bool ShaderManager::LoadShader(const string& _shaderName)
{
	// Check existing shadername first
	shared_ptr<ShaderProgram> oldShaderPro = nullptr; // cache it, only the new one is created successfully then we can remove it.
	FindShaderProgram(_shaderName, oldShaderPro);
	shaderMap.erase(_shaderName); // once shared_ptr<ShaderProgram> get removed from map, it will call desctructor of ShaderProgram to delete gl_shaderprogram
	if (CreateShaderProgram(_shaderName))
	{
		shaderMap[_shaderName]->Active();
		activeShader = _shaderName;
		return true;
	}
	else
	{
		shaderMap[_shaderName] = oldShaderPro; // put it back.
		return false;
	}
}


shared_ptr<ShaderProgram> ShaderManager::GetActiveShaderProgram() const
{
	if (activeShader.empty())
	{
		Print("Error: Active Shader Program is empty.");
		return nullptr;
	}
	shared_ptr<ShaderProgram> target;
	if (FindShaderProgram(activeShader, target))
		return target;
	else
		Print("[Error] No such \'" + activeShader + "\' shader in ShaderManager.");
	return nullptr;
}

shared_ptr<ShaderProgram> ShaderManager::TryActivateShaderProgram(const string& _shaderName)
{
	shared_ptr<ShaderProgram> target = nullptr;
	if (!FindShaderProgram(_shaderName, target))
	{
		if (CreateShaderProgram(_shaderName))
			target = shaderMap[_shaderName];
		else
			return nullptr; // can not create 
	}
	// activate it
	target->Active();
	activeShader = _shaderName;
	return target;
}

void ShaderManager::GenerateShaderFile(const std::string& _outputFileName, const std::string& _mainFileName)
{
	// If forget how to use, check "Resources/Shaders/UsageOfGenerateShader.md"

	// try open main file
	std::ifstream iStrm(_mainFileName);
	if (!iStrm.is_open())
	{
		Print("[Error] failed to open: " + _mainFileName);
		return;
	}

	// open/create output file
	std::ofstream oStrm(_outputFileName, std::ios::out);
	if (!oStrm.is_open())
	{
		Print("[Error] failed to open: " + _outputFileName);
		return;
	}

	std::string importTag = "#import:";

	std::string line;
	int lineNum = 0;
	while (std::getline(iStrm, line))
	{
		lineNum++;
		if (line.find(importTag) != std::string::npos)
		{
			//e.g. #import:"Resources/SubShaders/a.sub_fs"#
			std::size_t startFlagPos = line.find('#');
			std::size_t endFlagPos = line.find('#', importTag.size());
			if (endFlagPos == std::string::npos)
			{
				Print("[Error] line: " + std::to_string(lineNum) + ", can not find ending '#'.");
				return;
			}
			std::string subFileName = line.substr(importTag.size() + 1 + startFlagPos, endFlagPos - 2 - startFlagPos - importTag.size());

			std::ifstream subStrm(GLOBAL.shaderPathPrefix + subFileName);
			if (!subStrm.is_open())
			{
				Print("[Error] line: " + std::to_string(lineNum) + ", failed to open: " + GLOBAL.shaderPathPrefix + subFileName);
				return;
			}
			oStrm << subStrm.rdbuf() << "\n";
		}
		else
			oStrm << line << "\n";
	}

	Print("[Done] " + _outputFileName + " has been created successfully.");
}

void ShaderManager::GenerateShaderFilesFromConfig(nlohmann::json _data)
{
	// If forget how to use, check "Resources/Shaders/UsageOfGenerateShader.md"
	std::vector<std::string> inputs, outputs;
	if (_data.contains("inputs"))
		inputs = _data["inputs"].get<std::vector<std::string>>();
	if (_data.contains("outputs"))
		outputs = _data["outputs"].get<std::vector<std::string>>();
	if (inputs.size() != outputs.size())
	{
		Print("[Error] In GenerateShaderFilesFromConfig(), inputs and outputs don't have the same size.");
		return;
	}
	for (int i = 0; i < inputs.size(); i++)
		GenerateShaderFile(GLOBAL.shaderPathPrefix + outputs[i], GLOBAL.shaderPathPrefix + inputs[i]);
}
