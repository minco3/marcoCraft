#shader vertex
#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec3 vertexTexCoords;

out vec3 fragPosition;
flat out vec3 flatNormal;
flat out vec3 flatColor;
out vec2 fragTexCoordsXY;
flat out float flatTexCoordsZ;

uniform mat4 view;
uniform mat4 model;
uniform mat4 MVP;

void main()
{
    vec4 viewPos = view * model * vec4(vertexPosition, 1.0);
    fragPosition = viewPos.xyz;
    
    mat3 normalMatrix = transpose(inverse(mat3(view * model)));
    flatNormal = normalMatrix * vertexNormal;

    flatColor = vertexColor;

    fragTexCoordsXY = vertexTexCoords.xy;
    flatTexCoordsZ = vertexTexCoords.z;
    gl_Position = MVP * vec4(vertexPosition, 1.0);
}

#shader fragment
#version 330 core

in vec3 fragPosition;
flat in vec3 flatNormal;
flat in vec3 flatColor;
in vec2 fragTexCoordsXY;
flat in float flatTexCoordsZ;

layout(location = 0) out vec4 gColor;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec3 gPosition;


uniform sampler2DArray textureSlot;

void main()
{
    vec3 textureCoords = vec3(fragTexCoordsXY, flatTexCoordsZ);


    if (flatTexCoordsZ > 1.5 && flatTexCoordsZ < 2.5)
    {
        vec4 overlay;
        overlay = texture(textureSlot, vec3(textureCoords.xy, 4.0));
        gColor = overlay.a == 0.0 ? vec4(texture(textureSlot, textureCoords)) : vec4(flatColor, 1.0) * overlay;
    }
    else if (flatTexCoordsZ > 2.5 && flatTexCoordsZ < 3.5)
    {
        gColor = vec4(flatColor, 1.0) * texture(textureSlot, textureCoords);
    }
    else
    {
        gColor = texture(textureSlot, textureCoords);
    }
    
    gNormal = flatNormal;
    gPosition = fragPosition;
}