#version 450

// compile with glslangValidator -V shader.vert

out gl_PerVertex {
    vec4 gl_Position;
};

layout(location=0) out vec4 fragColor;

layout(binding=0) buffer PositionBuffer
{
    vec4 positions[];
} positionBuffer;

layout(binding=1) buffer ColorBuffer
{
    vec4 colors[];
} colorBuffer;

void main()
{
    gl_Position = vec4(positionBuffer.positions[gl_VertexIndex]);
    fragColor = colorBuffer.colors[gl_VertexIndex];
}
