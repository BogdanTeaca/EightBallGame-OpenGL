#version 330

// TEACA BOGDAN

layout(location = 0) out vec4 out_color;

// from vertex shader
in vec3 world_pos;
in vec3 world_nor;

// uniforms for light
uniform vec3 light_dir;
uniform vec3 light_pos;
uniform vec3 eye_pos;

// uniforms for spot
uniform float spot_on;
uniform float angle;

// uniforms for material
uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;
uniform vec3 object_color;

void main(){
	// fragment shader pentru shading Phong

	vec3 world_normal = normalize(world_nor);

	vec3 L = normalize(light_pos - world_pos);
	vec3 V = normalize(eye_pos - world_pos);
	vec3 H = normalize(L + V);

	vec3 ambient = 0.8 * vec3(255);
	float ambient_light = 0.8;
	ambient = ambient_light * vec3(255);

	float diffuse_light;
	float specular_light;
	float light;

	if(spot_on == 0){ // sursa de lumina de tip spot
		diffuse_light = material_kd * max(dot(L, world_normal), 0);
		specular_light = material_ks * pow(max(dot(world_normal, H), 0), material_shininess) * 2;
 
		float temp = 1 / pow(distance(light_pos, world_pos), 2);

		light = ambient_light + min(temp, 1) * (diffuse_light + specular_light);
	}else{
		float cut_off = radians(angle);
		float spot_light = dot(-L, light_dir);
		float spot_light_limit = cos(cut_off);
		 
		 if(spot_light > cos(cut_off)){
			ambient = ambient_light * vec3(255);

			diffuse_light = material_kd * max(dot(L, world_normal), 0);
			specular_light = material_ks * pow(max(dot(world_normal, H), 0), material_shininess) * 2;

			float linear_att = (spot_light - spot_light_limit) / (1 - spot_light_limit);
			float light_att_factor = pow(linear_att, 2);
			
			light = ambient_light + (diffuse_light + specular_light) * light_att_factor;
		 }else{
			light = ambient_light;
		 }
	}

	vec3 color = object_color * light;
	out_color = vec4(color, 1);
}