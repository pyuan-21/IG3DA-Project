#version 450 core

in vec2 fUV;
in vec3 fNormal; // need to be normalized

uniform int useAlbedoTex;
uniform vec3 albedoColor;
layout (binding = 0) uniform sampler2D albedoTex;

out vec4 colorResponse;

void main()
{
	vec3 normal = normalize(fNormal);

	if(useAlbedoTex==1)
		colorResponse = texture(albedoTex, fUV);
	else
		colorResponse = vec4(albedoColor, 1);
}