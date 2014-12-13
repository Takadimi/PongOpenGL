#ifndef GLFW_X360_CONTROLLER_H
#define GLFW_X360_CONTROLLER_H

#define X360_A_BUTTON				0
#define X360_B_BUTTON				1
#define X360_X_BUTTON				2
#define X360_Y_BUTTON				3
#define X360_LEFT_SHOULDER_BUTTON	4
#define X360_RIGHT_SHOULDER_BUTTON	5
#define X360_BACK_BUTTON			6
#define X360_START_BUTTON			7
#define X360_LEFT_STICK_CLICK		8
#define X360_RIGHT_STICK_CLICK		9
#define X360_DPAD_UP				10
#define X360_DPAD_RIGHT				11
#define X360_DPAD_DOWN				12
#define X360_DPAD_LEFT				13

int get_current_button_pressed(const unsigned char* results, const int size_of_results)
{
	for (int i = 0; i < size_of_results; i++)
	{
		if (results[i])
		{
			return i;
		}
	}

	// If no button is found, return -1 to signify that
	return 0;
}

#endif