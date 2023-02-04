#shader vertex
#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 coords;

out textureCoords;
uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(vertexPosition_modelspace, 1);
    textureCoords = coords;
}

#shader fragment
#version 330 core
in vec2 textureCoords;
out vec4 color;
uniform sampler2D textureSlot;
void main()
{
    color = texture(textureSlot, textureCoords);
}