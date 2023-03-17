#version 330 core  
out vec4 color; 

in vec3 vColor;

in vec3 vNormalVS;
in vec3 vPosVS;
in vec3 vLdirVS;
in vec3 vVdirVS;


uniform vec3 uDiffuseColor;
uniform int uShadingMode;

void main(void) 
{ 
	if(uShadingMode > 1){
		vec3 N;
		if(uShadingMode ==2) 
				N = normalize(vNormalVS);
			else
 				N = normalize(cross(dFdx(vPosVS),dFdy(vPosVS)));

		vec3 L = normalize(vLdirVS);
		vec3 V = normalize(vVdirVS);

		float LN = max(0.0,dot(L,N));
		vec3 diffuse =  LN*uDiffuseColor;
		
		vec3 R = -L+2*dot(N,L)*N;
		float VR = max(0.0,pow(dot(V,R),10));
		vec3 uSpecularColor = uDiffuseColor*vec3(0.2,0.2,1.0);
		vec3 specular = VR *uSpecularColor;
		color = vec4(diffuse+specular,1.0);
	}else
	color = vec4(vColor,1.0);
} 