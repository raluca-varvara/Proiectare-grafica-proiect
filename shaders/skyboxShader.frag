#version 410 core

in vec3 textureCoordinates;
in vec4 fPosEyes;
out vec4 color;

uniform samplerCube skybox;
uniform float fogDensity;

float computeFog()
{
 
 float fragmentDistance = length(fPosEyes);
 fragmentDistance = 100.0f;
 float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

 return clamp(fogFactor, 0.0f, 1.0f);
}

void main()
{
    vec4 color1 = texture(skybox, textureCoordinates);

    float fogFactor = computeFog();
    vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
	
    color = mix(fogColor, color1, fogFactor);
    //color = color1;
}
