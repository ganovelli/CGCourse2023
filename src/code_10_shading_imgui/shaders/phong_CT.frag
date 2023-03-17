#version 330 core  
out vec4 color; 
in vec3 vColor;
in vec3 vNormalVS;
in vec3 vLdirVS;
in vec3 vposVS; 
uniform int uShadingMode;
uniform vec3 uDiffuseColor;
uniform float uEta_2;
uniform float uAvgSlope;


/* phong */
vec3 phong ( vec3 L, vec3 pos, vec3 N){
	float LN = max(0.0,dot(L,N));

	vec3 R = -L+2*dot(L,N)*N;
	float spec = max(0.0,pow(dot(normalize(-pos),R),10));

	return LN*uDiffuseColor + spec * uDiffuseColor*vec3(0.2,0.2,0.8);
}
/* cook torrance */
vec3 cook_torrance ( vec3 L, vec3 pos, vec3 N){
	vec3 V = normalize(-pos);
	 
	float eta_1 = 1.0;
	float eta_2 = uEta_2;

	vec3 H = normalize(L+normalize(-pos));
	float alpha = acos(dot(H,N));
	float m_square = uAvgSlope*uAvgSlope;
	float D = 1.0/ (m_square*pow(cos(alpha),4.0)) * exp(-tan(alpha)*tan(alpha)/m_square);

	float projection_term = 2*dot(N,H)/dot(N,L);
	float G1 = projection_term*dot(N,L);
	float G2 = projection_term*dot(N,V);
	float G = min(1.0,min(G1,G2));

	float R0 = pow( (eta_1-eta_2)/(eta_1+eta_2),2.0);
	float F = R0+(1-R0)*pow(1-cos(alpha),5.0);

	return F*uDiffuseColor;
	return D/(dot(N,L)*dot(N,V))*uDiffuseColor;
//	return D*G*F/(dot(N,L)*dot(N,V))*uDiffuseColor;
}

void main(void) 
{ 
	
	if(uShadingMode == 2){
		color = vec4(phong(vLdirVS,normalize(vposVS),normalize(vNormalVS)),1.0);
	}
	else
	if(uShadingMode == 3){
		vec3 N = normalize(cross(dFdx(vposVS),dFdy(vposVS)));
		color = vec4(phong(vLdirVS,normalize(vposVS),N),1.0);
	}
	else
	if(uShadingMode == 4){
		color = vec4(cook_torrance(vLdirVS,vposVS,normalize(vNormalVS)),1.0);
	}else
	color = vec4(vColor,1.0);
} 