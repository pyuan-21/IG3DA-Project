#include "mesh.hpp"

using namespace IceRender;

Mesh::Mesh() {}
Mesh::~Mesh(){}

void Mesh::SetIndices(const vector<glm::uvec3>& _indices) { indices = _indices; }
void Mesh::SetPositions(const vector<glm::vec3>& _pos) { positions = _pos; }
void Mesh::SetNormals(const vector<glm::vec3>& _normals) { normals = _normals; }

const void* Mesh::GetData(MeshDataType _type) const
{
	switch (_type)
	{
	case MeshDataType::INDEX: return indices.data();
	case MeshDataType::POS:	return positions.data();
	case MeshDataType::NORMAL: return normals.data();
	}
	throw std::invalid_argument("[Exception] No such MeshDataType");
}

size_t Mesh::GetElementSize(MeshDataType _type) const
{
	switch (_type)
	{
	case MeshDataType::INDEX: 
		return sizeof(glm::uvec3);
	case MeshDataType::POS:
	case MeshDataType::NORMAL:
		return sizeof(glm::vec3);
	}
	throw std::invalid_argument("[Exception] No such MeshDataType");
}

size_t Mesh::GetElementCount(MeshDataType _type) const
{
	switch (_type)
	{
	case MeshDataType::INDEX: return indices.size();
	case MeshDataType::POS:	return positions.size();
	case MeshDataType::NORMAL: return normals.size();
	}
	throw std::invalid_argument("[Exception] No such MeshDataType");
}

size_t Mesh::GetBufferSize(MeshDataType _type) const
{
	return GetElementSize(_type) * GetElementCount(_type);
}

GLint Mesh::GetDataComponentType(MeshDataType _type) const
{
	switch (_type)
	{
	case MeshDataType::INDEX:
		return GL_UNSIGNED_INT; // because it's glm::uvec3
	case MeshDataType::POS:
	case MeshDataType::NORMAL:
		return GL_FLOAT; // because it's glm::vec3
	}
	throw std::invalid_argument("[Exception] No such MeshDataType");
}

GLint Mesh::GetDataComponentNum(MeshDataType _type) const
{
	switch (_type)
	{
	case MeshDataType::INDEX: // it's glm::uvec3
	case MeshDataType::POS:
	case MeshDataType::NORMAL:
		return 3;
	}
	throw std::invalid_argument("[Exception] No such MeshDataType");
}

void Mesh::SetIboIndex(const size_t& _iboIndex) { iboIndex = _iboIndex; }
void Mesh::SetVboIndex(const size_t& _vboIndex) { vboIndex = _vboIndex; }
void Mesh::SetVaoIndex(const size_t& _vaoIndex) { vaoIndex = _vaoIndex; }

size_t Mesh::GetIboIndex() const { return iboIndex; }
size_t Mesh::GetVboIndex() const { return vboIndex; }
size_t Mesh::GetVaoIndex() const { return vaoIndex; }

vector<glm::vec3> Mesh::GetNormals() const { return normals; }
vector<glm::vec3> Mesh::GetPositions() const { return positions; }
