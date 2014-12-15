#version 330 core

in vec2 texCoord;
out vec4 out_color;

uniform sampler2D sprite_texture;

void main()
{
	out_color = texture(sprite_texture, texCoord);

	//if (out_color.a == 1.0f)
	//{
	//	out_color.a = 0.2f;	
	//}

	//if (out_color == vec4(1.0f, 1.0f, 1.0f, 1.0f))
	//{
	//	discard;	
	//}
}