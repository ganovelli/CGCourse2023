#version 330 core 
layout (location = 0) in vec3 aPosition; 
layout (location = 1) in vec3 aColor; 
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aTexCoord;
 
out vec2 vTexCoord;

uniform mat4 uP;
uniform mat4 uV;
uniform mat4 uT;
uniform int uShadingMode;


void main(void) 
{ 
	vTexCoord = aTexCoord;
    gl_Position = uP*uV*uT*vec4(aPosition, 1.0); 
}
