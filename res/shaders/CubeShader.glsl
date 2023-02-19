#shader vertex
#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 coords;

out vec2 textureXY;
flat out float textureZ;
uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(vertexPosition_modelspace, 1);
    textureXY = vec2(coords.x, coords.y);
    textureZ = coords.z;
}

#shader fragment
#version 330 core

in vec2 textureXY;
flat in float textureZ;
out vec4 color;

vec3 textureCoords = vec3(textureXY, textureZ);

uniform sampler2DArray textureSlot;

void main()
{
    color = texture(textureSlot, textureCoords);
}