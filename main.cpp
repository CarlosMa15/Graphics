#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include<string>
#include <math.h>

#define M_PI 3.141592654f

// Theses are fields I used to control the mode of the program
// And also to save the previous point of the program
// And also to keep track of which point you are at.
std::string mode = "Line";	// Normal, Line, Circle
int counter = 0;				// 0 First Point, 1 Second Point
int firstX = 0;					// The First X Point If Needed
int firstY = 0;				// The First Y Point If Needed

unsigned int g_windowWidth = 600;
unsigned int g_windowHeight = 600;
char* g_windowName = "HW2-Rasterization";

GLFWwindow* g_window;

const int g_image_width = g_windowWidth;
const int g_image_height = g_windowHeight;

std::vector<float> g_image;

void putPixel(int x, int y)
{
	// clamp
	if (x >= g_image_width || x < 0 || y >= g_image_height || y < 0) return;

	// write
	g_image[y * g_image_width + x] = 1.0f;
}

//-------------------------------------------------------------------------------

/*****************************************************
* This Method returns the distance between two point *
*****************************************************/
int distance(int x1, int y1, int x2, int y2) {
	float distance = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2) * 1.0);
	return distance;
}

/********************************************************
* This Method draws the circle point given an x and a y *
********************************************************/
void circlePoints(int x1, int y1, int x, int y)
{
	putPixel(x1 + x, y1 + y);
	putPixel(x1 - x, y1 + y);
	putPixel(x1 + x, y1 - y);
	putPixel(x1 - x, y1 - y);
	putPixel(x1 + y, y1 + x);
	putPixel(x1 - y, y1 + x);
	putPixel(x1 + y, y1 - x);
	putPixel(x1 - y, y1 - x);
}

/************************************************************
* This Method returns the absolute value of the given value *
************************************************************/
int abs(int i)
{
	return i < 0 ? -i : i;
}

/*********************************************************************
* This Method takes care or drawing a line using Bresenham Algorithm *
*********************************************************************/
void drawLine(int x1, int y1, int x2, int y2)
{
	//// TODO: part of Homework Task 1
	//// This function should draw a line from pixel (x1, y1) to pixel (x2, y2)
	//// This draws lines using Bresenham Algorithm
	//// It takes care of all 8 possible care of the line

	int dx = x2 - x1;
	int dy = y2 - y1;
	int dx1 = 0, dy1 = 0, dx2 = 0, dy2 = 0;
	if (dx < 0) dx1 = -1; else if (dx > 0) dx1 = 1;
	if (dy < 0) dy1 = -1; else if (dy > 0) dy1 = 1;
	if (dx < 0) dx2 = -1; else if (dx > 0) dx2 = 1;
	int longest = abs(dx);
	int shortest = abs(dy);
	if (!(longest > shortest)) {
		longest = abs(dy);
		shortest = abs(dx);
		if (dy < 0) dy2 = -1; else if (dy > 0) dy2 = 1;
		dx2 = 0;
	}
	int numerator = longest >> 1;
	for (int i = 0; i <= longest; i++) {
		putPixel(x1, y1);
		numerator += shortest;
		if (!(numerator < longest)) {
			numerator -= longest;
			x1 += dx1;
			y1 += dy1;
		}
		else {
			x1 += dx2;
			y1 += dy2;
		}
	}
}

/***********************************************************************
* This Method takes care or drawing a circle using Bresenham Algorithm *
***********************************************************************/
void drawCircle(int x0, int y0, int R)
{
	// TODO: part of Homework Task 2
	// This function should draw a circle,
	// where pixel (x0, y0) is the center of the circle and R is the radius
	int x = 0, y = R;
	int decision = 3 - 2 * R;
	circlePoints(x0, y0, x, y);
	while (y >= x){
		x++;
		if (decision > 0)
		{
			y--;
			decision = decision + 4 * (x - y) + 10;
		}
		else
			decision = decision + 4 * x + 6;
		circlePoints(x0, y0, x, y);
	}
}

struct line
{
	int x1, x2, y1, y2;
	// TODO: part of Homework Task 3
	// This function should initialize the variables of struct line

	void init() { }	// SNEHA BHAKARE said "The purpose of the circle and line structs is to handle the draw
					// functions during mouse clicks. This is just one way of writing code and definitely
					// not the only way. Using these structs is optional but recommended. I hope this helps!"
					// I choose not to use then

	// This function should update the values of member variables and draw a line when 2 points are clicked. 

	void AddPoint(int x, int y) { } // SNEHA BHAKARE said "The purpose of the circle and line structs is to handle the draw
									// functions during mouse clicks. This is just one way of writing code and definitely
									// not the only way. Using these structs is optional but recommended. I hope this helps!"
									// I choose not to use then
};

struct circle
{
	int x0, y0, R;
	// TODO: part of Homework Task 3
	// This function should initialize the variables of struct circle

	void init() { } // SNEHA BHAKARE said "The purpose of the circle and line structs is to handle the draw
					// functions during mouse clicks. This is just one way of writing code and definitely
					// not the only way. Using these structs is optional but recommended. I hope this helps!"
					// I choose not to use then

	// This function should update the values of member variables and draw a circle when 2 points are clicked
	// The first clicked point should be the center of the circle
	// The second clicked point should be a point on the circle

	void AddPoint(int x, int y) { } // SNEHA BHAKARE said "The purpose of the circle and line structs is to handle the draw
									// functions during mouse clicks. This is just one way of writing code and definitely
									// not the only way. Using these structs is optional but recommended. I hope this helps!"
									// I choose not to use then
};

void glfwErrorCallback(int error, const char* description)
{
	std::cerr << "GLFW Error " << error << ": " << description << std::endl;
	exit(1);
}

void glfwKeyCallback(GLFWwindow* p_window, int p_key, int p_scancode, int p_action, int p_mods)
{
	if (p_key == GLFW_KEY_ESCAPE && p_action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(g_window, GL_TRUE);
	}
	else if (p_key == GLFW_KEY_L && p_action == GLFW_PRESS)
	{
		std::cout << "Line mode" << std::endl;
		// TODO: part of Homework Task 3
		// This part switch on the line mode
		mode = "Line";
	}
	else if (p_key == GLFW_KEY_C && p_action == GLFW_PRESS)
	{
		std::cout << "Circle mode" << std::endl;
		// TODO: part of Homework Task 3
		// This part should switch on the circle mode
		mode = "Circle";
	}
}

//!GLFW mouse callback
void glfwMouseButtonCallback(GLFWwindow* p_window, int p_button, int p_action, int p_mods)
{
	double xpos, ypos;
	glfwGetCursorPos(p_window, &xpos, &ypos);
	ypos = g_windowHeight - ypos;
	if (p_button == GLFW_MOUSE_BUTTON_LEFT && p_action == GLFW_PRESS)
	{
		std::cout << "You clicked pixel: " << xpos << ", " << ypos << std::endl;
		// TODO: part of Homework Task 3
		// This part should draw appropriate figure according to the current mode

		if (counter == 0) {
			putPixel(xpos, ypos);
			firstX = xpos;
			firstY = ypos;
			counter++;
		}
		else if (mode == "Line" && counter == 1) {
			drawLine(firstX, firstY, xpos, ypos);
			mode = "Normal";
			firstX = 0;
			firstY = 0;
			counter = 0;
		}
		else if (mode == "Circle" && counter == 1) {
			int R = distance(firstX, firstY, xpos, ypos);
			drawCircle(firstX, firstY, R);
			mode = "Normal";
			firstX = 0;
			firstY = 0;
			counter = 0;
		}


	}
}

//-------------------------------------------------------------------------------

struct color
{
	unsigned char r, g, b;
};

int ReadLine(FILE* fp, int size, char* buffer)
{
	int i;
	for (i = 0; i < size; i++) {
		buffer[i] = fgetc(fp);
		if (feof(fp) || buffer[i] == '\n' || buffer[i] == '\r') {
			buffer[i] = '\0';
			return i + 1;
		}
	}
	return i;
}

void initWindow()
{
	// initialize GLFW
	glfwSetErrorCallback(glfwErrorCallback);
	if (!glfwInit())
	{
		std::cerr << "GLFW Error: Could not initialize GLFW library" << std::endl;
		exit(1);
	}

	g_window = glfwCreateWindow(g_windowWidth, g_windowHeight, g_windowName, NULL, NULL);
	if (!g_window)
	{
		glfwTerminate();
		std::cerr << "GLFW Error: Could not initialize window" << std::endl;
		exit(1);
	}

	// callbacks
	glfwSetKeyCallback(g_window, glfwKeyCallback);
	glfwSetMouseButtonCallback(g_window, glfwMouseButtonCallback);

	// Make the window's context current
	glfwMakeContextCurrent(g_window);

	// turn on VSYNC
	glfwSwapInterval(1);
}

void initGL()
{
	glClearColor(1.f, 1.f, 1.f, 1.0f);
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawPixels(g_image_width, g_image_height, GL_LUMINANCE, GL_FLOAT, &g_image[0]);
}

void renderLoop()
{
	while (!glfwWindowShouldClose(g_window))
	{
		// clear buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		render();

		// Swap front and back buffers
		glfwSwapBuffers(g_window);

		// Poll for and process events
		glfwPollEvents();
	}
}

void initImage()
{
	g_image.resize(g_image_width * g_image_height);
}

bool writeImage()
{
	std::vector<color> tmpData;
	tmpData.resize(g_image_width * g_image_height);

	for (int i = 0; i < g_image_height; i++)
	{
		for (int j = 0; j < g_image_width; j++)
		{
			// make sure the value will not be larger than 1 or smaller than 0, which might cause problem when converting to unsigned char
			float tmp = g_image[i * g_image_width + j];
			if (tmp < 0.0f)	tmp = 0.0f;
			if (tmp > 1.0f)	tmp = 1.0f;

			tmpData[(g_image_height - i - 1) * g_image_width + j].r = unsigned char(tmp * 255.0);
			tmpData[(g_image_height - i - 1) * g_image_width + j].g = unsigned char(tmp * 255.0);
			tmpData[(g_image_height - i - 1) * g_image_width + j].b = unsigned char(tmp * 255.0);
		}
	}

	FILE* fp = fopen("data/out.ppm", "wb");
	if (!fp) return false;

	fprintf(fp, "P6\r");
	fprintf(fp, "%d %d\r", g_image_width, g_image_height);
	fprintf(fp, "255\r");
	fwrite(tmpData.data(), sizeof(color), g_image_width * g_image_height, fp);
	fclose(fp);

	return true;
}

void drawImage()
{
	drawLine(150, 10, 450, 10);
	drawLine(150, 310, 450, 310);
	drawLine(150, 10, 150, 310);
	drawLine(450, 10, 450, 310);
	drawLine(150, 310, 300, 410);
	drawLine(300, 410, 450, 310);

	drawCircle(500, 500, 50);
}

int main()
{
	initImage();
	drawImage();
	writeImage();

	// render loop
	initWindow();
	initGL();
	renderLoop();

	return 0;
}
