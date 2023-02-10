#include "sceneObject.hpp"

using namespace IceRender;

SceneObject::SceneObject(const string& _name) :name(_name), mesh(nullptr), material(nullptr), transform(make_shared<Transform>()), meshAABB(make_shared<AABB>()) {}
SceneObject::SceneObject(const string& _name, const shared_ptr<Mesh>& _mesh) :
	name(_name), mesh(_mesh), material(nullptr), transform(make_shared<Transform>()), meshAABB(make_shared<AABB>()) { meshAABB->Recompute(_mesh->GetPositions()); }
SceneObject::SceneObject(const string& _name, const shared_ptr<Mesh>& _mesh, const shared_ptr<Material> _material) : 
	name(_name), mesh(_mesh), material(_material), transform(make_shared<Transform>()), meshAABB(make_shared<AABB>()) { meshAABB->Recompute(_mesh->GetPositions()); } 
SceneObject::~SceneObject()
{
	mesh = nullptr;
	material = nullptr;
	transform = nullptr;
	meshAABB = nullptr;
}

void SceneObject::SetMesh(const shared_ptr<Mesh>& _mesh) { mesh = _mesh; meshAABB->Recompute(_mesh->GetPositions()); }
void SceneObject::SetMaterial(const shared_ptr<Material>& _material) { material = _material; }

shared_ptr<Mesh> SceneObject::GetMesh() const { return mesh; }
shared_ptr<Material> SceneObject::GetMaterial() const { return material; }

const string SceneObject::GetName() const { return name; }

shared_ptr<Transform> SceneObject::GetTransform() { return transform; }
shared_ptr<AABB> SceneObject::GetMeshAABB() { return meshAABB; }
shared_ptr<AABB> SceneObject::GetBoundingBox()
{
	// TODO: for better performance, maybe I should optimize here -> after all, scene bounding box will be computed each frame when using ShadowMap technique
	glm::vec3 min, max, size;
	min = meshAABB->GetMin();
	max = meshAABB->GetMax();
	size = max - min;
	glm::vec3 p[8]; // 8 corners of scene bounding box
	p[0] = min;
	p[1] = min + glm::vec3(size.x, 0, 0);
	p[2] = min + glm::vec3(0, size.y, 0);
	p[3] = min + glm::vec3(size.x, size.y, 0);

	p[4] = max;
	p[5] = max - glm::vec3(size.x, 0, 0);
	p[6] = max - glm::vec3(0, size.y, 0);
	p[7] = max - glm::vec3(size.x, size.y, 0);

	// build the bounding box from these 8 corner points
	shared_ptr<AABB> boundingBox = make_shared<AABB>();
	glm::mat4 modelMat = transform->ComputeTransformationMatrix();
	for (int i = 0; i < 8; i++)
	{
		glm::vec4 p_homo = glm::vec4(p[i], 1); // homogeneous point
		p_homo = modelMat * p_homo;
		p[i] = glm::vec3(p_homo.x, p_homo.y, p_homo.z);
		boundingBox->Extend(p[i]);
	}
	return boundingBox;
}
