#version 450

// compile with glslangValidator -V shader.vert

layout(location=0) in vec4 fragColor;

layout(location=0) out vec4 outColor;

void main()
{
    outColor = fragColor;
}
