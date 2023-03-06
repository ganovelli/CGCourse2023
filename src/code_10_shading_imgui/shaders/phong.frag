#version 330 core  
out vec4 color; 
in vec3 vColor;
in vec3 vNormalVS;
in vec3 vLdirVS;
in vec3 vposVS; 
uniform int uShadingMode;
uniform vec3 uDiffuseColor;


/* Phong */
vec3 phong ( vec3 L, vec3 pos, vec3 N){
	vec3 V = normalize(-pos);
	float LN = max(0.0,dot(L,N));

	vec3 R = -L+2*dot(L,N)*N;
	float spec = max(0.0,pow(dot(V,R),10));

	return LN*uDiffuseColor + spec * uDiffuseColor*vec3(0.2,0.2,0.8);
}

void main(void) 
{ 
	
	if(uShadingMode == 2){
		color = vec4(phong(vLdirVS,normalize(vposVS),normalize(vNormalVS)),1.0);
	}
	else
	if(uShadingMode == 3){
		vec3 N = normalize(cross(dFdx(vposVS),dFdy(vposVS)));
		color = vec4(phong(vLdirVS,vposVS,N),1.0);
	}
	else
	color = vec4(vColor,1.0);
} 