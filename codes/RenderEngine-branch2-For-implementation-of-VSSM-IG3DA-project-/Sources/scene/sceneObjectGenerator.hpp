#pragma once

#include <memory>
#include "sceneObject.hpp"
#include <string>

namespace IceRender
{
	namespace SceneObjectGenerator
	{
		shared_ptr<SceneObject> GenSphereObject(const std::string& _name, shared_ptr<Material> _material = make_shared<Material>(), const int& _hNum = 30, const int& _vNum = 30);

		// render screenquad by using specific textureID. [for verifying the texture correctness or just visualize it]
		shared_ptr<SceneObject> GenScreenQuadObject(const GLuint& _textureID);

		shared_ptr<SceneObject> GenOFFObject(const std::string& _name, const std::string& _fileName, shared_ptr<Material> _material = nullptr);

		shared_ptr<SceneObject> GenCubeOject(const std::string& _name, const float& _length, shared_ptr<Material> _material = nullptr);

		shared_ptr<SceneObject> GenPlaneOject(const std::string& _name, shared_ptr<Material> _material = nullptr);

		shared_ptr<SceneObject> GenOBJObject(const std::string& _name, const std::string& _fileName, shared_ptr<Material> _material = nullptr);
	}
}
