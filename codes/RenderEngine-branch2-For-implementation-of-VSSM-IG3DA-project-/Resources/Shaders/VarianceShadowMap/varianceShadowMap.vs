#version 450 core

layout (location = 0) in vec3 vPos;

uniform mat4 modelMat;
uniform mat4 lightMat; /*light space matrix(perspective or orthogonal projection)*/

/*don't use projected depth value. It will lose precision when getting near to far plane*/
//out vec4 projPos; /*normalized projected position, inside [-1, 1]^3 space*/
out vec3 worldPos;

void main()
{
	gl_Position = lightMat*modelMat*vec4(vPos, 1);
	//projPos = gl_Position/gl_Position.w;
	worldPos = (modelMat*vec4(vPos, 1)).xyz;
}