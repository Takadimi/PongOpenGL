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

	glfwSetWindowPos(window, (MONITOR_WIDTH / 2) - (SCREEN_WIDTH / 2), (MONITOR_HEIGHT / 2) - (SCREEN_HEIGHT / 2));
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

	// INITIALIZE VAOS

	// PADDLES 
	const GLfloat paddle_width = 0.04f;
	const GLfloat paddle_height = 0.5f;

	GLfloat paddle_vertices[] = {
		 (paddle_width / 2),  (paddle_height / 2), 0.4f, 0.3f, 0.6f,
		 (paddle_width / 2), -(paddle_height / 2), 0.4f, 0.3f, 0.6f,
		-(paddle_width / 2), -(paddle_height / 2), 0.4f, 0.3f, 0.6f,
		-(paddle_width / 2),  (paddle_height / 2), 0.4f, 0.3f, 0.6f,
	};

	GLuint paddle_indices[] = {
		0, 1, 2,
		2, 3, 0
	};
	
	GLuint paddle_vao = build_vao(paddle_vertices, sizeof(paddle_vertices), paddle_indices, sizeof(paddle_indices));

	// BALL
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

	GLuint ball_vao = build_vao(ball_vertices, sizeof(ball_vertices), ball_indices, sizeof(ball_indices));

	// INITIALIZE GAME OBJECTS
	Paddle paddle_player = { PLAYER, paddle_vao, glm::vec2(-0.95f, 0.0f) };
	Paddle paddle_computer = { COMPUTER, paddle_vao, glm::vec2(0.95f, 0.0f) };
	Ball ball = { ball_vao, glm::vec2(0.0f, 0.0f), 0.008f, 0.005f, false, true };

	last_time = (float) glfwGetTime();
	
	/* MAIN GAME LOOP */
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		handle_player_keyboard_input();

		glClearColor(0.2f, 0.2f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		delta_time = (float)(glfwGetTime() - last_time) * 100.0f;
		last_time = (float)glfwGetTime();

		/******* UPDATE *******/

		// COMPUTER PADDLE AI
		// TODO: Maybe introduce a variable by which we offset the y-position
		// the computer aims for by a value between [0.4f, -0.4f].
		// This should allow for the computer to make something other than shots that come
		// off the dead center of its paddle...
		if (ball.is_moving_right)
		{
			if (paddle_computer.position.y > ball.position.y)
			{
				computer_paddle_y_offset -= computer_paddle_y_velocity * delta_time;
			}
			else if (paddle_computer.position.y < ball.position.y)
			{
				computer_paddle_y_offset += computer_paddle_y_velocity * delta_time;
			}
		}
		else
		{
			if (paddle_computer.position.y > 0.0f)
			{
				computer_paddle_y_offset -= computer_paddle_y_velocity * delta_time;
			}
			else if (paddle_computer.position.y < 0.0f)
			{
				computer_paddle_y_offset += computer_paddle_y_velocity * delta_time;
			}
		}

		paddle_player.position.y = player_paddle_y_offset;
		paddle_computer.position.y = computer_paddle_y_offset;
		calculate_ball_position(&ball, delta_time, ball_width, ball_height);

		// COLLISION DETECTION GOES HERE
		if (ball.is_moving_right)
		{
			handle_collision(&ball, &paddle_computer, ball_width, ball_height, paddle_width, paddle_height);
		}
		else
		{
			handle_collision(&ball, &paddle_player, ball_width, ball_height, paddle_width, paddle_height);
		}

		/******* END UPDATE *******/

		/******* RENDER *******/

		// Drawing paddles and ball
		glUseProgram(shader_program_id);

		glBindVertexArray(paddle_player.vao);
		glUniform2f(glGetUniformLocation(shader_program_id, "position_offset"), paddle_player.position.x, paddle_player.position.y);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(paddle_computer.vao);
		glUniform2f(glGetUniformLocation(shader_program_id, "position_offset"), paddle_computer.position.x, paddle_computer.position.y);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(ball.vao);
		glUniform2f(glGetUniformLocation(shader_program_id, "position_offset"), ball.position.x, ball.position.y);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);

		/******* END RENDER *******/

		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);

	return 0;
}

GLuint build_vao(GLfloat* vertices, GLuint vertices_size)
{
	return build_vao(vertices, vertices_size, NULL, NULL);
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
	if (ball->is_moving_right)
	{
		ball->position.x += ball->x_speed * delta_time;
		if (ball->position.x > (1.0f - (ball_width / 2)))
		{
			ball->is_moving_right = false;
		}
	}
	else
	{
		ball->position.x -=  ball->x_speed * delta_time;
		if (ball->position.x < (-1.0f + (ball_width / 2)))
		{
			ball->is_moving_right = true;
		}
	}

	ball->position.y += ball->y_speed * delta_time;

	if (ball->position.y > (1.0f - (ball_height / 2)) ||
		ball->position.y < (-1.0f + (ball_height / 2)))
	{
		ball->y_speed *= -1.0f;
	}
}

void handle_collision(Ball* ball, Paddle* paddle, float ball_width, float ball_height, float paddle_width, float paddle_height)
{
	const float dir_one_offset = -0.01f;
	const float dir_two_offset = -0.005f;
	const float dir_three_offset = 0.0f;
	const float dir_four_offset = 0.005f;
	const float dir_five_offset = 0.01f;

	const float ball_paddle_offset = ball->position.y - paddle->position.y;

	if (is_intersecting_on_y_axis(ball, paddle, ball_height, paddle_height) &&
		is_intersecting_on_x_axis(ball, paddle, ball_width, paddle_width))
	{
		if (ball_paddle_offset <= 0.04f && ball_paddle_offset >= -0.04f)
		{
			ball->y_speed = dir_three_offset;
		}
		else if (ball_paddle_offset <= 0.15f && ball_paddle_offset > 0.04f)
		{
			ball->y_speed = dir_four_offset;
		}
		else if (ball_paddle_offset > 0.15f)
		{
			ball->y_speed = dir_five_offset;
		}
		else if (ball_paddle_offset < -0.04f && ball_paddle_offset >= -0.15f)
		{
			ball->y_speed = dir_two_offset;
		}
		else
		{
			ball->y_speed = dir_one_offset;
		}

		ball->is_moving_right = !ball->is_moving_right;
		ball->x_speed += 0.001f;
	}
}

bool is_intersecting_on_y_axis(Ball* ball, Paddle* paddle, float ball_height, float paddle_height)
{
	if ((ball->position.y - (ball_height / 2)) < (paddle->position.y + (paddle_height / 2)) &&
		(ball->position.y + (ball_height / 2)) > (paddle->position.y - (paddle_height / 2)))
	{
		return true;
	}

	return false;
}

bool is_intersecting_on_x_axis(Ball* ball, Paddle* paddle, float ball_width, float paddle_width)
{
	if (paddle->type == PLAYER)
	{
		if (ball->position.x - (ball_width / 2) < paddle->position.x + (paddle_width / 2))
			return true;
	}
	else if (paddle->type == COMPUTER)
	{
		if (ball->position.x + (ball_width / 2) > paddle->position.x - (paddle_width / 2))
			return true;
	}

	return false;
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
		player_paddle_y_offset += player_paddle_y_velocity * delta_time;
	}
	else if (keys[GLFW_KEY_S])
	{
		player_paddle_y_offset -= player_paddle_y_velocity * delta_time;
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
			player_paddle_y_offset += player_paddle_y_velocity * delta_time;
		}
		else if (button_pressed == X360_DPAD_DOWN)
		{
			player_paddle_y_offset -= player_paddle_y_velocity * delta_time;
		}
	}
}