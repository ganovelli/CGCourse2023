#version 460 core
layout (location = 0) in vec3 aPos;
	
uniform mat4 uT;
out vec2 TexCoords;
	
void main()
{
    TexCoords = ((uT*vec4(aPos, 1.0)).xy+vec2(1.f))*0.5;
    gl_Position = uT*vec4(aPos, 1.0);
}