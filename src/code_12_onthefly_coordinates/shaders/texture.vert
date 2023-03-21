#version 330 core 
layout (location = 0) in vec3 aPosition; 
layout (location = 1) in vec3 aColor; 
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aTexCoord;
layout (location = 4) in vec3 aTangent;
 
out vec2 vTexCoord;
out vec4 vProjTexCoord;
out vec4 vSkyboxTexCoord;
out vec3 vLdirVS;
out vec3 vNormalVS;
out vec3 vNormalWS;

uniform mat4 uP;
uniform mat4 uV;
uniform mat4 uT;
uniform vec4 uLdir;

uniform mat4 uLPView;
uniform mat4 uLPProj;

uniform int uRenderMode;


void main(void) 
{ 
	vec3 ViewVS  =  (vec4(0.0,0.0,0.0,1.0) -uV*uT*vec4(aPosition, 1.0)).xyz; 

	

	vLdirVS   = (uV*uLdir).xyz;
	vNormalVS = normalize((uV*uT*vec4(aNormal,0.0)).xyz);
	vNormalWS = normalize(( uT*vec4(aNormal,0.0)).xyz);

	vTexCoord = aTexCoord*vec2(1.0,1.0);
    gl_Position = uP*uV*uT*vec4(aPosition, 1.0); 
	
	vProjTexCoord  = uLPProj* uLPView*uT*vec4(aPosition, 1.0);
	vSkyboxTexCoord =  inverse(uV)*(uV*uT*vec4(aPosition, 1.0)-vec4(0,0,0,1.0));
}
