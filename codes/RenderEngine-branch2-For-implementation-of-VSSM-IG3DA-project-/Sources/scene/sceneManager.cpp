#include <algorithm>
#include "sceneManager.hpp"
#include "../globals.hpp"
#include "../helpers/utility.hpp"
#include "../light/pointLight.hpp"
#include "../light/directLight.hpp"
#include "../scene/sceneObjectGenerator.hpp"
#include "../material/phongMaterial.hpp"


using namespace IceRender;

SceneManager::SceneManager() :maxLightNum(5), ambient(0, 0, 0) {}
SceneManager::~SceneManager() { sceneObjs.clear(); lights.clear(); }

void SceneManager::Init()
{
	// can only assign here becasue GLOBAL need to be constructed first,
	renderMethod = GLOBAL.defaultRenderMethod;
}

vector<shared_ptr<SceneObject>>& SceneManager::GetAllSceneObject() { return sceneObjs; }

void SceneManager::AddSceneObj(const shared_ptr<SceneObject>& _obj)
{ 
	sceneObjs.push_back(_obj);
	GLOBAL.render->InitGPUData(sceneObjs[sceneObjs.size() - 1]);
}

void SceneManager::RemoveSceneObj(const string& _name)
{
	for (auto iter = sceneObjs.begin(); iter != sceneObjs.end(); iter++)
	{
		auto sceneObj = *iter;
		if (sceneObj->GetName() == _name) {
			GLOBAL.render->DeleteGPUData(sceneObj);
			sceneObjs.erase(iter);
			break;
		}
	}
}

shared_ptr<SceneObject> SceneManager::GetSceneObj(const string& _name) const
{
	for (auto iter = sceneObjs.begin(); iter != sceneObjs.end(); iter++)
	{
		if ((*iter)->GetName() == _name)
			return *iter;
	}
	return nullptr;
}

glm::vec3 SceneManager::GetAmbient() const { return ambient; }
void SceneManager::SetAmbient(const glm::vec3& _ambient) { ambient = _ambient; }

void SceneManager::AddLight(const shared_ptr<BaseLight>& _light) { if (lights.size() < maxLightNum)lights.push_back(_light); }
void SceneManager::RemoveLight(const string& _name)
{
	std::remove_if(lights.begin(), lights.end(), [&](const shared_ptr<BaseLight>& _light) {return _light->GetName() == _name; });
}
shared_ptr<BaseLight> SceneManager::GetBaseLight(const string& _name) const
{
	auto result = std::find_if(lights.begin(), lights.end(), [&](const shared_ptr<BaseLight>& _light) {return _light->GetName() == _name; });
	if (result != lights.end())
		return *result;
	return nullptr;
}
vector<shared_ptr<BaseLight>>& SceneManager::GetAllLight() { return lights; }
int SceneManager::GetLightNum() const { return lights.size(); }

void SceneManager::ClearAll() 
{ 
	// clear scene objects,
	for (auto iter = sceneObjs.begin(); iter != sceneObjs.end(); iter++)
		GLOBAL.render->DeleteGPUData(*iter); // [Note] don't forget release the GPU data to avoid memory leak
	sceneObjs.clear();
	
	// clear lights
	lights.clear();
}

void SceneManager::LoadFromSceneConfig(const string& _configPath)
{
	// TODO: [Keep updating here]
	// TODO: pyuan-21: this function is not finsihed. To update it later.(loading model mesh and etc.
	// TODO: also to improve it, split each initializer of light/sceneobject into a small function

	ClearAll(); // clear all first

	// bascially create scene objects, initialize their materials, create lights and etc.

	// load json file by using the config path
	nlohmann::json sceneData = Utility::LoadJsonFromFile(_configPath);

	if (sceneData != nullptr)
	{
		// refer: https://json.nlohmann.me/api/basic_json/
		
		// If forget how to use, check "Resources/Shaders/UsageOfGenerateShader.md"
		if (sceneData.contains("generate_shader"))
			GLOBAL.shaderMgr->GenerateShaderFilesFromConfig(sceneData["generate_shader"]);

		if (sceneData.contains("camera"))
		{
			nlohmann::json cameraData = sceneData["camera"];
			if (cameraData.contains("position"))
				GLOBAL.camCtrller->SetCameraPosition(Utility::LoadVec3FromJsonData(cameraData["position"]));
			if (cameraData.contains("rotation"))
				GLOBAL.camCtrller->SetCameraRotation(Utility::LoadVec3FromJsonData(cameraData["rotation"]));
		}

		// set environment ambient (TODO: improve it later)
		if (sceneData.contains("ambient"))
			GLOBAL.sceneMgr->SetAmbient(Utility::LoadVec3FromJsonData(sceneData["ambient"]));

		// set current render method
		// TODO: support different shaders later
		if (sceneData.contains("render_method"))
			GLOBAL.sceneMgr->SetCurrentRenderMethod(string(sceneData["render_method"]));

		// TODO: keep update here
		if (sceneData.contains("shadow_config"))
			GLOBAL.shadowMgr->LoadShadowRender(sceneData["shadow_config"]);
		
		// add lights
		if (sceneData.contains("lights"))
		{
			nlohmann::json lightsData = sceneData["lights"];
			for (int i = 0; i < lightsData.size(); i++)
			{
				// name and type are necessary
				nlohmann::json lightData = lightsData[i];
				string name = lightData["name"];
				string type = lightData["type"];
				shared_ptr<BaseLight> light;
				if (type == "point_light")
				{
					shared_ptr<PointLight> pointLight = make_shared<PointLight>(name);
					if (lightData.contains("position"))
						pointLight->GetTransform()->SetPosition(Utility::LoadVec3FromJsonData(lightData["position"]));

					if(lightData.contains("intensity"))
						pointLight->SetIntensity(lightData["intensity"]);

					if (lightData.contains("range"))
						pointLight->SetRange(lightData["range"]);

					// don't forget to refer it
					light = pointLight;
				}
				else if (type == "direct_light")
				{
					shared_ptr<DirectLight> directLight = make_shared<DirectLight>(name);
					if (lightData.contains("direction"))
						directLight->SetDirection(Utility::LoadVec3FromJsonData(lightData["direction"]));

					if (lightData.contains("intensity"))
						directLight->SetIntensity(lightData["intensity"]);

					// don't forget to refer it
					light = directLight;
				}

				// add light into scene
				if (light != nullptr)
				{
					if (lightData.contains("render_shadow"))
						light->SetRenderShadow(lightData["render_shadow"].get<bool>());

					GLOBAL.sceneMgr->AddLight(light);
				}
			}
		}

		// add scene objects
		if (sceneData.contains("scene_objects"))
		{
			nlohmann::json sceneObjsData = sceneData["scene_objects"];
			for (int i = 0; i < sceneObjsData.size(); i++)
			{
				// name and type are necessary
				nlohmann::json sceneObjData = sceneObjsData[i];
				string name = sceneObjData["name"];
				string sceneObjType = sceneObjData["type"];

				shared_ptr<SceneObject> sceneObj;
				shared_ptr<Material> material;

				// attach its material
				if (sceneObjData.contains("material"))
				{
					nlohmann::json materialData = sceneObjData["material"];
					string matType = materialData["type"];
					if (matType == "phong_mat")
					{
						auto phongMat = make_shared<PhongMaterial>();

						if (materialData.contains("color"))
							phongMat->SetColor(Utility::LoadVec3FromJsonData(materialData["color"]));

						if (materialData.contains("ka"))
							phongMat->SetAmbientCoef(Utility::LoadVec3FromJsonData(materialData["ka"]));

						if (materialData.contains("kd"))
							phongMat->SetDiffuseCoef(Utility::LoadVec3FromJsonData(materialData["kd"]));

						if (materialData.contains("ks"))
							phongMat->SetSpecularCoef(Utility::LoadVec3FromJsonData(materialData["ks"]));
						
						if (materialData.contains("shiness"))
							phongMat->SetShiness(materialData["shiness"]);
						
						if (materialData.contains("albedo_tex"))
						{
							GLuint albedo;
							std::string texPath = materialData["albedo_tex"];
							if (Utility::Load2DTexture(texPath, true, albedo))
							{
								phongMat->SetAlbedo(albedo);
							}
						}
						// don't forget to refer it
						material = phongMat;
					}
				}

				// material might be nullptr
				// create scene object, and try attach material if it exists
				if (sceneObjType == "sphere")
					sceneObj = SceneObjectGenerator::GenSphereObject(name, material);
				else if (sceneObjType == "cube" && sceneObjData.contains("cube_len"))
					sceneObj = SceneObjectGenerator::GenCubeOject(name, sceneObjData["cube_len"], material);
				else if (sceneObjType == "off" && sceneObjData.contains("model"))
					sceneObj = SceneObjectGenerator::GenOFFObject(name, sceneObjData["model"], material);
				else if (sceneObjType == "plane")
					sceneObj = SceneObjectGenerator::GenPlaneOject(name, material);
				else if (sceneObjType == "obj" && sceneObjData.contains("model"))
					sceneObj = SceneObjectGenerator::GenOBJObject(name, sceneObjData["model"], material);

				// add it into scene
				if (sceneObj != nullptr)
				{
					if (sceneObjData.contains("transform"))
					{
						// set transformation
						nlohmann::json transformData = sceneObjData["transform"];
						auto transform = sceneObj->GetTransform();
						if (transformData.contains("position"))
							transform->SetPosition(Utility::LoadVec3FromJsonData(transformData["position"]));
						if (transformData.contains("rotation"))
							transform->SetRotation(Utility::LoadVec3FromJsonData(transformData["rotation"]));
						if (transformData.contains("scale"))
							transform->SetScale(Utility::LoadVec3FromJsonData(transformData["scale"]));
					}

					GLOBAL.sceneMgr->AddSceneObj(sceneObj);
				}
			}
		}
	}
}

string SceneManager::GetCurrentRenderMethod() const { return renderMethod; }
void SceneManager::SetCurrentRenderMethod(const string& _value) { renderMethod = _value; }
shared_ptr<AABB> SceneManager::GetBoundingBox()
{
	// recompute the scene bounding box
	auto boundingBox = make_shared<AABB>();
	for (auto iter = sceneObjs.begin(); iter != sceneObjs.end(); iter++)
		boundingBox->Extend((*iter)->GetBoundingBox());
	return boundingBox;
}

