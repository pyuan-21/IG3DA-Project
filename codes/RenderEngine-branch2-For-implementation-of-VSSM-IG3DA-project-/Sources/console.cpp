#include "console.hpp"
#include "globals.hpp"
#include "shadermgr/shaderManager.hpp"
#include <string>
#include <iostream>
#include <vector>
#include "helpers/utility.hpp"


using namespace IceRender;

std::string Console::GetCommandFromConsole() const
{
	Print("[Console] enter command: ");

	cin.clear();
	std::string command;
	std::getline(cin, command);
	return command;
}

std::string Console::GetPreviousCommand() const { return preCommand; }

void Console::HandleCommand(const std::string& _command)
{
	bool valid = false;
	if (_command == "")
		valid = false; // do nothing

#define CommandMap(_commandStr, _codes) if(_command == _commandStr) {_codes; valid=true;}
	// ------------------------------------------------------------------------------ //

	CommandMap("wire_on", glPolygonMode(GL_FRONT_AND_BACK, GL_LINE))
	CommandMap("wire_off", glPolygonMode(GL_FRONT_AND_BACK, GL_FILL))
	CommandMap("print_active_shader", GLOBAL.shaderMgr->PrintActiveShader())
	CommandMap("exit_show_image", Utility::ExitShowImage())
	CommandMap("clear_all", GLOBAL.sceneMgr->ClearAll(); GLOBAL.shadowMgr->RemoveShadowRender(); GLOBAL.shaderMgr->Clear(); Print("Clear All things."))
	CommandMap("save_screenshot", if(Utility::SaveTextureToPNG("screenshot_"+Utility::GetCurrentTimeStr(), GLOBAL.WIN_WIDTH, GLOBAL.WIN_HEIGHT, GL_RGBA, 0)) Print("Screenshot saved."))

	// ------------------------------------------------------------------------------ //
#undef CommandMap

/* to simplify the codes, but not easy to debug if something wrong.but keep codes simple then it should not be a problem. * /
/* check expanded codes by viewing "CommandParamMap" directly[move cursor and hover on the "CommandParamMap"] */
#define CommandParamMap(_commandStr, _params, _codes) else if (_command.find(_commandStr) != std::string::npos && _command.length() > _commandStr.length()+1 \
&& _command[_commandStr.length()] == ' ') \
{ \
	_params = _command.substr(_commandStr.length()+1, _command.length() - _commandStr.length() - 1);/*params from command*/ \
	_codes \
	valid=true; \
}
	// ------------------------------------------------------------------------------ //
	
	CommandParamMap(
		std::string("show_fps"),
		std::string params,
		{ int isShow = std::stoi(params); GLOBAL.timeMgr->SetShowFPS(isShow);})

	CommandParamMap(
		std::string("log_cam_trans"),
		std::string params,
		{ int isLog = std::stoi(params); GLOBAL.camCtrller->SetLogCamInfo(isLog); 
		std::string msgPrefix = isLog ? "Enable" : "Disable";
		Print(msgPrefix + " logging camera transforma");})
	
	CommandParamMap(
		std::string("test_unit"),
		std::string params,
		{
			Print("testUnit-function name: " + params);
			try 
			{
				GLOBAL.testUnit->TryCallFunc(params);
			}
			catch (std::exception const& ex)
			{
				Print(NULL, "ss", "Exception: ", ex.what());
			}
		})

	CommandParamMap(
		std::string("load_shader"),
		std::string params,
		{ 
			if (params.find("\\") != std::string::npos)
			{
				Print("Path should use '/' to seperate. E.g. Simple/simple");
			}
			else
			{
				Print("Try to load shader: " + params);
				if (GLOBAL.shaderMgr->LoadShader(GLOBAL.shaderPathPrefix + params))
					GLOBAL.shaderMgr->PrintActiveShader();
				else
					Print("shader load failed.");
			}
		})

	CommandParamMap(
		std::string("set_cam_pos"),
		std::string params,
		{ 
			std::vector<std::string> v = Utility::SplitString(params, " ");
			if (v.size() != 3)
				Print("Error Params: Require at least 3 params. EX: set_cam_pos 1 2 3");
			else
			{
				glm::vec3 newPos(std::stof(v[0]), std::stof(v[1]), std::stof(v[2]));
				Utility::PrintVec3(newPos, "\nnew camera position:\n");
				GLOBAL.camCtrller->SetCameraPosition(newPos);
			}
		})

	CommandParamMap(
		std::string("set_cam_rot"),
		std::string params,
		{ 
			std::vector<std::string> v = Utility::SplitString(params, " ");
			if (v.size() != 3)
				Print("Error Params: Require at least 3 params. EX: set_cam_pos 1 2 3");
			else
			{
				glm::vec3 newRot(std::stof(v[0]), std::stof(v[1]), std::stof(v[2]));
				Utility::PrintVec3(newRot, "\nnew camera rotation:\n");
				GLOBAL.camCtrller->SetCameraRotation(newRot);
			}
		})

	CommandParamMap(
		std::string("show_image"),
		std::string params,
		{ 
			if (params.find("\\") != std::string::npos)
			{
				Print("Path should use '/' to seperate. E.g. Simple/earth.jpg");
			}
			else
			{
				Print("Try to show image: " + params);
				Utility::ShowImage(params);
			}
		})

	CommandParamMap(
		std::string("load_scene"),
		std::string params,
		{ 
			if (params.find("\\") != std::string::npos )
				Print("Path should use '/' to seperat");
			else if(params.find(".json") == std::string::npos)
				Print("Config file should be a json file");
			else
			{
				string configPath = "Resources/SceneConfigs/" + params;
				Print("Try to load scene from config file: " + configPath);
				GLOBAL.shaderMgr->Clear();
				GLOBAL.shadowMgr->RemoveShadowRender();
				GLOBAL.sceneMgr->LoadFromSceneConfig(configPath);
				GLOBAL.shadowMgr->InitShadowRender();
			}
		})

	CommandParamMap(
		std::string("set_render_method"),
		std::string params,
		{ 
			if (!params.empty())
			{
				GLOBAL.sceneMgr->SetCurrentRenderMethod(params);
				Print("Try set \""+params+"\" to RenderMethod");
			}
		})
	
	CommandParamMap(
		std::string("save_shadow_map"),
		std::string params,
		{ 
			if (!params.empty())
				GLOBAL.shadowMgr->SaveShadowMap(params);
		})

	CommandParamMap(
		std::string("load_shadow_config"),
		std::string params,
		{ 
			if (params.find("\\") != std::string::npos )
				Print("Path should use '/' to seperat");
			else if(params.find(".json") == std::string::npos)
				Print("Config file should be a json file");
			else
			{
				string configPath = "Resources/SceneConfigs/" + params;
				Print("Try to reload ShadowConfig from config file: " + configPath);

				nlohmann::json sceneData = Utility::LoadJsonFromFile(configPath);
				// TODO: keep update here
				if (sceneData != nullptr && sceneData.contains("shadow_config"))
				{
					GLOBAL.shadowMgr->RemoveShadowRender();
					GLOBAL.shadowMgr->LoadShadowRender(sceneData["shadow_config"]);
					GLOBAL.shadowMgr->InitShadowRender();
					Print("Reload ShadowRender successfully.");
				}
				else
					Print("Reload ShadowRender failed.");
			}
		})
	// ------------------------------------------------------------------------------ //
#undef CommandParamMap
	
	if (!valid)
		Print("Command invalid!");
	else
		preCommand = _command;
	// TODO: reload scene/mesh/files and etc.
}

void Console::PrintHelp()
{
	// TODO: keep update here
	std::string helpMsg;
	helpMsg.append("\n----------------------------------------------------------------------------------------------------------\n");
	helpMsg.append("Help Informations:\n\n");

	helpMsg.append("1- Camera Controll:\n\t-Up/Down/Left/Right: move camera, W/S/A/D: rotate camera, Z/X: zoom in/out camera.\n");
	helpMsg.append("\t-Press F2 to reset camera's transformation.\n");

	helpMsg.append("2- Press ` to enter command.\n");

	helpMsg.append("\t-Command: 'wire_on/wire_off' to enable/disable show wire.\n");
	helpMsg.append("\t-Command: 'print_active_shader' to show current shader.\n");

	helpMsg.append("\t-Command: 'show_fps 0/1' to disable/enable show FPS.\n");
	helpMsg.append("\t\te.g. 'show_fps 1' -> show FPS information in windows title.\n");

	helpMsg.append("\t-Command: 'log_cam_trans 0/1' to disable/enable show camera's transformation.\n");

	helpMsg.append("\t-Command: 'test_unit xxx' to call function name with xxx.\n");
	helpMsg.append("\t\te.g. 'test_unit Test1' -> Test1() function will be called, which is defined in testUnit.h/cpp.\n");

	helpMsg.append("\t-Command: 'load_shader xxx' to load and active xxx shader.\n");
	helpMsg.append("\t\tparams must be relative path to folder \"Resources/Shaders/\", e.g.\"Simple/simple\".\n");

	helpMsg.append("\t-Command: 'set_cam_pos/set_cam_rot x y z' to set camera's position or rotation.\n");
	helpMsg.append("\t\te.g. 'set_cam_pos 0 1 2' -> camera's position will become (0, 1, 2).\n");

	helpMsg.append("\t-Command: 'show_image params/exit_show_image' to show an image or exit to show it.\n");
	helpMsg.append("\t\tparams must be relative path to folder \"Resources/Images/\", e.g.\"Simple/earth.jpg\".\n");
	
	helpMsg.append("\t-Command: 'clear_all' to remove all sceneobjs and lights.\n");

	helpMsg.append("\t-Command: 'load_scene params' to reload the scene from config file.\n");
	helpMsg.append("\t\tparams must be a json file which can be located inside \"Resources/SceneConfigs/\", e.g.\"xx.json\".\n");

	helpMsg.append("\t-Command: 'set_render_method params' to set different render method.\n");
	helpMsg.append("\t\tparams must be one of methods specified in \"RasterizerRender.hpp\"\n");
	
	helpMsg.append("\t-Command: 'save_screenshot' to save current screenshot as PNG file into Output folder.\n");

	helpMsg.append("\t-Command: 'save_shadow_map params' to save light's shadowmap as PNG file into Output folder.\n");
	helpMsg.append("\t\tparams must be the light name.\n");
	
	helpMsg.append("\t-Command: 'load_shadow_config params' to reload the shadow config from scene config file.\n");
	helpMsg.append("\t\tparams must be a json file which can be located inside \"Resources/SceneConfigs/\", e.g.\"xx.json\".\n");
	
	helpMsg.append("\t-Press F3 to execute last command.\n");

	helpMsg.append("\n----------------------------------------------------------------------------------------------------------\n");

	Print(helpMsg);
}
