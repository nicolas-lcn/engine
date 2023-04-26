#version 330 core

in vec3 o_normalWorld;
in vec2 o_uv0;
in float o_height;

uniform sampler2D ground;
uniform sampler2D h_ground;
uniform sampler2D heights;

uniform sampler2D objectTex;
uniform bool isFragTerrain;
// Ouput data
out vec4 FragColor;

void main(){
	vec4 groundTex = texture(ground, o_uv0);
	// vec4 h_groundTex = texture(h_ground, o_uv0);
	// vec4 heightsTex = texture(heights, o_uv0);

	// if(!isFragTerrain) FragColor = texture(objectTex, o_uv0);
	if(!isFragTerrain) FragColor = vec4(0.5, 0.5, 0.5, 1);

	else
	{
		FragColor = groundTex;
		// if(o_height < 0.42) 
		// {
		// 	float c_g, c_r;
		// 	if(o_height < 0.38) c_g =  1;
		// 	else if(o_height > 0.4) c_g = 1 - (1 - o_height);
		// 	else c_g = (1 - o_height);
		// 	if(o_height < 0.38) c_r = 0; 
		// 	else if(o_height > 0.4) c_r = (1 - o_height);
		// 	else c_r = 1 - (1 - o_height);
		// 	FragColor = groundTex * c_g + h_groundTex * c_r;
		// }
		// else if(o_height >= 0.42 && o_height < 0.78)
		// {	
		// 	FragColor = h_groundTex;
		// }
		// else
		// {
		// 	float c_r, c_sr;
		// 	if(o_height < 0.78) c_sr = 1;
		// 	else if (o_height > 0.8) c_sr =  1 - (1 - o_height);
		// 	else c_sr = (1 - o_height);
		// 	if(o_height < 0.78) c_r =  0 ;
		// 	else if (o_height > 0.8) c_r = (1 - o_height);
		// 	else c_r =  1 - (1 - o_height);
		// 	FragColor = h_groundTex * c_r + heightsTex * c_sr;
		// }
		
	}
}


