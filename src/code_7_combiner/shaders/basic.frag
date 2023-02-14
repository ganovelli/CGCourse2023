#version 330 core  
out vec4 color; 
in vec3 vColor;
in vec3 vVSPos;
uniform vec3 uColor;

float flat_shading(vec3 lPos){
	vec3 N =  normalize(cross(dFdx(vVSPos),dFdy(vVSPos)));
	return dot(normalize(lPos-vVSPos),N);
}

void main(void) 
{ 
	/*	this is a lazy hack: if uColor.x is positive then use it,
		otherwise use the color interpolated from the vertices */
	if(uColor.x >= 0.0){ 
			color = vec4(uColor, 1.0); 
			color = (flat_shading(vec3(0,10,0))+flat_shading(vec3(0,10,0))+flat_shading(vec3(0,10,0))+flat_shading(vec3(0,-10,0)))*0.25*color;
		}
		else
			color = vec4(vColor, 1.0); 
} 