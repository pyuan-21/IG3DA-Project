#pragma once
#include "glad/glad.h"
#include <functional>

namespace IceRender
{
	struct SATConfig
	{
	public:
		int resWidth;
		int resHeight;
		GLuint inputTexID;
		int componentNum;
		std::function<void(GLuint& _texID, const int& _width, const int& _height)> texGenerator;

		SATConfig();
		SATConfig(const SATConfig& _config);
	};

	class SummedAreaTableGenerator
	{
		//refer: paper-"Fast Summed-Area Table Generation and its Applications, Hensley, 2005"
	private:
		SATConfig config;
		GLuint texA, texB; // two textures
		GLuint fbo; // framebuffer object
		std::string horShaderName, verShaderName; // horizontal, vertical pass shader name
		int N, M;

		// TODO: for now just support two-component texture, because I only use it in VSM for now. Later when I implement shader auto-generator by using differernt sub shader file,
		// then I can support dynamic number component. If not implementing sub shader files to create the final shader, 
		// it requires to write if-else codes in shader which slow down performance.

	public:
		~SummedAreaTableGenerator();

		void Init(const SATConfig& _config);
		void Generate();
		void Clear();
		GLuint GetSAT() const;
		void Reconstruct(GLuint& _outputTex);
		void BoxFilter(GLuint& _outputTex, const int& _kernelSize);
	};
}
