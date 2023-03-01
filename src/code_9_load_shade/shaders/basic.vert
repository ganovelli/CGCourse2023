#version 330 core 
layout (location = 0) in vec3 aPosition; 
layout (location = 1) in vec3 aColor; 
layout (location = 2) in vec3 aNormal;
 
out vec3 vColor;
out vec3 vVSPos;
out vec3 vNormal;

uniform mat4 uP;
uniform mat4 uV;
uniform mat4 uT;

void main(void) 
{ 
	vNormal = aNormal;
	vColor = aColor;
    gl_Position = uP*uV*uT*vec4(aPosition, 1.0); 
	vVSPos = (uV*uT*vec4(aPosition, 1.0)).xyz;
}
