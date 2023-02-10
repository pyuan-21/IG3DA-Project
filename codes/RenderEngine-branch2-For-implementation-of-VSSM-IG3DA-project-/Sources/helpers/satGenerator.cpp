#include "satGenerator.hpp"
#include "../helpers/utility.hpp"
#include "../mesh/meshGenerator.hpp"

using namespace IceRender;

SATConfig::SATConfig()
{
	resWidth = resHeight = componentNum = 0;
	inputTexID = 0;
	texGenerator = nullptr;
}

SATConfig::SATConfig(const SATConfig& _config)
{
	resWidth = _config.resWidth;
	resHeight = _config.resHeight;
	inputTexID = _config.inputTexID;
	componentNum = _config.componentNum;
	texGenerator = _config.texGenerator;
}

SummedAreaTableGenerator::~SummedAreaTableGenerator() { Clear(); };

void SummedAreaTableGenerator::Init(const SATConfig& _config)
{
	config = _config;

	// init N, M
	N = std::ceil(std::log2(config.resWidth));
	M = std::ceil(std::log2(config.resHeight));

	// create texA, texB
	config.texGenerator(texA, config.resWidth, config.resHeight);
	config.texGenerator(texB, config.resWidth, config.resHeight);
	
	// create framebuffer, texA is passed into uniform variable, then output to framebuffer(texB is bound to this framebuffer to received the output)
	glCreateFramebuffers(1, &fbo);
	glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0, texB, 0);
	glNamedFramebufferDrawBuffer(fbo, GL_COLOR_ATTACHMENT0);
	CheckGLError();

	if (glCheckNamedFramebufferStatus(fbo, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) { Print("SummedAreaTableGenerator:: Framebuffer not complete!"); return; }

	// init shader name.
	horShaderName = GLOBAL.shaderPathPrefix + "SAT/sat" + std::to_string(config.componentNum) + "H";
	verShaderName = GLOBAL.shaderPathPrefix + "SAT/sat" + std::to_string(config.componentNum) + "V";
}

void SummedAreaTableGenerator::Generate()
{
	// this function will be called every frame.(if it is used to genereate VSM's SAT)
	
	glViewport(0, 0, config.resWidth, config.resHeight); CheckGLError();

	// below function requires that two textures internal formats are compatible.
	//glCopyImageSubData(config.inputTexID, GL_TEXTURE_2D, 0, 0, 0, 0, texA, GL_TEXTURE_2D, 0, 0, 0, 0, config.resWidth, config.resHeight, 1); CheckGLError();

	glBindFramebuffer(GL_FRAMEBUFFER, fbo); CheckGLError();

	shared_ptr<Mesh> mesh = MeshGenerator::GenPlane(2, 2, Utility::zeroV3, Utility::backV3);
	shared_ptr<SceneObject> satObj = make_shared<SceneObject>("___SATObj___", mesh);
	GLOBAL.sceneMgr->AddSceneObj(satObj);
	GLuint texUnit = 0;

	shared_ptr<ShaderProgram> shaderPro;

	// copy inputTex to texA
	// copy-satrt
	std::string copyShaderName = GLOBAL.shaderPathPrefix + "SAT/sat" + std::to_string(config.componentNum) + "Copy";
	shaderPro = GLOBAL.shaderMgr->TryActivateShaderProgram(copyShaderName);
	shaderPro->Set("texInput", static_cast<int>(texUnit));
	glBindTextureUnit(texUnit, config.inputTexID);
	glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0, texA, 0);
	if (glCheckNamedFramebufferStatus(fbo, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) { Print("SummedAreaTableGenerator::CopyTexture, Framebuffer not complete!"); return; }
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Erase the color and z buffers.
	GLOBAL.render->Draw(satObj);
	//copy-end

	// render horizontal
	shaderPro = GLOBAL.shaderMgr->TryActivateShaderProgram(horShaderName);
	for (int i = 0; i < N; i++)
	{
		shaderPro->Set("i", i);
		shaderPro->Set("texInput", static_cast<int>(texUnit));
		glBindTextureUnit(texUnit, texA);

		glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0, texB, 0);
		if (glCheckNamedFramebufferStatus(fbo, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) { Print("SummedAreaTableGenerator:: Framebuffer not complete!"); return; }

		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Erase the color and z buffers.
		GLOBAL.render->Draw(satObj);
		// swap tA, tB
		GLuint temp = texA;
		texA = texB;
		texB = temp;
	}
	
	// render vertical
	shaderPro = GLOBAL.shaderMgr->TryActivateShaderProgram(verShaderName);
	for (int i = 0; i < M; i++)
	{
		shaderPro->Set("i", i);
		shaderPro->Set("texInput", static_cast<int>(texUnit));
		glBindTextureUnit(texUnit, texA);

		glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0, texB, 0);
		if (glCheckNamedFramebufferStatus(fbo, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) { Print("SummedAreaTableGenerator:: Framebuffer not complete!"); return; }

		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Erase the color and z buffers.
		GLOBAL.render->Draw(satObj);
		// swap tA, tB
		GLuint temp = texA;
		texA = texB;
		texB = temp;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// remove it
	GLOBAL.sceneMgr->RemoveSceneObj(satObj->GetName());
	glViewport(0, 0, GLOBAL.WIN_WIDTH, GLOBAL.WIN_HEIGHT); // set it back to normal
}

void SummedAreaTableGenerator::Clear()
{
	// don't forget to release OpenGL textures and etc.
	if (glIsTexture(texA))
		glDeleteTextures(1, &texA);
	texA = 0;

	if (glIsTexture(texB))
		glDeleteTextures(1, &texB);
	texB = 0;

	if (glIsFramebuffer(fbo))
		glDeleteFramebuffers(1, &fbo);
	fbo = 0;
}

GLuint SummedAreaTableGenerator::GetSAT() const { return texA; }

void SummedAreaTableGenerator::Reconstruct(GLuint& _outputTex)
{
	glViewport(0, 0, config.resWidth, config.resHeight); CheckGLError();

	glBindFramebuffer(GL_FRAMEBUFFER, fbo); CheckGLError();

	shared_ptr<Mesh> mesh = MeshGenerator::GenPlane(2, 2, Utility::zeroV3, Utility::backV3);
	shared_ptr<SceneObject> satObj = make_shared<SceneObject>("___SATObj___", mesh);
	GLOBAL.sceneMgr->AddSceneObj(satObj);
	GLuint texUnit = 0;
	// reconstruction shader name
	std::string reconShaderName = GLOBAL.shaderPathPrefix + "SAT/sat" + std::to_string(config.componentNum) + "Reconstruct";
	shared_ptr<ShaderProgram> shaderPro = GLOBAL.shaderMgr->TryActivateShaderProgram(reconShaderName);

	shaderPro->Set("texInput", static_cast<int>(texUnit));
	glBindTextureUnit(texUnit, texA);

	glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0, _outputTex, 0);
	if (glCheckNamedFramebufferStatus(fbo, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) { Print("SummedAreaTableGenerator::Reconstruct, Framebuffer not complete!"); return; }

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Erase the color and z buffers.
	GLOBAL.render->Draw(satObj);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// remove it
	GLOBAL.sceneMgr->RemoveSceneObj(satObj->GetName());
	glViewport(0, 0, GLOBAL.WIN_WIDTH, GLOBAL.WIN_HEIGHT); // set it back to normal
}

void SummedAreaTableGenerator::BoxFilter(GLuint& _outputTex, const int& _kernelSize)
{
	// outputTex should have the same resolution(texture size) as SAT
	// also kernelSize should be odd number, such as 1, 3, 5, 7...
	glViewport(0, 0, config.resWidth, config.resHeight); CheckGLError();

	glBindFramebuffer(GL_FRAMEBUFFER, fbo); CheckGLError();

	shared_ptr<Mesh> mesh = MeshGenerator::GenPlane(2, 2, Utility::zeroV3, Utility::backV3);
	shared_ptr<SceneObject> satObj = make_shared<SceneObject>("___SATObj___", mesh);
	GLOBAL.sceneMgr->AddSceneObj(satObj);
	GLuint texUnit = 0;
	// boxfilter shader name
	std::string boxFilterShaderName = GLOBAL.shaderPathPrefix + "SAT/sat" + std::to_string(config.componentNum) + "BoxFilter";
	shared_ptr<ShaderProgram> shaderPro = GLOBAL.shaderMgr->TryActivateShaderProgram(boxFilterShaderName);

	shaderPro->Set("halfKernelSize", _kernelSize / 2); CheckGLError();

	shaderPro->Set("texInput", static_cast<int>(texUnit)); CheckGLError();
	glBindTextureUnit(texUnit, texA); CheckGLError();

	glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0, _outputTex, 0); CheckGLError();
	if (glCheckNamedFramebufferStatus(fbo, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) { Print("SummedAreaTableGenerator::BoxFilter, Framebuffer not complete!"); return; }

	glClearColor(1, 1, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Erase the color and z buffers.
	GLOBAL.render->Draw(satObj);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// remove it
	GLOBAL.sceneMgr->RemoveSceneObj(satObj->GetName());
	glViewport(0, 0, GLOBAL.WIN_WIDTH, GLOBAL.WIN_HEIGHT); // set it back to normal
}
