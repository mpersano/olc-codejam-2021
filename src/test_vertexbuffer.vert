#version 450

// compile with glslangValidator -V -o test_vertexbuffer.spv test_vertexbuffer.vert

out gl_PerVertex {
    vec4 gl_Position;
};

layout(location=0) in vec4 inPosition;
layout(location=1) in vec4 inColor;

layout(location=0) out vec4 fragColor;

void main()
{
    gl_Position = inPosition;
    fragColor = inColor;
}
