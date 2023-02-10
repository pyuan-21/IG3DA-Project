#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <stdexcept>

namespace IceRender
{
	using namespace std;

	class Mesh
	{
	public:
		enum class MeshDataType
		{
			INDEX,
			POS,
			NORMAL,
		};

	private:
		vector<glm::uvec3> indices;
		vector<glm::vec3> positions;
		vector<glm::vec3> normals;
		
		size_t iboIndex; // index of ibo in buffers
		size_t vboIndex; // index of vbo in buffers
		size_t vaoIndex; // index of vao in vaos

	public:
		Mesh();
		~Mesh();

		void SetIndices(const vector<glm::uvec3>& _indices);
		void SetPositions(const vector<glm::vec3>& _pos);
		void SetNormals(const vector<glm::vec3>& _normals);
		
		void SetIboIndex(const size_t& _iboIndex);
		void SetVboIndex(const size_t& _vboIndex);
		void SetVaoIndex(const size_t& _vaoIndex);

		const void* GetData(MeshDataType _type) const;
		size_t GetElementSize(MeshDataType _type) const;
		size_t GetElementCount(MeshDataType _type) const;
		size_t GetBufferSize(MeshDataType _type) const;
		GLint GetDataComponentType(MeshDataType _type) const;
		GLint GetDataComponentNum(MeshDataType _type) const;
		size_t GetIboIndex() const;
		size_t GetVboIndex() const;
		size_t GetVaoIndex() const;
		vector<glm::vec3> GetNormals() const;
		vector<glm::vec3> GetPositions() const;
	};
}
