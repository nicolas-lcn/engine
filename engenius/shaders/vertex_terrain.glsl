#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertices_position_modelspace;
layout(location = 1) in vec3 normal_modelspace;
layout(location = 2) in vec2 uv0;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform sampler2D heightmap;

uniform bool isVertTerrain;

out vec3 o_normalWorld;
out vec2 o_uv0;
out float o_height;

void main(){
	mat4 MVP = projection * view * model;
	mat3 normalMatrix = mat3(transpose(inverse(model)));

	vec4 height = texture(heightmap, uv0);

	vec4 offset = vec4(vertices_position_modelspace, 1.0) + vec4(0, height.r, 0, 0);
	o_uv0 = uv0;
	o_height = offset.y;
	o_normalWorld = normalMatrix * normal_modelspace;
    if(isVertTerrain) gl_Position = MVP * offset;
    else gl_Position = MVP * vec4(vertices_position_modelspace, 1);

}

