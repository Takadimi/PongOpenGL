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
float left_paddle_y_offset = 0.0f;
float ball_horizontal_speed = 0.005f;
float ball_horizontal_speed_increment = 0.001f;
bool keys[1024];

enum Game_Object {
	PADDLE,
	BALL,
	OBJECT_TYPE_COUNT
};

struct Vao_Data
{
	GLfloat* vertices;
	GLuint vertices_size;
	GLuint* indices;
	GLuint indices_size;
};

struct Paddle
{
	GLuint vao;
	glm::vec2 origin;
	glm::vec2 current_pos;
};

struct Ball
{
	GLuint vao;
	glm::vec2 origin;
	glm::vec2 current_pos;
	bool is_moving_left;
	bool is_moving_up;
};

GLuint build_vao(Vao_Data data);
GLuint build_vao(GLfloat* vertices, GLuint vertices_size);
GLuint build_vao(GLfloat* vertices, GLuint vertices_size, GLuint* indices, GLuint indices_size);
void calculate_ball_position(Ball* ball, GLfloat delta_time, GLfloat ball_width, GLfloat ball_height);
void key_callback(GLFWwindow*, int, int, int, int);
void handle_player_keyboard_input();
void handle_player_controller_input();
