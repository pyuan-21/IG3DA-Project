#pragma once

#include <memory>
#include "../shadermgr/shaderProgram.hpp"


namespace IceRender
{
	// different render methods
	namespace RasterizerRender
	{
		void NoRender();
		void RenderSimple();
		void RenderPhong();
		void RenderSceenQuad();

#pragma region Rendering techinique for fun
		void RenderSonarLight(); // just for fun

#pragma endregion
	}

	namespace RayTracerRender
	{
		// TODO:
	}
}
