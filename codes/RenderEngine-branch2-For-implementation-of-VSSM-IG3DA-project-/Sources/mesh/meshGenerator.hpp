#pragma once

#include <memory>
#include "mesh.hpp"
#include <vector>
#include <string>


namespace IceRender
{
	namespace MeshGenerator
	{
		// _hNum: sampling amount in horizontal direction, _vNum: sampling amount in vertical direction
		shared_ptr<Mesh> GenSphere(const size_t& _hNum, const size_t& _vNum, const float& _radius);
		shared_ptr<Mesh> GenPlane(const float& _width, const float& _height, const glm::vec3& _center, const glm::vec3& _normal);
		shared_ptr<Mesh> GenCube(const float& _length);

		std::vector<glm::vec2> GenSphereUV(const size_t& _hNum, const size_t& _vNum);
		std::vector<glm::vec2> GenPlaneUV();

#pragma region load mesh from files
		// TODO: support loading other file which contains uv coordinates.

		// file name should be *.off (their parent folder should be "Resources\Models\OFF\")
		shared_ptr<Mesh> GenMeshFromOFF(const std::string _fileName);

		// file name should be *.obj (their parent folder should be "Resources\Models\OBJ\")
		shared_ptr<Mesh> GenMeshFromOBJ(const std::string _fileName, vector<glm::vec2>& _uv);
#pragma endregion
	};
}