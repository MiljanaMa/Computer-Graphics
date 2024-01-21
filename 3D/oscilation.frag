#version 330 core 

in vec4 colVec;
out vec4 outCol;

void main()
{
    outCol = vec4(colVec);
};