#version 330 core 
layout (location = 0) in vec3 aPosition; 
layout (location = 1) in vec3 aColor; 
layout (location = 2) in vec3 aNormal;
 
out vec3 vColor;
out vec3 vNormalVS;
out vec3 vPosVS;
out vec3 vLdirVS;
out vec3 vVdirVS;

uniform mat4 uP;
uniform mat4 uV;
uniform mat4 uT;
uniform vec4 uLdir;
uniform vec3 uDiffuseColor;


uniform int uShadingMode;

void main(void) 
{ 
	/* express normal, light direction and position in view space */
	vec3 NormalVS = normalize((uV*uT*vec4(aNormal,0.0)).xyz);
	vec3 LdirVS   = (uV*uLdir).xyz;
	vec3 posVS    = (uV*uT*vec4(aPosition, 1.0)).xyz;
	vec3 VdirVS   = - normalize(posVS);

	if(uShadingMode==0){
		vColor = uDiffuseColor;
	}
	else
	if(uShadingMode==1){
		float LN = max(0.0,dot(LdirVS,NormalVS));
		vec3 diffuse =  LN*uDiffuseColor;
		
		vec3 R = -LdirVS+2*dot(NormalVS,LdirVS)*NormalVS;
		float VR = max(0.0,pow(dot(VdirVS,R),10));
		vec3 uSpecularColor = uDiffuseColor*vec3(0.2,0.2,1.0);
		vec3 specular = VR *uSpecularColor;

		vColor = diffuse+specular;
	}
	else{
		vNormalVS	= NormalVS;
		vPosVS		= posVS;
		vLdirVS		= LdirVS;
		vVdirVS		= VdirVS;
	}


    gl_Position = uP*uV*uT*vec4(aPosition, 1.0); 
}
