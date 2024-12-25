#shader vertex
#version 330

layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec2 vertexCoords;

out vec2 texCoords;

void main()
{
    texCoords = vertexCoords;
    gl_Position = vec4(vertexPosition, 0.0, 1.0);
}

#shader fragment
#version 330

in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D buffer;

float near = 0.1f;
float far = 1000.0f;

float linearizeDepth(float depth)
{
    return (2.0 * near * far) / (far + near - (depth * 2.0 - 1.0) * (far - near));
}

void main()
{
    float depth = texture(buffer, texCoords).r;
    float linearDepth = (linearizeDepth(depth) - near) / (far - near);
    fragColor = vec4(vec3(
       min(linearDepth*5, 1)), 1.0);

    fragColor = vec4(texture(buffer, texCoords).b);
}