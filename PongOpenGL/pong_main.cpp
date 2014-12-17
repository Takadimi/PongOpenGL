#include "pong_main.h"
#include <SOIL.h>
#include "shader.h"
#include "glfw_x360_controller.h"

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
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLuint vert_shader_id = buildShader("shader.vert", GL_VERTEX_SHADER);
	GLuint frag_shader_id = buildShader("shader.frag", GL_FRAGMENT_SHADER);
	GLuint shader_program_id = buildProgram(vert_shader_id, frag_shader_id);

	glDeleteShader(vert_shader_id);
	glDeleteShader(frag_shader_id);

	const GLfloat paddle_width = 0.04f;
	const GLfloat paddle_height = 0.5f;

	GLfloat paddle_vertices[] = {
		 (paddle_width / 2),  (paddle_height / 2), 1.0f, 1.0f, 1.0f,
		 (paddle_width / 2), -(paddle_height / 2), 1.0f, 1.0f, 1.0f,
		-(paddle_width / 2), -(paddle_height / 2), 1.0f, 1.0f, 1.0f,
		-(paddle_width / 2),  (paddle_height / 2), 1.0f, 1.0f, 1.0f,
	};

	GLuint paddle_indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	Vao_Data paddle_vao_data = { paddle_vertices, sizeof(paddle_vertices), paddle_indices, sizeof(paddle_indices) };
	Paddle paddle_player = { build_vao(paddle_vao_data), glm::vec2(-0.95f, 0.0f), glm::vec2(0.0f, 0.0f) };
	Paddle paddle_computer = { build_vao(paddle_vao_data), glm::vec2(0.95f, 0.0f), glm::vec2(0.0f, 0.0f) };

	const GLfloat ball_width = 0.06f;
	const GLfloat ball_height = 0.08f;
	GLfloat ball_vertices[] = {
		 (ball_width / 2),  (ball_height / 2), 0.2f, 0.7f, 0.6f,
		 (ball_width / 2), -(ball_height / 2), 0.2f, 0.7f, 0.6f,
		-(ball_width / 2), -(ball_height / 2), 0.2f, 0.7f, 0.6f,
		-(ball_width / 2),  (ball_height / 2), 0.2f, 0.7f, 0.6f,
	};

	GLuint ball_indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	Vao_Data ball_vao_data = { ball_vertices, sizeof(ball_vertices), ball_indices, sizeof(ball_indices) };
	Ball ball = { build_vao(ball_vao_data), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true, true };

	last_time = (float) glfwGetTime();
	
	/* MAIN GAME LOOP */
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		handle_player_keyboard_input();

		glClearColor(0.2f, 0.2f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		delta_time = (float) (glfwGetTime() - last_time) * 100.0f;
		last_time = (float) glfwGetTime();

		// Calculate current position of each object 
		paddle_player.current_pos = glm::vec2(paddle_player.origin.x, paddle_player.origin.y + left_paddle_y_offset);
		paddle_computer.current_pos = glm::vec2(paddle_computer.origin.x, paddle_computer.origin.y + cos((float)glfwGetTime()));
		calculate_ball_position(&ball, delta_time, ball_width, ball_height);

		// COLLISION DETECTION GOES HERE
		if (ball.current_pos.x - (ball_width / 2) < paddle_player.current_pos.x &&
			(ball.current_pos.y - (ball_height / 2)) < (paddle_player.current_pos.y + (paddle_height / 2)) &&
			(ball.current_pos.y + (ball_height / 2)) > (paddle_player.current_pos.y - (paddle_height / 2)))
		{
			ball.is_moving_left = false;
		}

		// Drawing paddles and ball
		glUseProgram(shader_program_id);

		glBindVertexArray(paddle_player.vao);
		glUniform2f(glGetUniformLocation(shader_program_id, "position_offset"), paddle_player.current_pos.x, paddle_player.current_pos.y);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(paddle_computer.vao);
		glUniform2f(glGetUniformLocation(shader_program_id, "position_offset"), paddle_computer.current_pos.x, paddle_computer.current_pos.y);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(ball.vao);
		glUniform2f(glGetUniformLocation(shader_program_id, "position_offset"), ball.current_pos.x, ball.current_pos.y);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);

	return 0;
}

GLuint build_vao(Vao_Data data)
{
	return build_vao(data.vertices, data.vertices_size, data.indices, data.indices_size);
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

void calculate_ball_position(Ball* ball, GLfloat delta_time, GLfloat ball_width, GLfloat ball_height)
{
	if (ball->is_moving_left)
	{
		ball->current_pos.x -=  ball_horizontal_speed * delta_time;
		if (ball->current_pos.x < (-1.0f + (ball_width / 2)))
		{
			ball->is_moving_left = false;
			ball_horizontal_speed += ball_horizontal_speed_increment;
		}
	}
	else
	{
		ball->current_pos.x += ball_horizontal_speed * delta_time;
		if (ball->current_pos.x > (1.0f - (ball_width / 2)))
		{
			ball->is_moving_left = true;
			ball_horizontal_speed += ball_horizontal_speed_increment;
		}
	}

	if (ball->is_moving_up)
	{
		ball->current_pos.y += 0.005f * delta_time;
		if (ball->current_pos.y > (1.0f - (ball_height / 2)))
		{
			ball->is_moving_up = false;
		}
	}
	else
	{
		ball->current_pos.y -= 0.005f * delta_time;
		if (ball->current_pos.y < (-1.0f + (ball_height / 2)))
		{
			ball->is_moving_up = true;
		}
	}
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