#version 330 core

in vec2 chTex;
out vec4 outCol;

uniform sampler2D uTex;

void main()
{
    vec4 texColor = texture(uTex, chTex);

    // discard fragments that are transparent
    if (texColor.a <= 0.1)
        discard;
    texColor.a *= 0.5;

    // output the color with alpha
    outCol = texColor;
}