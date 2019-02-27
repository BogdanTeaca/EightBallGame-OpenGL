#version 330

// TEACA BOGDAN

layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec3 v_nor;
layout(location = 2) in vec2 v_tex;

// uniform matrices
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

// to fragment shader 
out vec3 world_pos;
out vec3 world_nor;

void main(){
	world_pos = (Model * vec4(v_pos, 1)).xyz;
	world_nor = normalize(mat3(Model) * v_nor);

	gl_Position = Projection * View * Model * vec4(v_pos, 1.0);
}
