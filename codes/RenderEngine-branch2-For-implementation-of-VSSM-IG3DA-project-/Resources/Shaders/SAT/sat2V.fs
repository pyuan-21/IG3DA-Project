#version 450 core
// vertical scan, read k texels at the same time. Each value is 3-components value.
// k must be 2^i. E.g. 2, 4, 8, 16. Only works for 8 at maximum in my practice

uniform int i;
uniform sampler2D texInput;
layout (location = 0) out vec2 texOutput;

void main()
{
	//refer: https://registry.khronos.org/OpenGL-Refpages/gl4/html/gl_FragCoord.xhtml
	// by default, the first pixel xy is (0.5,0.5) not (0,0)
	/*texelFetch() is using texel space coordinate*/
	
	vec2 result = vec2(0);
	ivec2 current = ivec2(gl_FragCoord.xy-vec2(0.5));
	
	//read k texels at the same time, k is 8
	int offset = int(pow(8, i));
	result += texelFetch(texInput, current, 0).rg; current.y += offset; //k=0
	result += texelFetch(texInput, current, 0).rg; current.y += offset; //k=1
	result += texelFetch(texInput, current, 0).rg; current.y += offset; //k=2
	result += texelFetch(texInput, current, 0).rg; current.y += offset; //k=3
	result += texelFetch(texInput, current, 0).rg; current.y += offset; //k=4
	result += texelFetch(texInput, current, 0).rg; current.y += offset; //k=5
	result += texelFetch(texInput, current, 0).rg; current.y += offset; //k=6
	result += texelFetch(texInput, current, 0).rg;						//k=7

	texOutput = result;
}
