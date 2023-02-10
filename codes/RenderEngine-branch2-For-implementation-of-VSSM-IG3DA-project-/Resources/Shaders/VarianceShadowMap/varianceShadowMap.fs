#version 450 core

//in vec4 projPos;
in vec3 worldPos;

/*SAT-VSM proposed*/
struct LightCamInfo
{
	float near;
	float far;
	vec3 lightCamPos;
	vec3 lightViewDir;
};
uniform LightCamInfo lightCamInfo;

layout (location = 0) out vec2 varDepths; /*variant depth information: depth and depthSquare*/

void main()
{
	/*Note, according to SAT-VSM, M2 can be computed by using mean and its derivative.*/
	/*There is no need to store depth square. Also, for fixing precison issue, using distance to light plane*/
	/*instead of projected Z value.*/
	vec3 v = worldPos - lightCamInfo.lightCamPos;
	vec3 proAxis = normalize(lightCamInfo.lightViewDir);
	float linearDepth = dot(v, proAxis);
	linearDepth = (linearDepth - lightCamInfo.near) / (lightCamInfo.far - lightCamInfo.near);
	linearDepth = clamp(linearDepth, 0, 1);

	/*If using projected depth, the computation precision here is really dependent on near and far planes*/
	/*we should use tight light view frustum which means near and far should be as close as possible*/
	/*But I use linear depth here, as SAT-VSM recommended*/

	// for comparsion, projected depth and linear depth
	//float projDepth = 0.5*(projPos.z+1); /*map [-1,1] to [0,1] in order to fit texture's need*/

	//float depth = projDepth;
	float depth = linearDepth;

	/*use SAT-VSM method to fix the bias computation*/
	/*Here E(x)(M1) is considered in a texel(fragment), therefore it is depth*/
	/*refer: https://developer.nvidia.com/gpugems/gpugems3/part-ii-light-and-shadows/chapter-8-summed-area-variance-shadow-maps*/

	float dx = dFdx(depth);
	float dy = dFdy(depth);
	float depthSquare = depth*depth + 0.25*(dx*dx+dy*dy); /*actually it is the Moment2 for this texel*/
	varDepths = vec2(depth, depthSquare); /*output depthSquare is neccessary because we want to linear interpolate it*/
}