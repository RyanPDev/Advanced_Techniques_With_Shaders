#version 330 core
	out vec4 FragColor;
	in vec3 Normal;
	in vec3 FragPos;
	in vec2 Uvs;
	uniform sampler2D diffuseTexture;
	uniform mat4 view;
	uniform vec3 lightPos;
	uniform vec3 lightColor;
	uniform vec3 ambientColor;
	uniform vec3 specularColor;
	uniform float lightIntensity;
	uniform float ambientStrength;
	uniform float diffuseStrength;
	uniform float specularStrength;
	uniform float shininessValue;
	uniform bool Stencil;
	vec3 norm;
	float diff;
	vec3 ambient;
	vec3 diffuse;
	vec3 viewPos;
	vec3 viewDir;
	vec3 lightDir;
	vec3 reflectDir;
	float spec;
	vec3 specular;
	vec3 result;
	void main(){
			
		FragColor = texture(diffuseTexture, Uvs);
		if (FragColor.a < 0.9f && !Stencil) 
			discard;
		else if(Stencil && FragColor.a >= 0.9f)
			discard;
		else{
		// AMBIENT // 
		ambient = ambientStrength * lightColor * ambientColor;
		
		// DIFFUSE //
		lightDir = normalize(-lightPos);
		norm = normalize(Normal);
		diff = max(dot(norm, -lightDir), 0.0);
		diffuse = diff * diffuseStrength * lightColor;
		// SPECULAR //
		viewPos = vec3(inverse(view)[3]);
		viewDir = normalize(viewPos - FragPos);
		reflectDir = reflect(lightDir, norm);
		spec = pow(max(dot(viewDir, reflectDir), 0.0), shininessValue);
		specular = specularStrength * spec * lightColor;
		result = (ambient + diffuse + (specular * specularColor)) * lightIntensity;
		
		FragColor *= vec4(result, 0.4f);
		}
}