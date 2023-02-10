#pragma once
#include <memory>
#include <map>
#include "shaderProgram.hpp"
#include "../nlohmann/json.hpp"

namespace IceRender
{
	using namespace std;

	class ShaderManager
	{
	private:
		string activeShader;
		map<string, shared_ptr<ShaderProgram>> shaderMap;

		bool FindShaderProgram(const string& _shaderName, shared_ptr<ShaderProgram>& result) const;

	public:
		/*we always assume all incoming parameters are valid*/

		void StopShaderProgram();
		bool ActiveShaderProgram(const string& _shaderName);
		bool CreateShaderProgram(const string& _shaderName);
		void Clear();
		void PrintActiveShader();
		string GetActiveShader() const;

		// create and active shader at run-time. (if "_shaderName" ShaderProgram already existed, it will be removed first and reload it)
		bool LoadShader(const string& _shaderName);

		shared_ptr<ShaderProgram> GetActiveShaderProgram() const;
		shared_ptr<ShaderProgram> TryActivateShaderProgram(const string& _shaderName); // return a shader program, if not exist, create and activate it.
		// todo: support to use uniform blocks (check RenderNote)

		// generate a shader file by using ".sub_fs/.sub_vs" and ".main_fs/.main_vs"
		// "_outputFileName" and "_mainFileName" should be a relative path to folder "Resources/Shaders".
		void GenerateShaderFile(const std::string& _outputFileName, const std::string& _mainFileName);

		void GenerateShaderFilesFromConfig(nlohmann::json _data);
	};

}
