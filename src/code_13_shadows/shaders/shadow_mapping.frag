#version 330 core  
out vec4 color; 

in vec4 vCoordLS;
in vec3 vNormalVS;
in vec3 vNormalWS;
in vec3 vLWS;

uniform int uRenderMode;

uniform sampler2D  uTextureImage;
uniform float uBias;

// this produce the Hue for v:0..1 (for debug purposes)
vec3 hsv2rgb(float  v)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(vec3(v,v,v) + K.xyz) * 6.0 - K.www);
    return   mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0),1.0);
}

void main(void) 
{	
	vec3 N = normalize(vNormalWS);
	vec3 L = normalize(vLWS);

	vec3 shaded = vec3(max(0.0,dot(vec3(0,1,0),normalize(vNormalVS))));
	if(uRenderMode==0) // just diffuse gray
		//color = vec4(vec3(max(0.0,dot(normalize(vLdirVS),normalize(vNormalVS)))),1.0);
		color = vec4(1.0,0.0,1.0,1.0);
		else
	if(uRenderMode==1) // basic shadow
	{
		vec4 pLS = (vCoordLS/vCoordLS.w)*0.5+0.5;
		float depth = texture(uTextureImage,pLS.xy).x;
		if(depth < pLS.z )
			shaded*=0.5;
         color = vec4(shaded,1.0);
	}
	else
	if(uRenderMode==2) // bias
	{
		vec4 pLS = (vCoordLS/vCoordLS.w)*0.5+0.5;
		float depth = texture(uTextureImage,pLS.xy).x;
		if(depth + uBias < pLS.z )
			shaded*=0.5;
         color = vec4(shaded,1.0);
	}else
	if(uRenderMode==3) // slope bias
	{
		float bias = clamp(uBias*tan(acos(dot(N,L))),0.0,1.05); 
		vec4 pLS = (vCoordLS/vCoordLS.w)*0.5+0.5;
		float depth = texture(uTextureImage,pLS.xy).x;
		if(depth + bias < pLS.z )
			shaded*=0.5;
         color = vec4(shaded,1.0);
	}else
	if(uRenderMode==4) // backfaces for watertight objects
	{
		vec4 pLS = (vCoordLS/vCoordLS.w)*0.5+0.5;
		float depth = texture(uTextureImage,pLS.xy).x;
		if(depth + uBias < pLS.z || dot(N,L)< 0.f)
			shaded*=0.5;
         color = vec4(shaded,1.0);
	}

} 