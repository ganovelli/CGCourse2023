#version 330 core  
out vec4 color; 

in vec2 vTexCoord;

uniform sampler2D uDepthMap;
uniform float uRadius;
uniform float  uDepthScale;

vec3 hash32(vec2 p)
{
	vec3 p3 = fract(vec3(p.xyx) * vec3(.1031, .1030, .0973));
  	p3 += dot(p3, p3.yxz+33.33);
  	return fract((p3.xxy+p3.yzz)*p3.zyx)*2.0-1.0;
}

bool test_sample(vec3 sample){
	float z = texture2D(uDepthMap,sample.xy).x;
	return   z > sample.z;
}

void main(void) 
{ 
	int n_samples = 256;
	float ao = 0.0;
			
	vec3 center = vec3(vTexCoord,texture2D(uDepthMap,vTexCoord).x);
 	if(center.z>0.99 ){
 	ao=1.0;}
 	else{ 
		vec3 h = hash32(gl_FragCoord.xy);
		vec3 sample;// = center + h*uRadius/512.0;
		for(int i=0; i < n_samples; ++i)
			{
				sample  = center + h*vec3(uRadius*vec2(1.f/512.f),uDepthScale);
				float z = texture2D(uDepthMap,sample.xy).x;
				if( test_sample(sample))
					ao+=1.0 / float(n_samples);
				h = hash32(h.xy*vec2(512.0,512.0));
			}	
		ao =clamp(ao*2.0,0.0,1.0);
	}
 	
	color = vec4(vec3(ao) ,1.0);

	
 //	 color =  vec4(gl_FragCoord.xyz/512.0,1.0);
// color = vec4(texture2D(uDepthMap,vTexCoord).xyz,1.0);
 //		color = vec4(hash32(gl_FragCoord.xy ) ,1.0);

}