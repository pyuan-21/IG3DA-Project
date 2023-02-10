#include "rasterizer.hpp"
#include "../globals.hpp"
#include "../scene/sceneObject.hpp"
#include "renderMethod.hpp"
#include "../helpers/utility.hpp"

using namespace std;
using namespace IceRender;

Rasterizer::Rasterizer() {}
Rasterizer::~Rasterizer() {}

void Rasterizer::Init()
{
	glViewport(0, 0, (GLint)GLOBAL.WIN_WIDTH, (GLint)GLOBAL.WIN_HEIGHT); // Dimension of the rendering region in the window
	InitRenderFuncMap();
}

void Rasterizer::Setting()
{
	// TODO: read configure to change setting.
	// configure can be changed at runtime
	
	glEnable(GL_CULL_FACE); // enable culling face
	glCullFace(GL_BACK); // Specifies the faces to cull (here the ones pointing away from the camera)

	glDepthFunc(GL_LESS); // Specify the depth test for the z-buffer
	glEnable(GL_DEPTH_TEST); // Enable the z-buffer test in the rasterization
}

void Rasterizer::Render()
{
	// TODO: maybe it is not good way to call render function here,
	// to reconstruct here later when implementing SSAO/SSDO/VSM and etc.
	string curRenderMethod = GLOBAL.sceneMgr->GetCurrentRenderMethod();
	if (!curRenderMethod.empty())
	{
		if (GLOBAL.shadowMgr->IsNeedShadowRender())
			GLOBAL.shadowMgr->RenderShadow();
		// call different render method based on current active shader program
		auto it = renderFuncMap.find(curRenderMethod);
		if (it != renderFuncMap.end())
			it->second();
		else
			Print("[Error] No corresponding render functions for current RenderMethod: " + curRenderMethod);
	}
}

void Rasterizer::Clear()
{
	// delete all buffers
	DeleteAllBuffers();

	// delete all VAO
	DeleteAllVertexArray();

	// Clear all shader program
	GLOBAL.shaderMgr->Clear();

	renderFuncMap.clear();
}

size_t Rasterizer::CreateBuffer()
{
	GLuint buffID;
	glCreateBuffers(1, &buffID);
	
	for (size_t i = 0; i < buffers.size(); i++)
	{
		if (buffers[i] == 0)
		{
			buffers[i] = buffID;
			return i; // find an empty slot
		}
	}
	
	// no empty slot found, then push back
	buffers.push_back(buffID);
	return buffers.size() - 1; // return index of buffers
}

void Rasterizer::DeleteBuffers(vector<size_t>& _indices)
{
	//std::sort(_indices.begin(), _indices.end(), std::greater<size_t>()); // ensure indices are descreasing order. because we need to delete element from back to head.
	for (auto iter = _indices.begin(); iter != _indices.end(); iter++)
	{
		GLuint buffID = buffers[*iter];
		glDeleteBuffers(1, &buffID);
		//buffers.erase(buffers.begin() + *iter); // [Note] Don't delete the element in buffers, because it will affect the reference index for other scene object!
		buffers[*iter] = 0; // [Note] Instead, just put it equal to zero, in order to indicate this slot is empty
	}
}

size_t Rasterizer::CreateVertexArray()
{
	/*
	* Setting up VAO
	* VAO is not related to Models. It is only related to VertexShader.
	* For now I only create one VAO
	*/
	GLuint vaoID;
	glCreateVertexArrays(1, &vaoID);

	for (size_t i = 0; i < vaos.size(); i++)
	{
		if (vaos[i] == 0)
		{
			vaos[i] = vaoID;
			return i; // find an empty slot
		}
	}

	vaos.push_back(vaoID);
	return vaos.size() - 1; // return index of vaos
}

void Rasterizer::DeleteVertexArray(vector<size_t>& _indices)
{
	//std::sort(_indices.begin(), _indices.end(), std::greater<size_t>()); // ensure indices are descreasing order. because we need to delete element from back to head.
	for (auto iter = _indices.begin(); iter != _indices.end(); iter++)
	{
		//GLuint vaoID = *(vaos.begin() + *iter);
		GLuint vaoID = vaos[*iter];
		glDeleteVertexArrays(1, &vaoID);
		
		//vaos.erase(vaos.begin() + *iter);  // [Note] Don't delete the element in vaos, because it will affect the reference index for other scene object!
		vaos[*iter] = 0;  // [Note] Instead, just put it equal to zero, in order to indicate this slot is empty
	}
}

void Rasterizer::DeleteAllBuffers()
{
	for (auto iter = buffers.begin(); iter != buffers.end(); iter++)
	{
		auto bufferID = *iter;
		if (bufferID != 0)
			glDeleteBuffers(1, &bufferID);
	}
	buffers.clear();
}

void Rasterizer::DeleteAllVertexArray()
{
	for (auto iter = vaos.begin(); iter != vaos.end(); iter++)
	{
		auto vaoID = *iter;
		if (vaoID != 0)
			glDeleteVertexArrays(1, &vaoID);
	}
	vaos.clear();
}

void Rasterizer::InitGPUData(shared_ptr<SceneObject>& _sceneObj)
{
	/*
	* The basic idea to use buffer here is that:
	* For each model(which may contains only one mesh), we use two buffer to store its data:
	* - one buffer for triangles indices only
	* - one buffer for vertex attribute such as position, normal, uv and so on. I will put them into one single buffer by using offset
	* - one vertex array for reading these two buffers
	*/
	shared_ptr<Mesh> meshPtr = _sceneObj->GetMesh();
	size_t iboIndex = CreateBuffer();
	meshPtr->SetIboIndex(iboIndex);
	size_t vboIndex = CreateBuffer();
	meshPtr->SetVboIndex(vboIndex);

	// (1) initialize two-buffers
	// initialize indices buffer
	GLuint ibo = buffers[iboIndex];
	size_t bufferSize = meshPtr->GetBufferSize(Mesh::MeshDataType::INDEX);
	glNamedBufferStorage(ibo, bufferSize, meshPtr->GetData(Mesh::MeshDataType::INDEX), GL_DYNAMIC_STORAGE_BIT); // allocate and initilize buffer

	// initialize vertices buffer
	GLuint vbo = buffers[vboIndex];
	size_t pBufSize = meshPtr->GetBufferSize(Mesh::MeshDataType::POS); // memory size of position data 
	size_t nBufSize = meshPtr->GetBufferSize(Mesh::MeshDataType::NORMAL); // memory size of normal data
	size_t uvBufSize = 0;
	shared_ptr<Material> materialPtr = _sceneObj->GetMaterial();
	if (materialPtr)
		uvBufSize = materialPtr->GetUVDataSize();
	size_t totalBufSize = pBufSize + nBufSize + uvBufSize;
	glNamedBufferStorage(vbo, totalBufSize, NULL, GL_DYNAMIC_STORAGE_BIT); // allocate enough size buffer
	// initialize buffer

	glNamedBufferSubData(vbo, 0, pBufSize, meshPtr->GetData(Mesh::MeshDataType::POS)); // initialize posistion data
	glNamedBufferSubData(vbo, pBufSize, nBufSize, meshPtr->GetData(Mesh::MeshDataType::NORMAL)); // initialize normal data

	if (uvBufSize > 0)
		glNamedBufferSubData(vbo, pBufSize + nBufSize, uvBufSize, materialPtr->GetUVData()); // initialize albedo data

	// (2) initialize how to read these two buffers
	// initialize vertex array
	size_t vaoIndex = CreateVertexArray();
	meshPtr->SetVaoIndex(vaoIndex);
	glBindVertexArray(vaos[vaoIndex]); // enable this VertexArray
	// Here, data-binding must call before setting up VertexAttributeArray/Pointer
	glBindBuffer(GL_ARRAY_BUFFER, vbo); // for vertex data. 
	/*
	* The below glVertexAttribPointer() setting is actually related to the current active Vertex/Frag shader. The first parameter is related to 'location' in shader.
	*/
	GLint componentType = meshPtr->GetDataComponentType(Mesh::MeshDataType::POS);
	GLint componentNum = meshPtr->GetDataComponentNum(Mesh::MeshDataType::POS);
	// because pos,normal,albedo are using the same data-type, therefore they share the same componentType and componentNum
	glVertexAttribPointer(0, 3, componentType, GL_FALSE, componentNum * sizeof(componentType), 0); // second parameter range is vec1,vec2,vec3,vec4, depending which data you are using.
	glVertexAttribPointer(1, 3, componentType, GL_FALSE, componentNum * sizeof(componentType), (const void*)pBufSize); // last parameter is offset, it's related to above buffer setting
	if (uvBufSize > 0)
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), (const void*)(pBufSize + nBufSize));

	glEnableVertexAttribArray(0); // location=0 in shader, you can EnableVertexAttribArray first then set its VeretxArrayPointer. The order doesn't matter.
	glEnableVertexAttribArray(1); // location=1 in shader
	if (uvBufSize > 0)
		glEnableVertexAttribArray(2); // location=2 in shader

	// Don't forget to bind indices!
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo); // for vertex-index. Index can be called at the end. Because we don't need VertexAttribArray/Pointer to get these data

	glBindVertexArray(0); // After setting up, bind 0 to avoid modification of vao. If we want to use it, bind vao again.
}

void Rasterizer::DeleteGPUData(const shared_ptr<SceneObject>& _sceneObj)
{
	vector<size_t> buffersIndices; // Indices which point to the buffers
	vector<size_t> vaoIndices; // Indices which point to the vao
	shared_ptr<Mesh> meshPtr = _sceneObj->GetMesh();
	buffersIndices.push_back(meshPtr->GetIboIndex());
	buffersIndices.push_back(meshPtr->GetVboIndex());
	vaoIndices.push_back(meshPtr->GetVaoIndex());
	DeleteBuffers(buffersIndices);
	DeleteVertexArray(vaoIndices);
}

void Rasterizer::Draw(const shared_ptr<SceneObject>& _sceneObj)
{
	auto meshPtr = _sceneObj->GetMesh();
	GLuint vao = vaos[meshPtr->GetVaoIndex()];
	glBindVertexArray(vao);
	GLsizei triangleCount = meshPtr->GetElementCount(Mesh::MeshDataType::INDEX);
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(triangleCount * 3), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Rasterizer::InitRenderFuncMap()
{
	// TODO: keep update here if any new render function
	renderFuncMap["NoRender"] = RasterizerRender::NoRender;
	renderFuncMap["RenderSimple"] = RasterizerRender::RenderSimple;
	renderFuncMap["RenderPhong"] = RasterizerRender::RenderPhong;
	renderFuncMap["RenderSceenQuad"] = RasterizerRender::RenderSceenQuad;

	// below is for fun
	renderFuncMap["RenderSonarLight"] = RasterizerRender::RenderSonarLight;
}

