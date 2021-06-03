#version 330
out vec4 FragColor;
in vec2 texCoord;
uniform sampler2D diffuseTexture;

void main(){
	FragColor = vec4(1, 1, 1, 1);
}