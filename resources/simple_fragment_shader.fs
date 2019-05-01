#version 330 core
in vec3 ObjectColor;

out vec4 FragColor;


void main()
{
    FragColor = vec4(ObjectColor, 1.0);
    //FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}  
 
