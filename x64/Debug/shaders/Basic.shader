
#Shader Vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
out vec3 initcolor;
void main()
{
   gl_Position = vec4(aPos.x+0.2f,aPos.y+0.5,aPos.z, 1.0);
   initcolor = aColor;
};


#Shader Fragment
#version 330 core
out vec4 FragColor;
in vec3 initcolor;
uniform vec4 ourColor;
void main()
{
   FragColor = vec4(initcolor,1.0)*ourColor;
};