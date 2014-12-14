#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <SOIL.h>
#include "shader.h"
#include "glfw_x360_controller.h"

#define MONITOR_WIDTH 1920
#define MONITOR_HEIGHT 1080
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// GLOBALS
float delta_time = 0.0f;
float last_time = 0.0f;
float left_paddle_y_offset = 0.0f;
bool keys[1024];

struct paddle
{
	GLuint vao;
	glm::vec2 origin;
	glm::vec2 current_pos;
};

struct ball
{
	GLuint vao;
	glm::vec2 origin;
	glm::vec2 current_pos;
};

// Function prototypes
GLuint build_vao(GLfloat* vertices, GLuint vertices_size);
GLuint build_vao(GLfloat* vertices, GLuint vertices_size, GLuint* indices, GLuint indices_size);
void init_paddle(paddle* paddle, GLfloat* vertices, GLuint vertices_size, GLuint* indices, GLuint indices_size);
void init_ball(ball* ball, GLfloat* vertices, GLuint vertices_size, GLuint* indices, GLuint indices_size);
void key_callback(GLFWwindow*, int, int, int, int);
void handle_player_keyboard_input();
void handle_player_controller_input();

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

	paddle paddle_player;
	init_paddle(&paddle_player, paddle_vertices, sizeof(paddle_vertices), paddle_indices, sizeof(paddle_indices));
	paddle_player.origin = glm::vec2(-0.95f, 0.0f);
	
	paddle paddle_computer;
	init_paddle(&paddle_computer, paddle_vertices, sizeof(paddle_vertices), paddle_indices, sizeof(paddle_indices));
	paddle_computer.origin = glm::vec2(0.95f, 0.0f);

	last_time = (float) glfwGetTime();
	
	/* MAIN GAME LOOP */
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		handle_player_keyboard_input();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		delta_time = (float) (glfwGetTime() - last_time) * 100.0f;
		last_time = (float) glfwGetTime();

		glUseProgram(shader_program_id);

		glBindVertexArray(paddle_player.vao);
		paddle_player.current_pos = glm::vec2(paddle_player.origin.x, paddle_player.origin.y + left_paddle_y_offset);
		glUniform2f(glGetUniformLocation(shader_program_id, "position_offset"), paddle_player.current_pos.x, paddle_player.current_pos.y);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(paddle_computer.vao);
		paddle_computer.current_pos = glm::vec2(paddle_computer.origin.x, paddle_computer.origin.y + cos((float)glfwGetTime()));
		glUniform2f(glGetUniformLocation(shader_program_id, "position_offset"), paddle_computer.current_pos.x, paddle_computer.current_pos.y);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);

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

void init_paddle(paddle* paddle, GLfloat* vertices, GLuint vertices_size, GLuint* indices, GLuint indices_size)
{
	paddle->vao = build_vao(vertices, vertices_size, indices, indices_size);
}

void init_ball(ball* ball, GLfloat* vertices, GLuint vertices_size, GLuint* indices, GLuint indices_size)
{
	ball->vao = build_vao(vertices, vertices_size, indices, indices_size);
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

void handle_player_keyboard_input()
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

void handle_player_controller_input()
{
	if (glfwJoystickPresent(GLFW_JOYSTICK_1))
	{
		int size;
		const unsigned char* results = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &size);
		int button_pressed = get_current_button_pressed(results, size);

		if (button_pressed == X360_DPAD_UP)
		{
			left_paddle_y_offset += 0.008f * delta_time;
		}
		else if (button_pressed == X360_DPAD_DOWN)
		{
			left_paddle_y_offset -= 0.008f * delta_time;
		}
	}
}