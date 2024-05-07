#version 330 core

in vec3 frg_tex_coord;
in vec4 frg_color;
in vec4 frg_grd_color;
in vec3 frg_color_offset_is_positive;
in vec3 frg_color_offset;

out vec4 out_color;

uniform sampler2DArray sampler;
uniform bool is_texture_used;

void main()
{
	if(is_texture_used){
		//out_color = texture(sampler, frg_tex_coord.xyz);
		float x = frg_tex_coord.x;
		float y = frg_tex_coord.y;
		float layer = frg_tex_coord.z;
		out_color = texture(sampler, vec3(x,y,layer)); 
	}else{
		out_color = frg_color;
	}
	out_color.rgb += frg_grd_color.rgb;
	if( frg_color_offset_is_positive.r > 0.0 ){ out_color.r += frg_color_offset.r; } else { out_color.r -= frg_color_offset.r;}
	if( frg_color_offset_is_positive.g > 0.0 ){ out_color.g += frg_color_offset.g; } else { out_color.g -= frg_color_offset.g;}
	if( frg_color_offset_is_positive.b > 0.0 ){ out_color.b += frg_color_offset.b; } else { out_color.b -= frg_color_offset.b;}
	out_color.rgb = clamp(out_color.rgb, vec3(0.0), vec3(1.0));

} 