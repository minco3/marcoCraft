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
    vec2 texelSize = 1.0 / vec2(textureSize(ssaoTexture, 0));
    float result = 0.0;
    for (int x = -2; x<2; x++)
    {
        for (int y = -2; y<2; y++)
        {
            vec2 offset = vec2(x, y) * texelSize;
            result += texture(ssaoTexture, fragTexCoords + offset).r;
        }
    }

    color = result / (4.0 * 4.0);
}