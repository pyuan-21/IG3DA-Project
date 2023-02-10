#version 450 core

layout (location = 0) in vec3 vPos;
//layout (location = 1) in vec3 vNormal;
//layout (location = 2) in vec2 vUV;

uniform mat4 modelMat;
uniform mat4 lightMat; /*light space matrix(perspective or orthogonal projection)*/

void main()
{
	gl_Position = lightMat*modelMat*vec4(vPos, 1);
}