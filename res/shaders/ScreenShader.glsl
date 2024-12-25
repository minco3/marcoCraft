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

uniform sampler2D gAlbedo;
uniform sampler2D gNormal;
uniform sampler2D gPosition;
uniform sampler2D gDepth;
uniform sampler2D ssaoTexture;
uniform sampler2D shadowTexture;

uniform mat4 lightMVP;
uniform mat4 inverseMVP;
uniform vec3 lightPos;
uniform bool occlusion;

float near = 0.1f;
float far = 1000.0f;

float linearizeDepth(float depth)
{
    return (2.0 * near * far) / (far + near - (depth * 2.0 - 1.0) * (far - near));
}

float normalDepth(float depth)
{
    return (linearizeDepth(depth) - near) / (far - near);
}

vec4 fogColor = vec4(1.0, 1.0, 1.0, 1.0);

const float offset = 1.0 / 600.0;

vec3 lightColor = vec3(1.0, 1.0, 1.0);

void main()
{
    vec3 FragPos = texture(gPosition, texCoords).rgb;
    vec3 Normal = texture(gNormal, texCoords).rgb * 2.0 - 1.0;
    vec3 Diffuse = texture(gAlbedo, texCoords).rgb;
    float Depth = texture(gDepth, texCoords).r;

    vec4 clipSpacePos = vec4(
        texCoords * 2.0 - 1.0, // Convert from [0, 1] to NDC [-1, 1]
        Depth * 2.0 - 1.0,        // Depth in NDC [-1, 1]
        1.0
    );

    vec4 worldSpacePos = inverseMVP * clipSpacePos;
    worldSpacePos /= worldSpacePos.w;

    vec4 shadowFragPos = lightMVP * worldSpacePos;
    shadowFragPos.xyz /= shadowFragPos.w;
    shadowFragPos.xyz = (shadowFragPos.xyz + 1.0)/ 2.0;

    float shadowDepth = texture(shadowTexture, shadowFragPos.xy).r;

    float AmbientOcclusion = pow(texture(ssaoTexture, texCoords).r, 2);

    vec3 lighting = Diffuse * 0.5 * (occlusion ? AmbientOcclusion : 1.0); //ambient

    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lightColor;

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
    // col += kernel[0] * texture(gAlbedo, texCoords + vec2(-offset, offset )); // top left
    // col += kernel[1] * texture(gAlbedo, texCoords + vec2(0.0f,    offset )); // top center
    // col += kernel[2] * texture(gAlbedo, texCoords + vec2(offset,  offset )); // top right
    // col += kernel[3] * texture(gAlbedo, texCoords + vec2(-offset, 0.0f   )); // left
    // col += kernel[4] * texture(gAlbedo, texCoords + vec2(0.0f,    0.0f   )); // center
    // col += kernel[5] * texture(gAlbedo, texCoords + vec2(offset,  0.0f   )); // right
    // col += kernel[6] * texture(gAlbedo, texCoords + vec2(-offset, -offset)); // bottom left
    // col += kernel[7] * texture(gAlbedo, texCoords + vec2(0.0f,    -offset)); // bottom center
    // col += kernel[8] * texture(gAlbedo, texCoords + vec2(offset,  -offset)); // bottom right
    // fragColor = col;

    lighting += diffuse;

    if (shadowDepth < shadowFragPos.z - 0.00001)
    {
        lighting /= 2.0;
    }
    fragColor = vec4(lighting, 1.0);
}