#version 450 core

in vec2 fUV;

uniform int useAlbedoTex;
uniform vec3 albedoColor;
uniform sampler2D albedoTex;

out vec4 colorResponse;

void main()
{
	if(useAlbedoTex==1)
		colorResponse = texture(albedoTex, fUV);
	else
		colorResponse = vec4(albedoColor, 1);
}