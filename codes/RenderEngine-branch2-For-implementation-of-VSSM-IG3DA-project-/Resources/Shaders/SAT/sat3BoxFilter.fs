#version 450 core
// filter the original image(RGB) from SAT, just get the mean

uniform int halfKernelSize;
uniform sampler2D texInput; //SAT
layout (location = 0) out vec3 texOutput;

void main()
{
	ivec2 center = ivec2(gl_FragCoord.xy-vec2(0.5));
	
	ivec2 minCorner = center - ivec2(halfKernelSize+1);
	ivec2 maxCorner = center + ivec2(halfKernelSize);
	// don't clamp corner, because we do need some values which are out of range to compute true sum.
	vec4 result = texelFetch(texInput, maxCorner, 0) - texelFetch(texInput, ivec2(minCorner.x, maxCorner.y), 0) - texelFetch(texInput, ivec2(maxCorner.x, minCorner.y), 0) + texelFetch(texInput, minCorner, 0);

	// compute the real number over kernel area, must clamp corner.
	ivec2 texSize = textureSize(texInput, 0);
	minCorner = clamp(minCorner, ivec2(0), texSize-ivec2(1));
	maxCorner = clamp(maxCorner, ivec2(0), texSize-ivec2(1));
	int totalNum = max(maxCorner.x-minCorner.x, 1) * max(maxCorner.y-minCorner.y, 1);
	vec3 loss = vec3(0.5); //totalLoss = loss * totalNum-> its mean loss is loss
	texOutput = result.xyz/totalNum + loss;
}