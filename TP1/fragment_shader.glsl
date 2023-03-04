#version 330 core

in vec3 o_normalWorld;
in vec2 o_uv0;
in float o_height;

uniform sampler2D ground;
uniform sampler2D h_ground;
uniform sampler2D heights;
// Ouput data
out vec4 FragColor;

void main(){
	vec4 groundTex = texture(ground, o_uv0);
	vec4 h_groundTex = texture(h_ground, o_uv0);
	vec4 heightsTex = texture(heights, o_uv0);

    FragColor = mix(groundTex, (mix(h_groundTex, heightsTex, o_height)), o_height);
}
