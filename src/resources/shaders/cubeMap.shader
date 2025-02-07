#Shader Vertex
#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
} 

#Shader Fragment
#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube CubeMap;

void main()
{    
    FragColor = texture(CubeMap, TexCoords);
}