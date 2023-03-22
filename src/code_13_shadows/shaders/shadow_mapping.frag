#version 330 core  
out vec4 color; 

in vec4 vCoordLS;
in vec3 vNormalVS;
in vec3 vNormalWS;

uniform int uRenderMode;

uniform sampler2D  uTextureImage;


// this produce the Hue for v:0..1 (for debug purposes)
vec3 hsv2rgb(float  v)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(vec3(v,v,v) + K.xyz) * 6.0 - K.www);
    return   mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0),1.0);
}

void main(void) 
{ 
	if(uRenderMode==0) // just diffuse gray
		//color = vec4(vec3(max(0.0,dot(normalize(vLdirVS),normalize(vNormalVS)))),1.0);
		color = vec4(1.0,0.0,1.0,1.0);
		else
	if(uRenderMode==1) // basic shadow
	{
		color = texture(uTextureImage, (vCoordLS/vCoordLS.w).xy *0.5+0.5);
	}
} 