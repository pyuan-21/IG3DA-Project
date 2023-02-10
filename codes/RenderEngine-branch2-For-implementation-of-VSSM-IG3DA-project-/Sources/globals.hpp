#pragma once

#include <memory>
#include <map>
#include "singleton/singleton.hpp"
#include "console.hpp"
#include "rasterizer/rasterizer.hpp"
#include "helpers//logger.hpp"
#include "scene/sceneManager.hpp"
#include "test_unit/testunit.hpp"
#include "input/keyInput.hpp"
#include "camera/cameraController.hpp"
#include "time/timeManager.hpp"
#include "shadow/shadow.hpp"

namespace IceRender
{
	// all variables written here will be stored during program life cycle. Make sure these variables are necessary to be stored for this whole life cycle.
	class Globals
	{
		// in order to allow 'Singleton' class to access 'Globals' class
		friend class Singleton<Globals>;

	private:
		Globals();
		~Globals();

	public:
		// TODO: move all gloabl variable setting into one configure.file, then read it and use it.
		// TODO: all configurable variables below!!!(to .json file for instance)
		// screen
		int WIN_WIDTH;
		int WIN_HEIGHT;

#pragma region camera parameters
		// camera parameters by default
		const float camFOV = 60;
		// the distance btw near and far should be narrow to avoid losing precision of Z-depth when vertex is close to far plane
		// it is not easy to define the z-fighting interval(wherer projected z-depth changes very slowly when moving a long enough distance along the direction from near to far
		// unity default near=0.3f, far=1000f
		const float camNear = 0.3f;
		const float camFar = 1000.0f;

		const float camMoveSpeed = 1e-2f;
		const float camRotSpeed = 1e-2f;
		const float camZoomSpeed = 1e-1f;
#pragma endregion
		const std::string defaultRenderMethod; // specify which render method is used

		const std::string shaderPathPrefix;
		const std::string defaultShaderName; // specify which shader is used

		const std::string imagePathPrefix;

		std::map<int, glm::vec2> attenuationMap;

		std::shared_ptr<Logger> logger;
		std::shared_ptr<Console> console;
		std::shared_ptr<Rasterizer> render;
		std::shared_ptr<ShaderManager> shaderMgr;
		std::shared_ptr<SceneManager> sceneMgr;
		std::shared_ptr<TestUnit> testUnit;
		std::shared_ptr<KeyInput> keyInput;
		std::shared_ptr<CameraController> camCtrller;
		std::shared_ptr<TimeManager> timeMgr;
		std::shared_ptr<ShadowManager> shadowMgr;
	};

}

#ifndef ICE_GLOBAL
#define ICE_GLOBAL
#define GLOBAL Singleton<Globals>::GetInstance()
#endif // !ICE_GLOBAL

