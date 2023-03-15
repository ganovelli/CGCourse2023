#version 330 core  
out vec4 color; 

in vec2 vTexCoord;
uniform int uShadingMode;
uniform vec3 uDiffuseColor;

uniform sampler2D uTextureImage;


// this produce the Hue for v:0..1 (for debug purposes)
vec3 hsv2rgb(float  v)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(vec3(v,v,v) + K.xyz) * 6.0 - K.www);
    return   mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0),1.0);
}

void main(void) 
{ 
	if(uShadingMode==0)
	color = vec4(vTexCoord,0.0,1.0);
	else
	if(uShadingMode==1)
		color = texture2D(uTextureImage,vec2(vTexCoord.x,vTexCoord.y));
	if(uShadingMode==2){
		float rho = 512.f*max(length(dFdx(vTexCoord)),length(dFdy(vTexCoord)));
		float level = floor( log(rho)/log(2.f));
		color = texture2D(uTextureImage,vec2(vTexCoord.x,vTexCoord.y)) * 0.5+
				vec4(hsv2rgb(level/9.f)* 0.5,1.0);		
	}
} 