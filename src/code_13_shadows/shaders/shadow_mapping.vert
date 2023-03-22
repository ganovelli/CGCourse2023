#version 330 core 
layout (location = 0) in vec3 aPosition; 
layout (location = 2) in vec3 aNormal;
 
out vec4 vCoordLS;
out vec3 vNormalVS;
out vec3 vNormalWS;

uniform mat4 uP;
uniform mat4 uV;
uniform mat4 uT;
uniform mat4	 uLightMatrix;

uniform int uRenderMode;

void main(void) 
{ 
    gl_Position = uP*uV*uT*vec4(aPosition, 1.0); 
	
	vNormalVS = normalize((uV*uT*vec4(aNormal,0.0)).xyz);
	vNormalWS = normalize(( uT*vec4(aNormal,0.0)).xyz);
	vCoordLS =  uLightMatrix*uT*vec4(aPosition, 1.0);
}
