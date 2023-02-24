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

uniform sampler2D gNormal;
uniform sampler2D gPosition;
uniform sampler2D texNoise;

uniform vec3 samples[64];

int kernelSize = 64;
float radius = 0.5;
float bias = 0.025;

const vec2 noiseScale = vec2(1920.0/4.0, 1080.0/4.0);

uniform mat4 Projection;

float near = 0.1f;
float far = 100.0f;

float linearizeDepth(float depth)
{
    return (2.0 * near * far) / (far + near - (depth * 2.0 - 1.0) * (far - near));
}

vec4 fogColor = vec4(1.0, 1.0, 1.0, 1.0);

void main()
{

    vec3 fragPos = texture(gPosition, fragTexCoords).xyz;
    vec3 normal = normalize(texture(gNormal, fragTexCoords).rgb);
    vec3 randomVec = texture(texNoise, fragTexCoords * noiseScale).xyz;

    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));

    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    
    float occlusion = 0.0;

    for (int i=0; i<kernelSize; i++)
    {
        vec3 sample = TBN * samples[i];
        sample = fragPos + sample * radius;

        vec4 offset = vec4(sample, 1.0);

        offset = Projection * offset;
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;

        vec3 occludorPos = texture(gPosition, offset.xy).rgb;

        float rangeCheck = smoothstep(0.0, 1.0, radius / length(fragPos - occludorPos));

        occlusion += (occludorPos.z >= sample.z + bias ? 1.0 : 0.0) * rangeCheck;
    }

    color = 1.0 - occlusion / kernelSize;
}