#shader vertex
#version 330 core

layout (location = 0) in vec3 vertexPosition;

uniform mat4 LightSpaceMatrix;


void main()
{
    gl_Position = LightSpaceMatrix * vec4(vertexPosition, 1.0);
}

#shader fragment
#version 330 core

void main()
{
    // left empty on purpose
}