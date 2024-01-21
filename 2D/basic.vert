#version 330 core

layout(location = 0) in vec2 inPos;
uniform vec2 uPos;
uniform vec4 inColVec;
out vec4 colVec;

void main()
{
	gl_Position = vec4(inPos.x-uPos.x , inPos.y+uPos.y, 0.0, 1.0);
	colVec = inColVec;
}