#version 450 core

layout (location = 0) out vec3 fColour;

in vec3 col;
in vec3 nor;
in vec3 FragPosWorldSpace;
in vec4 FragPosProjectedLightSpace;

uniform vec3 lightPos;
uniform vec3 lightDirection;
uniform vec3 lightColour;
uniform vec3 camPos;
uniform sampler2D shadowMap;

float shadowOnFragment(vec4 FragPosProjectedLightSpace)
{
	vec3 ndc = FragPosProjectedLightSpace.xyz / FragPosProjectedLightSpace.w;
	vec3 ss = (ndc + 1) * 0.5;
	float fragDepth = ss.z;
	float litDepth = texture(shadowMap, ss.xy).r;

	vec3 n_nor = normalize(nor);
	vec3 n_toLight = normalize(-lightDirection);
	float bias = max(0.05 * (1.0 - dot(n_nor, n_toLight)), 0.005);

	float shadow = 0.f;
	shadow = fragDepth > (litDepth+bias) ? 1.0 : 0.0;

	if(fragDepth > 1)
	{
		shadow = 0.f;
	}

	return shadow;
}

float CalculateDirectionalIllumination()
{
	float ambient = 0.1f;
	vec3 n_nor = normalize(nor);
	vec3 n_toLight = normalize(-lightDirection);
	float diffuse = max(dot(n_nor, n_toLight), 0.0);
	vec3 n_fromLight = normalize(lightDirection);
	vec3 n_refLight = reflect(n_fromLight, n_nor);
	vec3 camDirection = camPos - FragPosWorldSpace;
	vec3 n_camDirection = normalize(camDirection);
	float specular = pow(max(dot(n_camDirection, n_refLight), 0.0), 128.0);
	
	float shadow = shadowOnFragment(FragPosProjectedLightSpace);
	float phong = ambient + ((1.f - shadow) * (diffuse + specular));
	return phong;
}

float CalculatePositionalIllumination()
{
	float ambient = 0.1f;
	vec3 n_nor = normalize(nor);
	vec3 n_toLight = normalize(lightPos - FragPosWorldSpace);
	float diffuse = max(dot(n_nor, n_toLight), 0.0);
	vec3 n_fromLight = -n_toLight;
	vec3 n_refLight = reflect(n_fromLight, n_nor);
	vec3 camDirection = camPos - FragPosWorldSpace;
	vec3 n_camDirection = normalize(camDirection);
	float specular = pow(max(dot(n_camDirection, n_refLight), 0.0), 128.0);
	float distance = length(lightPos - FragPosWorldSpace);
	float attenuation = 1.0 / (1.0 + (0.05 * distance) + (0.002 * distance * distance));
	float phong = (ambient + diffuse + specular) * attenuation;
	return phong;
}

float CalculateSpotIllumination()
{
	float ambient = 0.1f;
	vec3 n_nor = normalize(nor);
	vec3 n_toLight = normalize(lightPos - FragPosWorldSpace);
	float diffuse = max(dot(n_nor, n_toLight), 0.0);
	vec3 n_fromLight = -n_toLight;
	vec3 n_refLight = reflect(n_fromLight, n_nor);
	vec3 camDirection = camPos - FragPosWorldSpace;
	vec3 n_camDirection = normalize(camDirection);
	float specular = pow(max(dot(n_camDirection, n_refLight), 0.0), 128.0);
	float distance = length(lightPos - FragPosWorldSpace);
	float attenuation = 1.0 / (1.0 + (0.05 * distance) + (0.002 * distance * distance));
	float phi = cos(15.0 * (3.14159265358979323846 / 180.0));
	vec3 n_spotDir = normalize(lightDirection);
	float theta = dot(n_fromLight, n_spotDir);
	float phong;

	if(theta > phi)
	{
		phong = (ambient + diffuse + specular) * attenuation;
	}
	else
	{
		phong = ambient * attenuation;
	}

	return phong;
}

void main()
{
	float phong = CalculateDirectionalIllumination();
	fColour = vec3(phong * col * lightColour);
}