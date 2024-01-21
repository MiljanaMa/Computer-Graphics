#version 330 core

out vec4 outCol;
in vec4 colVec;

void main()
{
	outCol = colVec;
}