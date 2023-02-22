#shader vertex
#version 330

layout(location = 0) in vec3 position;
// layout(location = 1) in vec3 color;
// layout(location = 2) in vec3 coords;

uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(position, 1.0);
}

#shader geometry
#version 330
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

out vec3 normal;

void main()
{
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    normal = normalize(cross(b, a));

    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
    EmitVertex();

    gl_Position = gl_in[2].gl_Position;
    EmitVertex();

    EndPrimitive();
}

#shader fragment
#version 330

out vec4 color;

in vec3 normal;

void main()
{
    color = vec4(normal, 1.0);    
}