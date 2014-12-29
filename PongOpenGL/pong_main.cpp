#include "pong_main.h"
#include <ctime>
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
	shader_program_id = buildProgram(vert_shader_id, frag_shader_id);

	glDeleteShader(vert_shader_id);
	glDeleteShader(frag_shader_id);

	// INITIALIZE VAOS

	// PADDLES 
	const GLfloat paddle_width	= 0.04f;
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
	const GLfloat ball_width	= 0.06f;
	const GLfloat ball_height	= 0.08f;

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

	Game_State game_state;

	// INITIALIZE GAME OBJECTS
	game_state.player	= { PLAYER, paddle_vao, paddle_width, paddle_height, glm::vec2(-0.95f, 0.0f), 0.0f };
	game_state.computer	= { COMPUTER, paddle_vao, paddle_width, paddle_height, glm::vec2(0.95f, 0.0f), 0.0f };
	game_state.ball		= { ball_vao, ball_width, ball_height, glm::vec2(0.0f, 0.0f), 0.008f, 0.005f, false, true };

	srand(time(0));
	game_state.last_time = (float) glfwGetTime();
	
	/* MAIN GAME LOOP */
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		handle_player_keyboard_input(&game_state);

		glClearColor(0.2f, 0.2f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		game_state.delta_time = (float)(glfwGetTime() - game_state.last_time) * 100.0f;
		game_state.last_time = (float)glfwGetTime();

		if (game_in_progress)
		{
			game_update_and_render(&game_state);
		}
		else
		{
			reset_game(&game_state);
		}

		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);

	return 0;
}

void game_update_and_render(Game_State* gs)
{
	// TODO: Clean this up
	if (gs->last_time - gs->ball_pause_time > 1.0f)
	{
		gs->ball_moving = true;
	}

	/******* UPDATE *******/

	// COMPUTER PADDLE AI
	float computer_paddle_target_offset = ((float) rand() / (float) RAND_MAX) / 2.0f;

	if (gs->ball.is_moving_right)
	{
		if (gs->computer.position.y > gs->ball.position.y + computer_paddle_target_offset)
		{
			gs->computer.y_offset -= gs->computer_paddle_y_velocity * gs->delta_time;
		}
		else if (gs->computer.position.y < gs->ball.position.y - computer_paddle_target_offset)
		{
			gs->computer.y_offset += gs->computer_paddle_y_velocity * gs->delta_time;
		}
	}
	else
	{
		if (gs->computer.position.y > 0.0f)
		{
			gs->computer.y_offset -= gs->computer_paddle_y_velocity * gs->delta_time;
		}
		else if (gs->computer.position.y < 0.0f)
		{
			gs->computer.y_offset += gs->computer_paddle_y_velocity * gs->delta_time;
		}
	}

	gs->player.position.y = gs->player.y_offset;
	gs->computer.position.y = gs->computer.y_offset;

	if (gs->ball_moving)
	{
		calculate_ball_position(gs, gs->ball.width, gs->ball.height);
	}

	// COLLISION DETECTION GOES HERE
	if (gs->ball.is_moving_right)
	{
		handle_collision(&gs->ball, &gs->computer, gs->ball.width, gs->ball.height, gs->computer.width, gs->computer.height);
	}
	else
	{
		handle_collision(&gs->ball, &gs->player, gs->ball.width, gs->ball.height, gs->player.width, gs->player.height);
	}

	/******* END UPDATE *******/

	/******* RENDER *******/

	// Drawing paddles and ball
	glUseProgram(shader_program_id);

	glBindVertexArray(gs->player.vao);
	glUniform2f(glGetUniformLocation(shader_program_id, "position_offset"), gs->player.position.x, gs->player.position.y);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(gs->computer.vao);
	glUniform2f(glGetUniformLocation(shader_program_id, "position_offset"), gs->computer.position.x, gs->computer.position.y);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(gs->ball.vao);
	glUniform2f(glGetUniformLocation(shader_program_id, "position_offset"), gs->ball.position.x, gs->ball.position.y);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

	/******* END RENDER *******/
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

void calculate_ball_position(Game_State* gs, GLfloat ball_width, GLfloat ball_height)
{
	if (gs->ball.is_moving_right)
	{
		gs->ball.position.x += gs->ball.x_speed * gs->delta_time;
		if (gs->ball.position.x > (1.0f - (ball_width / 2)))
		{
			gs->player_score++;
			if (gs->player_score == 5)
			{
				game_in_progress = false;
			}
			reset_ball(gs);
			printf("PLAYER: %d CPU: %d\n", gs->player_score, gs->computer_score);
		}
	}
	else
	{
		gs->ball.position.x -=  gs->ball.x_speed * gs->delta_time;
		if (gs->ball.position.x < (-1.0f + (ball_width / 2)))
		{
			gs->computer_score++;
			if (gs->computer_score == 5)
			{
				game_in_progress = false;
			}
			reset_ball(gs);
			printf("PLAYER: %d CPU: %d\n", gs->player_score, gs->computer_score);
		}
	}

	gs->ball.position.y += gs->ball.y_speed * gs->delta_time;

	if (gs->ball.position.y > (1.0f - (ball_height / 2)) ||
		gs->ball.position.y < (-1.0f + (ball_height / 2)))
	{
		gs->ball.y_speed *= -1.0f;
	}
}

void reset_ball(Game_State* gs)
{
	gs->ball.position.x = 0.0f;
	gs->ball.position.y = 0.0f;
	gs->ball.x_speed = 0.008f;
	gs->ball_moving = false;
	gs->ball_pause_time = glfwGetTime();
}

void handle_collision(Ball* ball, Paddle* paddle, float ball_width, float ball_height, float paddle_width, float paddle_height)
{
	const float dir_one_offset = -0.0125f;
	const float dir_two_offset = -0.0075f;
	const float dir_three_offset = 0.0f;
	const float dir_four_offset = 0.0075f;
	const float dir_five_offset = 0.0125f;

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

void reset_game(Game_State* gs)
{
	gs->player_score = 0;
	gs->computer_score = 0;
	gs->ball_moving = false;
	gs->player.y_offset = 0.0f;
	gs->computer.y_offset = 0.0f;

	printf("PRESS 'N' TO START A NEW GAME OR 'Q' TO QUIT\n");
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	if (keys[GLFW_KEY_N])
	{
		if (!game_in_progress)
		{
			game_in_progress = true;
		}
	}

	if (keys[GLFW_KEY_Q])
	{
		if (!game_in_progress)
		{
			glfwSetWindowShouldClose(window, true);
		}
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

void handle_player_keyboard_input(Game_State* gs)
{
	if (keys[GLFW_KEY_W])
	{
		gs->player.y_offset += gs->player_paddle_y_velocity * gs->delta_time;
	}
	else if (keys[GLFW_KEY_S])
	{
		gs->player.y_offset -= gs->player_paddle_y_velocity * gs->delta_time;
	}
}

void handle_player_controller_input(Game_State* gs)
{
	if (glfwJoystickPresent(GLFW_JOYSTICK_1))
	{
		int size;
		const unsigned char* results = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &size);
		int button_pressed = get_current_button_pressed(results, size);

		if (button_pressed == X360_DPAD_UP)
		{
			gs->player.y_offset += gs->player_paddle_y_velocity * gs->delta_time;
		}
		else if (button_pressed == X360_DPAD_DOWN)
		{
			gs->player.y_offset -= gs->player_paddle_y_velocity * gs->delta_time;
		}
	}
}