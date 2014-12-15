#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 in_texCoord;

out vec2 texCoord;

void main()
{
	texCoord = vec2(in_texCoord.x, 1.0f - in_texCoord.y);
	gl_Position = vec4(position, 0.0f, 1.0f);
}