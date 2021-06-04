#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 objm;
uniform mat4 mvp;

void main()
{
    TexCoords = aTexCoords;
	vec3 FragPos = vec3(objm * vec4(aPos, 1.0));
	gl_Position = mvp * vec4(FragPos, 1.0);
}