#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec3 color;

out vec3 fragColor;

uniform vec2 position_offset;

void main()
{
	fragColor = color;
	gl_Position = vec4(position, 0.0f, 1.0f) + vec4(position_offset, 0.0f, 0.0f);
}