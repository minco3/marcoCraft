#shader vertex
#version 330

layout(location = 0) in vec2 gPosition;
layout(location = 1) in vec2 gTexCoords;

out vec2 fragTexCoords;

void main()
{
    fragTexCoords = gTexCoords;
    gl_Position = vec4(gPosition, 0.0, 1.0);
}

#shader fragment
#version 330

in vec2 fragTexCoords;
out float color;

uniform sampler2D ssaoTexture;

void main()
{
    color = texture(ssaoTexture, fragTexCoords).r;
}