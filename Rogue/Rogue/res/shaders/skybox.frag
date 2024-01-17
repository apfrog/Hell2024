#version 420 core

layout (location = 0) out vec4 FragColor;
layout (binding = 0) uniform samplerCube cubeMap;

in vec3 TexCoords;

void main() {		
    FragColor.rgb = texture(cubeMap, TexCoords).rgb;
   // FragColor.rgb = TexCoords;
    FragColor.a = 1.0;
}
