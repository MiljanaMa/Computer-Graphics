#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

uniform mat4 uP;
uniform mat4 uV;
uniform mat4 uM;

out vec2 UV;
out vec3 vWorldSpaceFragment;
out vec3 vWorldSpaceNormal;

void main() {
	vWorldSpaceFragment = vec3(uM * vec4(aPos, 1.0f));
	vWorldSpaceNormal = normalize(mat3(transpose(inverse(uM))) * aNormal);
	UV = aUV;
	gl_Position = uP * uV * uM * vec4(aPos, 1.0f);
}