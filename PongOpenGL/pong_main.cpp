#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <SOIL.h>
#include "shader.h"

#define MONITOR_WIDTH 1920
#define MONITOR_HEIGHT 1080
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

GLuint build_vao(GLfloat* vertices, GLuint vertices_size);
GLuint build_vao(GLfloat* vertices, GLuint vertices_size, GLuint* indices, GLuint indices_size);
void key_callback(GLFWwindow*, int, int, int, int);
void handle_player_input();

// GLOBALS
float delta_time = 0.0f;
float last_time = 0.0f;
float left_paddle_y_offset = 0.0f;
bool keys[1024];

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

	GLuint paddle_vao = build_vao(paddle_vertices, sizeof(paddle_vertices), paddle_indices, sizeof(paddle_indices));

	last_time = (float) glfwGetTime();
	
	/* MAIN GAME LOOP */
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		handle_player_input();

		delta_time = ((float)glfwGetTime() - last_time) * 100.0f;
		last_time = (float) glfwGetTime();

		//printf("DELTA TIME: %f\n", delta_time);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader_program_id);
		glBindVertexArray(paddle_vao);
		glUniform2f(glGetUniformLocation(shader_program_id, "position_offset"), -0.95f, left_paddle_y_offset);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glUniform2f(glGetUniformLocation(shader_program_id, "position_offset"), 0.95f, cos((float)glfwGetTime()));
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}

	return 0;
}

GLuint build_vao(GLfloat* vertices, GLuint vertices_size)
{
	GLuint vao = build_vao(vertices, vertices_size, NULL, NULL);

	return vao;
}

GLuint build_vao(GLfloat* vertices, GLuint vertices_size, GLuint* indices, GLuint indices_size)
{
	GLuint vao, vbo;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	if (indices != NULL && indices_size != NULL)
	{
		GLuint ebo;
		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	}

	glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	return vao;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	if (action == GLFW_PRESS)
	{
		keys[key] = true;
	}
	else if (action == GLFW_RELEASE)
	{
		keys[key] = false;
	}
}

void handle_player_input()
{
	if (keys[GLFW_KEY_W])
	{
		left_paddle_y_offset += 0.008f * delta_time;
	}
	else if (keys[GLFW_KEY_S])
	{
		left_paddle_y_offset -= 0.008f * delta_time;
	}
}