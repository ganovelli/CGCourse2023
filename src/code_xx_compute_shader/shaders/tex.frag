#version 460 core
out vec4 FragColor;
	
in vec2 TexCoords;
out vec4 color;	
uniform sampler2D tex;
	
void main()
{             
    vec4 texCol = texture(tex, TexCoords).rgba;      
    color = vec4(sqrt(texCol.rgb/texCol.a), 1.0);

}