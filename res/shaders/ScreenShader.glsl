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

const float offset = 1.0 / 600.0;

void main()
{

    // float kernel[9] = float[](
    //     -1, -1, -1,
    //     -1,  9, -1,
    //     -1, -1, -1
    // );
    // float kernel[9] = float[](
    //     1.0/16, 2.0/16, 1.0/16,
    //     2.0/16, 4.0/16, 2.0/16,
    //     1.0/16, 2.0/16, 1.0/16
    // );
    
    // vec4 col = vec4(0.0);
    // col += kernel[0] * texture(textureSlot, textureCoords + vec2(-offset, offset )); // top left
    // col += kernel[1] * texture(textureSlot, textureCoords + vec2(0.0f,    offset )); // top center
    // col += kernel[2] * texture(textureSlot, textureCoords + vec2(offset,  offset )); // top right
    // col += kernel[3] * texture(textureSlot, textureCoords + vec2(-offset, 0.0f   )); // left
    // col += kernel[4] * texture(textureSlot, textureCoords + vec2(0.0f,    0.0f   )); // center
    // col += kernel[5] * texture(textureSlot, textureCoords + vec2(offset,  0.0f   )); // right
    // col += kernel[6] * texture(textureSlot, textureCoords + vec2(-offset, -offset)); // bottom left
    // col += kernel[7] * texture(textureSlot, textureCoords + vec2(0.0f,    -offset)); // bottom center
    // col += kernel[8] * texture(textureSlot, textureCoords + vec2(offset,  -offset)); // bottom right
    // color = col;

    vec4 depthValue = texture(depthTextureSlot, textureCoords);
    float depth = depthValue.r;
    float linearDepth = (linearizeDepth(depth) - near) / (far - near);
    float smoothDepth = smoothstep(0.0, 1.0, linearDepth);
    color = mix(texture(textureSlot, textureCoords), fogColor, smoothDepth);

}