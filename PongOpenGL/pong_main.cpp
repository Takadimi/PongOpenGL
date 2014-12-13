#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <SOIL.h>
#include "shader.h"

#define MONITOR_WIDTH 1920
#define MONITOR_HEIGHT 1080
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

void key_callback(GLFWwindow*, int, int, int, int);

int main(void)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, false);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pong | OpenGL", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);

	glewExperimental = true;
	glewInit();

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glEnable(GL_DEPTH_TEST);

	GLuint vert_shader_id = buildShader("shader.vert", GL_VERTEX_SHADER);
	GLuint frag_shader_id = buildShader("shader.frag", GL_FRAGMENT_SHADER);
	GLuint shader_program_id = buildProgram(vert_shader_id, frag_shader_id);

	glDeleteShader(vert_shader_id);
	glDeleteShader(frag_shader_id);

	GLfloat paddle_vertices[] = {
		 0.02f,  0.25f, 1.0f, 1.0f, 1.0f,
		 0.02f, -0.25f, 1.0f, 1.0f, 1.0f,
		-0.02f, -0.25f, 1.0f, 1.0f, 1.0f,
		-0.02f,  0.25f, 1.0f, 1.0f, 1.0f,
	};

	GLuint paddle_indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	GLuint paddle_vao, paddle_vbo, paddle_ebo;

	glGenVertexArrays(1, &paddle_vao);
	glBindVertexArray(paddle_vao);

	glGenBuffers(1, &paddle_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, paddle_vbo);

	glGenBuffers(1, &paddle_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, paddle_ebo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(paddle_vertices), paddle_vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(paddle_indices), paddle_indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader_program_id);
		glBindVertexArray(paddle_vao);
		glUniform2f(glGetUniformLocation(shader_program_id, "position_offset"), -0.95f, sin((float)glfwGetTime()));
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glUniform2f(glGetUniformLocation(shader_program_id, "position_offset"), 0.95f, cos((float)glfwGetTime()));
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}

	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}