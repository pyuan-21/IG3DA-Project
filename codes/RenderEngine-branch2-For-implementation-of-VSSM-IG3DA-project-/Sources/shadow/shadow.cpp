#include "shadow.hpp"
#include "../globals.hpp"
#include "../helpers/utility.hpp"
#include "../mesh/meshGenerator.hpp"


using namespace IceRender;
using namespace std;

ShadowManager::ShadowManager() : useTightSpace(false) {}

ShadowManager::~ShadowManager()
{
	RemoveShadowRender();
}

void ShadowManager::RemoveShadowRender()
{
	if (shadowRender != nullptr)
		shadowRender->Clear();
	shadowRender = nullptr;
}

void ShadowManager::LoadShadowRender(nlohmann::json _data)
{
	// TODO: keep update here
	string method;
	if (_data.contains("shadow_method"))
	{
		RemoveShadowRender();

		method = string(_data["shadow_method"]);

		if (method == "ShadowMap" || method == "VarianceShadowMap" || method == "VSSM")
		{
			shared_ptr<BasicShadowMapRender> basicShadowMapRender;

			if (method == "VarianceShadowMap")
			{
				auto vsmRender = make_shared<VarianceShadowMapRender>();

				if (_data.contains("kernel_size"))
					vsmRender->SetKernelSize(_data["kernel_size"]);
				else
					vsmRender->SetKernelSize(1);

				if (_data.contains("variance_min"))
					vsmRender->SetVarianceMin(_data["variance_min"]);
				else
					vsmRender->SetVarianceMin(0);

				if (_data.contains("p_min"))
					vsmRender->SetPMin(_data["p_min"]);
				else
					vsmRender->SetPMin(0);

				if (_data.contains("use_sat"))
					vsmRender->SetUseSAT(_data["use_sat"].get<bool>());
				else
					vsmRender->SetUseSAT(false);

				// setting PCSS
				bool usePCSS = false;
				if (_data.contains("use_pcss"))
					usePCSS = _data["use_pcss"];

				if (usePCSS)
				{
					int maxSearchSize, lightSize, minPenumbraSize, maxPenumbraSize;
					float penumbraRatio;

					if (_data.contains("max_search_size"))
						maxSearchSize = _data["max_search_size"];
					else
						maxSearchSize = 3;

					if (_data.contains("light_size"))
						lightSize = _data["light_size"];
					else
						lightSize = 3;

					if (_data.contains("min_penumbra_size"))
						minPenumbraSize = _data["min_penumbra_size"];
					else
						minPenumbraSize = 5;

					if (_data.contains("max_penumbra_size"))
						maxPenumbraSize = _data["max_penumbra_size"];
					else
						maxPenumbraSize = 7;

					if (_data.contains("penumbra_ratio"))
						penumbraRatio = _data["penumbra_ratio"];
					else
						penumbraRatio = 1.0;

					vsmRender->InitPCSS(true, maxSearchSize, lightSize, minPenumbraSize, maxPenumbraSize, penumbraRatio);
				}
				else
					vsmRender->InitPCSS(false, 0, 0, 0, 0, 0);

				basicShadowMapRender = vsmRender;
			}
			else if (method == "VSSM")
			{
				auto vssmRender = make_shared<VSSMRender>();

				vssmRender->InitSubdivision(_data["M"], _data["N"]);

				if (_data.contains("variance_min"))
					vssmRender->SetVarianceMin(_data["variance_min"]);
				else
					vssmRender->SetVarianceMin(0);

				if (_data.contains("p_min"))
					vssmRender->SetPMin(_data["p_min"]);
				else
					vssmRender->SetPMin(0);

				// setting PCSS
				int maxSearchSize, lightSize, minPenumbraSize, maxPenumbraSize;
				float penumbraRatio;

				if (_data.contains("max_search_size"))
					maxSearchSize = _data["max_search_size"];
				else
					maxSearchSize = 3;

				if (_data.contains("light_size"))
					lightSize = _data["light_size"];
				else
					lightSize = 3;

				if (_data.contains("min_penumbra_size"))
					minPenumbraSize = _data["min_penumbra_size"];
				else
					minPenumbraSize = 5;

				if (_data.contains("max_penumbra_size"))
					maxPenumbraSize = _data["max_penumbra_size"];
				else
					maxPenumbraSize = 7;

				if (_data.contains("penumbra_ratio"))
					penumbraRatio = _data["penumbra_ratio"];
				else
					penumbraRatio = 1.0;

				vssmRender->InitPCSS(true, maxSearchSize, lightSize, minPenumbraSize, maxPenumbraSize, penumbraRatio);

				basicShadowMapRender = vssmRender;
			}
			else
			{
				auto smRender = make_shared<ShadowMapRender>();

				if (_data.contains("bias"))
					smRender->SetBias(_data["bias"]);
				else
					smRender->SetBias(0);

				{
					// setting PCF
					bool usePCF = false;
					if (_data.contains("use_pcf"))
						usePCF = _data["use_pcf"];

					if (usePCF)
					{
						if (_data.contains("pcf_kernel_size"))
							smRender->InitPCF(true, _data["pcf_kernel_size"]);
						else
							smRender->InitPCF(true, 1);
					}
					else
						smRender->InitPCF(false, 0);
				}

				{
					// setting PCSS
					bool usePCSS = false;
					if (_data.contains("use_pcss"))
						usePCSS = _data["use_pcss"];

					if (usePCSS)
					{
						int maxSearchSize, lightSize, minPenumbraSize, maxPenumbraSize;
						float penumbraRatio;

						if (_data.contains("max_search_size"))
							maxSearchSize = _data["max_search_size"];
						else
							maxSearchSize = 3;

						if (_data.contains("light_size"))
							lightSize = _data["light_size"];
						else
							lightSize = 3;

						if (_data.contains("min_penumbra_size"))
							minPenumbraSize = _data["min_penumbra_size"];
						else
							minPenumbraSize = 5;

						if (_data.contains("max_penumbra_size"))
							maxPenumbraSize = _data["max_penumbra_size"];
						else
							maxPenumbraSize = 7;

						if (_data.contains("penumbra_ratio"))
							penumbraRatio = _data["penumbra_ratio"];
						else
							penumbraRatio = 1.0;

						smRender->InitPCSS(true, maxSearchSize, lightSize, minPenumbraSize, maxPenumbraSize, penumbraRatio);
					}
					else
						smRender->InitPCSS(false, 0, 0, 0, 0, 0);
				}

				basicShadowMapRender = smRender;
			}

			if (_data.contains("resolution"))
			{
				auto res = _data["resolution"].get<std::vector<int>>();
				basicShadowMapRender->SetResolution(res[0], res[1]);
			}
			else
				basicShadowMapRender->SetResolution(GLOBAL.WIN_WIDTH, GLOBAL.WIN_HEIGHT);

			if (_data.contains("use_tight_space"))
				GLOBAL.shadowMgr->SetUseTightSpace(_data["use_tight_space"].get<bool>());


			shadowRender = basicShadowMapRender;
		}
	}
}

void ShadowManager::InitShadowRender()
{
	if (shadowRender != nullptr)
		shadowRender->Init();
}

void ShadowManager::RenderShadow()
{
	if (shadowRender)
		shadowRender->Render();
}

bool ShadowManager::IsNeedShadowRender() { return shadowRender != nullptr; }

void ShadowManager::SaveShadowMap(const std::string& _lightName) const
{
	if (shadowRender == nullptr)
	{
		Print("Error: No enable shadow map technique");
		return;
	}
	auto lights = GLOBAL.sceneMgr->GetAllLight();
	int index = -1;
	for (int i = 0; i < lights.size(); i++)
	{
		if (lights[i]->GetName() == _lightName)
		{
			if (lights[i]->IsRenderShadow())
				index = i;
			else
				index = -2;
			break;
		}
	}
	if (index == -1)
		Print("Error: No light called " + _lightName);
	else if (index == -2)
		Print("Error: light " + _lightName + " is not enable to render shadow.");
	else
	{
		auto basicShadowMapRender = dynamic_pointer_cast<BasicShadowMapRender>(shadowRender);
		basicShadowMapRender->SaveShadowMap(index, _lightName);
	}
}

std::shared_ptr<BasicShadowRender> ShadowManager::GetShadowRender() { return shadowRender; }

bool ShadowManager::IsUseTightSpace() const { return useTightSpace; }
void ShadowManager::SetUseTightSpace(const bool& _value) { useTightSpace = _value; }


#pragma region Basic Shadow Map Class Definition
void BasicShadowMapRender::GetResolution(int& _width, int& _height) const { _width = resWidth; _height = resHeight; }
void BasicShadowMapRender::SetResolution(int _w, int _h) { resWidth = _w; resHeight = _h; }

void BasicShadowMapRender::Init() {/*do nothing*/ }
void BasicShadowMapRender::Render() {/*do nothing*/ }
void BasicShadowMapRender::Clear() { components.clear(); }
GLuint BasicShadowMapRender::GetDepthFrameBuffer(const int& _lightIndex) {/*do nothing*/ return 0; }
GLuint BasicShadowMapRender::GetDepthTexture(const int& _lightIndex) {/*do nothing*/ return 0; }
void BasicShadowMapRender::SaveShadowMap(const int& _lightIndex, const std::string& _lightName) {/*do nothing*/ }
void BasicShadowMapRender::InitComputeLightRatioParameters(shared_ptr<ShaderProgram>& _shaderPro, GLuint& _texUnit) {/*do nothing*/ }
int BasicShadowMapRender::AddComponent(std::shared_ptr<BasicShadowComponent>& _component) { components.push_back(_component); return components.size() - 1; }
std::shared_ptr<BasicShadowComponent>& BasicShadowMapRender::GetComponent(const int& _index) { return components[_index]; }
#pragma endregion

#pragma region Shadow Components
/*PCF*/
void PercentageCloserFilter::SetPCFKernelSize(const int& _size) { pcfKernelSize = _size; }
int PercentageCloserFilter::GetPCFKernelSize() const { return pcfKernelSize; }

/*PCSS*/
void PercentageCloserSoftFilter::SetParams(const int& _maxSearchSize, const int& _lightSize, const int& _minPenumbraSize, const int& _maxPenumbraSize, const float& _penumbraRatio)
{
	maxSearchSize = _maxSearchSize;
	lightSize = _lightSize;
	minPenumbraSize = _minPenumbraSize;
	maxPenumbraSize = _maxPenumbraSize;
	penumbraRatio = _penumbraRatio;
}
void PercentageCloserSoftFilter::GetParams(int& _maxSearchSize, int& _lightSize, int& _minPenumbraSize, int& _maxPenumbraSize, float& _penumbraRatio) const
{
	_maxSearchSize = maxSearchSize;
	_lightSize = lightSize;
	_minPenumbraSize = minPenumbraSize;
	_maxPenumbraSize = maxPenumbraSize;
	_penumbraRatio = penumbraRatio;
}
#pragma endregion


#pragma region ShadowMap Technique
void ShadowMapRender::Init()
{
	depthMap.clear();

	auto lights = GLOBAL.sceneMgr->GetAllLight();
	// TODO: to improve here, search paper how to solve large number light sources situation
	// create depth shadow map for each light which need to render shadow
	for (int i = 0; i < lights.size(); i++)
	{
		if (!lights[i]->IsRenderShadow())
			continue;
		
		/*----------------------------------------------------depth relevant start----------------------------------------------------*/
		glm::vec<2, GLuint> depthData;
		GLuint depthTex;
		// create a depth texture
		glCreateTextures(GL_TEXTURE_2D, 1, &depthTex); if (CheckGLError()) { Print("Error in ShadowMapRender::Init."); return; };
		// allocate storage for it
		// [Note] below using 32F for depth component is for float precision. (if i am correct, 32F can be used to store float value including negative and value greater than 1)
		// [Note]"GL_DEPTH_COMPONENT16" is enough, no need to use 32F. Compared results can be found "Output\ShadowMaps": "ShadowMap-Depth16" vs "ShadowMap-Depth32F".
		glTextureStorage2D(depthTex, 1, GL_DEPTH_COMPONENT16, resWidth, resHeight); if (CheckGLError()) { Print("Error in ShadowMapRender::Init."); return; };
		//glTextureStorage2D(depthTex, 1, GL_DEPTH_COMPONENT32F, resWidth, resHeight); if (CheckGLError()) { Print("Error in ShadowMapRender::Init."); return; };
		// Set the default filtering modes
		//glTextureParameteri(depthTex, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTextureParameteri(depthTex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(depthTex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(depthTex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		// Set up wrapping modes
		// not using GL_CLAMP_TOEDGE! If pixel gets outside of Texture, make its depth to 1.
		glTextureParameteri(depthTex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTextureParameteri(depthTex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		const float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTextureParameterfv(depthTex, GL_TEXTURE_BORDER_COLOR, borderColor); if (CheckGLError()) { Print("Error in ShadowMapRender::Init."); return; };
		// not using mipmap for depth tex because it is non-linear (after projection). But it can be used to store the true depth(z) before projection then mipmap is possible in VSM
		depthData[0] = depthTex;

		// Create FBO to render depth into
		GLuint depthFBO;
		glCreateFramebuffers(1, &depthFBO);
		// Attach the depth texture to it. (OpenGL 4.5 usage, refer: https://registry.khronos.org/OpenGL-Refpages/gl4/html/glFramebufferTexture.xhtml)
		glNamedFramebufferTexture(depthFBO, GL_DEPTH_ATTACHMENT, depthTex, 0);
		// Disable color rendering as there are no color attachments
		glNamedFramebufferDrawBuffer(depthFBO, GL_NONE);
		//glDrawBuffer(GL_NONE); // don't use it, because it will set current binding framebuffer status, but we just want to set the shadow map framebuffer not the normal framebuffer
		
		// finally check if framebuffer is complete
		if (glCheckNamedFramebufferStatus(depthFBO, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) { Print("ShadowMapRender:: Framebuffer not complete!"); return; }
		//depthFrameBuffers.push_back(depthFBO);
		depthData[1] = depthFBO;

		depthMap[i] = depthData;
		/*----------------------------------------------------depth relevant done----------------------------------------------------*/
	}
}

void ShadowMapRender::Render()
{
	/*---------------------------------------------- depth texture render start ----------------------------------------------*/
	// becareful, it seems the driver determines OpenGL clip. That's why the topic about clip the mesh in modeling domain still makes sense.
	// set the viewport to the size of the depth texture (each time render something into framebuffer, we need to specify its resolution)
	glViewport(0, 0, resWidth, resHeight);
	shared_ptr<ShaderProgram> shaderPro = GLOBAL.shaderMgr->TryActivateShaderProgram(GLOBAL.shaderPathPrefix + "ShadowMap/shadowMap");
	auto lights = GLOBAL.sceneMgr->GetAllLight();
	for (int i = 0; i < lights.size(); i++)
	{
		if (!lights[i]->IsRenderShadow())
			continue;

		glBindFramebuffer(GL_FRAMEBUFFER, depthMap[i][1]);
		glClearDepth(1.0f);
		glClear(GL_DEPTH_BUFFER_BIT);

		// compute light matrix
		LightCamInfo lightCamInfo;
		glm::mat4 lightMat = lights[i]->GetLightSpaceMat(lightCamInfo);
		shaderPro->Set("lightMat", lightMat);

		auto sceneObjs = GLOBAL.sceneMgr->GetAllSceneObject(); // not copy data, just return reference &
		for (auto iter = sceneObjs.begin(); iter != sceneObjs.end(); iter++)
		{
			auto sceneObj = *iter;
			glm::mat4 modelMat = sceneObj->GetTransform()->ComputeTransformationMatrix();
			shaderPro->Set("modelMat", modelMat);
			GLOBAL.render->Draw(sceneObj);
		}
	}
	// unbind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, GLOBAL.WIN_WIDTH, GLOBAL.WIN_HEIGHT); // set it back to normal

	/*---------------------------------------------- depth texture render done ----------------------------------------------*/
}

void ShadowMapRender::Clear()
{
	BasicShadowMapRender::Clear();

	for (auto iter = depthMap.begin(); iter != depthMap.end(); iter++)
	{
		auto texID = iter->second[0];
		if (glIsTexture(texID))
			glDeleteTextures(1, &texID);

		auto frameBufferID = iter->second[1];
		if (glIsFramebuffer(frameBufferID))
			glDeleteFramebuffers(1, &frameBufferID);
	}
	depthMap.clear();
}

GLuint ShadowMapRender::GetDepthFrameBuffer(const int& _lightIndex) { return depthMap[_lightIndex][1]; }
GLuint ShadowMapRender::GetDepthTexture(const int& _lightIndex) { return depthMap[_lightIndex][0]; }

void ShadowMapRender::SaveShadowMap(const int& _lightIndex, const std::string& _lightName)
{
	GLuint fbo = depthMap[_lightIndex][1];
	if (Utility::SaveTextureToPNG("ShadowMap_light_" + _lightName, resWidth, resHeight, GL_DEPTH_COMPONENT, fbo))
		Print("ShadowMap saved.");
}

void ShadowMapRender::SetBias(const float& _bias) { bias = _bias; }

void ShadowMapRender::InitComputeLightRatioParameters(shared_ptr<ShaderProgram>& _shaderPro, GLuint& _texUnit)
{
	_shaderPro->Set("bias", bias);

	// only focus on "ShadowMap/lightRatioPCF.sub_fs"
	if (pcfIndex != -1)
	{
		_shaderPro->Set("usePCF", 1);
		auto pcfCom = static_pointer_cast<PercentageCloserFilter>(GetComponent(pcfIndex));
		_shaderPro->Set("pcfHalfKernelSize", pcfCom->GetPCFKernelSize() / 2);
	}

	// only focus on "ShadowMap/lightRatioPCSS.sub_fs"
	if (pcssIndex != -1)
	{
		_shaderPro->Set("usePCSS", 1);

		auto pcssCom = static_pointer_cast<PercentageCloserSoftFilter>(GetComponent(pcssIndex));
		int maxSearchSize, lightSize, minPenumbraSize, maxPenumbraSize;
		float penumbraRatio;
		pcssCom->GetParams(maxSearchSize, lightSize, minPenumbraSize, maxPenumbraSize, penumbraRatio);
		_shaderPro->Set("maxSearchSize", maxSearchSize);
		_shaderPro->Set("lightSize", lightSize);
		_shaderPro->Set("minPenumbraSize", minPenumbraSize);
		_shaderPro->Set("maxPenumbraSize", maxPenumbraSize);
		_shaderPro->Set("penumbraRatio", penumbraRatio);
	}

	auto lights = GLOBAL.sceneMgr->GetAllLight();
	for (int i = 0; i < lights.size(); i++)
	{
		if (!lights[i]->IsRenderShadow())
			continue;

		LightCamInfo lightCamInfo;
		glm::mat4 lightMat = lights[i]->GetLightSpaceMat(lightCamInfo);
		_shaderPro->Set("lightCamInfos[" + std::to_string(i) + "].lightMat", lightMat); CheckGLError();
		_shaderPro->Set("lightCamInfos[" + std::to_string(i) + "].lightViewDir", lightCamInfo.lightViewDir); CheckGLError();

		if (pcssIndex != -1)
		{
			_shaderPro->Set("lightCamInfos[" + std::to_string(i) + "].near", lightCamInfo.near);
			_shaderPro->Set("lightCamInfos[" + std::to_string(i) + "].lightCamPos", lightCamInfo.lightCamPos);
		}

		// be careful with this texUnit.
		_shaderPro->Set("shadowMaps[" + std::to_string(i) + "]", static_cast<int>(_texUnit));
		glBindTextureUnit(_texUnit++, GetDepthTexture(i));
	}
}

void ShadowMapRender::InitPCF(const bool& _usePCF, const int& _pcfKernelSize)
{
	if (_usePCF)
	{
		auto pcfCom = std::make_shared<PercentageCloserFilter>();
		pcfCom->SetPCFKernelSize(_pcfKernelSize);
		pcfIndex = AddComponent(static_pointer_cast<BasicShadowComponent>(pcfCom));
	}
	else
		pcfIndex = -1;
}

void ShadowMapRender::InitPCSS(const bool& _usePCSS, const int& _maxSearchSize, const int& _lightSize, const int& _minPenumbraSize, const int& _maxPenumbraSize, const float& _penumbraRatio)
{
	if (_usePCSS)
	{
		auto pcssCom = std::make_shared<PercentageCloserSoftFilter>();
		pcssCom->SetParams(_maxSearchSize, _lightSize, _minPenumbraSize, _maxPenumbraSize, _penumbraRatio);
		pcssIndex = AddComponent(static_pointer_cast<BasicShadowComponent>(pcssCom));
	}
	else
		pcssIndex = -1;
}

#pragma endregion

#pragma region Variant Shadow Map Techniques
void VarianceShadowMapRender::Init()
{
	depthMap.clear();
	satGeneratorMap.clear();

	auto lights = GLOBAL.sceneMgr->GetAllLight();

	for (int i = 0; i < lights.size(); i++)
	{
		if (!lights[i]->IsRenderShadow())
			continue;

		/*----------------------------------------------------VSM-depth/depthSquare relevant start----------------------------------------------------*/
		glm::vec<3, GLuint> data;
		GLuint depthTex;
		// create a variant depth texture
		glCreateTextures(GL_TEXTURE_2D, 1, &depthTex); if (CheckGLError()) { Print("Error in VarianceShadowMapRender::Init."); return; };

		// allocate storage for it, one for depth, one for depth_square
		// [Important] Be careful, here by using SAT-VSM: the depth and depth_square is differernt from original VSM, 
		// depth is actually considered as mean of the whole pixel(texel), which means it is the E(x)(Moment 1)
		// [Note]"GL_RG16F" means to store floating-point value. I have tried GLRG, which will normalize the value into range [0.0, 1.0] and it will lose a lot of precision.
		// [Note]Comparing with "GL_RG32F", "GL_RG16F" is quite enough. --> check "Output/VSM/" "VSM-GL_RB16F.png" and "VSM-GL_RB32F.png"
		//glTextureStorage2D(depthTex, 1, GL_RG16F, resWidth, resHeight); if (CheckGLError()) { Print("Error in VarianceShadowMapRender::Init."); return; };
		glTextureStorage2D(depthTex, 1, GL_RG32F, resWidth, resHeight); if (CheckGLError()) { Print("Error in VarianceShadowMapRender::Init."); return; };

		// enable mip-map
		glGenerateTextureMipmap(depthTex);
		// can not use Anisotropic filtering in OpenGL 4.5. It is supported since 4.6- refer: https://www.khronos.org/opengl/wiki/Sampler_Object#Anisotropic_filtering
		//glTextureParameteri(depthTex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//glTextureParameteri(depthTex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		// using GL_LINEAR is better, compare the "VSM_nearest_texture.png" and "VSM_linear_texture.png" in the folder "\Output\VSM\"
		// linear method will significantly reduce the shadow acne.
		//glTextureParameteri(depthTex, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(depthTex, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); if (CheckGLError()) { Print("Error in VarianceShadowMapRender::Init."); return; };
		glTextureParameteri(depthTex, GL_TEXTURE_MAG_FILTER, GL_LINEAR); if (CheckGLError()) { Print("Error in VarianceShadowMapRender::Init."); return; };

		//[TODO]As SAT-VSM proposed, multiple-sampling-anti-aliasing(MSAA) can be used on the shadow maps. e.g. glTexImage2DMultisample(Check OpenGL Red book or online tutorials later)

		// Set up wrapping modes
		// not using GL_CLAMP_TOEDGE! If pixel gets outside of Texture, make its depth to 1.
		glTextureParameteri(depthTex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTextureParameteri(depthTex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		const float borderColor[] = { 1.0f, 1.0f, 0.0f, 0.0f };
		glTextureParameterfv(depthTex, GL_TEXTURE_BORDER_COLOR, borderColor); if (CheckGLError()) { Print("Error in VarianceShadowMapRender::Init."); return; };
		data[0] = depthTex;

		// Create FBO to render depth into
		GLuint depthFBO;
		glCreateFramebuffers(1, &depthFBO);
		// Attach color attachement (OpenGL 4.5 usage, refer: https://registry.khronos.org/OpenGL-Refpages/gl4/html/glFramebufferTexture.xhtml)
		glNamedFramebufferTexture(depthFBO, GL_COLOR_ATTACHMENT0, depthTex, 0);
		// enable color attachments
		glNamedFramebufferDrawBuffer(depthFBO, GL_COLOR_ATTACHMENT0);

		// [Important] We must create and bind a depthbuffer! Otherwise there is no way to update the depth information!!!
		GLuint depthRB; // this render buffer has the same size as ShadowMap does.
		glCreateRenderbuffers(1, &depthRB);
		glNamedRenderbufferStorage(depthRB, GL_DEPTH_COMPONENT, resWidth, resHeight); if (CheckGLError()) { Print("Error in VarianceShadowMapRender::Init."); return; };
		glNamedFramebufferRenderbuffer(depthFBO, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRB); if (CheckGLError()) { Print("Error in VarianceShadowMapRender::Init."); return; };
		data[2] = depthRB;

		// finally check if framebuffer is complete
		if (glCheckNamedFramebufferStatus(depthFBO, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) { Print("VarianceShadowMapRender:: Framebuffer not complete!"); return; }
		//depthFrameBuffers.push_back(depthFBO);
		data[1] = depthFBO;

		depthMap[i] = data;

		/*----------------------------------------------------VSM-depth/depthSquare relevant done----------------------------------------------------*/

		/*----------------------------------------------------SAT for them relevant start----------------------------------------------------*/
		// Generate SAT for this VSM
		if (useSAT)
		{
			SATConfig config;
			config.resWidth = resWidth;
			config.resHeight = resHeight;
			config.componentNum = 2; // only depth and depthSquare
			config.texGenerator = [](GLuint& _texID, const int& _width, const int& _height)
			{
				glCreateTextures(GL_TEXTURE_2D, 1, &_texID); if (CheckGLError()) { Print("Error in VarianceShadowMapRender::texGenerator."); return; };
				// [Important, Note] GL_RG16F is not enough to provide enough floating-point precision for SAT
				glTextureStorage2D(_texID, 1, GL_RG32F, _width, _height); if (CheckGLError()) { Print("Error in VarianceShadowMapRender::texGenerator."); return; };
				glTextureParameteri(_texID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); if (CheckGLError()) { Print("Error in VarianceShadowMapRender::texGenerator."); return; };
				glTextureParameteri(_texID, GL_TEXTURE_MAG_FILTER, GL_LINEAR); if (CheckGLError()) { Print("Error in VarianceShadowMapRender::texGenerator."); return; };
				glTextureParameteri(_texID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
				glTextureParameteri(_texID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
				const float borderColor[] = { 0,0,0,0 }; // [Important, Note] must be zero value. according to paper. When adding elements outside of range, it should add zero.
				glTextureParameterfv(_texID, GL_TEXTURE_BORDER_COLOR, borderColor); if (CheckGLError()) { Print("Error in VarianceShadowMapRender::texGenerator."); return; };
			};
			config.inputTexID = depthTex;

			std::shared_ptr<SummedAreaTableGenerator> satGenerator = std::make_shared<SummedAreaTableGenerator>();
			satGenerator->Init(config);
			satGeneratorMap[i] = satGenerator;
		}
		/*----------------------------------------------------SAT for them relevant done----------------------------------------------------*/
	}

	// set some parameter
	// refer: https://registry.khronos.org/OpenGL-Refpages/gl4/
	glHint(GL_FRAGMENT_SHADER_DERIVATIVE_HINT, GL_NICEST); // for dFdx, dFdy
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST); 
}

void VarianceShadowMapRender::Render()
{
	/*---------------------------------------------- VSM-depth/depthSquare texture render start ----------------------------------------------*/
	// TODO: maybe tight light view space is required to improve the precision issue.(for now I just enable it)
	// set the viewport to the size of the depth texture (each time render something into framebuffer, we need to specify its resolution)
	// [TODO] future work could be render VSM and its SAT for static scenes from one light source(sun light)-> to improve FPS
	shared_ptr<ShaderProgram> shaderPro = GLOBAL.shaderMgr->TryActivateShaderProgram(GLOBAL.shaderPathPrefix + "VarianceShadowMap/varianceShadowMap");
	auto lights = GLOBAL.sceneMgr->GetAllLight();
	for (int i = 0; i < lights.size(); i++)
	{
		// [TODO] if render it each frame, it will slow down FPS. For now, only the light info has changed(position change) then generate VSM and its SAT
		if (!lights[i]->IsRenderShadow())
			continue;

		//glBindFramebuffer(GL_FRAMEBUFFER, depthFrameBuffers[i]);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMap[i][1]); CheckGLError();
		glClearColor(1, 1, 0, 1); // first two component should be 1, because they are corresponding to depth and depth_square, the blue&alpha not used
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Erase the color and z buffers.
		glViewport(0, 0, resWidth, resHeight); CheckGLError();

		// compute light matrix
		LightCamInfo lightCamInfo;
		glm::mat4 lightMat = lights[i]->GetLightSpaceMat(lightCamInfo);
		shaderPro->Set("lightMat", lightMat); CheckGLError();

		shaderPro->Set("lightCamInfo.near", lightCamInfo.near); CheckGLError();
		shaderPro->Set("lightCamInfo.far", lightCamInfo.far); CheckGLError();
		shaderPro->Set("lightCamInfo.lightCamPos", lightCamInfo.lightCamPos); CheckGLError();
		shaderPro->Set("lightCamInfo.lightViewDir", lightCamInfo.lightViewDir); CheckGLError();

		auto sceneObjs = GLOBAL.sceneMgr->GetAllSceneObject(); // not copy data, just return reference &
		for (auto iter = sceneObjs.begin(); iter != sceneObjs.end(); iter++)
		{
			auto sceneObj = *iter;
			glm::mat4 modelMat = sceneObj->GetTransform()->ComputeTransformationMatrix();
			shaderPro->Set("modelMat", modelMat); CheckGLError();
			GLOBAL.render->Draw(sceneObj);
		}
		// unbind framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, GLOBAL.WIN_WIDTH, GLOBAL.WIN_HEIGHT); // set it back to normal
	}
	/*---------------------------------------------- VSM-depth/depthSquare texture render done ----------------------------------------------*/

	/*----------------------------------------------------SAT render start----------------------------------------------------*/
	//[Important] the reason why I do another iteration is that: BoxFilter() will generate a new sceneobj and add into sceneMgr. It will change sceneObjs??
	// also, waiting all VSM get rendered is normal logic.
	if (useSAT)
	{
		for (int i = 0; i < lights.size(); i++)
		{
			// [TODO] if render it each frame, it will slow down FPS. For now, only the light info has changed(position change) then generate VSM and its SAT
			if (!lights[i]->IsRenderShadow())
				continue;
			// create SAT for the depth/depthSquare texture
			satGeneratorMap[i]->Generate();
		}
	}
	/*----------------------------------------------------SAT render done----------------------------------------------------*/
}

void VarianceShadowMapRender::Clear()
{
	BasicShadowMapRender::Clear();

	for (auto iter = depthMap.begin(); iter != depthMap.end(); iter++)
	{
		auto texID = iter->second[0];
		if (glIsTexture(texID))
			glDeleteTextures(1, &texID);

		auto frameBufferID = iter->second[1];
		if (glIsFramebuffer(frameBufferID))
			glDeleteFramebuffers(1, &frameBufferID);

		// be careful, don't forget this one.
		auto rbID = iter->second[2];
		if (glIsRenderbuffer(rbID))
			glDeleteRenderbuffers(1, &rbID);
	}
	depthMap.clear();

	satGeneratorMap.clear(); /*it will automatically release OpenGL objects. Check ~SATGenerator().*/
}

GLuint VarianceShadowMapRender::GetDepthFrameBuffer(const int& _lightIndex) { return depthMap[_lightIndex][1]; }
GLuint VarianceShadowMapRender::GetDepthTexture(const int& _lightIndex) { return depthMap[_lightIndex][0]; }

void VarianceShadowMapRender::SaveShadowMap(const int& _lightIndex, const std::string& _lightName)
{
	GLuint fbo = depthMap[_lightIndex][1];
	if (Utility::SaveTextureToPNG("ShadowMap_light_" + _lightName, resWidth, resHeight, GL_RGB, fbo))
		Print("VarianceShadowMap saved.");

	// below code will read depth texture from framebuffer not color buffer
	if (Utility::SaveTextureToPNG("ShadowMap_light_" + _lightName + "_depth_ground_truth", resWidth, resHeight, GL_DEPTH_COMPONENT, fbo))
		Print("ShadowMap saved.");
}

void VarianceShadowMapRender::SetKernelSize(const int& _kernelSize) { kernelSize = _kernelSize; }
void VarianceShadowMapRender::SetVarianceMin(const float& _varMin) { varMin = _varMin; }
void VarianceShadowMapRender::SetPMin(const float& _pMin) { pMin = _pMin; }

void VarianceShadowMapRender::SetUseSAT(const bool& _value) { useSAT = _value; }
bool VarianceShadowMapRender::IsUseSAT() const { return useSAT; }
GLuint VarianceShadowMapRender::GetSAT(const int& _lightIndex) { return satGeneratorMap[_lightIndex]->GetSAT(); }

std::shared_ptr<SummedAreaTableGenerator> VarianceShadowMapRender::GetSATGenerator(const int& _lightIndex) { return satGeneratorMap[_lightIndex]; }

void VarianceShadowMapRender::InitComputeLightRatioParameters(shared_ptr<ShaderProgram>& _shaderPro, GLuint& _texUnit)
{
	// set VSM relevant parameters
	_shaderPro->Set("halfKernelSize", kernelSize / 2);
	_shaderPro->Set("varMin", varMin);
	_shaderPro->Set("pMin", pMin);

	// set SAT
	if (useSAT)
		_shaderPro->Set("useSAT", 1);
	else
		_shaderPro->Set("useSAT", 0);

	// only focus on "VarianceShadowMap/lightRatioPCSS.sub_fs"
	if (pcssIndex != -1)
	{
		_shaderPro->Set("usePCSS", 1);

		auto pcssCom = static_pointer_cast<PercentageCloserSoftFilter>(GetComponent(pcssIndex));
		int maxSearchSize, lightSize, minPenumbraSize, maxPenumbraSize;
		float penumbraRatio;
		pcssCom->GetParams(maxSearchSize, lightSize, minPenumbraSize, maxPenumbraSize, penumbraRatio);
		_shaderPro->Set("maxSearchSize", maxSearchSize);
		_shaderPro->Set("lightSize", lightSize);
		_shaderPro->Set("minPenumbraSize", minPenumbraSize);
		_shaderPro->Set("maxPenumbraSize", maxPenumbraSize);
		_shaderPro->Set("penumbraRatio", penumbraRatio);
	}

	auto lights = GLOBAL.sceneMgr->GetAllLight();
	for (int i = 0; i < lights.size(); i++)
	{
		if (!lights[i]->IsRenderShadow())
			continue;

		// compute light matrix
		LightCamInfo lightCamInfo;
		glm::mat4 lightMat = lights[i]->GetLightSpaceMat(lightCamInfo);
		_shaderPro->Set("lightCamInfos[" + std::to_string(i) + "].lightMat", lightMat);
		_shaderPro->Set("lightCamInfos[" + std::to_string(i) + "].near", lightCamInfo.near);
		_shaderPro->Set("lightCamInfos[" + std::to_string(i) + "].far", lightCamInfo.far);
		_shaderPro->Set("lightCamInfos[" + std::to_string(i) + "].lightCamPos", lightCamInfo.lightCamPos);
		_shaderPro->Set("lightCamInfos[" + std::to_string(i) + "].lightViewDir", lightCamInfo.lightViewDir);

		// set shadow map
		_shaderPro->Set("shadowMaps[" + std::to_string(i) + "]", static_cast<int>(_texUnit));
		glBindTextureUnit(_texUnit++, GetDepthTexture(i));

		// set SAT maps
		if (useSAT)
		{
			_shaderPro->Set("SATMaps[" + std::to_string(i) + "]", static_cast<int>(_texUnit));
			glBindTextureUnit(_texUnit++, GetSAT(i));
		}
	}
}

void VarianceShadowMapRender::InitPCSS(const bool& _usePCSS, const int& _maxSearchSize, const int& _lightSize, const int& _minPenumbraSize, const int& _maxPenumbraSize, const float& _penumbraRatio)
{
	if (_usePCSS)
	{
		auto pcssCom = std::make_shared<PercentageCloserSoftFilter>();
		pcssCom->SetParams(_maxSearchSize, _lightSize, _minPenumbraSize, _maxPenumbraSize, _penumbraRatio);
		pcssIndex = AddComponent(static_pointer_cast<BasicShadowComponent>(pcssCom));
	}
	else
		pcssIndex = -1;
}
#pragma endregion


#pragma region VSSM
void VSSMRender::Init()
{
	depthMap.clear();
	satGeneratorMap.clear();

	auto lights = GLOBAL.sceneMgr->GetAllLight();

	for (int i = 0; i < lights.size(); i++)
	{
		if (!lights[i]->IsRenderShadow())
			continue;

		/*----------------------------------------------------VSM-depth/depthSquare relevant start----------------------------------------------------*/
		glm::vec<3, GLuint> data;
		GLuint depthTex;
		// create a variant depth texture
		glCreateTextures(GL_TEXTURE_2D, 1, &depthTex); if (CheckGLError()) { Print("Error in VSSMRender::Init."); return; };
		glTextureStorage2D(depthTex, 1, GL_RG32F, resWidth, resHeight); if (CheckGLError()) { Print("Error in VSSMRender::Init."); return; };

		// enable mip-map
		glGenerateTextureMipmap(depthTex);
		glTextureParameteri(depthTex, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); if (CheckGLError()) { Print("Error in VSSMRender::Init."); return; };
		glTextureParameteri(depthTex, GL_TEXTURE_MAG_FILTER, GL_LINEAR); if (CheckGLError()) { Print("Error in VSSMRender::Init."); return; };

		glTextureParameteri(depthTex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTextureParameteri(depthTex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		const float borderColor[] = { 1.0f, 1.0f, 0.0f, 0.0f };
		glTextureParameterfv(depthTex, GL_TEXTURE_BORDER_COLOR, borderColor); if (CheckGLError()) { Print("Error in VSSMRender::Init."); return; };
		data[0] = depthTex;

		// Create FBO to render depth into
		GLuint depthFBO;
		glCreateFramebuffers(1, &depthFBO);
		// Attach color attachement (OpenGL 4.5 usage, refer: https://registry.khronos.org/OpenGL-Refpages/gl4/html/glFramebufferTexture.xhtml)
		glNamedFramebufferTexture(depthFBO, GL_COLOR_ATTACHMENT0, depthTex, 0);
		// enable color attachments
		glNamedFramebufferDrawBuffer(depthFBO, GL_COLOR_ATTACHMENT0);

		// [Important] We must create and bind a depthbuffer! Otherwise there is no way to update the depth information!!!
		GLuint depthRB; // this render buffer has the same size as ShadowMap does.
		glCreateRenderbuffers(1, &depthRB);
		glNamedRenderbufferStorage(depthRB, GL_DEPTH_COMPONENT, resWidth, resHeight); if (CheckGLError()) { Print("Error in VSSMRender::Init."); return; };
		glNamedFramebufferRenderbuffer(depthFBO, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRB); if (CheckGLError()) { Print("Error in VSSMRender::Init."); return; };
		data[2] = depthRB;

		// finally check if framebuffer is complete
		if (glCheckNamedFramebufferStatus(depthFBO, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) { Print("VSSMRender:: Framebuffer not complete!"); return; }
		//depthFrameBuffers.push_back(depthFBO);
		data[1] = depthFBO;

		depthMap[i] = data;

		/*----------------------------------------------------VSM-depth/depthSquare relevant done----------------------------------------------------*/

		/*----------------------------------------------------SAT for them relevant start----------------------------------------------------*/
		// Generate SAT for this VSM
		SATConfig config;
		config.resWidth = resWidth;
		config.resHeight = resHeight;
		config.componentNum = 2; // only depth and depthSquare
		config.texGenerator = [](GLuint& _texID, const int& _width, const int& _height)
		{
			glCreateTextures(GL_TEXTURE_2D, 1, &_texID); if (CheckGLError()) { Print("Error in VSSMRender::texGenerator."); return; };
			// [Important, Note] GL_RG16F is not enough to provide enough floating-point precision for SAT
			glTextureStorage2D(_texID, 1, GL_RG32F, _width, _height); if (CheckGLError()) { Print("Error in VSSMRender::texGenerator."); return; };
			glTextureParameteri(_texID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); if (CheckGLError()) { Print("Error in VSSMRender::texGenerator."); return; };
			glTextureParameteri(_texID, GL_TEXTURE_MAG_FILTER, GL_LINEAR); if (CheckGLError()) { Print("Error in VSSMRender::texGenerator."); return; };
			glTextureParameteri(_texID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTextureParameteri(_texID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			const float borderColor[] = { 0,0,0,0 }; // [Important, Note] must be zero value. according to paper. When adding elements outside of range, it should add zero.
			glTextureParameterfv(_texID, GL_TEXTURE_BORDER_COLOR, borderColor); if (CheckGLError()) { Print("Error in VSSMRender::texGenerator."); return; };
		};
		config.inputTexID = depthTex;

		std::shared_ptr<SummedAreaTableGenerator> satGenerator = std::make_shared<SummedAreaTableGenerator>();
		satGenerator->Init(config);
		satGeneratorMap[i] = satGenerator;
		/*----------------------------------------------------SAT for them relevant done----------------------------------------------------*/
	}

	// set some parameter
	// refer: https://registry.khronos.org/OpenGL-Refpages/gl4/
	glHint(GL_FRAGMENT_SHADER_DERIVATIVE_HINT, GL_NICEST); // for dFdx, dFdy
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
}

void VSSMRender::Render()
{
	/*---------------------------------------------- VSM-depth/depthSquare texture render start ----------------------------------------------*/
	// TODO: maybe tight light view space is required to improve the precision issue.(for now I just enable it)
	// set the viewport to the size of the depth texture (each time render something into framebuffer, we need to specify its resolution)
	// [TODO] future work could be render VSM and its SAT for static scenes from one light source(sun light)-> to improve FPS
	shared_ptr<ShaderProgram> shaderPro = GLOBAL.shaderMgr->TryActivateShaderProgram(GLOBAL.shaderPathPrefix + "VarianceShadowMap/varianceShadowMap");
	auto lights = GLOBAL.sceneMgr->GetAllLight();
	for (int i = 0; i < lights.size(); i++)
	{
		// [TODO] if render it each frame, it will slow down FPS. For now, only the light info has changed(position change) then generate VSM and its SAT
		if (!lights[i]->IsRenderShadow())
			continue;

		//glBindFramebuffer(GL_FRAMEBUFFER, depthFrameBuffers[i]);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMap[i][1]); CheckGLError();
		glClearColor(1, 1, 0, 1); // first two component should be 1, because they are corresponding to depth and depth_square, the blue&alpha not used
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Erase the color and z buffers.
		glViewport(0, 0, resWidth, resHeight); CheckGLError();

		// compute light matrix
		LightCamInfo lightCamInfo;
		glm::mat4 lightMat = lights[i]->GetLightSpaceMat(lightCamInfo);
		shaderPro->Set("lightMat", lightMat); CheckGLError();

		shaderPro->Set("lightCamInfo.near", lightCamInfo.near); CheckGLError();
		shaderPro->Set("lightCamInfo.far", lightCamInfo.far); CheckGLError();
		shaderPro->Set("lightCamInfo.lightCamPos", lightCamInfo.lightCamPos); CheckGLError();
		shaderPro->Set("lightCamInfo.lightViewDir", lightCamInfo.lightViewDir); CheckGLError();

		auto sceneObjs = GLOBAL.sceneMgr->GetAllSceneObject(); // not copy data, just return reference &
		for (auto iter = sceneObjs.begin(); iter != sceneObjs.end(); iter++)
		{
			auto sceneObj = *iter;
			glm::mat4 modelMat = sceneObj->GetTransform()->ComputeTransformationMatrix();
			shaderPro->Set("modelMat", modelMat); CheckGLError();
			GLOBAL.render->Draw(sceneObj);
		}
		// unbind framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, GLOBAL.WIN_WIDTH, GLOBAL.WIN_HEIGHT); // set it back to normal
	}
	/*---------------------------------------------- VSM-depth/depthSquare texture render done ----------------------------------------------*/

	/*----------------------------------------------------SAT render start----------------------------------------------------*/
	//[Important] the reason why I do another iteration is that: BoxFilter() will generate a new sceneobj and add into sceneMgr. It will change sceneObjs??
	// also, waiting all VSM get rendered is normal logic.
	for (int i = 0; i < lights.size(); i++)
	{
		// [TODO] if render it each frame, it will slow down FPS. For now, only the light info has changed(position change) then generate VSM and its SAT
		if (!lights[i]->IsRenderShadow())
			continue;
		// create SAT for the depth/depthSquare texture
		satGeneratorMap[i]->Generate();
	}
	/*----------------------------------------------------SAT render done----------------------------------------------------*/
}

void VSSMRender::Clear()
{
	BasicShadowMapRender::Clear();

	for (auto iter = depthMap.begin(); iter != depthMap.end(); iter++)
	{
		auto texID = iter->second[0];
		if (glIsTexture(texID))
			glDeleteTextures(1, &texID);

		auto frameBufferID = iter->second[1];
		if (glIsFramebuffer(frameBufferID))
			glDeleteFramebuffers(1, &frameBufferID);

		// be careful, don't forget this one.
		auto rbID = iter->second[2];
		if (glIsRenderbuffer(rbID))
			glDeleteRenderbuffers(1, &rbID);
	}
	depthMap.clear();

	satGeneratorMap.clear(); /*it will automatically release OpenGL objects. Check ~SATGenerator().*/
}

GLuint VSSMRender::GetDepthFrameBuffer(const int& _lightIndex) { return depthMap[_lightIndex][1]; }
GLuint VSSMRender::GetDepthTexture(const int& _lightIndex) { return depthMap[_lightIndex][0]; }

void VSSMRender::SaveShadowMap(const int& _lightIndex, const std::string& _lightName)
{
	GLuint fbo = depthMap[_lightIndex][1];
	if (Utility::SaveTextureToPNG("ShadowMap_light_" + _lightName, resWidth, resHeight, GL_RGB, fbo))
		Print("VarianceShadowMap saved.");

	// below code will read depth texture from framebuffer not color buffer
	if (Utility::SaveTextureToPNG("ShadowMap_light_" + _lightName + "_depth_ground_truth", resWidth, resHeight, GL_DEPTH_COMPONENT, fbo))
		Print("ShadowMap saved.");
}

void VSSMRender::SetVarianceMin(const float& _varMin) { varMin = _varMin; }
void VSSMRender::SetPMin(const float& _pMin) { pMin = _pMin; }

GLuint VSSMRender::GetSAT(const int& _lightIndex) { return satGeneratorMap[_lightIndex]->GetSAT(); }

void VSSMRender::InitComputeLightRatioParameters(shared_ptr<ShaderProgram>& _shaderPro, GLuint& _texUnit)
{
	// set VSM relevant parameters
	_shaderPro->Set("varMin", varMin);
	_shaderPro->Set("pMin", pMin);

	_shaderPro->Set("M", M);
	_shaderPro->Set("N", N);

	auto pcssCom = static_pointer_cast<PercentageCloserSoftFilter>(GetComponent(pcssIndex));
	int maxSearchSize, lightSize, minPenumbraSize, maxPenumbraSize;
	float penumbraRatio;
	pcssCom->GetParams(maxSearchSize, lightSize, minPenumbraSize, maxPenumbraSize, penumbraRatio);
	_shaderPro->Set("lightSize", lightSize);
	_shaderPro->Set("minPenumbraSize", minPenumbraSize);
	_shaderPro->Set("maxPenumbraSize", maxPenumbraSize);

	auto lights = GLOBAL.sceneMgr->GetAllLight();
	for (int i = 0; i < lights.size(); i++)
	{
		if (!lights[i]->IsRenderShadow())
			continue;

		// compute light matrix
		LightCamInfo lightCamInfo;
		glm::mat4 lightMat = lights[i]->GetLightSpaceMat(lightCamInfo);
		_shaderPro->Set("lightCamInfos[" + std::to_string(i) + "].lightMat", lightMat);
		_shaderPro->Set("lightCamInfos[" + std::to_string(i) + "].near", lightCamInfo.near);
		_shaderPro->Set("lightCamInfos[" + std::to_string(i) + "].far", lightCamInfo.far);
		_shaderPro->Set("lightCamInfos[" + std::to_string(i) + "].lightCamPos", lightCamInfo.lightCamPos);
		_shaderPro->Set("lightCamInfos[" + std::to_string(i) + "].lightViewDir", lightCamInfo.lightViewDir);

		// set shadow map
		_shaderPro->Set("shadowMaps[" + std::to_string(i) + "]", static_cast<int>(_texUnit));
		glBindTextureUnit(_texUnit++, GetDepthTexture(i));

		// set SAT maps
		_shaderPro->Set("SATMaps[" + std::to_string(i) + "]", static_cast<int>(_texUnit));
		glBindTextureUnit(_texUnit++, GetSAT(i));
	}
}

void VSSMRender::InitPCSS(const bool& _usePCSS, const int& _maxSearchSize, const int& _lightSize, const int& _minPenumbraSize, const int& _maxPenumbraSize, const float& _penumbraRatio)
{
	if (_usePCSS)
	{
		auto pcssCom = std::make_shared<PercentageCloserSoftFilter>();
		pcssCom->SetParams(_maxSearchSize, _lightSize, _minPenumbraSize, _maxPenumbraSize, _penumbraRatio);
		pcssIndex = AddComponent(static_pointer_cast<BasicShadowComponent>(pcssCom));
	}
	else
		pcssIndex = -1;
}

void VSSMRender::InitSubdivision(const int& _M, const int& _N) { M = _M; N = _N; }

#pragma endregion
