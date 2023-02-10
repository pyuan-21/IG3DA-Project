#include "shaderProgram.hpp"
#include "../globals.hpp"
#include "../helpers/logger.hpp"
#include "../helpers/utility.hpp"

using namespace IceRender;

ShaderProgram::ShaderProgram()
{
	// Create a shader program
	id = glCreateProgram(); // This function returns 0 if an error occurs creating the program object.
}

ShaderProgram::~ShaderProgram()
{
	//Print("ShaderProgram id: " + std::to_string(id) + " has been deleted."); // for debug
	glDeleteProgram(id);
	id = 0;
}

string ShaderProgram::ReadFileToString(const string& _fPath)
{
	string str;
	if (_fPath.empty())
	{
		Print("[Error] file path is empty when calling ShaderManager::ReadFileToString().");
		return str;
	}
	ifstream s(_fPath);
	if (!s.is_open()) {
		Print("[Error] failed to open: " + _fPath);
		return str;
	}
	std::stringstream ss;
	ss << s.rdbuf();
	str = ss.str();
	s.close();
	return str;
}

bool ShaderProgram::LoadShader(const string& _fPath, const ShaderType& _type, GLuint& _shaderID)
{
	string sourceCode = ReadFileToString(_fPath);
	if (sourceCode.empty())
	{
		return false;
	}
	//Print(sourceCode);

	// 1- Create a shader object
	_shaderID = glCreateShader(_type);
	// 2- Associate the source code to it
	const GLchar* shaderSource = (const GLchar*)sourceCode.c_str(); // Interface the C++ string through a C pointer
	glShaderSource(_shaderID, 1, &shaderSource, NULL); // becase we only pass one shaderSource, the length array is not neccessary.
	// 3- Compile your shader source.
	glCompileShader(_shaderID);
	// 4- Check shader status
	GLint compileStatus = 0;
	glGetShaderiv(_shaderID, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus == GL_FALSE)
	{
		// 4.1- If compilation failed, get the info
		GLint maxLen = 0;
		glGetShaderiv(_shaderID, GL_INFO_LOG_LENGTH, &maxLen);
		std::string errorLog;
		errorLog.resize(maxLen);
		glGetShaderInfoLog(_shaderID, maxLen, NULL, &errorLog[0]);
		Print("[Compilation Error]" + _fPath + ":\n" + errorLog);
		// Exit with failure.
		glDeleteShader(_shaderID); // Don't leak the shader.
		return false;
	}
	// 5- Attach shader to shader-program
	if (!IsValid())
		return false;
	
	glAttachShader(id, _shaderID); // Set the vertex shader as the one ot be used with the program/pipeline

	// 6- update string which indicates whether using shader
	switch (_type)
	{
	case GL_VERTEX_SHADER:
		vShader = _fPath;
		break;
	case GL_FRAGMENT_SHADER:
		fShader = _fPath;
		break;
	default:
		break;
	}
	return true;
}

bool ShaderProgram::Link(const GLuint& _vShaderID, const GLuint& _fShaderID)
{
	// Check shader id validation
	if (glIsShader(_vShaderID) == GL_FALSE)
	{
		Print(NULL, "sds", "[Error]vertex shader id: ", _vShaderID, " is no longer a valid shader id.");
		return false;
	}
	if (glIsShader(_fShaderID) == GL_FALSE)
	{
		Print(NULL, "sds", "[Error]fragment shader id: ", _fShaderID, " is no longer a valid shader id.");
		return false;
	}

	// Check validation of 'id'
	if (!IsValid())
		return false;

	// Link shader program
	glLinkProgram(id);

	// Note the different functions here: glGetProgram* instead of glGetShader*.
	GLint isLinked = 0;
	glGetProgramiv(id, GL_LINK_STATUS, (int*)&isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::string errorLog;
		errorLog.resize(maxLength);
		glGetProgramInfoLog(id, maxLength, &maxLength, &errorLog[0]);

		glDeleteProgram(id); // delet current
		id = glCreateProgram(); // create new one

		Print("[Link Shader Program Error]:\n" + errorLog);

		return false;
	}

	// Always detach shaders after a successful link.
	glDetachShader(id, _vShaderID);
	glDetachShader(id, _fShaderID);

	return true;
}

bool ShaderProgram::IsValid()
{
	if (glIsProgram(id) == GL_FALSE)
	{
		Print(NULL, "sds", "[Error]id: ", id, " is no longer a valid shader program id");
		return false;
	}
	return true;
}

bool ShaderProgram::LoadShader(const string& _vShaderPath, const string& _fShaderPath)
{
	GLuint vShaderID, fShaderID;
	
	if (!LoadShader(_vShaderPath, GL_VERTEX_SHADER, vShaderID))
		return false;
	if (!LoadShader(_fShaderPath, GL_FRAGMENT_SHADER, fShaderID))
		return false;

	bool result = Link(vShaderID, fShaderID);

	// Don't leak shaders no matter whether it linked successfully or not.
	glDeleteShader(vShaderID);
	glDeleteShader(fShaderID);

	return result;
}

bool ShaderProgram::Active()
{
	if (!IsValid())
		return false;
	glUseProgram(id);
	return true;
}

void ShaderProgram::PrintShader()
{
	Print("Current Active Vertex Shader: " + vShader);
	Print("Current Active Fragment Shader: " + fShader);
}

void ShaderProgram::Set(const string& _name, bool _val) { Set(_name, static_cast<int>(_val)); }
void ShaderProgram::Set(const string& _name, int _val)
{
	GLint location = glGetUniformLocation(id, _name.c_str());
	if (!CheckGLError() && location != -1)
		glUniform1i(location, _val); // to integer
	else
		Print("Can't not get uniform: " + _name);
}

void ShaderProgram::Set(const string& _name, float _val)
{
	GLint location = glGetUniformLocation(id, _name.c_str());
	if (!CheckGLError() && location != -1)
		glUniform1f(location, _val);
	else
		Print("Can't not get uniform: " + _name);
}

void ShaderProgram::Set(const string& _name, const glm::vec2& _val)
{
	GLint location = glGetUniformLocation(id, _name.c_str());
	if (!CheckGLError() && location != -1)
		glUniform2fv(location, 1, glm::value_ptr(_val));
	else
		Print("Can't not get uniform: " + _name);
}

void ShaderProgram::Set(const string& _name, const glm::vec3& _val)
{
	GLint location = glGetUniformLocation(id, _name.c_str());
	if (!CheckGLError() && location != -1)
		glUniform3fv(location, 1, glm::value_ptr(_val));
	else
		Print("Can't not get uniform: " + _name);
}

void ShaderProgram::Set(const string& _name, const glm::vec4& _val)
{
	GLint location = glGetUniformLocation(id, _name.c_str());
	if (!CheckGLError() && location != -1)
		glUniform4fv(location, 1, glm::value_ptr(_val));
	else
		Print("Can't not get uniform: " + _name);
}

void ShaderProgram::Set(const string& _name, const glm::mat4& _val)
{
	GLint location = glGetUniformLocation(id, _name.c_str());
	if (!CheckGLError() && location != -1)
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(_val));
	else
		Print("Can't not get uniform: " + _name);
}
