#version 330
	layout (points) in;
	layout (triangle_strip, max_vertices = 4) out;
	uniform float width;
	uniform float height;
	uniform vec3 forwardVector;
	out vec2 texCoord;
	uniform mat4 mvp;
	uniform mat4 view;
	uniform mat4 objm;

	void main() {
		vec3 pos = gl_in[0].gl_Position.xyz;		
		vec3 up = vec3(0.0, 1.0, 0.0);
    	vec3 right = normalize(cross(forwardVector, up));
		
		pos -= (right * width/2);
		gl_Position = objm * vec4(pos, 1.0);
		texCoord = vec2( 0.0, 0.0 );
		EmitVertex();
		
		pos.y += height;
		gl_Position = objm * vec4(pos, 1.0);
		texCoord = vec2( 1.0, 0.0 );
		EmitVertex();
		
		pos += (right * width);
		pos.y -= height;
		gl_Position = objm * vec4(pos, 1.0);
		texCoord = vec2( 0.0, 1.0 );
		EmitVertex();
		
		pos.y += height;
		gl_Position = objm  * vec4(pos, 1.0);
		texCoord = vec2( 1.0, 1.0 );
		EmitVertex();

		EndPrimitive();
}