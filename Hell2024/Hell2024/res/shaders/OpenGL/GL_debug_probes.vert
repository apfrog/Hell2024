#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 projection;
uniform mat4 view;

uniform int probeSpaceWidth;
uniform int probeSpaceHeight;
uniform int probeSpaceDepth;
uniform float probeSpacing;

out vec3 Color;

void main() {

	const float cubeScale = 0.03;

	const int indexZ = gl_InstanceID % probeSpaceDepth;
	const int indexY = (gl_InstanceID / probeSpaceDepth) % probeSpaceHeight;
	const int indexX = gl_InstanceID / (probeSpaceHeight * probeSpaceDepth); 
	
	float x = indexX * probeSpacing;
	float y = indexY * probeSpacing;
	float z = indexZ * probeSpacing;
	
	const mat4 model = mat4(
		1.0,  0.0, 0.0, 0.0, 
		0.0,  1.0, 0.0, 0.0, 
		0.0,  0.0, 1.0, 0.0, 
		  x,    y,   z, 1.0
	);	

	Color = vec3(1,0,0);
	gl_Position = projection * view * model * vec4(aPos * cubeScale, 1.0);
}