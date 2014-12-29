#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#define MONITOR_WIDTH 1920
#define MONITOR_HEIGHT 1080
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// GLOBALS
bool game_in_progress = false;
bool keys[1024];
GLuint shader_program_id;

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
	float width;
	float height;
	glm::vec2 position;
	float y_offset;
};

struct Ball
{
	GLuint vao;
	float width;
	float height;
	glm::vec2 position;
	float x_speed;
	float y_speed;
	bool is_moving_right;
	bool is_moving_up;
};

struct Game_State
{
	float delta_time					= 0.0f;
	float ball_pause_time				= 0.0f;
	float last_time						= 0.0f;
	unsigned int player_score			= 0;
	unsigned int computer_score			= 0;
	bool ball_moving					= false;
	float player_paddle_y_velocity		= 0.015f;
	float computer_paddle_y_velocity	= 0.02055f;
	Paddle player;
	Paddle computer;
	Ball ball;
};

void game_update_and_render(Game_State* game_state);
GLuint build_vao(GLfloat* vertices, GLuint vertices_size);
GLuint build_vao(GLfloat* vertices, GLuint vertices_size, GLuint* indices, GLuint indices_size);
void calculate_ball_position(Game_State* gs, GLfloat ball_width, GLfloat ball_height);
void reset_ball(Game_State* gs);
void handle_collision(Ball* ball, Paddle* paddle, float ball_width, float ball_height, float paddle_width, float paddle_height);
bool is_intersecting_on_y_axis(Ball* ball, Paddle* paddle, float ball_height, float paddle_height);
bool is_intersecting_on_x_axis(Ball* ball, Paddle* paddle, float ball_width, float paddle_width);
void reset_game(Game_State* gs);
void key_callback(GLFWwindow*, int, int, int, int);
void handle_player_keyboard_input(Game_State* gs);
void handle_player_controller_input(Paddle* player);
