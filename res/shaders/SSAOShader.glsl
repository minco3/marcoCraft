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
uniform sampler2D depthTextureSlot;

float near = 0.1f;
float far = 100.0f;

float linearizeDepth(float depth)
{
    return (2.0 * near * far) / (far + near - (depth * 2.0 - 1.0) * (far - near));
}

vec4 fogColor = vec4(1.0, 1.0, 1.0, 1.0);

void main()
{
    vec4 depthValue = texture(depthTextureSlot, textureCoords);
    float depth = depthValue.r;
    float linearDepth = (linearizeDepth(depth) - near) / (far - near);
    // float smoothDepth = smoothstep(0.0, 1.0, linearDepth);
    color = texture(textureSlot, textureCoords);
}