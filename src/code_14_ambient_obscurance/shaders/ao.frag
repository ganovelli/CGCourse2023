#version 330 core  
out vec4 color; 

in vec4 vCoordLS;
in vec3 vNormalWS;
in vec3 vVWS;
in vec3 vLWS;

uniform int uRenderMode;
uniform vec3 uDiffuseColor;

uniform sampler2D  uShadowMap;
uniform ivec2 uShadowMapSize;
uniform float uBias;

// this produce the Hue for v:0..1 (for debug purposes)
vec3 hsv2rgb(float  v)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(vec3(v,v,v) + K.xyz) * 6.0 - K.www);
    return   mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0),1.0);
}
float linstep(float low, float high, float v){
    return clamp((v-low)/(high-low), 0.0, 1.0);
}
 	
void main(void) 
{	
	vec3 N = normalize(vNormalWS);
	vec3 L = normalize(vLWS);
	vec3 V = normalize(vVWS);

	vec3 shaded = (vec3(max(0.0,dot(L,N))) +vec3(0.6,0.6,0.6));
	color = vec4(shaded,1.0);
} 