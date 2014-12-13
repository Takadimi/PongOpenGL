#ifndef SHADER_H
#define SHADER_H

#include <iostream>
#include <GL/glew.h>

GLuint buildShader(const char* file_path, GLenum type);
GLuint buildProgram(GLuint vertex_shader, GLuint fragment_shader);

#endif