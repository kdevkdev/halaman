#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec3 aNormal;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 inverseModel;

out vec3 Normal;
out vec3 FragPos;

void main()
{
    // multiplying a vector from the left to a matrix corresponds to multiplying it from the right to the transposed matrix (which is the aim here)
    //Normal = aNormal*mat3(inverseModel);
    
    //FragPos = vec3(model * vec4(aPos, 1.0));
    
    gl_Position = projection * view * model * vec4(aPos, 1.0);
} 
