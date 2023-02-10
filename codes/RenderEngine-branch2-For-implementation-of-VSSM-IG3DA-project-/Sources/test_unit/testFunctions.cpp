#include <vector>
#include <glm/glm.hpp>
#include "testunit.hpp"
#include "../globals.hpp"
#include <memory>
#include "../mesh/meshGenerator.hpp"
#include "testFunctions.hpp"
#include "../helpers/utility.hpp"
#include "../scene/sceneObjectGenerator.hpp"
#include "../light/pointLight.hpp"
#include "../material/phongMaterial.hpp"
#include "stb_image.h"

using namespace std;
using namespace IceRender;
// ---------------------------------------------------------------------------------------------------------------------------------------------------------
// below are Test functions definition
// ---------------------------------------------------------------------------------------------------------------------------------------------------------

void TestFunctions::Test1()
{
	Print("Calling Test1");

	// may be I will use it later
	vector<glm::uvec3> vIndices; // = { 0,1,2,0,2,3 };
	vIndices.push_back(glm::uvec3(0, 1, 2));
	vIndices.push_back(glm::uvec3(0, 2, 3));

	vector<glm::vec3> vPos; // = { -1.0, -1.0, 0.0, 1.0, -1.0, 0.0, 1.0, 1.0, 0.0, -1.0, 1.0, 0.0 }; // screen quad
	vPos.push_back(glm::vec3(-1, -1, 0));
	vPos.push_back(glm::vec3(1, -1, 0));
	vPos.push_back(glm::vec3(1, 1, 0));
	vPos.push_back(glm::vec3(-1, 1, 0));

	vector<glm::vec3> vNormals;
	vNormals.push_back(glm::vec3(0, 0, 1));
	vNormals.push_back(glm::vec3(0, 0, 1));
	vNormals.push_back(glm::vec3(0, 0, 1));
	vNormals.push_back(glm::vec3(0, 0, 1));

	vector<glm::vec3> vColor;
	vColor.push_back(glm::vec3(1, 0, 0));
	vColor.push_back(glm::vec3(1, 1, 0));
	vColor.push_back(glm::vec3(1, 0, 1));
	vColor.push_back(glm::vec3(0, 1, 0));
	vColor.push_back(glm::vec3(0, 0, 1));
	vColor.push_back(glm::vec3(1, 1, 1));

	shared_ptr<Mesh> mesh = make_shared<Mesh>();
	mesh->SetIndices(vIndices);
	mesh->SetPositions(vPos);
	mesh->SetNormals(vNormals);

	shared_ptr<SceneObject> obj = make_shared<SceneObject>("obj_1", mesh);

	GLOBAL.sceneMgr->AddSceneObj(obj);
}

void TestFunctions::Test2()
{
	Print("Calling Test2");
	
#pragma region test plane
	//float a = glm::length(Utility::backV3);
	//float b = glm::length(Utility::rightV3);
	////glm::vec3 normal = Utility::RotateVector(Utility::backV3, Utility::upV3, 80);
	////glm::vec3 normal = Utility::frontV3;
	//glm::vec3 normal = Utility::backV3;
	//float len = glm::length(normal);
	//shared_ptr<Mesh> mesh = MeshGenerator::GenPlane(1, 1, Utility::zeroV3, glm::normalize(normal));
	//shared_ptr<SceneObject> obj = make_shared<SceneObject>("obj_1", mesh);
	//GLOBAL.sceneMgr->AddSceneObj(obj);
	//obj->GetTransform()->SetSubPosition(-2, 2);
#pragma endregion

	auto obj2 = SceneObjectGenerator::GenSphereObject("obj_2");

	// load texture
	GLuint albedo;
	std::string texPath = "Simple/earth.jpg";
	texPath = "Test/test_uv.jpg";
	if (Utility::Load2DTexture(texPath, true, albedo))
		obj2->GetMaterial()->SetAlbedo(albedo);

	GLOBAL.sceneMgr->AddSceneObj(obj2);
}

void TestFunctions::Test3()
{
	Print("Test3 calling...");

	auto shadowMapRender = dynamic_pointer_cast<VarianceShadowMapRender>(GLOBAL.shadowMgr->GetShadowRender());
	int width, height;
	shadowMapRender->GetResolution(width, height);

	// verify SAT correctness, reconstruct image from SAT
	GLuint originalTex = 0;
	glCreateTextures(GL_TEXTURE_2D, 1, &originalTex);
	glTextureStorage2D(originalTex, 1, GL_RG16F, width, height); if (CheckGLError()) return;
	// (for test 2-components)
	//glTextureStorage2D(originalTex, 1, GL_RG16, width, height); if (CheckGLError()) return;
	glTextureParameteri(originalTex, GL_TEXTURE_WRAP_S, GL_REPEAT); if (CheckGLError()) return;
	glTextureParameteri(originalTex, GL_TEXTURE_WRAP_T, GL_REPEAT); if (CheckGLError()) return;
	glTextureParameteri(originalTex, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); if (CheckGLError()) return;
	glTextureParameteri(originalTex, GL_TEXTURE_MAG_FILTER, GL_LINEAR); if (CheckGLError()) return;

	auto satGenerator = shadowMapRender->GetSATGenerator(1);
	satGenerator->Reconstruct(originalTex);

	Utility::RenderScreenQuad(originalTex);
}

void TestFunctions::TestPhong()
{
	Print("Calling TestPhong");

	// add lights first
	shared_ptr<PointLight> light1 = make_shared<PointLight>("point_light_1");
	light1->GetTransform()->SetPosition(glm::vec3(10, 10, 10));
	light1->SetIntensity(5);
	// TODO: modify light's color, intensity, range, position and etc.
	GLOBAL.sceneMgr->AddLight(light1);

	shared_ptr<PointLight> light2 = make_shared<PointLight>("point_light_2");
	light2->GetTransform()->SetPosition(glm::vec3(-10, -10, 10));
	light2->SetRange(2000);
	GLOBAL.sceneMgr->AddLight(light2);

	glm::vec3 ambient(0.5f);
	GLOBAL.sceneMgr->SetAmbient(ambient);

	// TODO: modify its material's color, albedo texture, ka, kd, ks and etc
	auto material = make_shared<PhongMaterial>();
	//material->SetColor(glm::vec3(0, 1, 0));

	glm::vec3 ka = Utility::oneV3;
	material->SetAmbientCoef(ka);

	glm::vec3 kd(0.5f);
	material->SetDiffuseCoef(kd);

	glm::vec3 ks(1.f);
	material->SetSpecularCoef(ks);
	material->SetShiness(32);
	
	auto obj1 = SceneObjectGenerator::GenSphereObject("obj_1", material);
	GLOBAL.sceneMgr->AddSceneObj(obj1);

	GLuint albedo;
	std::string texPath = "Simple/earth.jpg";
	if (Utility::Load2DTexture(texPath, true, albedo))
		material->SetAlbedo(albedo);

	GLOBAL.sceneMgr->SetCurrentRenderMethod("RenderPhong");
}

void TestFunctions::TestSAT()
{
	/*example for using SAT and test its correctness by using SAT to reconstruct the original texture, don't delete or modify this function*/

	Print("Calling TestSAT...");

	GLOBAL.timeMgr->StartRecord();

	GLuint textureID = 0;
	int width, height;
	std::string texPath = "Simple/earth.jpg";
	{
		int channel;
		std::string total = GLOBAL.imagePathPrefix + texPath;
		stbi_set_flip_vertically_on_load(true); // it enables to load an image as OpenGL expects!!!
		unsigned char* data = stbi_load(total.c_str(), &width, &height, &channel, 0);
		if (data)
		{
			glCreateTextures(GL_TEXTURE_2D, 1, &textureID); if (CheckGLError()) { stbi_image_free(data); return; }
			// input internal format is not important
			glTextureStorage2D(textureID, 1, GL_RGB12, width, height); if (CheckGLError()) { stbi_image_free(data); return; }
			glTextureSubImage2D(textureID, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data); if (CheckGLError()) { stbi_image_free(data); return; }
			stbi_image_free(data);
			glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_REPEAT); if (CheckGLError()) return;
			glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_REPEAT); if (CheckGLError()) return;
			glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); if (CheckGLError()) return;
			glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR); if (CheckGLError()) return;
		}
		else
		{
			Print("[Error] Can not open file " + texPath + " to load texture, reason: " + stbi_failure_reason());
			return;
		}
	}

	// Generate SAT for this VSM
	SATConfig config;
	config.resWidth = width;
	config.resHeight = height;
	config.componentNum = 3; // just test for 3 RGB image
	//config.componentNum = 2; // just test for 2 RG image (for test 2-components)
	// definte texture generator, using the same texture parameters setting as input(depthTex) does.
	config.texGenerator = [](GLuint& _texID, const int& _width, const int& _height)
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &_texID); if (CheckGLError()) { Print("Error in Test3()."); return; };
		// beacause RGB sum up will be greater than 1. Therefore we should use floating-point
		//glTextureStorage2D(_texID, 1, GL_RGB16F, _width, _height); if (CheckGLError()) { Print("Error in Test3()."); return; }; // GL_RGB16F is not enough
		//(for test 2-components)
		//glTextureStorage2D(_texID, 1, GL_RG32F, _width, _height); if (CheckGLError()) { Print("Error in VarianceShadowMapRender::texGenerator."); return; };
		glTextureStorage2D(_texID, 1, GL_RGB32F, _width, _height); if (CheckGLError()) { Print("Error in Test3()."); return; }; // Only GL_RGB32F works
		glTextureParameteri(_texID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); if (CheckGLError()) { Print("Error in Test3()."); return; };
		glTextureParameteri(_texID, GL_TEXTURE_MAG_FILTER, GL_LINEAR); if (CheckGLError()) { Print("Error in Test3()."); return; };
		glTextureParameteri(_texID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTextureParameteri(_texID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		const float borderColor[] = { 0,0,0,0 }; // [Important, Note] must be zero value. according to paper. When adding elements outside of range, it should add zero.
		glTextureParameterfv(_texID, GL_TEXTURE_BORDER_COLOR, borderColor); if (CheckGLError()) { Print("Error in Test3()."); return; };
	};
	config.inputTexID = textureID;

	std::shared_ptr<SummedAreaTableGenerator> satGenerator = std::make_shared<SummedAreaTableGenerator>();
	satGenerator->Init(config);
	
	// render
	satGenerator->Generate();

	// verify SAT correctness, reconstruct image from SAT
	GLuint originalTex = 0;
	glCreateTextures(GL_TEXTURE_2D, 1, &originalTex);
	glTextureStorage2D(originalTex, 1, GL_RGB12, width, height); if (CheckGLError()) return;
	// (for test 2-components)
	//glTextureStorage2D(originalTex, 1, GL_RG16, width, height); if (CheckGLError()) return;
	glTextureParameteri(originalTex, GL_TEXTURE_WRAP_S, GL_REPEAT); if (CheckGLError()) return;
	glTextureParameteri(originalTex, GL_TEXTURE_WRAP_T, GL_REPEAT); if (CheckGLError()) return;
	glTextureParameteri(originalTex, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); if (CheckGLError()) return;
	glTextureParameteri(originalTex, GL_TEXTURE_MAG_FILTER, GL_LINEAR); if (CheckGLError()) return;

	/*test Reconstruction*/
	satGenerator->Reconstruct(originalTex);

	/*test BoxFilter*/
	int kernelSize = 3;
	satGenerator->BoxFilter(originalTex, kernelSize);

	double timeSpan = GLOBAL.timeMgr->EndRecord();
	Print("SAT spends: " + std::to_string(timeSpan));

	satGenerator = nullptr;

	Utility::RenderScreenQuad(originalTex);
}

void TestFunctions::Test4()
{
	/*Save depth texture to PNG files*/
	//int width, height;
	//shared_ptr<VarianceShadowMapRender> shadowMapRender1 = dynamic_pointer_cast<VarianceShadowMapRender>(GLOBAL.shadowMgr->GetShadowRender());
	//shadowMapRender1->GetResolution(width, height);
	//Utility::SaveTextureToPNG("DepthTex", width, height, GL_RGB, shadowMapRender1->GetDepthFrameBuffer(0));

	/*Render depth texture to screen for observation and stop rende the shadow*/
	//GLuint depthTex = shadowMapRender1->GetDepthTexture(0);
	//Utility::RenderScreenQuad(depthTex);
	//GLOBAL.sceneMgr->GetBaseLight("pl")->SetRenderShadow(false);

}
