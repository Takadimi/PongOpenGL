#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#define MONITOR_WIDTH 1920
#define MONITOR_HEIGHT 1080
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// GLOBALS
float delta_time = 0.0f;
float last_time = 0.0f;
float player_paddle_y_offset = 0.0f;
float computer_paddle_y_offset = 0.0f;
float player_paddle_y_velocity = 0.015f;
float computer_paddle_y_velocity = 0.015f;
bool keys[1024];

enum Paddle_Type
{
	PLAYER,
	COMPUTER,
	NUM_PADDLE_TYPES
};

struct Paddle
{
	Paddle_Type type;
	GLuint vao;
	glm::vec2 position;
};

struct Ball
{
	GLuint vao;
	glm::vec2 position;
	float x_speed;
	float y_speed;
	bool is_moving_right;
	bool is_moving_up;
};

GLuint build_vao(GLfloat* vertices, GLuint vertices_size);
GLuint build_vao(GLfloat* vertices, GLuint vertices_size, GLuint* indices, GLuint indices_size);
void calculate_ball_position(Ball* ball, GLfloat delta_time, GLfloat ball_width, GLfloat ball_height);
void handle_collision(Ball* ball, Paddle* paddle, float ball_width, float ball_height, float paddle_width, float paddle_height);
bool is_intersecting_on_y_axis(Ball* ball, Paddle* paddle, float ball_height, float paddle_height);
bool is_intersecting_on_x_axis(Ball* ball, Paddle* paddle, float ball_width, float paddle_width);
void key_callback(GLFWwindow*, int, int, int, int);
void handle_player_keyboard_input();
void handle_player_controller_input();
