#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
// instance pos may or may not be used (for example for grid)
//layout (location = 2) in vec3 aInstancePos;


uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;



out vec3 ObjectColor;

void main()
{   
    ObjectColor = aColor;
    
    //gl_Position = projection *  view * model * vec4(aPos+aInstancePos, 1.0);
    gl_Position = projection * model *  vec4 (aPos, 1.0);
    //gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
} 
 
