#shader vertex
#version 330 core

layout (location = 0) in vec2 position;
out vec2 TexCoords;

uniform mat4 projection;
uniform vec2 coords;

void main()
{
    gl_Position = projection * vec4(position, 0.0, 1.0);
    TexCoords = coords;
}


#shader fragment
#version 330 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    color = vec4(textColor, 1.0) * sampled;    
}