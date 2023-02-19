#shader vertex
#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 coords;

out vec2 textureXY;
flat out float textureZ;
out vec3 fragmentColor;
uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(vertexPosition_modelspace, 1);
    textureXY = vec2(coords.x, coords.y);
    textureZ = coords.z;
    fragmentColor = color;
}

#shader fragment
#version 330 core

in vec2 textureXY;
flat in float textureZ;
in vec3 fragmentColor;
out vec4 color;

uniform sampler2DArray textureSlot;

vec3 textureCoords = vec3(textureXY, textureZ);

void main()
{
    vec4 overlay;
    if (textureCoords.z > 1.5 && textureCoords.z < 2.5)
    {
        overlay = texture(textureSlot, vec3(textureCoords.x, textureCoords.y, 4.0));
        color = overlay.a == 0.0 ? vec4(texture(textureSlot, textureCoords)) : vec4(fragmentColor, 1.0) * overlay;
    }
    else
    {
        color = vec4(fragmentColor, 1.0) * texture(textureSlot, textureCoords);
    }
}