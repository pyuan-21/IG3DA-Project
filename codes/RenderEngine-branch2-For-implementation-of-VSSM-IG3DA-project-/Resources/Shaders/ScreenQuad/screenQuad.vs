#version 450 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vUV;

out vec2 fUV;

void main()
{
	gl_Position = vec4(vPos, 1);
	fUV = vUV;
}