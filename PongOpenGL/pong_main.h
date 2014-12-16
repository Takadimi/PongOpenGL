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
	OBJECT_COUNT
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
	bool is_moving_right;
	bool is_moving_up;
};

struct Score_Image
{
	GLint width;
	GLint height;
	unsigned char* image_data;
};

struct Score_Sprite
{
	GLuint vao;
	GLuint texture;
};
GLuint build_vao(Vao_Data data);
GLuint build_vao(GLfloat* vertices, GLuint vertices_size);
GLuint build_vao(GLfloat* vertices, GLuint vertices_size, GLuint* indices, GLuint indices_size);
GLuint build_sprite_vao(GLfloat* vertices, GLuint vertices_size, GLuint* indices, GLuint indices_size);
void init_paddle(Paddle* paddle, GLfloat* vertices, GLuint vertices_size, GLuint* indices, GLuint indices_size);
void init_ball(Ball* ball, GLfloat* vertices, GLuint vertices_size, GLuint* indices, GLuint indices_size);
void calculate_ball_position(Ball* ball, GLfloat delta_time, GLfloat ball_width, GLfloat ball_height);
void load_image(Score_Image* texture, const char* texture_file_path);
GLuint create_sprite(unsigned char* image_byte_array, const unsigned int width_offset, const unsigned int height_offset);
void dispose_of_image(unsigned char* image_byte_array);
void key_callback(GLFWwindow*, int, int, int, int);
void handle_player_keyboard_input();
void handle_player_controller_input();
