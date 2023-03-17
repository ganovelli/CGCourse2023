#version 460 core
out vec4 FragColor;
	
in vec2 TexCoords;
out vec4 color;	
uniform sampler2D tex;
	
void main()
{             
    vec3 texCol = texture(tex, TexCoords).rgb;      
    color = vec4(texCol, 1.0);

}