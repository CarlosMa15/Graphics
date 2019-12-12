#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <map>

#define M_PI 3.141592654f

unsigned int g_windowWidth = 800;
unsigned int g_windowHeight = 600;
char* g_windowName = "HW3-3D-Basics";

GLFWwindow* g_window;

// Model data
std::vector<float> g_meshVertices;
std::vector<float> g_meshNormals;
std::vector<unsigned int> g_meshIndices;
GLfloat g_modelViewMatrix[16];

// Default options
bool enablePersp = true;
bool teapotSpin = false;
bool enableDolly = false;
bool showCheckerboard = false;

// Dolly zoom options 
float fov = M_PI / 4.f;
float distance = 4.5f;
float halfWidth = distance * std::tan(fov / 2);

// Auxiliary math functions
float dotProduct(const float* a, const float* b)
{
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

void crossProduct(const float* a, const float* b, float* r)
{
	r[0] = a[1] * b[2] - a[2] * b[1];
	r[1] = a[2] * b[0] - a[0] * b[2];
	r[2] = a[0] * b[1] - a[1] * b[0];
}

float radianToDegree(float angleInRadian) {
	return angleInRadian * 180.f / M_PI;
}

void normalize(float* a)
{
	const float len = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);

	a[0] /= len;
	a[1] /= len;
	a[2] /= len;
}

// This method returns the requested vector
std::vector<float> getVertexByIndex(int i) {
	std::vector<float> vertex;
	vertex.push_back(g_meshVertices[i * 3 + 0]);
	vertex.push_back(g_meshVertices[i * 3 + 1]);
	vertex.push_back(g_meshVertices[i * 3 + 2]);
	return vertex;
}

// This method returns the requested normal vector
std::vector<float> getVertexNormalByIndex(int i) {
	std::vector<float> vertex;
	vertex.push_back(g_meshNormals[i * 3 + 0]);
	vertex.push_back(g_meshNormals[i * 3 + 1]);
	vertex.push_back(g_meshNormals[i * 3 + 2]);
	return vertex;
}

// This method returns the difference of two vectors
std::vector<float> vertexDifference(std::vector<float> first, std::vector<float> last) {
	std::vector<float> difference;
	difference.push_back(first[0] - last[0]);
	difference.push_back(first[1] - last[1]);
	difference.push_back(first[2] - last[2]);
	return difference;
}

// This method returns the sum of two vectors
std::vector<float> vertexSum(std::vector<float> first, std::vector<float> last) {
	std::vector<float> sum;
	sum.push_back(first[0] + last[0]);
	sum.push_back(first[1] + last[1]);
	sum.push_back(first[2] + last[2]);
	return sum;
}

// This method returns the cross product of 2 vectors
std::vector<float> vertexCrossProduct(std::vector<float> first, std::vector<float> last) {
	std::vector<float> crossProduct;
	crossProduct.push_back((first[1] * last[2]) - (first[2] * last[1]));
	crossProduct.push_back((first[2] * last[0]) - (first[0] * last[2]));
	crossProduct.push_back((first[0] * last[1]) - (first[1] * last[0]));
	return crossProduct;
}

// This method returns the normalized vector
std::vector<float> vertextNormalize(std::vector<float> vertex) {
	std::vector<float> normalized;
	float first = vertex[0];
	float second = vertex[1];
	float third = vertex[2];
	float norm = std::sqrt((first * first) + (second * second) + (third * third));
	normalized.push_back(first / norm);
	normalized.push_back(second / norm);
	normalized.push_back(third / norm);
	return normalized;
}

void computeNormals()
{
	g_meshNormals.resize(g_meshVertices.size());

	// TASK 1

	// Sets Everthing To Zero
	for (int i = 0; i < g_meshNormals.size(); i++) {
		g_meshNormals[i] = 0;
	}

	// Create a counter starting with zero at each vertex
	std::map<int, int> indicesCounter;
	for (int i = 0; i < (g_meshVertices.size() / 3); i++) {
		indicesCounter[i] = 0;
	}

	// Iterate through each Triangle
	for (int i = 0; i < g_meshIndices.size() / 3; i++) {

		// Get the vertex index
		int firstVertexIndex = g_meshIndices[i * 3 + 0];
		int secondVertexIndex = g_meshIndices[i * 3 + 1];
		int thirdVertexIndex = g_meshIndices[i * 3 + 2];

		// Increment counters
		indicesCounter[firstVertexIndex] = indicesCounter[firstVertexIndex] + 1;
		indicesCounter[secondVertexIndex] = indicesCounter[secondVertexIndex] + 1;
		indicesCounter[thirdVertexIndex] = indicesCounter[thirdVertexIndex] + 1;

		// Get the vertex
		std::vector<float> P = getVertexByIndex(firstVertexIndex);
		std::vector<float> Q = getVertexByIndex(secondVertexIndex);
		std::vector<float> R = getVertexByIndex(thirdVertexIndex);

		// Get Vectors
		std::vector<float> PQ = vertexDifference(Q, P);
		std::vector<float> PR = vertexDifference(R, P);

		// Get Vectors Cross Product / Normal Vector / Normalized
		std::vector<float> normal = vertexCrossProduct(PQ, PR);
		std::vector<float> normalized = vertextNormalize(normal);

		// Added the results
		std::vector<float> firstNormal = getVertexNormalByIndex(firstVertexIndex);
		std::vector<float> sum1 = vertexSum(firstNormal, normalized);
		g_meshNormals[firstVertexIndex * 3 + 0] = sum1[0];
		g_meshNormals[firstVertexIndex * 3 + 1] = sum1[1];
		g_meshNormals[firstVertexIndex * 3 + 2] = sum1[2];

		// Added the results
		std::vector<float> secondNormal = getVertexNormalByIndex(secondVertexIndex);
		std::vector<float> sum2 = vertexSum(secondNormal, normalized);
		g_meshNormals[secondVertexIndex * 3 + 0] = sum2[0];
		g_meshNormals[secondVertexIndex * 3 + 1] = sum2[1];
		g_meshNormals[secondVertexIndex * 3 + 2] = sum2[2];

		// Added the results
		std::vector<float> thirdNormal = getVertexNormalByIndex(thirdVertexIndex);
		std::vector<float> sum3 = vertexSum(thirdNormal, normalized);
		g_meshNormals[thirdVertexIndex * 3 + 0] = sum3[0];
		g_meshNormals[thirdVertexIndex * 3 + 1] = sum3[1];
		g_meshNormals[thirdVertexIndex * 3 + 2] = sum3[2];
	}

	// Getting the avergae of each
	for (int i = 0; i < g_meshNormals.size() / 3; i++) {
		int count = indicesCounter[i];
		g_meshNormals[i * 3 + 0] = g_meshNormals[i * 3 + 0] / count;
		g_meshNormals[i * 3 + 1] = g_meshNormals[i * 3 + 1] / count;
		g_meshNormals[i * 3 + 2] = g_meshNormals[i * 3 + 2] / count;
	}

}

void loadObj(std::string p_path)
{
	std::ifstream nfile;
	nfile.open(p_path);
	std::string s;

	while (nfile >> s)
	{
		if (s.compare("v") == 0)
		{
			float x, y, z;
			nfile >> x >> y >> z;
			g_meshVertices.push_back(x);
			g_meshVertices.push_back(y);
			g_meshVertices.push_back(z);
		}
		else if (s.compare("f") == 0)
		{
			std::string sa, sb, sc;
			unsigned int a, b, c;
			nfile >> sa >> sb >> sc;

			a = std::stoi(sa);
			b = std::stoi(sb);
			c = std::stoi(sc);

			g_meshIndices.push_back(a - 1);
			g_meshIndices.push_back(b - 1);
			g_meshIndices.push_back(c - 1);
		}
		else
		{
			std::getline(nfile, s);
		}
	}

	computeNormals();

	std::cout << p_path << " loaded. Vertices: " << g_meshVertices.size() / 3 << " Triangles: " << g_meshIndices.size() / 3 << std::endl;
}

double getTime()
{
	return glfwGetTime();
}

void glfwErrorCallback(int error, const char* description)
{
	std::cerr << "GLFW Error " << error << ": " << description << std::endl;
	exit(1);
}

void togglePerspective() {

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Perspective Projection
	if (enablePersp)
	{
		// Dolly zoom computation
		if (enableDolly) {
			// TASK 3
			// Your code for dolly zoom computation goes here
			// You can use getTime() to change fov over time
			// distance should be recalculated here using the Equation 2 in the description file
			
			fov = std::abs(std::sin(getTime())) + .1;
			distance = std::abs(halfWidth / std::tan(fov / 2));
		}

		float fovInDegree = radianToDegree(fov);
		gluPerspective(fovInDegree, (GLfloat)g_windowWidth / (GLfloat)g_windowHeight, 1.0f, distance  + 40);
	} 

	// Othogonal Projection
	else
	{
		// Scale down the object for a better view in orthographic projection
		glScalef(0.5, 0.5, 0.5);

		// TASK 3
		// Your code for orthogonal projection goes here
		// (Hint: you can use glOrtho() function in OpenGL)
		glOrtho(-1.2f, 1.2f, -.9f, .9f, 1.0f, 40.f);
	}
}

void glfwKeyCallback(GLFWwindow* p_window, int p_key, int p_scancode, int p_action, int p_mods)
{
	if (p_key == GLFW_KEY_ESCAPE && p_action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(g_window, GL_TRUE);
	}
	if (p_key == GLFW_KEY_P && p_action == GLFW_PRESS) {

		// Perspective Projection
		enablePersp = true;
		togglePerspective();
		std::cout << "Perspective activated\n";

	}
	if (p_key == GLFW_KEY_O && p_action == GLFW_PRESS) {

		// Orthographic Projection
		enablePersp = false;
		togglePerspective();
		std::cout << "Orthogonal activated\n";

	}
	if (p_key == GLFW_KEY_S && p_action == GLFW_PRESS) {

		// Toggle Spinning
		if (!teapotSpin) {
			std::cout << "Teapot spinning on\n";
		}
		else {
			std::cout << "Teapot spinning off\n";
		}
		teapotSpin = !teapotSpin;
	}
	if (p_key == GLFW_KEY_D && p_action == GLFW_PRESS) {

		// Toggle dolly zoom
		if (!enableDolly)
		{
			std::cout << "Dolly zoom on\n";
		}
		else {
			std::cout << "Dolly zoom off\n";
		}
		enableDolly = !enableDolly;
	}
	if (p_key == GLFW_KEY_C && p_action == GLFW_PRESS) {

		// Show/hide Checkerboard
		if (!showCheckerboard)
		{
			std::cout << "Show checkerboard\n";
		}
		else {
			std::cout << "Hide checkerboard\n";
		}
		showCheckerboard = !showCheckerboard;
	}
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

	// Make the window's context current
	glfwMakeContextCurrent(g_window);

	// turn on VSYNC
	glfwSwapInterval(1);
}

void initGL()
{
	glClearColor(1.f, 1.f, 1.f, 1.0f);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
}

void printHotKeys() {
	std::cout << "\nHot Keys..\n"
		<< "Orthogonal Projection:  O\n"
		<< "Perspective Projection: P\n"
		<< "Toggle Spinning:        S\n"
		<< "Toggle Dolly Zoom:      D\n"
		<< "Show/hide Checkerboard: C\n"
		<< "Exit:                   Esc\n\n";
}

void clearModelViewMatrix()
{
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			g_modelViewMatrix[4 * i + j] = 0.0f;
		}
	}
}

void updateModelViewMatrix()
{
	clearModelViewMatrix();

	// TASK 2

	// This sets the starting point
	g_modelViewMatrix[0] = 1.0f;
	g_modelViewMatrix[5] = 1.0f;
	g_modelViewMatrix[10] = 1.0f;
	g_modelViewMatrix[14] = -distance;
	g_modelViewMatrix[15] = 1.0f;

	if (teapotSpin) {

		// Matrix Multiplication
		double time = getTime();
		float one = g_modelViewMatrix[0] * std::cos(time) + g_modelViewMatrix[2] * (-std::sin(time));
		float two = g_modelViewMatrix[1] * 1;
		float three = g_modelViewMatrix[0] * std::sin(time) + g_modelViewMatrix[2] * std::cos(time);
		float four = g_modelViewMatrix[4] * std::cos(time) + g_modelViewMatrix[6] * (-std::sin(time));
		float five = g_modelViewMatrix[5] * 1;
		float six = g_modelViewMatrix[4] * std::sin(time) + g_modelViewMatrix[6] * std::cos(time);
		float seven = g_modelViewMatrix[8] * std::cos(time) + g_modelViewMatrix[10] * (-std::sin(time));
		float eight = g_modelViewMatrix[9] * 1;
		float nine = g_modelViewMatrix[8] * std::sin(time) + g_modelViewMatrix[10] * std::cos(time);

		// Setting new rotated matrix
		g_modelViewMatrix[0] = one;
		g_modelViewMatrix[1] = two;
		g_modelViewMatrix[2] = three;
		g_modelViewMatrix[4] = four;
		g_modelViewMatrix[5] = five;
		g_modelViewMatrix[6] = six;
		g_modelViewMatrix[8] = seven;
		g_modelViewMatrix[9] = eight;
		g_modelViewMatrix[10] = nine;

	}

}

void setModelViewMatrix()
{
	glMatrixMode(GL_MODELVIEW);
	updateModelViewMatrix();
	glLoadMatrixf(g_modelViewMatrix);
}

void drawTeapot() {
	glBegin(GL_TRIANGLES);
	for (size_t f = 0; f < g_meshIndices.size(); ++f)
	{
		const float scale = 0.1f;
		const unsigned int idx = g_meshIndices[f];
		const float x = scale * g_meshVertices[3 * idx + 0];
		const float y = scale * g_meshVertices[3 * idx + 1];
		const float z = scale * g_meshVertices[3 * idx + 2];

		const float nx = g_meshNormals[3 * idx + 0];
		const float ny = g_meshNormals[3 * idx + 1];
		const float nz = g_meshNormals[3 * idx + 2];

		glNormal3f(nx, ny, nz);
		glVertex3f(x, y, z);
	}
	glEnd();
}
void renderTeapot() {
	drawTeapot();
}

void drawCheckerBoard() {
	int checkerCount = g_windowWidth;
	int checkerSize = (g_windowWidth) / checkerCount;

	glBegin(GL_QUADS);
	for (int i = 0; i < checkerCount; ++i) {
		for (int j = 0; j < checkerCount; ++j) {
			if ((i + j) % 2 == 0)
				glColor3f(0.0, 0.1, 1.0);
			else
				glColor3f(1.0, 0.0, 1.0);

			float x = i - checkerCount / 2; // to be between -1 and 1
			float z = j - checkerCount / 2;
			x *= checkerSize;
			z *= checkerSize;
			float y = -1.0f;
			glVertex3f(x, y, z);
			glVertex3f(x, y, z - checkerSize);
			glVertex3f(x + checkerSize, y, z - checkerSize);
			glVertex3f(x + checkerSize, y, z);
		}
	}
	glEnd();
}
void renderCheckerBoard() {

	/*
	/* If you want to keep checkerboard still while rotating the
	/* the teapot, you need to change the transformation for the
	/* checkerboard plane
	*/
	glMatrixMode(GL_MODELVIEW);
	clearModelViewMatrix();

	g_modelViewMatrix[0] = 1;
	g_modelViewMatrix[2] = 0;
	g_modelViewMatrix[5] = 1;
	g_modelViewMatrix[8] = 0;
	g_modelViewMatrix[10] = 1;
	g_modelViewMatrix[14] = -distance;
	g_modelViewMatrix[15] = 1.0f;

	glLoadMatrixf(g_modelViewMatrix);

	// Disable shading for the checkerboard
	glDisable(GL_LIGHTING);
	drawCheckerBoard();
	glEnable(GL_LIGHTING);
}

void render()
{
	togglePerspective();
	setModelViewMatrix();
	renderTeapot();
	if (showCheckerboard)
		renderCheckerBoard();
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

int main()
{
	initWindow();
	initGL();
	loadObj("data/teapot.obj");
	printHotKeys();
	renderLoop();
}
