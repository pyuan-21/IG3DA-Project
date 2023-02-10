#pragma once
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <string>
#include <glm/glm.hpp>

namespace IceRender
{

	typedef int ShaderType;

	using namespace std;

	class ShaderProgram
	{
	private:
		GLuint id;
		string vShader; // file path of vertex shader
		string fShader; // file path of fragment shader

		string ReadFileToString(const string& _fPath);
		bool LoadShader(const string& _fPath, const ShaderType& _type, GLuint& _shaderID);
		bool Link(const GLuint& _vShaderID, const GLuint& _fShaderID);
		bool IsValid();

	public:
		ShaderProgram();
		~ShaderProgram();

		bool LoadShader(const string& _vShaderPath, const string& _fShaderPath);
		bool Active();

		void PrintShader();

		void Set(const string& _name, bool _val);
		void Set(const string& _name, int _val);
		void Set(const string& _name, float _val);
		void Set(const string& _name, const glm::vec2& _val);
		void Set(const string& _name, const glm::vec3& _val);
		void Set(const string& _name, const glm::vec4& _val);
		void Set(const string& _name, const glm::mat4& _val);
	};
}