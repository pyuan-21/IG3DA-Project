#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <glm/glm.hpp>
#include "../shadermgr/shaderManager.hpp"
#include <string>
#include "../mesh/mesh.hpp"
#include <algorithm>
#include "../scene/sceneManager.hpp"
#include <map>
#include <string>
#include <functional>

namespace IceRender
{
	using namespace std;

	class Rasterizer
	{
	private:
		vector<GLuint> vaos; // store all bufferID, '0' means invalid ID, non-zero means valid ID(also works for OpenGL buffer ID)
		vector<GLuint> buffers; // store all bufferID, '0' means invalid ID, non-zero means valid ID(same for OpenGL vaoID)
		map<string, function<void()>> renderFuncMap;

		size_t CreateBuffer(); // Call CreateBuffers() to create one buffer for each model, in order to store positions, normals, materials(which is related to albedo), or uv
		size_t CreateVertexArray();

		void DeleteBuffers(vector<size_t>& _indices); // input indices of buffervec
		void DeleteVertexArray(vector<size_t>& _indices);

		void InitRenderFuncMap();

	public:
		Rasterizer();
		~Rasterizer();

		void Init();

		// set something such as culling face.
		void Setting();

		// TODO: to see reduce draw calls, for now we just set 2 buffers & 1 VeterxArray for each Mesh (because VAO could not read two different buffers at the same time)
		void Render();

		void Draw(const shared_ptr<SceneObject>& _sceneObj);

		void Clear();

		void DeleteAllBuffers();
		void DeleteAllVertexArray();

		// Response to dynamically add/remove mesh
		void InitGPUData(shared_ptr<SceneObject>& _sceneObj); // when scene add meshes, create buffers&vao for them
		void DeleteGPUData(const shared_ptr<SceneObject>& _sceneObj); // when scene remove meshes, delete buffers&vao for them
	};
}