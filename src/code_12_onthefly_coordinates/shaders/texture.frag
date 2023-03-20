#version 330 core  
out vec4 color; 

in vec2 vTexCoord;
in vec4 vProjTexCoord;
in vec4 vSkyboxTexCoord;
in vec3 vLdirVS;
in vec3 vNormalVS;
in vec3 vNormalWS;
in vec3 vLdirTS;
in vec3 vVdirTS;

uniform int uRenderMode;
uniform vec3 uDiffuseColor;

uniform sampler2D uTextureImage;
uniform sampler2D uBumpmapImage;
uniform sampler2D uNormalmapImage;
uniform samplerCube uSkybox;
uniform samplerCube uReflectionMap;

uniform mat4 uV;
uniform mat4 uT;

/* Diffuse */
vec3 diffuse( vec3 L, vec3 N){
	return  max(0.0,dot(L,N))*texture2D(uTextureImage,vTexCoord.xy).xyz;
}

// this produce the Hue for v:0..1 (for debug purposes)
vec3 hsv2rgb(float  v)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(vec3(v,v,v) + K.xyz) * 6.0 - K.www);
    return   mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0),1.0);
}

void main(void) 
{ 
	if(uRenderMode==0)
	color = vec4(vTexCoord,0.0,1.0);
	else
	if(uRenderMode==1){ 
		vec2 tc = ( (vProjTexCoord/vProjTexCoord.w).xy *0.5+1.0) ;
		vec4 c = texture2D(uTextureImage,tc); 
		vec3 cc =  vec3(max(0.0,dot(normalize(vLdirVS),normalize(vNormalVS)))) + c.xyz*c.w; 
		color = vec4(cc,1.0);
		}else
	if(uRenderMode==2){ 
		color = texture(uSkybox,normalize(vSkyboxTexCoord.xyz)); 
		//color = vec4(normalize(vSkyboxTexCoord.xyz)*0.5+1.0,1.0);
		}else
	if(uRenderMode == 3){
		vec3 r = reflect(normalize(vSkyboxTexCoord.xyz),normalize(vNormalWS));
		color = texture(uSkybox,r); 
	}else
	if(uRenderMode == 4){
		vec3 r = refract(normalize(vSkyboxTexCoord.xyz),normalize(vNormalWS),1.01);
		color = texture(uSkybox,r); 
	}
	if(uRenderMode == 5){
		vec3 r = reflect(normalize(vSkyboxTexCoord.xyz),normalize(vNormalWS));
		//color = texture(uReflectionMap,normalize(vNormalWS)); 
		color = texture(uReflectionMap,r); 
	}
} 