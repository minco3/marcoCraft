#shader vertex
#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec3 vertexTexCoords;

out vec3 fragPosition;
out vec3 fragColor;
flat out vec3 flatNormal;
out vec2 fragTexCoordsXY;
flat out float flatTexCoordsZ;

uniform mat4 view;
uniform mat4 model;
uniform mat4 MVP;

void main()
{
    vec4 viewPos = view * model * vec4(vertexPosition, 1.0);
    fragPosition = viewPos.xyz;
    
    fragColor = vertexColor;

    mat3 normalMatrix = transpose(inverse(mat3(view * model)));
    flatNormal = normalMatrix * vertexNormal;

    fragTexCoordsXY = vertexTexCoords.xy;
    flatTexCoordsZ = vertexTexCoords.z;
    gl_Position = MVP * vec4(vertexPosition, 1.0);
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 gColor;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec3 gPosition;

in vec3 fragPosition;
in vec2 fragTexCoordsXY;
flat in float flatTexCoordsZ;
flat in vec3 flatNormal;
in vec3 fragColor;

uniform sampler2DArray textureSlot;

void main()
{
    vec3 textureCoords = vec3(fragTexCoordsXY, flatTexCoordsZ);

    vec4 overlay;
    if (textureCoords.z > 1.5 && textureCoords.z < 2.5)
    {
        overlay = texture(textureSlot, vec3(textureCoords.x, textureCoords.y, 4.0));
        gColor = overlay.a == 0.0 ? vec4(texture(textureSlot, textureCoords)) : vec4(fragColor, 1.0) * overlay;
    }
    else
    {
        gColor = vec4(fragColor, 1.0) * texture(textureSlot, textureCoords);
    }

    gNormal = flatNormal;
    gPosition = fragPosition;
}