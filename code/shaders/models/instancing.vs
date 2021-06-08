#version 330 core
	layout(location = 0) in vec3 aPos;
	layout(location = 1) in vec2 aUvs;
	layout(location = 2) in vec3 aNormal;
	out vec3 FragPos;
	out vec3 Normal;
	out vec2 Uvs;
	uniform mat4 model[10];
	uniform mat4 view;
	uniform mat4 projection;
	void main(){
		FragPos = vec3(model[gl_InstanceID] * vec4(aPos, 1.0));
		Normal = mat3(transpose(inverse(model[gl_InstanceID]))) * aNormal;
		gl_Position = projection * view * vec4(FragPos, 1.0);
		Uvs = aUvs;
}