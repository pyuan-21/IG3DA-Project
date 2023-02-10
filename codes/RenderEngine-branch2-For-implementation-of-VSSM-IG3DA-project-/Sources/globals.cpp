#include "globals.hpp"

using namespace IceRender;

Globals::Globals() : defaultRenderMethod("RenderSimple"), shaderPathPrefix("Resources/Shaders/"), defaultShaderName("Simple/simple"), imagePathPrefix("Resources/Images/")
{
	WIN_WIDTH = 800;
	WIN_HEIGHT = 600;

	// using https://www.desmos.com/calculator and formual 1/(1+l*x+q*x*x) which is "\frac{1}{1+l\cdot x+q\cdot x^{2}}" in this website
	// and setting all these parameters by hand
	attenuationMap[5200] = glm::vec2(1e-6, 1e-5);
	attenuationMap[3200] = glm::vec2(2e-6, 1e-5);
	attenuationMap[2000] = glm::vec2(5e-6, 1e-5);
	attenuationMap[1200] = glm::vec2(1e-5, 1e-5);
	attenuationMap[600] = glm::vec2(5e-5, 1e-5);
	attenuationMap[300] = glm::vec2(1e-3, 1e-5);

	// initialize all global variables
	logger = make_shared<Logger>();
	console = make_shared<Console>();
	render = make_shared<Rasterizer>();
	shaderMgr = make_shared<ShaderManager>();
	sceneMgr = make_shared<SceneManager>();
	testUnit = make_shared<TestUnit>();
	keyInput = make_shared<KeyInput>();
	camCtrller = make_shared<CameraController>();
	timeMgr = make_shared<TimeManager>();
	shadowMgr = make_shared<ShadowManager>();
}

Globals::~Globals()
{
	// TODO: keep update here if adding anything global here, don't delete this todo, as for reminders
	logger = nullptr;
	console = nullptr;
	render = nullptr;
	shaderMgr = nullptr;
	sceneMgr = nullptr;
	testUnit = nullptr;
	keyInput = nullptr;
	camCtrller = nullptr;
	timeMgr = nullptr;
	shadowMgr = nullptr;
}