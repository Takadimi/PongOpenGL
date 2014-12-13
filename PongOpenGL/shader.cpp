#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "shader.h"

GLuint buildShader(const char* file_path, GLenum type)
{
	GLuint id = glCreateShader(type);

	std::ifstream shader_file(file_path);
	std::stringstream shader_stream;

	shader_stream << shader_file.rdbuf();
	std::string temp_str = shader_stream.str();
	const GLchar* shader_string = temp_str.c_str();

	shader_file.close();

	// std::cout << "SHADER TYPE: " << type << std::endl << shader_string << std::endl << std::endl;
	glShaderSource(id, 1, &shader_string, NULL);
	glCompileShader(id);

	GLint compilation_result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &compilation_result);
	if (!compilation_result)
	{
		GLchar info_log[512];
		glGetShaderInfoLog(id, 512, NULL, info_log);
		std::cout << info_log << std::endl;
	}

	return id;
}

GLuint buildProgram(GLuint vertex_shader, GLuint fragment_shader)
{
	GLuint program = glCreateProgram();

	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	GLint success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		GLchar infoLog[512];
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	return program;
}