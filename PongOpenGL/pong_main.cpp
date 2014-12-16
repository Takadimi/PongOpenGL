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

	Score_Image score_sprite_sheet;
	load_image(&score_sprite_sheet, "pong_scores.png");

	GLuint number_one_texture = create_sprite(score_sprite_sheet.image_data, score_sprite_sheet.width, score_sprite_sheet.height);
	dispose_of_image(score_sprite_sheet.image_data);

	GLuint vert_shader_id = buildShader("shader.vert", GL_VERTEX_SHADER);
	GLuint frag_shader_id = buildShader("shader.frag", GL_FRAGMENT_SHADER);
	GLuint shader_program_id = buildProgram(vert_shader_id, frag_shader_id);

	glDeleteShader(vert_shader_id);
	glDeleteShader(frag_shader_id);

	GLuint sprite_vert_shader_id = buildShader("sprite_shader.vert", GL_VERTEX_SHADER);
	GLuint sprite_frag_shader_id = buildShader("sprite_shader.frag", GL_FRAGMENT_SHADER);
	GLuint sprite_shader_program_id = buildProgram(sprite_vert_shader_id, sprite_frag_shader_id);

	glDeleteShader(sprite_vert_shader_id);
	glDeleteShader(sprite_frag_shader_id);

	GLfloat score_card_vertices[] = {
		-0.3f,  0.3f, 0.0f, 1.0f,
		 0.3f,  0.3f, 1.0f, 1.0f,
		 0.3f, -0.3f, 1.0f, 0.0f,
		-0.3f, -0.3f, 0.0f, 0.0f
	};

	GLuint score_card_indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	Score_Sprite number_one;
	number_one.vao = build_sprite_vao(score_card_vertices, sizeof(score_card_vertices), score_card_indices, sizeof(score_card_indices));
	number_one.texture = number_one_texture;

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

	Paddle paddle_player;
	init_paddle(&paddle_player, paddle_vertices, sizeof(paddle_vertices), paddle_indices, sizeof(paddle_indices));
	paddle_player.origin = glm::vec2(-0.95f, 0.0f);
	
	Paddle paddle_computer;
	init_paddle(&paddle_computer, paddle_vertices, sizeof(paddle_vertices), paddle_indices, sizeof(paddle_indices));
	paddle_computer.origin = glm::vec2(0.95f, 0.0f);

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

	Ball ball;
	init_ball(&ball, ball_vertices, sizeof(ball_vertices), ball_indices, sizeof(ball_indices));
	ball.origin = glm::vec2(0.0f, 0.0f);
	ball.current_pos = ball.origin;
	ball.is_moving_right = false;
	ball.is_moving_up = true;

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
			ball.is_moving_right = false;
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

		// Drawing score card texture
		glUseProgram(sprite_shader_program_id);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, number_one.texture);
		glUniform1i(glGetUniformLocation(sprite_shader_program_id, "sprite_texture"), 0);

		glBindVertexArray(number_one.vao);
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

// TODO (Ethan): Factor this back into the build_vao function by adding arguments for attrib pointer size
GLuint build_sprite_vao(GLfloat* vertices, GLuint vertices_size, GLuint* indices, GLuint indices_size)
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

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	return vao;
}

void init_paddle(Paddle* paddle, GLfloat* vertices, GLuint vertices_size, GLuint* indices, GLuint indices_size)
{
	paddle->vao = build_vao(vertices, vertices_size, indices, indices_size);
}

void init_ball(Ball* ball, GLfloat* vertices, GLuint vertices_size, GLuint* indices, GLuint indices_size)
{
	ball->vao = build_vao(vertices, vertices_size, indices, indices_size);
}

void calculate_ball_position(Ball* ball, GLfloat delta_time, GLfloat ball_width, GLfloat ball_height)
{
	if (ball->is_moving_right)
	{
		ball->current_pos.x -=  ball_horizontal_speed * delta_time;
		if (ball->current_pos.x < (-1.0f + (ball_width / 2)))
		{
			ball->is_moving_right = false;
			ball_horizontal_speed += ball_horizontal_speed_increment;
		}
	}
	else
	{
		ball->current_pos.x += ball_horizontal_speed * delta_time;
		if (ball->current_pos.x > (1.0f - (ball_width / 2)))
		{
			ball->is_moving_right = true;
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

void load_image(Score_Image* image, const char* texture_file_path)
{
	image->image_data = SOIL_load_image(texture_file_path, &(image->width), &(image->height), 0, SOIL_LOAD_RGBA);

	if (image->image_data == NULL)
	{
		printf("Error loading score sprite sheet!\n");
	}
}

GLuint create_sprite(unsigned char* image_byte_array, const unsigned int width_offset, const unsigned int height_offset)
{
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_offset, height_offset, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_byte_array);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	return texture;
}

void dispose_of_image(unsigned char* image_byte_array)
{
	SOIL_free_image_data(image_byte_array);
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