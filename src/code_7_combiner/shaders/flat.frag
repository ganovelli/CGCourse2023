#version 330 core  
out vec4 color; 
in vec3 vColor;
in vec3 vVSPos;
uniform vec4 uColor;

void main(void) 
{ 
	color = uColor; 
} 