#version 330 core  
layout (location = 0) out vec4 color; 
layout (location = 1) out vec4 normal;

in vec3 vNVS;

// this produce the Hue for v:0..1 (for debug purposes)
vec3 hsv2rgb(float  v)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(vec3(v,v,v) + K.xyz) * 6.0 - K.www);
    return   mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0),1.0);
}


void main(void) 
{ 
 	color = vec4(gl_FragCoord.z,0.0,0.0,1.0);
 	normal = vec4(normalize(vNVS)*0.5+0.5,1.0);
} 