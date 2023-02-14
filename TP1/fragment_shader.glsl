#version 330 core

in vec3 o_normalWorld;
in vec2 o_uv0;
// Ouput data
out vec3 color;

void main(){

        color =vec3(0.2, 0.2,0.4) * vec3(o_uv0,1);

}
