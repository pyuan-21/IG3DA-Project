#version 450 core
// reconstuct the original image(RGB) from SAT

uniform sampler2D texInput; //SAT
layout (location = 0) out vec3 texOutput;

void main()
{
	ivec2 current = ivec2(gl_FragCoord.xy-vec2(0.5));
	
	/*texelFetch() is using texel space coordinate*/
	/*pixel(x,y)=SAT(x,y)-SAT(x-1,y)-SAT(x,y-1)+SAT(x-1,y-1)*/
	vec4 result = texelFetch(texInput, current, 0) - texelFetch(texInput, current-ivec2(1,0), 0) - texelFetch(texInput, current-ivec2(0,1), 0) + texelFetch(texInput, current-ivec2(1,1), 0);

	/*paper said biasing SAT each element will lose 0.5*x*y, but when using SAT to reconstruct it, actually it only lose 0.5 comparing with original texture*/
	vec3 loss = vec3(0.5); 
	texOutput = result.xyz + loss;
}