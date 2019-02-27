#version 330

// TEACA BOGDAN

layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec3 v_nor;
layout(location = 2) in vec2 v_tex;

// uniform matrices
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

uniform float time;
uniform vec3 cam_forward;
uniform vec2 ball;
uniform float cue_thickness;
uniform float cue_length;

// to fragment shader 
out vec3 world_pos;
out vec3 world_nor;

// matrici de transformari pentru a face animatia tacului

mat4 RotateOY(float rad){
	return mat4(
		cos(rad), 0, -sin(rad), 0,
		0, 1, 0, 0,
		sin(rad), 0, cos(rad), 0,
		0, 0, 0, 1
	);
}

mat4 RotateOX(float rad){
	return mat4(
		1, 0, 0, 0,
		0, cos(rad), sin(rad), 0,
		0, -sin(rad), cos(rad), 0,
		0, 0, 0, 1
	);
}

mat4 Translate(float translateX, float translateY, float translateZ){
	return mat4(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		translateX, translateY, translateZ, 1
	);
}


mat4 Scale(float scaleX, float scaleY, float scaleZ){
	return mat4(
		scaleX, 0, 0, 0,
		0, scaleY, 0, 0,
		0, 0, scaleZ, 0,
		0, 0, 0, 1
	);
}

void main(){
	mat4 mod = Model;

	// translatam tacul la pozitia bilei

	mod *= Translate(ball.x, 1, ball.y);

	// rotim tacul in functie de directia camerei

	vec3 camera_forward = vec3(cam_forward.x, 0, cam_forward.z);
	camera_forward = normalize(camera_forward);
	
	if (camera_forward.z < 0) {
		mod *= RotateOY(radians(90) + acos(dot(camera_forward, vec3(1, 0, 0))));
	}else {
		mod *= RotateOY(radians(270) + acos(dot(camera_forward, vec3(-1, 0, 0))));
	}

	// inclinam tacul

	mod *= RotateOX(radians(10));

	// facem animatia de translatie

	mod *= Translate(0, 0, sin(time * 2) / 2 - cue_length * 3 / 4);

	// scalam tacul astfel incat sa arate ca un bat

	mod *= Scale(cue_thickness, cue_thickness, cue_length);

	world_pos = (mod * vec4(v_pos, 1)).xyz;
	world_nor = normalize(mat3(mod) * v_nor);

	gl_Position = Projection * View * mod * vec4(v_pos, 1.0f);
}
