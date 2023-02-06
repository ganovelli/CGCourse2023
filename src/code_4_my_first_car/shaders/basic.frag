#version 330 core  
out vec4 color; 
in vec3 vColor;
uniform vec3 uColor;
void main(void) 
{ 
	/*	this is a lazy hack: if uColor.x is positive then use it,
		otherwise use the color interpolated from the vertices */
	if(uColor.x >= 0.0)
	color = vec4(uColor, 1.0); 
		else
	color = vec4(vColor, 1.0); 

} 