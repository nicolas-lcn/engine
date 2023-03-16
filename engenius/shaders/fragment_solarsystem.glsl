#version 330 core

in vec3 o_normalWorld;
in vec2 o_uv0;

uniform sampler2D tex;
// Ouput data
out vec4 FragColor;

void main(){
	FragColor = texture(tex, o_uv0);
	//FragColor = vec4(vec3(0.2, 0.2,0.4), 1);
}
