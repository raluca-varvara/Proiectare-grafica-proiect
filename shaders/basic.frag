 #version 410 core

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoords;
in vec4 fPosEye;
in vec4 fragPosLightSpace;

out vec4 fColor;

//matrices
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;
//lighting
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 lampColor;
uniform vec3 position;
uniform vec3 positions[20];

uniform float constant;
uniform float linear;
uniform float quadratic;

uniform int day;
uniform float fogDensity;

// textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

//components
vec3 ambient;
vec3 diffuse;
vec3 specular;

vec3 ambient1 = vec3(0.0f);
vec3 diffuse1 = vec3(0.0f);
vec3 specular1 = vec3(0.0f);

float ambientStrength = 0.4f;
float specularStrength = 0.5f;
float shininess = 32.0f;
vec3 colorPoint = vec3(0.0f);

void computeDirLight()
{
    //compute eye space coordinates
    //vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(normalMatrix * fNormal);

    //normalize light direction
    vec3 lightDirN = vec3(normalize(view * vec4(lightDir, 0.0f)));

    //compute view direction (in eye coordinates, the viewer is situated at the origin
    vec3 viewDir = normalize(- fPosEye.xyz);

    //compute ambient light
    ambient = ambientStrength * lightColor;

    //compute diffuse light
    diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;

    //compute specular light
    vec3 reflectDir = normalize(reflect(-lightDirN, normalEye));
    float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), 15);
    specular = specularStrength * specCoeff * lightColor;
}

void calcPointLights(){
	
	for(int i=0; i<20; i++){

		int j = i;
		//transform normal
		vec3 normalEye = normalize(fNormal);	

		vec4 lightPosEye1 = view * model * vec4(positions[j], 1.0f);
		vec3 lightPosEye = lightPosEye1.xyz;

		//compute light direction
		vec3 lightDirN = normalize(lightPosEye - fPosEye.xyz);
	
		//compute view direction 
		vec3 viewDirN = normalize(-fPosEye.xyz);
		vec3 reflection = normalize(reflect(lightDirN, normalEye));
		float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
		
		float distance = length(lightPosEye - fPosEye.xyz); 
		float attenuation = 1.0 / (constant + linear * distance + 
  				     quadratic * (distance * distance));
					 
		vec3 ambientp = attenuation * ambientStrength *lampColor;
		vec3 diffusep = attenuation * max(dot(normalEye, lightDirN), 0.0f) * lampColor;
		vec3 specularp = attenuation * specularStrength * specCoeff * lampColor;
		
		ambientp *= texture(diffuseTexture, fTexCoords);
		diffusep *= texture(diffuseTexture, fTexCoords);
		specularp *= texture(specularTexture, fTexCoords);
	
		vec3 colorPointNew = min((ambient + diffuse) + specular, 1.0f);
		colorPoint = colorPoint + colorPointNew;
		ambient1 += ambientp;
		diffuse1 += diffusep;
		specular1 += specularp;

	}
}

float computeShadow()
{
	vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	normalizedCoords = normalizedCoords * 0.5 + 0.5;
	float closestDepth = texture(shadowMap, normalizedCoords.xy).r;
	float currentDepth = normalizedCoords.z;
	float bias = max(0.05f * (1.0f - dot(fNormal, lightDir)), 0.005f);
        float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;

	if (normalizedCoords.z > 1.0f)
		return 0.0f;

	return shadow;

}

float computeFog()
{
 
 	float fragmentDistance = length(fPosEye);
 	float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

 	return clamp(fogFactor, 0.0f, 1.0f);
}

void main() 
{	
	ambient = vec3(0.0f);
	diffuse = vec3(0.0f);
	specular = vec3(0.0f);
    if(day == 1){
		computeDirLight();
		ambient *= texture(diffuseTexture, fTexCoords).rgb;
		diffuse *= texture(diffuseTexture, fTexCoords).rgb;
		specular *= texture(specularTexture, fTexCoords).rgb;
	}
	
    	calcPointLights();
		ambient += ambient1;
		diffuse += diffuse1;
		specular += specular1;
	

    //compute final vertex color
    	vec3 color = min((ambient + diffuse) + specular , 1.0f);
	if(day == 1){
		float shadow = computeShadow();
		color = min((ambient + (1.0f - shadow)*diffuse) + (1.0f - shadow)*specular, 1.0f);
	}
	

	float fogFactor = computeFog();
	vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
	
	fColor = mix(fogColor, vec4(color, 1.0f), fogFactor);

    //fColor = vec4(color, 1.0f);
}
