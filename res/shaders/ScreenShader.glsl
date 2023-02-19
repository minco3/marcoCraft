#shader vertex
#version 330

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 coords;

out vec2 textureCoords;

void main()
{
    textureCoords = coords;
    gl_Position = vec4(position, 0.0, 1.0);
}

#shader fragment
#version 330

in vec2 textureCoords;
out vec4 color;

uniform sampler2D textureSlot;

void main()
{
    color = texture(textureSlot, textureCoords);
}