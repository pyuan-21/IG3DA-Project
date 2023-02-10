#version 450 core
// copy texture

uniform sampler2D texInput; // input
layout (location = 0) out vec2 texOutput;

void main()
{
	/*for improving float-piont precision, recenter pixel value at origin.*/
	/*if value is in range [0,1] then map it to [-0.5, 0.5]*/
	/*first thing is to use sign of floating-point number, second thing is to small the maximum SAT value*/

	ivec2 current = ivec2(gl_FragCoord.xy-vec2(0.5));
	texOutput = texelFetch(texInput, current, 0).rg - vec2(0.5);
}