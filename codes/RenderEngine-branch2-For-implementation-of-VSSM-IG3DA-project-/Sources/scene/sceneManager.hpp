#pragma once

#include <memory>
#include <vector>
#include "sceneObject.hpp"
#include "../light/baseLight.hpp"
#include "../shadow/shadow.hpp"
#include "json.hpp"
#include "../spatial_structure/AABB.hpp"


namespace IceRender
{
	using namespace std;

	class SceneManager
	{
	private:
		vector<shared_ptr<SceneObject>> sceneObjs; // all sceneObjects (current)

		int maxLightNum; // TODO: read configure to set it as well as shader's maxLightNum(to edit file dynamically?)
		vector<shared_ptr<BaseLight>> lights;

		glm::vec3 ambient; // TODO: to improve code struct later. We should put light setting into a configure file ?

		string renderMethod; // specify current render method

	public:
		SceneManager();
		~SceneManager();

		void Init();

		vector<shared_ptr<SceneObject>>& GetAllSceneObject();
		void AddSceneObj(const shared_ptr<SceneObject>& _obj);
		void RemoveSceneObj(const string& _name);
		shared_ptr<SceneObject> GetSceneObj(const string& _name) const;

		glm::vec3 GetAmbient() const;
		void SetAmbient(const glm::vec3& _ambient);

		void AddLight(const shared_ptr<BaseLight>& _light);
		void RemoveLight(const string& _name);
		shared_ptr<BaseLight> GetBaseLight(const string& _name) const;
		vector<shared_ptr<BaseLight>>& GetAllLight();
		int GetLightNum() const;

		void ClearAll();

		// initialize scene by loading config file
		void LoadFromSceneConfig(const string& _configPath);

		string GetCurrentRenderMethod() const;
		void SetCurrentRenderMethod(const string& _value);

		shared_ptr<AABB> GetBoundingBox();
	};
}