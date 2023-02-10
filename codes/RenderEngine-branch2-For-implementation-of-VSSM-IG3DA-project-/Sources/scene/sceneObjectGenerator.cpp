#include "sceneObjectGenerator.hpp"
#include "../mesh/meshGenerator.hpp"
#include "../material/material.hpp"
#include <string>
#include "../helpers/logger.hpp"
#include "../globals.hpp"
#include "../helpers/utility.hpp"


using namespace IceRender;

shared_ptr<SceneObject> SceneObjectGenerator::GenSphereObject(const std::string& _name, shared_ptr<Material> _material, const int& _hNum, const int& _vNum)
{
	shared_ptr<Mesh> mesh = MeshGenerator::GenSphere(_hNum, _vNum, 0.5f);

	_material->SetUV(MeshGenerator::GenSphereUV(_hNum, _vNum));

	shared_ptr<SceneObject> obj = make_shared<SceneObject>(_name, mesh, _material);
	return obj;
}

shared_ptr<SceneObject> SceneObjectGenerator::GenScreenQuadObject(const GLuint& _textureID)
{
	shared_ptr<SceneObject> screenQuadObj = GLOBAL.sceneMgr->GetSceneObj("screen_quad");
	if (screenQuadObj == nullptr)
	{
		shared_ptr<Mesh> mesh = MeshGenerator::GenPlane(2, 2, glm::vec3(0, 0, -1), Utility::backV3);
		shared_ptr<Material> material = make_shared<Material>();
		material->SetUV(MeshGenerator::GenPlaneUV());
		material->SetAlbedo(_textureID);
		screenQuadObj = make_shared<SceneObject>("screen_quad", mesh, material);
	}
	return screenQuadObj;
}

shared_ptr<SceneObject> SceneObjectGenerator::GenOFFObject(const std::string& _name, const std::string& _fileName, shared_ptr<Material> _material)
{
	shared_ptr<Mesh> mesh = MeshGenerator::GenMeshFromOFF(_fileName);
	if (mesh != nullptr)
		return make_shared<SceneObject>(_name, mesh, _material);
	return nullptr;
}

shared_ptr<SceneObject> SceneObjectGenerator::GenCubeOject(const std::string& _name, const float& _length, shared_ptr<Material> _material)
{
	shared_ptr<Mesh> mesh = MeshGenerator::GenCube(_length);
	return make_shared<SceneObject>(_name, mesh, _material);
}

shared_ptr<SceneObject> SceneObjectGenerator::GenPlaneOject(const std::string& _name, shared_ptr<Material> _material)
{
	// unit plane
	shared_ptr<Mesh> mesh = MeshGenerator::GenPlane(1.0f, 1.0f, Utility::zeroV3, Utility::upV3);
	if (_material != nullptr)
		_material->SetUV(MeshGenerator::GenPlaneUV());
	return make_shared<SceneObject>(_name, mesh, _material);
}

shared_ptr<SceneObject> SceneObjectGenerator::GenOBJObject(const std::string& _name, const std::string& _fileName, shared_ptr<Material> _material)
{
	vector<glm::vec2> uv;
	shared_ptr<Mesh> mesh = MeshGenerator::GenMeshFromOBJ(_fileName, uv);
	if (mesh != nullptr)
	{
		if (_material != nullptr)
			_material->SetUV(uv);
		return make_shared<SceneObject>(_name, mesh, _material);
	}
	return nullptr;
}