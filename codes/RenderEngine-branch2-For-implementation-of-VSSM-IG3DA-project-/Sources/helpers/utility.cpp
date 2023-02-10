#define _USE_MATH_DEFINES
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image.h"
#include "utility.hpp"
#include <math.h>
#include "../globals.hpp"
#include "../scene/sceneObjectGenerator.hpp"
#include "stb_image_write.h"
#include "../mesh/meshGenerator.hpp"


using namespace IceRender;

bool Utility::_glCheckError(const char* file, int line)
{
	bool hasError = false;
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		Print("Error: " + error + " occurs in " + file + " (" + std::to_string(line) + ")");
		hasError = true;
	}
	return hasError;
}

float Utility::GetAngleInRadians(const glm::vec3& _from, const glm::vec3& _to)
{
	float cosAngle = glm::dot(_from, _to);
	return acos(cosAngle);
}

float Utility::GetAngleInDegree(const glm::vec3& _from, const glm::vec3& _to)
{
#pragma warning( push )
#pragma warning( disable : 26451 ) // no need to consider Arithmetic Overflow here. It won't happen.
	return GetAngleInRadians(_from, _to) * 180.0f / M_PI;
#pragma warning( pop )
}

glm::quat Utility::GetQuaternion(const glm::vec3& _from, const glm::vec3& _to, const float _radians)
{
	glm::vec3 rotAxis = glm::cross(_from, _to); // rotation axis
	return glm::angleAxis(_radians, rotAxis);
}

glm::vec3 Utility::RotateVector(const glm::vec3& _orign, const glm::vec3& _axis, const float _degree)
{
	glm::quat q = glm::angleAxis(float(M_PI * _degree / 180.0f), _axis);
	return q * _orign;
}

void Utility::PrintVec3(const glm::vec3& _vec, const std::string _title)
{
	std::string vecStr = _title + "(" + std::to_string(_vec.x) + ", " + std::to_string(_vec.y) + ", " + std::to_string(_vec.z) + ")\n";
	Print(vecStr);
}

void Utility::PrintTransform(const Transform& _trans, const std::string _title)
{
	std::string str = _title;
	std::vector<glm::vec3> v{ _trans.GetPosition(), _trans.GetRotation(),_trans.GetScale() };
	std::vector<std::string> s{ "Position","EulerAngles","Scale" };
	for (int i = 0; i < v.size(); i++)
		str += s[i] + ": [" + std::to_string(v[i].x) + ", " + std::to_string(v[i].y) + ", " + std::to_string(v[i].z) + "]\n";
	Print(str);
}

void Utility::PrintMat4(const glm::mat4& _mat, const std::string _title)
{
	std::string str = _title;
	for (int col = 0; col < 4; col++)
	{
		str.append("col " + std::to_string(col) +
			": (" + std::to_string(_mat[col][0]) + ", " +
			std::to_string(_mat[col][1]) + ", " +
			std::to_string(_mat[col][2]) + ", " +
			std::to_string(_mat[col][3]) + ")\n");
	}
	Print(str);
}

std::vector<std::string> Utility::SplitString(const std::string& _input, const std::string& _delimiter)
{
	std::vector<std::string> result;
	std::size_t start = 0;
	std::size_t current = _input.find(_delimiter, start);
	while (current != std::string::npos)
	{
		if (current != start)
			result.push_back(std::string(_input, start, current - start));
		start = current + _delimiter.length();
		current = _input.find(_delimiter, start);
	}
	if (start < _input.length())
		result.push_back(std::string(_input, start));
	return result;
}

bool Utility::Load2DTexture(const std::string& _fileName, const bool& _defaultSetting, GLuint& _textureID)
{
	// [TODO] for now only load RGB, not including Alpha channel.
	// just simply load file and create a texture.
	// This texture has not been setting parameters well.
	// load and generate the texture
	int width, height, channel;
	std::string total = GLOBAL.imagePathPrefix + _fileName;
	// refer: https://stackoverflow.com/questions/19770296/should-i-vertically-flip-the-lines-of-an-image-loaded-with-stb-image-to-use-in-o
	stbi_set_flip_vertically_on_load(true); // it enables to load an image as OpenGL expects!!!
	unsigned char* data = stbi_load(total.c_str(), &width, &height, &channel, 0);
	if (data)
	{
		// OpenGL 4.5 way to initialize texture:
		glCreateTextures(GL_TEXTURE_2D, 1, &_textureID); if (CheckGLError()) { stbi_image_free(data); return false; }
		//Print("Genreate new textureID: " + std::to_string(_textureID));
		// TODO: to read OpenGL book carefully to figure out how to set levels correctly, here if I set levels greater than 1, 
		// it will not give us a correct texture rendering result-> move far away we will not see the texture.
		// [Important] if load .png, we should use GL_RGBA12 at least
		glTextureStorage2D(_textureID, 1, GL_RGB12, width, height); if (CheckGLError()) { stbi_image_free(data); return false; }
		// [Note] 32F is to store floating-point value into texture, but now we are reading UNSIGNED_BYTE data which will be normalized into [0,1], then it is not neccessary to use 32F
		//glTextureStorage2D(_textureID, 1, GL_RGB32F, width, height); if (CheckGLError()) { stbi_image_free(data); return false; }
		// [Important] if load .png, we should use GL_RGBA
		glTextureSubImage2D(_textureID, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data); if (CheckGLError()) { stbi_image_free(data); return false; }
		//glGenerateMipmap(GL_TEXTURE_2D);
		glGenerateTextureMipmap(_textureID);
		stbi_image_free(data);
		if (_defaultSetting)
		{
			// set the texture wrapping/filtering options
			glTextureParameteri(_textureID, GL_TEXTURE_WRAP_S, GL_REPEAT); if (CheckGLError()) return false;
			glTextureParameteri(_textureID, GL_TEXTURE_WRAP_T, GL_REPEAT); if (CheckGLError()) return false;
			glTextureParameteri(_textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); if (CheckGLError()) return false;
			glTextureParameteri(_textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR); if (CheckGLError()) return false;
		}
		return true;
	}
	else
	{
		Print("[Error] Can not open file " + _fileName + " to load texture, reason: " + stbi_failure_reason());
		return false;
	}
}

void Utility::RenderScreenQuad(const GLuint& _textureID)
{
	// be careful, "GLOBAL.sceneMgr->RemoveSceneObj" function will release "_textureID" texture attached to "screen_quad"!
	GLOBAL.sceneMgr->RemoveSceneObj("screen_quad"); // only one "screen_quad" is alowed to exist.

	shared_ptr<SceneObject> obj = SceneObjectGenerator::GenScreenQuadObject(_textureID);
	GLOBAL.sceneMgr->AddSceneObj(obj);

	GLOBAL.sceneMgr->SetCurrentRenderMethod("RenderSceenQuad");
}

void Utility::ShowImage(const std::string& _fileName)
{
	GLuint textureID = 0;
	if (Utility::Load2DTexture(_fileName, true, textureID))
		RenderScreenQuad(textureID);
}

void Utility::ExitShowImage()
{
	Print("ExitShowImage");
	// Delete screen_quad, and use normal shader
	GLOBAL.sceneMgr->RemoveSceneObj("screen_quad");
	GLOBAL.sceneMgr->SetCurrentRenderMethod("RenderSimple");
}

nlohmann::json Utility::LoadJsonFromFile(const string& _filePath)
{
	ifstream s(_filePath);
	if (!s.is_open())
	{
		Print("[Error] failed to open: " + _filePath);
		return nullptr;
	}
	std::stringstream ss;
	ss << s.rdbuf();
	string jsonStr = ss.str();

	s.close();
	// just test
	// Print(jsonStr);

	// refer: https://github.com/nlohmann/json#examples
	nlohmann::json sceneData = nullptr;
	try
	{
		sceneData = nlohmann::json::parse(jsonStr);
		if (sceneData.contains("shader"))
		{
			auto ss = sceneData["shader"];
		}
		return sceneData;
	}
	catch (const nlohmann::json::parse_error& e)
	{
		Print(NULL, "ss", "Exception: ", e.what());
	}
	catch (const nlohmann::json::exception& e)
	{
		Print(NULL, "ss", "Exception: ", e.what());
	}
	catch (const std::exception& e)
	{
		Print(NULL, "ss", "Exception: ", e.what());
	}
	return nullptr;
}

glm::vec3 Utility::LoadVec3FromJsonData(nlohmann::json _data)
{
	std::vector<float> fdata;
	try
	{
		fdata = _data.get<std::vector<float>>();
		if (fdata.size() == 1)
			return glm::vec3(fdata[0]);
		else
			return glm::vec3(fdata[0], fdata[1], fdata[2]);
	}
	catch (const nlohmann::json::parse_error& e)
	{
		Print(NULL, "ss", "Exception: ", e.what());
	}
	catch (const nlohmann::json::exception& e)
	{
		Print(NULL, "ss", "Exception: ", e.what());
	}
	catch (const std::exception& e)
	{
		Print(NULL, "ss", "Exception: ", e.what());
	}
	return glm::vec3();
}

bool Utility::SaveTextureToPNG(const std::string& _fileName, const int& _width, const int& _height, const GLenum& _channelType, const GLuint& _framebuffer)
{
	int channelNum;
	if (_channelType == GL_RGBA)
		channelNum = 4;
	else if (_channelType == GL_RGB)
		channelNum = 3;
	else if (_channelType == GL_DEPTH_COMPONENT)
		channelNum = 1;
	else
	{
		Print("Error: SaveTextureToPNG() don't accept any channel type except GL_RGBA or GL_RGB");
		return false;
	}
	int data_size = _width * _height * channelNum;
	GLubyte* pixels = new GLubyte[data_size];
	glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer); CheckGLError();
	glReadPixels(0, 0, _width, _height, _channelType, GL_UNSIGNED_BYTE, pixels); CheckGLError();
	stbi_flip_vertically_on_write(1); // it enables to write an image as OpenGL expects!!!
	int result = stbi_write_png(("Output/" + _fileName + ".png").c_str(), _width, _height, channelNum, pixels, channelNum * _width);
	if (result == 0)
		Print("Fail to save texture.");
	else
		Print("Succeed to save texture.");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return result != 0;
}

std::string Utility::GetCurrentTimeStr()
{
	// TODO: pass a parameter to customize time string output
	time_t rawTime;
	struct tm* timeInfo;
	time(&rawTime);
	timeInfo = localtime(&rawTime);
	std::string timeStr = std::string(asctime(timeInfo));
	timeStr = timeStr.substr(0, timeStr.size() - 1); // timeStr is like: "Fri Dec 30 22:49:28 2022"
	auto timeSubstrList = SplitString(timeStr, " ");
	auto hmsList = SplitString(timeSubstrList[3], ":");
	timeSubstrList.erase(timeSubstrList.begin() + 3);
	
	// TODO: for now just return "Dec_30_2022_22h_49m_28s"
	timeStr = "";
	for (int i = 1; i < timeSubstrList.size(); i++)
		timeStr.append(timeSubstrList[i] + "_");
	timeStr.append(hmsList[0] + "h_");
	timeStr.append(hmsList[1] + "m_");
	timeStr.append(hmsList[2] + "s");
	return timeStr;
}
