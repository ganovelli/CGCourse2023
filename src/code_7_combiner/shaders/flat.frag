#version 330 core  
out vec4 color; 
in vec3 vColor;
in vec3 vVSPos;
uniform vec3 uColor;

void main(void) 
{ 
	color = vec4(1.0,1.0,1.0+0.001*uColor.x, 1.0); 
} 