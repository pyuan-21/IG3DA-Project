#pragma once

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"
#include "../transform/transform.hpp"
#include <string>
#include <vector>
#include "../globals.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "../nlohmann/json.hpp"


namespace IceRender
{
	namespace Utility
	{
		bool _glCheckError(const char* file, int line);

#ifndef ICE_ERROR_CHECKER
#define ICE_ERROR_CHECKER
// [TODO] In release version, for performance, just define CheckGLError to empty.
#define CheckGLError() Utility::_glCheckError(__FILE__, __LINE__) 
#endif // !

		const float zeroFlag = 1e-3;

		// positions or directions
		const glm::vec3 zeroV3(0, 0, 0);
		const glm::vec3 upV3(0, 1, 0); // Y axis(positive direction)
		const glm::vec3 downV3(0, -1, 0);
		const glm::vec3 leftV3(-1, 0, 0);
		const glm::vec3 rightV3(1, 0, 0); // X axis(positive direction)
		const glm::vec3 frontV3(0, 0, -1);
		const glm::vec3 backV3(0, 0, 1); // Z axis(positive direction)
		const glm::vec2 zeroV2(0, 0);
		const glm::vec2 upV2(0, 1);
		const glm::vec2 downV2(0, -1);
		const glm::vec2 leftV2(-1, 0);
		const glm::vec2 rightV2(1, 0);

		// rotation
		const glm::mat4 mat4Identity = glm::identity<glm::mat4>();
		const glm::quat quatIdentity = glm::quat_identity<float, glm::packed_highp>();

		// scale
		const glm::vec3 oneV3(1, 1, 1);

		// color
		const glm::vec3 black(0, 0, 0);
		const glm::vec3 white(1, 1, 1);
		const glm::vec3 red(1, 0, 0);
		const glm::vec3 green(0, 1, 0);
		const glm::vec3 blue(0, 0, 1);

		// _from, _to must be unit vectors
		float GetAngleInRadians(const glm::vec3& _from, const glm::vec3& _to);

		// _from, _to must be unit vectors
		float GetAngleInDegree(const glm::vec3& _from, const glm::vec3& _to);

		// _from, _to must be unit vectors
		glm::quat GetQuaternion(const glm::vec3& _from, const glm::vec3& _to, const float _radians);

		// _axis must be unit vectors
		glm::vec3 RotateVector(const glm::vec3& _orign, const glm::vec3& _axis, const float _degree);

		void PrintVec3(const glm::vec3& _vec, const std::string _title = "");
		void PrintTransform(const Transform& _trans, const std::string _title = "");
		void PrintMat4(const glm::mat4& _mat, const std::string _title = "");

		std::vector<std::string> SplitString(const std::string& _input, const std::string& _delimiter);

		// _fileName should be the relative path to "Resource/Images", use '/' to separate folders.
		bool Load2DTexture(const std::string& _fileName, const bool& _defaultSetting, GLuint& _textureID);

		// This function can be later used to render ShadowMap or something else.
		void RenderScreenQuad(const GLuint& _textureID);
		void ShowImage(const std::string& _fileName);
		void ExitShowImage();

		nlohmann::json LoadJsonFromFile(const string& _filePath);
		glm::vec3 LoadVec3FromJsonData(nlohmann::json _data);

		// refer: https://stackoverflow.com/questions/56140002/saving-a-glteximage2d-to-the-file-system-for-inspection
		// to save texture to a file, we need to read it from framebuffer(which means we need to bind texture to a framebuffer)
		bool SaveTextureToPNG(const std::string& _fileName, const int& _width, const int& _height, const GLenum& _channelType, const GLuint& _framebuffer);

		std::string GetCurrentTimeStr();
	}
}