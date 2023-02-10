#include "renderMethod.hpp"
#include "../globals.hpp"
#include "../light/pointLight.hpp"
#include "../light/directLight.hpp"
#include "../material/phongMaterial.hpp"
#include "../helpers/utility.hpp"


using namespace IceRender;

void RasterizerRender::NoRender() {/*do nothing*/ };

void RasterizerRender::RenderSimple()
{
	glClearColor(0.67f, 0.84f, 0.90f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Erase the color and z buffers.
	glm::mat4 viewMat = GLOBAL.camCtrller->GetActiveCamera()->GetViewMatrix();
	glm::mat4 projectMat = GLOBAL.camCtrller->GetActiveCamera()->GetProjectionMatrix();
	// try to get "Simple/simple" shader pro, if not exist, create/activate it and return it
	shared_ptr<ShaderProgram> shaderPro = GLOBAL.shaderMgr->TryActivateShaderProgram(GLOBAL.shaderPathPrefix + "Simple/simple");
	shaderPro->Set("viewMat", viewMat);
	shaderPro->Set("projectMat", projectMat);
	auto sceneObjs = GLOBAL.sceneMgr->GetAllSceneObject(); // not copy data, just return reference &
	for (auto iter = sceneObjs.begin(); iter != sceneObjs.end(); iter++)
	{
		auto sceneObj = *iter;
		glm::mat4 modelMat = sceneObj->GetTransform()->ComputeTransformationMatrix();
		shaderPro->Set("modelMat", modelMat);

		auto material = sceneObj->GetMaterial();
		if (material && material->GetUVDataSize() > 0 && material->GetAlbedo() != 0)
		{
			// OpenGL 4.5 way to use texture:
			GLuint texUnit = 0;
			glBindTextureUnit(texUnit, material->GetAlbedo()); // this function to bind texture object to sampler2D variable with "binding=0"
			//glActiveTexture(GL_TEXTURE0 + texUnit); // no need actually for now
			shaderPro->Set("useAlbedoTex", 1);
		}
		else if(material)
		{
			shaderPro->Set("useAlbedoTex", 0);
			shaderPro->Set("albedoColor", material->GetColor());
		}
		else
			shaderPro->Set("useAlbedoTex", 0);

		GLOBAL.render->Draw(sceneObj);
	}
}

void RasterizerRender::RenderPhong()
{
	glViewport(0, 0, GLOBAL.WIN_WIDTH, GLOBAL.WIN_HEIGHT); // set it back to normal
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.67f, 0.84f, 0.90f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Erase the color and z buffers.

	GLuint texUnit = 0; // each shader start with texture0

	glm::mat4 viewMat = GLOBAL.camCtrller->GetActiveCamera()->GetViewMatrix();
	glm::mat4 projectMat = GLOBAL.camCtrller->GetActiveCamera()->GetProjectionMatrix();
	shared_ptr<ShaderProgram> shaderPro = GLOBAL.shaderMgr->TryActivateShaderProgram(GLOBAL.shaderPathPrefix + "Phong/phong");
	shaderPro->Set("viewMat", viewMat);
	shaderPro->Set("projectMat", projectMat);

	bool needShadowRender = GLOBAL.shadowMgr->IsNeedShadowRender();
	shared_ptr<BasicShadowMapRender> basicShadowMapRender;
	if (needShadowRender)
		basicShadowMapRender = dynamic_pointer_cast<BasicShadowMapRender>(GLOBAL.shadowMgr->GetShadowRender());

	// pass light information to current shader
	shaderPro->Set("ambientLight", GLOBAL.sceneMgr->GetAmbient());
	auto lights = GLOBAL.sceneMgr->GetAllLight();
	shaderPro->Set("activeLightNum", static_cast<int>(lights.size()));
	for (int i = 0; i < lights.size(); i++)
	{
		shaderPro->Set("lights[" + std::to_string(i) + "].type", static_cast<int>(lights[i]->GetType()));
		shaderPro->Set("lights[" + std::to_string(i) + "].color", lights[i]->GetColor());
		shaderPro->Set("lights[" + std::to_string(i) + "].pos", lights[i]->GetTransform()->GetPosition());
		shaderPro->Set("lights[" + std::to_string(i) + "].intensity", lights[i]->GetIntensity());

		if (needShadowRender && lights[i]->IsRenderShadow())
			shaderPro->Set("lights[" + std::to_string(i) + "].renderShadow", 1);
		else
			shaderPro->Set("lights[" + std::to_string(i) + "].renderShadow", 0);

		if (lights[i]->GetType() == LightType::POINT)
		{
			shared_ptr<PointLight> pointLight = static_pointer_cast<PointLight>(lights[i]);
			shaderPro->Set("lights[" + std::to_string(i) + "].attenuation", pointLight->GetAttenuation());
		}
		else if (lights[i]->GetType() == LightType::DIRECT)
		{
			shared_ptr<DirectLight> directLight = static_pointer_cast<DirectLight>(lights[i]);
			shaderPro->Set("lights[" + std::to_string(i) + "].dir", -directLight->GetDirection()); // shader is using the direction from fragment to light source, then here we should pass -direction.
		}
	}

	// allow different shadow tecnique to use different light ratio sub shader.(pass parameters to final shader)
	// [Be careful] must pass texUnit into this function in order to bind the correct texture location in shader.
	basicShadowMapRender->InitComputeLightRatioParameters(shaderPro, texUnit);

	// TODO: I need to experience then I can optimize the draw call to handle multiple same object rendering(use instance), and static scene environemnt rendering?
	auto sceneObjs = GLOBAL.sceneMgr->GetAllSceneObject(); // not copy data, just return reference &
	for (auto iter = sceneObjs.begin(); iter != sceneObjs.end(); iter++)
	{
		auto sceneObj = *iter;
		glm::mat4 modelMat = sceneObj->GetTransform()->ComputeTransformationMatrix();
		shaderPro->Set("modelMat", modelMat);
		// pass material information to shader
		auto material = static_pointer_cast<PhongMaterial>(sceneObj->GetMaterial());
		shaderPro->Set("material.ka", material->GetAmbientCoef());
		shaderPro->Set("material.kd", material->GetDiffuseCoef());
		shaderPro->Set("material.ks", material->GetSpecularCoef());
		shaderPro->Set("material.shiness", material->GetShiness());
		if (material && material->GetUVDataSize() > 0 && material->GetAlbedo() != 0)
		{
			// OpenGL 4.5 way to use texture:
			// refer: https://www.khronos.org/opengl/wiki/Example_Code
			// TODO: but not sure whether I am doing right or not. Finish reading OpenGL book later then back to here.
			shaderPro->Set("albedoTex", static_cast<int>(texUnit));
			glBindTextureUnit(texUnit++, material->GetAlbedo()); // this function to bind texture object to sampler2D variable with "binding=texUnit"
			shaderPro->Set("useAlbedoTex", 1);
		}
		else
		{
			shaderPro->Set("useAlbedoTex", 0);
			shaderPro->Set("material.color", material->GetColor());
		}
		GLOBAL.render->Draw(sceneObj);
	}
}

void RasterizerRender::RenderSceenQuad()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Erase the color and z buffers.
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	shared_ptr<ShaderProgram> shaderPro = GLOBAL.shaderMgr->TryActivateShaderProgram(GLOBAL.shaderPathPrefix + "ScreenQuad/screenQuad");
	auto screenQuadObj = GLOBAL.sceneMgr->GetSceneObj("screen_quad");
	auto material = screenQuadObj->GetMaterial();
	GLuint texID = material->GetAlbedo();
	if (material && material->GetUVDataSize() > 0 && glIsTexture(texID))
	{
		/*below code can render the original size texture*/
		//int width, height;
		//glGetTextureLevelParameteriv(texID, 0, GL_TEXTURE_WIDTH, &width);
		//glGetTextureLevelParameteriv(texID, 0, GL_TEXTURE_HEIGHT, &height);
		//glViewport(0, 0, width, height); // set it back to normal
		// OpenGL 4.5 way to use texture:
		shaderPro->Set("useAlbedoTex", 1);
		GLuint texUnit = 0;
		glBindTextureUnit(texUnit, texID);
		shaderPro->Set("albedoTex", static_cast<int>(texUnit));
	}
	else
	{
		shaderPro->Set("useAlbedoTex", 0);
		shaderPro->Set("albedoColor", material->GetColor());
	}
	GLOBAL.render->Draw(screenQuadObj);
	glViewport(0, 0, GLOBAL.WIN_WIDTH, GLOBAL.WIN_HEIGHT); // set it back to normal
}

void RasterizerRender::RenderSonarLight()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glClearColor(0.67f, 0.84f, 0.90f, 1.f);
	glClearColor(0.0f, 0.0f, 0.0f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Erase the color and z buffers.

	GLuint texUnit = 0; // each shader start with texture0

	glm::mat4 viewMat = GLOBAL.camCtrller->GetActiveCamera()->GetViewMatrix();
	glm::mat4 projectMat = GLOBAL.camCtrller->GetActiveCamera()->GetProjectionMatrix();
	shared_ptr<ShaderProgram> shaderPro = GLOBAL.shaderMgr->TryActivateShaderProgram(GLOBAL.shaderPathPrefix + "SonarLight/sonarLight");
	shaderPro->Set("viewMat", viewMat);
	shaderPro->Set("projectMat", projectMat);

	// Here is the idea, we use such an area, which is inside two spheres with different radius: r1 and r2, to represent the sonar wave
	// All vertices inside this wave get lit up(distance to light is inside range [r1,r2] is lit up)
	// These two radius can be represent by using r-width/2, r+width/2, where width is user-defined, r is the distance of this wave to the sonar source.
	// to simulate the propagation of wave, we just move the r from 0 to waveInterval and repeat it.
	// compute the sonar light radius
	static float waveMaxDepth;
	static float waveSpeed;
	static float waveWidth;
	static float waveInterval;
	static float waveMoveOffset = 0;
	// TODO: to delete below codes, for now they are just temporary codes
	// TODO: to support save .gif as output to show something interesting like this "SonarLight"
	{
		// below codes are written for debug
		// below scene setting is an example for setting up the maxDepth, width, waveSpeed
		string configPath = "Resources/SceneConfigs/SonarLightScene/sonar_light.json";
		nlohmann::json sceneData = Utility::LoadJsonFromFile(configPath);

		/*
			"wave_width": 0.01,
			"max_depth": 2.0,
			"wave_speed": 0.5,
			"wave_interval": 0.5
		*/
		waveWidth = sceneData["sonar_light_config"]["wave_width"].get<float>();
		waveMaxDepth = sceneData["sonar_light_config"]["max_depth"].get<float>();
		waveSpeed = sceneData["sonar_light_config"]["wave_speed"].get<float>();
		waveInterval = sceneData["sonar_light_config"]["wave_interval"].get<float>();
	}

	shaderPro->Set("waveMaxDepth", waveMaxDepth);
	shaderPro->Set("waveWidth", waveWidth);
	shaderPro->Set("waveInterval", waveInterval);
	
	waveMoveOffset += waveSpeed * GLOBAL.timeMgr->GetDeltaTime();
	if (waveMoveOffset >= waveInterval)
		waveMoveOffset = 0;

	//Print("waveMoveOffset: " + std::to_string(waveMoveOffset));
	shaderPro->Set("waveMoveOffset", waveMoveOffset);

	// pass light information to current shader
	shaderPro->Set("ambientLight", GLOBAL.sceneMgr->GetAmbient());
	auto lights = GLOBAL.sceneMgr->GetAllLight();
	shaderPro->Set("activeLightNum", static_cast<int>(lights.size()));
	for (int i = 0; i < lights.size(); i++)
	{
		shaderPro->Set("lights[" + std::to_string(i) + "].type", static_cast<int>(lights[i]->GetType()));
		shaderPro->Set("lights[" + std::to_string(i) + "].color", lights[i]->GetColor());
		shaderPro->Set("lights[" + std::to_string(i) + "].pos", lights[i]->GetTransform()->GetPosition());
		shaderPro->Set("lights[" + std::to_string(i) + "].intensity", lights[i]->GetIntensity());

		if (lights[i]->GetType() == LightType::POINT)
		{
			shared_ptr<PointLight> pointLight = static_pointer_cast<PointLight>(lights[i]);
			shaderPro->Set("lights[" + std::to_string(i) + "].attenuation", pointLight->GetAttenuation());
		}
		else if (lights[i]->GetType() == LightType::DIRECT)
		{
			shared_ptr<DirectLight> directLight = static_pointer_cast<DirectLight>(lights[i]);
			shaderPro->Set("lights[" + std::to_string(i) + "].dir", -directLight->GetDirection()); // shader is using the direction from fragment to light source, then here we should pass -direction.
		}
	}

	// TODO: I need to experience then I can optimize the draw call to handle multiple same object rendering(use instance), and static scene environemnt rendering?
	auto sceneObjs = GLOBAL.sceneMgr->GetAllSceneObject(); // not copy data, just return reference &
	for (auto iter = sceneObjs.begin(); iter != sceneObjs.end(); iter++)
	{
		auto sceneObj = *iter;
		glm::mat4 modelMat = sceneObj->GetTransform()->ComputeTransformationMatrix();
		shaderPro->Set("modelMat", modelMat);
		// pass material information to shader
		auto material = static_pointer_cast<PhongMaterial>(sceneObj->GetMaterial());
		shaderPro->Set("material.ka", material->GetAmbientCoef());
		shaderPro->Set("material.kd", material->GetDiffuseCoef());
		shaderPro->Set("material.ks", material->GetSpecularCoef());
		shaderPro->Set("material.shiness", material->GetShiness());
		if (material && material->GetUVDataSize() > 0 && material->GetAlbedo() != 0)
		{
			// OpenGL 4.5 way to use texture:
			// refer: https://www.khronos.org/opengl/wiki/Example_Code
			// TODO: but not sure whether I am doing right or not. Finish reading OpenGL book later then back to here.
			shaderPro->Set("albedoTex", static_cast<int>(texUnit));
			glBindTextureUnit(texUnit++, material->GetAlbedo()); // this function to bind texture object to sampler2D variable with "binding=texUnit"
			shaderPro->Set("useAlbedoTex", 1);
		}
		else
		{
			shaderPro->Set("useAlbedoTex", 0);
			shaderPro->Set("material.color", material->GetColor());
		}
		GLOBAL.render->Draw(sceneObj);
	}
}
