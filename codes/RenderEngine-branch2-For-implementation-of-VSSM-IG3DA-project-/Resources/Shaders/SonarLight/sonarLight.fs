#version 450 core

in vec3 fPos;
in vec3 fNormal;
in vec2 fUV;

/*matrix*/
uniform mat4 modelMat;
uniform mat4 viewMat;

/*ambient*/
uniform vec3 ambientLight;

/*lights*/
uniform int activeLightNum;
const int maxLightNum = 5; /*TODO:don't forget to modify its corresponding variable in SceneManager*/
struct Light
{
	int type;
	vec3 color, pos, dir; /*color, position, direction*/
	float intensity;
	vec2 attenuation;
};
uniform Light lights[maxLightNum];

uniform int useAlbedoTex;

/*material*/ 
struct Material
{
	vec3 ka, kd, ks, color; /*coefficient for ambient, diffuse, specular and color*/
	float shiness;
	sampler2D albedoTex;
};
uniform Material material; 

/*Sonar light parameters*/
uniform float waveMaxDepth;
uniform float waveWidth;
uniform float waveInterval;
uniform float waveMoveOffset;

out vec4 colorResponse;

float ComputeLightVisibleRatio(float distance)
{
	float minRadius, maxRadius, midRadius, halfInterval;
	for(float curRadius=waveMoveOffset; curRadius<=waveMaxDepth; curRadius+=waveInterval)
	{
		minRadius = max(0, curRadius-waveWidth/2);
		maxRadius = min(waveMaxDepth, curRadius+waveWidth/2);
		midRadius = (minRadius+maxRadius)/2;
		halfInterval = (maxRadius-minRadius)/2;
		if(distance>=minRadius && distance<=maxRadius)
			return 1.0-abs(distance-midRadius)/halfInterval; /*the light attenuate from wave mid point to both side*/
	}
	return 0;
}

void main()
{
	vec3 albedo;
	if(useAlbedoTex==1)
		albedo = texture(material.albedoTex, fUV).rgb;
	else
		albedo = material.color;

	vec3 ePos = (viewMat*modelMat*vec4(fPos, 1)).xyz; /*vertex position in eye space*/

	vec3 lRes = vec3(0); /*lighting response*/

	float lvr = ComputeLightVisibleRatio(length(ePos)); /*light visible ratio*/
	if(lvr <= 0.0)
	{
		colorResponse = vec4(vec3(0), 1);
		return; /*early return, save computation time*/
	}

	vec3 ambient = ambientLight*material.ka;

	for(int i=0;i<activeLightNum;i++)
	{
		/*lighting computation is in "eye space"*/
		Light light = lights[i];
		float lI = light.intensity;
		vec3 lPos = (viewMat*vec4(light.pos, 1)).xyz;
		vec3 lDir;
		if(light.type == 0)
		{
			/*point light*/ 
			lDir = lPos - ePos;
			float d = length(lDir);
			lI *= 1.0 / (1 + d*light.attenuation.x + pow(d,2)*light.attenuation.y);
		}
		else if(light.type == 1)
		{
			lDir = (viewMat*vec4(light.dir, 0)).xyz;
		}
		lDir = normalize(lDir);
		vec3 lC = light.color*lI; /*light color(or color intensity) at this vertex.*/
		
		/*refer: https://en.wikipedia.org/wiki/Phong_reflection_model, but I have my own modification*/

		vec3 eN = normalize((transpose(inverse(viewMat*modelMat))*vec4(fNormal, 0)).xyz);

		/*diffuse*/ 
		/*use 0 for normal's forth component in homogenous coordinate, cause translation should not be applied to normal vector*/
		vec3 diffuse = vec3(0);		
		float lDirDotN = dot(lDir,eN);
		if(lDirDotN > 0)
			/*albedo and incoming light control the color response at this vertex point*/
			diffuse = material.kd*lDirDotN*lC;

		/*specular*/
		vec3 specular = vec3(0);
		vec3 lRef = normalize(2*lDirDotN*eN-lDir); /*light reflection direction*/
		vec3 eV = normalize(-ePos); /*eye view direction at this vertex. In eye space, camera is at origin*/
		float lRefDotN = dot(lRef, eV);
		if(lRefDotN > 0)
			specular = material.ks*pow(lRefDotN, material.shiness)*lC;

		lRes += diffuse + specular;
	}

	lRes += ambient;
	lRes *= albedo; /*albedo determines how much lighting reflect from the surface*/

	colorResponse = vec4(lvr*lRes, 1);
}