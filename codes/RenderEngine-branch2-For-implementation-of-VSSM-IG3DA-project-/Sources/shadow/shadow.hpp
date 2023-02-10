#pragma once
#include <memory>
#include "json.hpp"
#include <vector>
#include "glad/glad.h"
#include "../shadermgr/shaderProgram.hpp"
#include <map>
#include "../helpers/satGenerator.hpp"
#include <utility>

namespace IceRender
{
	class BasicShadowRender
	{
	public:
		virtual void Init() = 0;
		virtual void Render() = 0;
		virtual void Clear() = 0;
	};

	// TODO: don't ask me why I designed things like this, I have no idea just want to make it work first. To recontruct this codes later when I have more experience.
	class ShadowManager
	{
	private:
		std::shared_ptr<BasicShadowRender> shadowRender;
		bool useTightSpace;

	public:
		ShadowManager();
		~ShadowManager();

		void RemoveShadowRender();

		// create shadow render from json data
		void LoadShadowRender(nlohmann::json _data);

		void InitShadowRender(); // each time light sources change, we should call it

		void RenderShadow();

		bool IsNeedShadowRender(); // if there exists ShadowRender, it means we need to render shadow

		// For Debug use
		void SaveShadowMap(const std::string& _lightName) const;

		std::shared_ptr<BasicShadowRender> GetShadowRender();

		bool IsUseTightSpace() const;
		void SetUseTightSpace(const bool& _value);
	};

#pragma region Shadow Components
	class BasicShadowComponent {};

	/*PCF*/
	class PercentageCloserFilter : public BasicShadowComponent
	{
	private:
		int pcfKernelSize; // size is the length of one edge, must be odd number. EX: kernelSize=5, means filter area contains 5*5 texels in total

	public:
		//[TODO] can generate SAT for it when pcfKernelSize is too large.
		// limitation of PCF, although it softens shadwo edges, but it still gives a bad result(we can see many "box" shadow area). Aliasing issue.
		void SetPCFKernelSize(const int& _size);
		int GetPCFKernelSize() const;
	};

	/*PCSS*/
	class PercentageCloserSoftFilter : public BasicShadowComponent
	{
	private:
		int maxSearchSize; // unlike PCSS proposed, I separate search area size and light size, because only tiny search area can be considered as planar. Also for performance
		int lightSize; // area light size
		int minPenumbraSize, maxPenumbraSize;
		float penumbraRatio;

	public:
		//[Note] according to paper PCSS "Percentage-Closer Soft Shadows (Randima Fernando, NVIDIA Corporation)"
		// the blocker search area size is proportional to both the light size and the distance to the light.
		void SetParams(const int& _maxSearchSize, const int& _lightSize, const int& _minPenumbraSize, const int& _maxPenumbraSize, const float& _penumbraRatio);
		void GetParams(int& _maxSearchSize, int& _lightSize, int& _minPenumbraSize, int& _maxPenumbraSize, float& _penumbraRatio) const;
	};

#pragma endregion



#pragma region Shadow Map Techniques
	/*This class just put some common method used in traditional ShadowMaps and VarianceShadowMaps here*/
	class BasicShadowMapRender : public BasicShadowRender
	{
	protected:
		int resWidth, resHeight; // resolution: it should generally be at least as big as the default framebuffer(our OpenGL window) - refer OpenGL Book Page 551

		std::vector<std::shared_ptr<BasicShadowComponent>> components;

	public:
		void GetResolution(int& _width, int& _height) const;
		void SetResolution(int _w, int _h);

		void Init() override;
		void Render() override;
		void Clear() override;
		virtual GLuint GetDepthFrameBuffer(const int& _lightIndex);
		virtual GLuint GetDepthTexture(const int& _lightIndex);
		virtual void SaveShadowMap(const int& _lightIndex, const std::string& _lightName); // use command "save_shadow_map lightName" to check output

		/*when calling lighting(shading) shader to compute the final surface color for each fragment, we can use this function to pass some parameters for computing its shadow*/
		virtual void InitComputeLightRatioParameters(shared_ptr<ShaderProgram>& _shaderPro, GLuint& _texUnit);

		int AddComponent(std::shared_ptr<BasicShadowComponent>& _component);
		std::shared_ptr<BasicShadowComponent>& GetComponent(const int& _index);
	};


	// basic shadow map technique
	class ShadowMapRender : public BasicShadowMapRender
	{
	private:
		//[TODO] maybe it is better to use one framebuffer and bind different textures to it. E.g, if we now need to render 5 lights, we can just create one FBO, then bind 5 depthTextures to it.
		// (split lighting and shadowing into two seperated passes)
		std::map<int, glm::vec<2, GLuint>> depthMap; // key is the light index, depth texture at index=0 (X component), its framebuffer id at index=1 (Y component).

		float bias; // bias

		// component index. If index = -1, it means not using the relevant component
		int pcfIndex;
		int pcssIndex;

	public:
		void Init() override;
		void Render() override;
		void Clear() override;
		GLuint GetDepthFrameBuffer(const int& _lightIndex) override;
		GLuint GetDepthTexture(const int& _lightIndex) override;
		void SaveShadowMap(const int& _lightIndex, const std::string& _lightName) override; // use command "save_shadow_map lightName" to check output
		
		void SetBias(const float& _biasMin);
		
		void InitComputeLightRatioParameters(shared_ptr<ShaderProgram>& _shaderPro, GLuint& _texUnit) override;

		/*PCF*/
		void InitPCF(const bool& _usePCF, const int& _pcfKernelSize);

		/*PCSS*/
		void InitPCSS(const bool& _usePCSS, const int& _maxSearchSize, const int& _lightSize, const int& _minPenumbraSize, const int& _maxPenumbraSize, const float& _penumbraRatio);
	};
#pragma endregion

#pragma region Variant Shadow Map Techniques

	// [TODO], separate some functions(such as blur, PCF) out of ShadowMapRender, make them as components. For now, PCF is not used in VSM which it still takes some memory.
	class VarianceShadowMapRender : public BasicShadowMapRender
	{
	private:
		// [Note] to see the results, we can use "Utility::RenderScreenQuad(GetDepthTexture(i));", where i is light index.
		std::map<int, glm::vec<3, GLuint>> depthMap; // key is the light index, depth texture at index=0 (X component), its framebuffer id at index=1 (Y component), depthRenderBuffer at index=2(Z component)

		int kernelSize; // use to compute the mean of depth over a kernel area, this size is the length of one edge, must be odd number. EX: kernelSize=5, means filter area contains 5*5 texels in total
		// dependent on scene
		float varMin; // setting a minimum variance can eliminate the shadow acne issue(biasing)
		float pMin; // setting a minimum upper bound "p" can reduce the "light bleeding" issue, but it darken the penumbra area.

		/*SAT relevant*/
		bool useSAT;
		std::map<int, std::shared_ptr<SummedAreaTableGenerator>> satGeneratorMap; // summed-area table generator for (depth,depth_square)

		/*PCSS*/
		int pcssIndex; //[TODO] I have tried integrate PCSS into vsm, actually there is no big difference. VSM is enough(sometimes we even don't need the SAT)//may be delete relevant codes later

	public:
		void Init() override;
		void Render() override;
		void Clear() override;
		GLuint GetDepthFrameBuffer(const int& _lightIndex) override;
		GLuint GetDepthTexture(const int& _lightIndex) override;
		void SaveShadowMap(const int& _lightIndex, const std::string& _lightName) override; // use command "save_shadow_map lightName" to check output

		void SetKernelSize(const int& _kernelSize);
		void SetVarianceMin(const float& _varMin);
		void SetPMin(const float& _pMin);

		void SetUseSAT(const bool& _value);
		bool IsUseSAT() const;
		GLuint GetSAT(const int& _lightIndex);

		// for debug
		std::shared_ptr<SummedAreaTableGenerator> GetSATGenerator(const int& _lightIndex);

		void InitComputeLightRatioParameters(shared_ptr<ShaderProgram>& _shaderPro, GLuint& _texUnit) override;

		/*PCSS*/
		void InitPCSS(const bool& _usePCSS, const int& _maxSearchSize, const int& _lightSize, const int& _minPenumbraSize, const int& _maxPenumbraSize, const float& _penumbraRatio);
	};

	class VSSMRender : public BasicShadowMapRender
	{
	private:
		// [Note] to see the results, we can use "Utility::RenderScreenQuad(GetDepthTexture(i));", where i is light index.
		std::map<int, glm::vec<3, GLuint>> depthMap; // key is the light index, depth texture at index=0 (X component), its framebuffer id at index=1 (Y component), depthRenderBuffer at index=2(Z component)

		// dependent on scene
		float varMin; // setting a minimum variance can eliminate the shadow acne issue(biasing)
		float pMin; // setting a minimum upper bound "p" can reduce the "light bleeding" issue, but it darken the penumbra area.

		/*SAT relevant*/
		std::map<int, std::shared_ptr<SummedAreaTableGenerator>> satGeneratorMap; // summed-area table generator for (depth,depth_square)

		/*PCSS*/
		int pcssIndex; //[TODO] I have tried integrate PCSS into vsm, actually there is no big difference. VSM is enough(sometimes we even don't need the SAT)//may be delete relevant codes later

		/*VSSM*/
		int M;
		int N;

	public:
		void Init() override;
		void Render() override;
		void Clear() override;
		GLuint GetDepthFrameBuffer(const int& _lightIndex) override;
		GLuint GetDepthTexture(const int& _lightIndex) override;
		void SaveShadowMap(const int& _lightIndex, const std::string& _lightName) override; // use command "save_shadow_map lightName" to check output

		void SetVarianceMin(const float& _varMin);
		void SetPMin(const float& _pMin);

		GLuint GetSAT(const int& _lightIndex);

		void InitComputeLightRatioParameters(shared_ptr<ShaderProgram>& _shaderPro, GLuint& _texUnit) override;

		/*PCSS*/
		void InitPCSS(const bool& _usePCSS, const int& _maxSearchSize, const int& _lightSize, const int& _minPenumbraSize, const int& _maxPenumbraSize, const float& _penumbraRatio);

		void InitSubdivision(const int& _M, const int& _N);
	};
#pragma endregion
}
