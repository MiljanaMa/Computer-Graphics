#version 330 core

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 inTex;
uniform vec2 uPos;

out vec2 chTex;

void main(){
    if(gl_VertexID == 2 || gl_VertexID == 3)
    {
        gl_Position = vec4(inPos.x + uPos.x, inPos.y + uPos.y, 0.0, 1.0);
    }
    else
    {
        gl_Position = vec4(inPos.x + uPos.x, inPos.y, 0.0, 1.0);
    }
    chTex = inTex;
}