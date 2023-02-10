#pragma once
#include <string>
#include "../mesh/mesh.hpp"
#include <memory>
#include "../transform/transform.hpp"
#include "../material/material.hpp"
#include "../spatial_structure/AABB.hpp"


namespace IceRender
{
	using namespace std;

	class SceneObject
	{
	private:
		string name;
		shared_ptr<Mesh> mesh;
		shared_ptr<Transform> transform;
		shared_ptr<Material> material;
		shared_ptr<AABB> meshAABB;

	public:
		SceneObject(const string& _name);
		SceneObject(const string& _name, const shared_ptr<Mesh>& _mesh);
		SceneObject(const string& _name, const shared_ptr<Mesh>& _mesh, const shared_ptr<Material> _material);
		~SceneObject();

		void SetMesh(const shared_ptr<Mesh>& _mesh);
		void SetMaterial(const shared_ptr<Material>& _material);

		shared_ptr<Mesh> GetMesh() const; // allow any operation outside to change the mesh directly
		shared_ptr<Material> GetMaterial() const;
		
		const string GetName() const;

		shared_ptr<Transform> GetTransform(); // allow any operation outside to change the transform directly
		shared_ptr<AABB> GetMeshAABB(); // AABB of mesh
		shared_ptr<AABB> GetBoundingBox(); // considering the actual mesh will have transformation(translation, rotation, scale), therefore, we need to compute the run-time boundingbox for it
	};
}

