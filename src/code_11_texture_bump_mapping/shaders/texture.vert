#version 330 core 
layout (location = 0) in vec3 aPosition; 
layout (location = 1) in vec3 aColor; 
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aTexCoord;
layout (location = 4) in vec3 aTangent;
 
out vec2 vTexCoord;
out vec3 vLdirVS;
out vec3 vLdirTS;
out vec3 vVdirTS;

uniform mat4 uP;
uniform mat4 uV;
uniform mat4 uT;
uniform vec4 uLdir;
uniform int uRenderMode;


void main(void) 
{ 
    // computing the tangent frame
    vec3 tangent = normalize(aTangent);
    vec3 bitangent = normalize(cross(aNormal,tangent));
	
	mat3 TF;

	TF[0] = tangent;
	TF[1] = bitangent;
	TF[2] = normalize(aNormal);
	TF = transpose(TF);

	vLdirTS   =    TF * (inverse(uT)*uLdir).xyz;

	vec3 ViewVS  =  (vec4(0.0,0.0,0.0,1.0) -uV*uT*vec4(aPosition, 1.0)).xyz; 

	vVdirTS	  =    TF * (inverse(uT)*inverse(uV)* vec4(ViewVS,0.0)).xyz;

	vLdirVS   = (uV*uLdir).xyz;
	vTexCoord = aTexCoord*vec2(1.0,1.0);
    gl_Position = uP*uV*uT*vec4(aPosition, 1.0); 
}
