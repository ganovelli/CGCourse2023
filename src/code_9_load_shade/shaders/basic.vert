#version 330 core 
layout (location = 0) in vec3 aPosition; 
layout (location = 1) in vec3 aColor; 
layout (location = 2) in vec3 aNormal;
 
out vec3 vColor;

uniform mat4 uP;
uniform mat4 uV;
uniform mat4 uT;
uniform vec4 uLdir;
uniform vec3 uDiffuseColor;

/* basic diffuse component */
vec3 diffuse ( vec3 L, vec3 pos, vec3 N){
	float LN = max(0.0,dot(L,N));
	return LN*uDiffuseColor;
}

void main(void) 
{ 
	/* lazy trick.  if uDiffuseColor.x is negative, it means I don't want to compute the diffuse lighting, just 
	pass the per-vertex color attribute to the fragment shader */
	if(uDiffuseColor.x >= 0){ 
		/* express normal, light directoin and position in view space */
		vec3 NormalVS= normalize((uV*uT*vec4(aNormal,0.0)).xyz);
		vec3 LdirVS = (uV*uLdir).xyz;
		vec3 posVS = (uV*uT*vec4(aPosition, 1.0)).xyz;

		/* compute the diffuse color  */
		vColor = diffuse(LdirVS,posVS,NormalVS);
		}
		else
	vColor = aColor;

    gl_Position = uP*uV*uT*vec4(aPosition, 1.0); 
}
