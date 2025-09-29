// Include standard headers
#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <time.h>
#include <windows.h> // only for windows OS 
#pragma comment(lib, "Winmm.lib")
// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
using namespace std;

/// <summary>
/// //////////////////////////////////////////////
/// </summary>
glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;


glm::mat4 getViewMatrix() {
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix() {
	return ProjectionMatrix;
}

////////////////////////////////////////////// <summary>
/// Add camera function here
/// </summary>
//η κάμερα υλοποιήθηκε μέσα στο κύριο loop της main
void camera_function()
{

}

/////////////////////////////////////////////////

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;


	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}



	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}



	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}


	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}
///////////////////////////////////////////////////


void halfCube(float* vertices) {
	float x_center = 0, y_center = 0, z_center = 0;

	for (int i = 0; i < 36; i++) {
		x_center += vertices[i * 3];
		y_center += vertices[i * 3 + 1];
		z_center += vertices[i * 3 + 2];
	}
	x_center /= 36;
	y_center /= 36;
	z_center /= 36;

	for (int i = 0; i < 36; i++) {
		vertices[i * 3] = x_center + (vertices[i * 3] - x_center) * 0.5;
		vertices[i * 3 + 1] = y_center + (vertices[i * 3 + 1] - y_center) * 0.5;
		vertices[i * 3 + 2] = z_center + (vertices[i * 3 + 2] - z_center) * 0.5;
	}
}

int lock = 0;
clock_t lock_start_time; 
int lock_duration = 2000; // in milliseconds
void check_unlock() {
	if (lock && ((clock() - lock_start_time) * 1000 / CLOCKS_PER_SEC >= lock_duration)) {
		lock = 0;
	}
}
int lock2 = 0;
int lock_duration2 = 5000;
clock_t lock_start_time2;
void check_unlock2() {
	if (lock2 && ((clock() - lock_start_time2) * 1000 / CLOCKS_PER_SEC >= lock_duration2)) {
		lock2 = 0;
	}
}

unsigned char* data1;
unsigned char* data2;
unsigned char* data3;
int w;
int h;
int comp;
unsigned int texture;
void updateTexture(int imageSelect) {
	unsigned char* currentData = (imageSelect == 0) ? data1
		: (imageSelect == 1) ? data2 
		: data3; 

	glDeleteTextures(1, &texture); 
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, currentData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}


int main(void)
{

	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	//Δημιουργία παραθύρου
	window = glfwCreateWindow(950, 950, u8"Εργασία 1Γ – 2024 – Κυνήγι Θησαυρού", NULL, NULL);


	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// background color
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders

	GLuint programID = LoadShaders("P1CVertexShader.vertexshader", "P1CFragmentShader.fragmentshader");

	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 4.0f, 0.1f, 100.0f);
	// Camera matrix
	glm::mat4 View = glm::lookAt(
		glm::vec3(5.0f, 5.0f, 5.0f), // Camera in World Space
		glm::vec3(0.0f, 0.0f, 0.0f), // and looks at the origin
		glm::vec3(0.0f, 0.0f, 1.0f));  // Head is up 
	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model = glm::mat4(1.0f);
	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 MVP = Projection * View * Model;

	GLfloat len = 5.0f, wid = 2.5f, heig = 2.5f;
	// vertex buffer λαβύρινθου
	static const GLfloat cube[] = {
		-5.0f, -5.0f, 0.0f,//z=0
		-4.0f, -5.0f, 0.0f,
		-5.0f,  2.0f, 0.0f,//3
		-4.0f, -5.0f, 0.0f,
		-4.0f,  2.0f, 0.0f,
		-5.0f,  2.0f, 0.0f,//6

		-4.0f, -5.0f, 0.0f,
		 5.0f, -5.0f, 0.0f,
		 5.0f, -4.0f, 0.0f,
		-4.0f, -5.0f, 0.0f,
		 5.0f, -4.0f, 0.0f,
		-4.0f, -4.0f, 0.0f,//12

		 4.0f, -4.0f, 0.0f,
		 5.0f, -4.0f, 0.0f,
		 5.0f, -3.0f, 0.0f,
		 4.0f, -4.0f, 0.0f,
		 5.0f, -3.0f, 0.0f,
		 4.0f, -3.0f, 0.0f,//18

		-5.0f,  4.0f, 0.0f,
		 5.0f,  4.0f, 0.0f,
		-5.0f,  5.0f, 0.0f,
		 5.0f,  4.0f, 0.0f,
		 5.0f,  5.0f, 0.0f,
		-5.0f,  5.0f, 0.0f,//24
		 4.0f, -2.0f, 0.0f,
		 5.0f, -2.0f, 0.0f,
		 4.0f,  4.0f, 0.0f,
		 5.0f, -2.0f, 0.0f,
		 5.0f,  4.0f, 0.0f,
		 4.0f,  4.0f, 0.0f,
		-5.0f,  3.0f, 0.0f,
		-4.0f,  3.0f, 0.0f,
		-4.0f,  4.0f, 0.0f,
		-5.0f,  3.0f, 0.0f,
		-4.0f,  4.0f, 0.0f,
		-5.0f,  4.0f, 0.0f,
		-3.0f,  2.0f, 0.0f,
		 1.0f,  2.0f, 0.0f,
		 1.0f,  3.0f, 0.0f,
		-3.0f,  2.0f, 0.0f,
		 1.0f,  3.0f, 0.0f,
		-3.0f,  3.0f, 0.0f,
		 2.0f,  0.0f, 0.0f,
		 3.0f,  0.0f, 0.0f,
		 3.0f,  3.0f, 0.0f,
		 2.0f,  0.0f, 0.0f,
		 3.0f,  3.0f, 0.0f,
		 2.0f,  3.0f, 0.0f,
		-3.0f,  0.0f, 0.0f,
		-2.0f,  0.0f, 0.0f,
		-2.0f,  2.0f, 0.0f,
		-3.0f,  0.0f, 0.0f,
		-2.0f,  2.0f, 0.0f,
		-3.0f,  2.0f, 0.0f,
		 0.0f, -2.0f, 0.0f,
		 1.0f, -2.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
		 0.0f, -2.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
		 0.0f,  1.0f, 0.0f,
		-1.0f,  0.0f, 0.0f,
		 0.0f,  0.0f, 0.0f,
		 0.0f,  1.0f, 0.0f,
		-1.0f,  0.0f, 0.0f,
		 0.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-3.0f, -2.0f, 0.0f,
		-1.0f, -2.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		-3.0f, -2.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		-3.0f, -1.0f, 0.0f,
		 1.0f, -2.0f, 0.0f,
		 3.0f, -2.0f, 0.0f,
		 3.0f, -1.0f, 0.0f,
		 1.0f, -2.0f, 0.0f,
		 3.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 2.0f, -3.0f, 0.0f,
		 3.0f, -3.0f, 0.0f,
		 3.0f, -2.0f, 0.0f,
		 2.0f, -3.0f, 0.0f,
		 3.0f, -2.0f, 0.0f,
		 2.0f, -2.0f, 0.0f,
		-1.0f, -4.0f, 0.0f,
		 1.0f, -4.0f, 0.0f,
		 1.0f, -3.0f, 0.0f,
		-1.0f, -4.0f, 0.0f,
		 1.0f, -3.0f, 0.0f,
		-1.0f, -3.0f, 0.0f,
		-3.0f, -4.0f, 0.0f,
		-2.0f, -4.0f, 0.0f,
		-2.0f, -3.0f, 0.0f,
		-3.0f, -4.0f, 0.0f,
		-2.0f, -3.0f, 0.0f,
		-3.0f, -3.0f, 0.0f,
		-5.0f, -5.0f, 1.0f,//z=1
		-4.0f, -5.0f, 1.0f,
		-5.0f, 2.0f, 1.0f,
		-4.0f, -5.0f, 1.0f,
		-4.0f, 2.0f, 1.0f,
		-5.0f, 2.0f, 1.0f,
		-4.0f, -5.0f, 1.0f,
		5.0f, -5.0f, 1.0f,
		5.0f, -4.0f, 1.0f,
		-4.0f, -5.0f, 1.0f,
		5.0f, -4.0f, 1.0f,
		-4.0f, -4.0f, 1.0f,
		4.0f, -4.0f, 1.0f,
		5.0f, -4.0f, 1.0f,
		5.0f, -3.0f, 1.0f,
		4.0f, -4.0f, 1.0f,
		5.0f, -3.0f, 1.0f,
		4.0f, -3.0f, 1.0f,
		-5.0f, 4.0f, 1.0f,
		5.0f, 4.0f, 1.0f,
		-5.0f, 5.0f, 1.0f,
		5.0f, 4.0f, 1.0f,
		5.0f, 5.0f, 1.0f,
		-5.0f, 5.0f, 1.0f,
		4.0f, -2.0f, 1.0f,
		5.0f, -2.0f, 1.0f,
		4.0f, 4.0f, 1.0f,
		5.0f, -2.0f, 1.0f,
		5.0f, 4.0f, 1.0f,
		4.0f, 4.0f, 1.0f,
		-5.0f, 3.0f, 1.0f,
		-4.0f, 3.0f, 1.0f,
		-4.0f, 4.0f, 1.0f,
		-5.0f, 3.0f, 1.0f,
		-4.0f, 4.0f, 1.0f,
		-5.0f, 4.0f, 1.0f,
		-3.0f, 2.0f, 1.0f,
		1.0f, 2.0f, 1.0f,
		1.0f, 3.0f, 1.0f,
		-3.0f, 2.0f, 1.0f,
		1.0f, 3.0f, 1.0f,
		-3.0f, 3.0f, 1.0f,
		2.0f, 0.0f, 1.0f,
		3.0f, 0.0f, 1.0f,
		3.0f, 3.0f, 1.0f,
		2.0f, 0.0f, 1.0f,
		3.0f, 3.0f, 1.0f,
		2.0f, 3.0f, 1.0f,
		-3.0f, 0.0f, 1.0f,
		-2.0f, 0.0f, 1.0f,
		-2.0f, 2.0f, 1.0f,
		-3.0f, 0.0f, 1.0f,
		-2.0f, 2.0f, 1.0f,
		-3.0f, 2.0f, 1.0f,
		0.0f, -2.0f, 1.0f,
		1.0f, -2.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		0.0f, -2.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		-1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		-1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-3.0f, -2.0f, 1.0f,
		-1.0f, -2.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		-3.0f, -2.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		-3.0f, -1.0f, 1.0f,
		1.0f, -2.0f, 1.0f,
		3.0f, -2.0f, 1.0f,
		3.0f, -1.0f, 1.0f,
		1.0f, -2.0f, 1.0f,
		3.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		2.0f, -3.0f, 1.0f,
		3.0f, -3.0f, 1.0f,
		3.0f, -2.0f, 1.0f,
		2.0f, -3.0f, 1.0f,
		3.0f, -2.0f, 1.0f,
		2.0f, -2.0f, 1.0f,
		-1.0f, -4.0f,1.0f,
		1.0f, -4.0f, 1.0f,
		1.0f, -3.0f, 1.0f,
		-1.0f, -4.0f, 1.0f,
		1.0f, -3.0f, 1.0f,
		-1.0f, -3.0f, 1.0f,
		-3.0f, -4.0f, 1.0f,
		-2.0f, -4.0f, 1.0f,
		-2.0f, -3.0f, 1.0f,
		-3.0f, -4.0f, 1.0f,
		-2.0f, -3.0f, 1.0f,
		-3.0f, -3.0f, 1.0f,
		-5.0f, 2.0f, 0.0f,
		-5.0f, -5.0f, 0.0f,
		-5.0f, -5.0f, 1.0f,
		-5.0f, 2.0f, 0.0f,
		-5.0f, -5.0f, 1.0f,
		-5.0f, 2.0f, 1.0f,//198

		-4.0f, 2.0f, 0.0f,//199
		-4.0f, -5.0f, 0.0f,
		-4.0f, -5.0f, 1.0f,

		-4.0f, 2.0f, 0.0f,
		-4.0f, -5.0f, 1.0f,
		-4.0f, 2.0f, 1.0f,//204

		-5.0f, 2.0f, 0.0f,
		-4.0f, 2.0f, 0.0f,
		-4.0f, 2.0f, 1.0f,

		-5.0f, 2.0f, 0.0f,
		-4.0f, 2.0f, 1.0f,
		-5.0f, 2.0f, 1.0f,//210

		-5.0f, -5.0f, 0.0f,
		 5.0f, -5.0f, 0.0f,
		 5.0f, -5.0f, 1.0f,
		-5.0f, -5.0f, 0.0f,
		 5.0f, -5.0f, 1.0f,
		-5.0f, -5.0f, 1.0f,//216

		-5.0f, -4.0f, 0.0f,
		5.0f, -4.0f, 0.0f,
		5.0f, -4.0f, 1.0f,
		-5.0f, -4.0f, 0.0f,
		5.0f, -4.0f, 1.0f,
		-5.0f, -4.0f, 1.0f,//222

		5.0f, -5.0f, 0.0f,
		5.0f, -3.0f, 0.0f,
		5.0f, -3.0f, 1.0f,
		5.0f, -5.0f, 0.0f,
		5.0f, -3.0f, 1.0f,
		5.0f, -5.0f, 1.0f,//228

		4.0f, -3.0f, 0.0f,
		5.0f, -3.0f, 0.0f,
		5.0f, -3.0f, 1.0f,
		4.0f, -3.0f, 0.0f,
		5.0f, -3.0f, 1.0f,
		4.0f, -3.0f, 1.0f,//234

		4.0f, -4.0f, 0.0f,
		4.0f, -4.0f, 1.0f,
		4.0f, -3.0f, 1.0f,
		4.0f, -4.0f, 0.0f,
		4.0f, -3.0f, 1.0f,
		4.0f, -3.0f, 0.0f,//240

		1.0f, -4.0f, 0.0f,
		1.0f, -4.0f, 1.0f,
		1.0f, -3.0f, 1.0f,
		1.0f, -4.0f, 0.0f,
		1.0f, -3.0f, 1.0f,
		1.0f, -3.0f, 0.0f,//246

		-1.0f, -4.0f, 0.0f,
		-1.0f, -4.0f, 1.0f,
		-1.0f, -3.0f, 1.0f,
		-1.0f, -4.0f, 0.0f,
		-1.0f, -3.0f, 1.0f,
		-1.0f, -3.0f, 0.0f,//252

		-2.0f, -4.0f, 0.0f,
		-2.0f, -4.0f, 1.0f,
		-2.0f, -3.0f, 1.0f,
		-2.0f, -4.0f, 0.0f,
		-2.0f, -3.0f, 1.0f,
		-2.0f, -3.0f, 0.0f,//258

		-3.0f, -4.0f, 0.0f,
		-3.0f, -4.0f, 1.0f,
		-3.0f, -3.0f, 1.0f,
		-3.0f, -4.0f, 0.0f,
		-3.0f, -3.0f, 1.0f,
		-3.0f, -3.0f, 0.0f,//264

		-3.0f, -2.0f, 0.0f,
		-3.0f, -2.0f, 1.0f,
		-3.0f, -1.0f, 1.0f,
		-3.0f, -2.0f, 0.0f,
		-3.0f, -1.0f, 1.0f,
		-3.0f, -1.0f, 0.0f,//270

		-1.0f, -2.0f, 0.0f,
		-1.0f, -2.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		-1.0f, -2.0f, 0.0f,
		-1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 0.0f,//276

		-3.0f, -2.0f, 0.0f,
		-1.0f, -2.0f, 0.0f,
		-1.0f, -2.0f, 1.0f,
		-3.0f, -2.0f, 0.0f,
		-1.0f, -2.0f, 1.0f,
		-3.0f, -2.0f, 1.0f,//282

		 0.0f, -2.0f, 0.0f,
		 2.0f, -2.0f, 0.0f,
		 2.0f, -2.0f, 1.0f,
		 0.0f, -2.0f, 0.0f,
		 2.0f, -2.0f, 1.0f,
		 0.0f, -2.0f, 1.0f,//288


		 -3.0f, -1.0f, 0.0f,
		 -1.0f, -1.0f, 0.0f,
		 -1.0f, -1.0f, 1.0f,
		 -3.0f, -1.0f, 0.0f,
		 -1.0f, -1.0f, 1.0f,
		 -3.0f, -1.0f, 1.0f,//294


		  1.0f, -1.0f, 0.0f,
		  3.0f, -1.0f, 0.0f,
		  3.0f, -1.0f, 1.0f,
		  1.0f, -1.0f, 0.0f,
		  3.0f, -1.0f, 1.0f,
		  1.0f, -1.0f, 1.0f,//300

		 -1.0f, 0.0f, 0.0f,
		  1.0f, 0.0f, 0.0f,
		  1.0f, 0.0f, 1.0f,
		 -1.0f, 0.0f, 0.0f,
		  1.0f, 0.0f, 1.0f,
		 -1.0f, 0.0f, 1.0f,//306

		 -1.0f, 1.0f, 0.0f,
		 1.0f, 1.0f, 0.0f,
		 1.0f, 1.0f, 1.0f,
		 -1.0f, 1.0f, 0.0f,
		 1.0f, 1.0f, 1.0f,
		 -1.0f, 1.0f, 1.0f,//312

		 -1.0f, -3.0f, 0.0f,
		 1.0f, -3.0f, 0.0f,
		 1.0f, -3.0f, 1.0f,
		 -1.0f, -3.0f, 0.0f,
		 1.0f, -3.0f, 1.0f,
		 -1.0f, -3.0f, 1.0f,//318

		 1.0f, -1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f, 1.0f,
		 1.0f, -1.0f, 0.0f,
		 1.0f,  1.0f, 1.0f,
		 1.0f, -1.0f, 1.0f,//324

		 0.0f, -2.0f, 0.0f,
		 0.0f, 0.0f, 0.0f,
		 0.0f, 0.0f, 1.0f,
		 0.0f, -2.0f, 0.0f,
		 0.0f, 0.0f, 1.0f,
		 0.0f, -2.0f, 1.0f,//330

		 3.0f, -3.0f, 0.0f,
		 3.0f, -1.0f, 0.0f,
		 3.0f, -1.0f, 1.0f,
		 3.0f, -3.0f, 0.0f,
		 3.0f, -1.0f, 1.0f,
		 3.0f, -3.0f, 1.0f,//336

		 -2.0f, 0.0f, 0.0f,
		 -2.0f, 2.0f, 0.0f,
		 -2.0f, 2.0f, 1.0f,
		 -2.0f, 0.0f, 0.0f,
		 -2.0f, 2.0f, 1.0f,
		 -2.0f, 0.0f, 1.0f,//342

		 -3.0f, 0.0f, 0.0f,
		 -3.0f, 3.0f, 0.0f,
		 -3.0f, 3.0f, 1.0f,
		 -3.0f, 0.0f, 0.0f,
		 -3.0f, 3.0f, 1.0f,
		 -3.0f, 0.0f, 1.0f,//348

		  2.0f, 0.0f, 0.0f,
		  2.0f, 3.0f, 0.0f,
		  2.0f, 3.0f, 1.0f,
		  2.0f, 0.0f, 0.0f,
		  2.0f, 3.0f, 1.0f,
		  2.0f, 0.0f, 1.0f,//354

		  3.0f, 0.0f, 0.0f,
		  3.0f, 3.0f, 0.0f,
		  3.0f, 3.0f, 1.0f,
		  3.0f, 0.0f, 0.0f,
		  3.0f, 3.0f, 1.0f,
		  3.0f, 0.0f, 1.0f,//360

		  2.0f, 0.0f, 0.0f,
		  3.0f, 0.0f, 0.0f,
		  3.0f, 0.0f, 1.0f,
		  2.0f, 0.0f, 0.0f,
		  3.0f, 0.0f, 1.0f,
		  2.0f, 0.0f, 1.0f,//366

		  2.0f, 3.0f, 0.0f,
		  3.0f, 3.0f, 0.0f,
		  3.0f, 3.0f, 1.0f,
		  2.0f, 3.0f, 0.0f,
		  3.0f, 3.0f, 1.0f,
		  2.0f, 3.0f, 1.0f,//372

		  2.0f, -3.0f, 0.0f,
		  3.0f, -3.0f, 0.0f,
		  3.0f, -3.0f, 1.0f,
		  2.0f, -3.0f, 0.0f,
		  3.0f, -3.0f, 1.0f,
		  2.0f, -3.0f, 1.0f,//378

		  -3.0f, 0.0f, 0.0f,
		  -2.0f, 0.0f, 0.0f,
		  -2.0f, 0.0f, 1.0f,
		  -3.0f, 0.0f, 0.0f,
		  -2.0f, 0.0f, 1.0f,
		  -3.0f, 0.0f, 1.0f,//384

		  -3.0f, -3.0f, 0.0f,
		  -2.0f, -3.0f, 0.0f,
		  -2.0f, -3.0f, 1.0f,
		  -3.0f, -3.0f, 0.0f,
		  -2.0f, -3.0f, 1.0f,
		  -3.0f, -3.0f, 1.0f,//390

		  -5.0f, 3.0f, 0.0f,
		  -4.0f, 3.0f, 0.0f,
		  -4.0f, 3.0f, 1.0f,
		  -5.0f, 3.0f, 0.0f,
		  -4.0f, 3.0f, 1.0f,
		  -5.0f, 3.0f, 1.0f,//396

		   4.0f, -2.0f, 0.0f,
		   5.0f, -2.0f, 0.0f,
		   5.0f, -2.0f, 1.0f,
		   4.0f, -2.0f, 0.0f,
		   5.0f, -2.0f, 1.0f,
		   4.0f, -2.0f, 1.0f,//402

		   -1.0f, 0.0f, 1.0f,
		   -1.0f, 1.0f, 1.0f,
		   -1.0f, 1.0f, 0.0f,
		   -1.0f, 0.0f, 1.0f,
		   -1.0f, 1.0f, 0.0f,
		   -1.0f, 0.0f, 0.0f,//408

			2.0f, -3.0f, 1.0f,
			2.0f, -2.0f, 1.0f,
			2.0f, -2.0f, 0.0f,
			2.0f, -3.0f, 1.0f,
			2.0f, -2.0f, 0.0f,
			2.0f, -3.0f, 0.0f,//414

			 1.0f, 2.0f, 1.0f,
			 1.0f, 3.0f, 1.0f,
			 1.0f, 3.0f, 0.0f,
			 1.0f, 2.0f, 1.0f,
			 1.0f, 3.0f, 0.0f,
			 1.0f, 2.0f, 0.0f,//420

			 -4.0f, 3.0f, 1.0f,
			 -4.0f, 4.0f, 1.0f,
			 -4.0f, 4.0f, 0.0f,
			 -4.0f, 3.0f, 1.0f,
			 -4.0f, 4.0f, 0.0f,
			 -4.0f, 3.0f, 0.0f,//426

			-3.0f, 3.0f, 0.0f,
			 1.0f, 3.0f, 0.0f,
			 1.0f, 3.0f, 1.0f,
			-3.0f, 3.0f, 0.0f,
			 1.0f, 3.0f, 1.0f,
			-3.0f, 3.0f, 1.0f,//432

			-3.0f, 2.0f, 0.0f,
			1.0f, 2.0f, 0.0f,
			1.0f, 2.0f, 1.0f,
			-3.0f, 2.0f, 0.0f,
			1.0f, 2.0f, 1.0f,
			-3.0f, 2.0f, 1.0f,//438

			-5.0f, 5.0f, 0.0f,
			5.0f, 5.0f, 0.0f,
			5.0f, 5.0f, 1.0f,
			-5.0f, 5.0f, 0.0f,
			5.0f, 5.0f, 1.0f,
			-5.0f, 5.0f, 1.0f,//444

			-5.0f, 4.0f, 0.0f,
			5.0f, 4.0f, 0.0f,
			5.0f, 4.0f, 1.0f,
			-5.0f, 4.0f, 0.0f,
			5.0f, 4.0f, 1.0f,
			-5.0f, 4.0f, 1.0f,//450

			-5.0f, 5.0f, 0.0f,
			-5.0f, 3.0f, 0.0f,
			-5.0f, 3.0f, 1.0f,
			-5.0f, 5.0f, 0.0f,
			-5.0f, 3.0f, 1.0f,
			-5.0f, 5.0f, 1.0f,//456

			5.0f, -2.0f, 0.0f,
			5.0f, 5.0f, 0.0f,
			5.0f, 5.0f, 1.0f,
			5.0f, -2.0f, 0.0f,
			5.0f, 5.0f, 1.0f,
			5.0f, -2.0f, 1.0f,//462

			4.0f, -2.0f, 0.0f,
			4.0f, 5.0f, 0.0f,
			4.0f, 5.0f, 1.0f,
			4.0f, -2.0f, 0.0f,
			4.0f, 5.0f, 1.0f,
			4.0f, -2.0f, 1.0f,//468



	};
	//player vertex buffer
	static GLfloat shape_2_buffer[] = {
		-4.75f, 2.25f, 0.25f,
		-4.25f, 2.25f, 0.25f,
		-4.25f, 2.75f, 0.25f,
		-4.75f, 2.25f, 0.25f,
		-4.25f, 2.75f, 0.25f,
		-4.75f, 2.75f, 0.25f,

		-4.75f, 2.25f, 0.75f,
		-4.25f, 2.25f, 0.75f,
		-4.25f, 2.75f, 0.75f,
		-4.75f, 2.25f, 0.75f,
		-4.25f, 2.75f, 0.75f,
		-4.75f, 2.75f, 0.75f,

		-4.75f, 2.25f, 0.25f,
		-4.25f, 2.25f, 0.25f,
		-4.25f, 2.25f, 0.75f,
		-4.75f, 2.25f, 0.25f,
		-4.25f, 2.25f, 0.75f,
		-4.75f, 2.25f, 0.75f,

		-4.75f, 2.75f, 0.25f,
		-4.75f, 2.25f, 0.25f,
		-4.75f, 2.25f, 0.75f,
		-4.75f, 2.75f, 0.25f,
		-4.75f, 2.25f, 0.75f,
		-4.75f, 2.75f, 0.75f,

		-4.25f, 2.75f, 0.25f,
		-4.25f, 2.25f, 0.25f,
		-4.25f, 2.25f, 0.75f,
		-4.25f, 2.75f, 0.25f,
		-4.25f, 2.25f, 0.75f,
		-4.25f, 2.75f, 0.75f,

		-4.75f, 2.75f, 0.25f, 
		-4.25f, 2.75f, 0.25f,
		-4.25f, 2.75f, 0.75f,
		-4.75f, 2.75f, 0.25f,
		-4.25f, 2.75f, 0.75f,
		-4.75f, 2.75f, 0.75f,



	};
	// tresure vertex buffer
	static GLfloat shape_3_buffer[] = {
		-3.9f, 2.1f, 0.1f,
		-3.1f, 2.1f, 0.1f,
		-3.1f, 2.9f, 0.1f,
		-3.9f, 2.1f, 0.1f,
		-3.1f, 2.9f, 0.1f,
		-3.9f, 2.9f, 0.1f,

		-3.9f, 2.1f, 0.9f,
		-3.1f, 2.1f, 0.9f,
		-3.1f, 2.9f, 0.9f,
		-3.9f, 2.1f, 0.9f,
		-3.1f, 2.9f, 0.9f,
		-3.9f, 2.9f, 0.9f,

		-3.9f, 2.1f, 0.1f,
		-3.1f, 2.1f, 0.1f,
		-3.1f, 2.1f, 0.9f,
		-3.9f, 2.1f, 0.1f,
		-3.1f, 2.1f, 0.9f,
		-3.9f, 2.1f, 0.9f,

		-3.9f, 2.9f, 0.1f,
		-3.9f, 2.1f, 0.1f,
		-3.9f, 2.1f, 0.9f,
		-3.9f, 2.9f, 0.1f,
		-3.9f, 2.1f, 0.9f,
		-3.9f, 2.9f, 0.9f,

		-3.1f, 2.9f, 0.1f,
		-3.1f, 2.1f, 0.1f,
		-3.1f, 2.1f, 0.9f,
		-3.1f, 2.9f, 0.1f,
		-3.1f, 2.1f, 0.9f,
		-3.1f, 2.9f, 0.9f,

		-3.9f, 2.9f, 0.1f,
		-3.1f, 2.9f, 0.1f,
		-3.1f, 2.9f, 0.9f,
		-3.9f, 2.9f, 0.1f,
		-3.1f, 2.9f, 0.9f,
		-3.9f, 2.9f, 0.9f,
	};
	GLfloat a = 0.0f;
	// labyrinth color buffer
	static const GLfloat color[] = {
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,

		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,

		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,

		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,

		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,

		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,

		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f,  0.000f,  1.000f,a,
		0.000f, 0.000f,  1.000f, a,
		0.000f, 0.000f,  1.000f, a,
		0.000f, 0.000f,  1.000f, a,
		0.000f, 0.000f,  1.000f, a,
		0.000f, 0.000f,  1.000f, a,
		0.000f, 0.000f,  1.000f, a,
		0.000f, 0.000f,  1.000f, a,
		0.000f, 0.000f,  1.000f, a,
		0.000f, 0.000f,  1.000f, a,
		0.000f, 0.000f,  1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,
		0.000f, 0.000f, 1.000f, a,


	};
	//player color buffer 
	static const GLfloat colortwo[] = {
		1.000f, 1.000f, 0.000f, a,
		1.000f, 1.000f, 0.000f, a,
		1.000f, 1.000f, 0.000f, a,
		1.000f, 1.000f, 0.000f, a,
		1.000f, 1.000f, 0.000f, a,
		1.000f, 1.000f, 0.000f, a,
		1.000f, 1.000f, 0.000f, a,
		1.000f, 1.000f, 0.000f, a,
		1.000f, 1.000f, 0.000f, a,
		1.000f, 1.000f, 0.000f, a,
		1.000f, 1.000f, 0.000f, a,
		1.000f, 1.000f, 0.000f, a,
		1.000f, 1.000f, 0.000f, a,
		1.000f, 1.000f, 0.000f, a,
		1.000f, 1.000f, 0.000f, a,
		1.000f, 1.000f, 0.000f, a,
		1.000f, 1.000f, 0.000f, a,
		1.000f, 1.000f, 0.000f, a,
		1.000f, 1.000f, 0.000f, a,
		1.000f, 1.000f, 0.000f, a,
		1.000f, 1.000f, 0.000f, a,
		1.000f, 1.000f, 0.000f, a,
		1.000f, 1.000f, 0.000f, a,
		1.000f, 1.000f, 0.000f, a,
		1.000f, 1.000f, 0.000f, a,
		1.000f, 1.000f, 0.000f, a,
		1.000f, 1.000f, 0.000f, a,
		1.000f, 1.000f, 0.000f, a,
		1.000f, 1.000f, 0.000f, a,
		1.000f, 1.000f, 0.000f, a,
		1.000f, 1.000f, 0.000f, a,
		1.000f, 1.000f, 0.000f, a,
		1.000f, 1.000f, 0.000f, a,
		1.000f, 1.000f, 0.000f, a,
		1.000f, 1.000f, 0.000f, a,
		1.000f, 1.000f, 0.000f, a,

	};
	//tresure colour buffer 
	static const GLfloat colorthree[] = {
		1.000f, 0.000f, 0.000f, a,
		1.000f, 0.000f, 0.000f, a,
		1.000f, 0.000f, 0.000f, a,
		1.000f, 0.000f, 0.000f, a,
		1.000f, 0.000f, 0.000f, a,
		1.000f, 0.000f, 0.000f, a,
		1.000f, 0.000f, 0.000f, a,
		1.000f, 0.000f, 0.000f, a,
		1.000f, 0.000f, 0.000f, a,
		1.000f, 0.000f, 0.000f, a,
		1.000f, 0.000f, 0.000f, a,
		1.000f, 0.000f, 0.000f, a,
		1.000f, 0.000f, 0.000f, a,
		1.000f, 0.000f, 0.000f, a,
		1.000f, 0.000f, 0.000f, a,
		1.000f, 0.000f, 0.000f, a,
		1.000f, 0.000f, 0.000f, a,
		1.000f, 0.000f, 0.000f, a,
		1.000f, 0.000f, 0.000f, a,
		1.000f, 0.000f, 0.000f, a,
		1.000f, 0.000f, 0.000f, a,
		1.000f, 0.000f, 0.000f, a,
		1.000f, 0.000f, 0.000f, a,
		1.000f, 0.000f, 0.000f, a,
		1.000f, 0.000f, 0.000f, a,
		1.000f, 0.000f, 0.000f, a,
		1.000f, 0.000f, 0.000f, a,
		1.000f, 0.000f, 0.000f, a,
		1.000f, 0.000f, 0.000f, a,
		1.000f, 0.000f, 0.000f, a,
		1.000f, 0.000f, 0.000f, a,
		1.000f, 0.000f, 0.000f, a,
		1.000f, 0.000f, 0.000f, a,
		1.000f, 0.000f, 0.000f, a,
		1.000f, 0.000f, 0.000f, a,
		1.000f, 0.000f, 0.000f, a,

	};
	//uv coordinates
	static const GLfloat uv_coordinates[] = {
			0.0f,0.0f,
			1.0f,0.0f,
			1.0f,1.0f,
			0.0f,0.0f,
			1.0f,1.0f,
			0.0f,1.0f,
			0.0f,0.0f,
			1.0f,0.0f,
			1.0f,1.0f,
			0.0f,0.0f,
			1.0f,1.0f,
			0.0f,1.0f,
			0.0f,0.0f,
			1.0f,0.0f,
			1.0f,1.0f,
			0.0f,0.0f,
			1.0f,1.0f,
			0.0f,1.0f,
			0.0f,0.0f,
			1.0f,0.0f,
			1.0f,1.0f,
			0.0f,0.0f,
			1.0f,1.0f,
			0.0f,1.0f,
			0.0f,0.0f,
			1.0f,0.0f,
			1.0f,1.0f,
			0.0f,0.0f,
			1.0f,1.0f,
			0.0f,1.0f,
			0.0f,0.0f,
			1.0f,0.0f,
			1.0f,1.0f,
			0.0f,0.0f,
			1.0f,1.0f,
			0.0f,1.0f,

	};

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uv_coordinates), uv_coordinates, GL_STATIC_DRAW);

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

	GLuint VBO2;
	glGenBuffers(1, &VBO2);

	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);

	GLuint colorbuffertwo;
	glGenBuffers(1, &colorbuffertwo);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffertwo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colortwo), colortwo, GL_STATIC_DRAW);

	GLuint VBO3;
	glGenBuffers(1, &VBO3);

	GLuint colorbufferthree;
	glGenBuffers(1, &colorbufferthree);
	glBindBuffer(GL_ARRAY_BUFFER, colorbufferthree);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colorthree), colorthree, GL_STATIC_DRAW);

	
	unsigned char* data;
	srand(time(NULL)); //seed initialization
	int imageSelect = rand() % 3;
	data1 = stbi_load("coins.jpg", &w, &h, &comp, 0);
	data2 = stbi_load("coins2.jpg", &w, &h, &comp, 0);
	data3 = stbi_load("coins3.jpg", &w, &h, &comp, 0);
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
 
	bool stall = false;
	// Camera
	float xPos = 0.0f, yPos = 0.0f, zPos = 20.0f;
	float rotationX = 0.0f, rotationY = 0.0f;
	float zoom = 45.0f;  // Zoom angle (fov)
	float panX = 0.0f, panY = 0.0f;  // Panning

	int cubeX = 0;
	int cubeY = 2;
	int tresureX = 1;
	int tresureY = 2;
	int newtresureX = 1 ;
	int newtresureY = 2;
	GLboolean mapArray[10][10] = { //mapping for valid player movement
		{GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE},
		{GL_FALSE,GL_TRUE ,GL_TRUE ,GL_TRUE ,GL_TRUE ,GL_TRUE ,GL_TRUE ,GL_TRUE ,GL_TRUE ,GL_FALSE},
		{GL_TRUE ,GL_TRUE ,GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE,GL_TRUE ,GL_FALSE,GL_TRUE ,GL_FALSE},
		{GL_FALSE,GL_TRUE ,GL_FALSE,GL_TRUE ,GL_TRUE ,GL_TRUE ,GL_TRUE ,GL_FALSE,GL_TRUE ,GL_FALSE},
		{GL_FALSE,GL_TRUE ,GL_FALSE,GL_TRUE ,GL_FALSE,GL_FALSE,GL_TRUE ,GL_FALSE,GL_TRUE ,GL_FALSE},
		{GL_FALSE,GL_TRUE ,GL_TRUE ,GL_TRUE ,GL_TRUE ,GL_FALSE,GL_TRUE ,GL_TRUE ,GL_TRUE ,GL_FALSE},
		{GL_FALSE,GL_TRUE ,GL_FALSE,GL_FALSE,GL_TRUE ,GL_FALSE,GL_FALSE,GL_FALSE,GL_TRUE ,GL_FALSE},
		{GL_FALSE,GL_TRUE ,GL_TRUE ,GL_TRUE ,GL_TRUE ,GL_TRUE ,GL_TRUE ,GL_FALSE,GL_TRUE ,GL_TRUE },
		{GL_FALSE,GL_TRUE ,GL_FALSE,GL_TRUE ,GL_FALSE,GL_FALSE,GL_TRUE ,GL_TRUE ,GL_TRUE ,GL_FALSE},
		{GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE}
	};

	do {
		check_unlock();
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Use our shader
		glUseProgram(programID);

		// 1rst attribute buffer : vertices
		glUniform1i(glGetUniformLocation(programID, "uUseTexture"), GL_TRUE);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glVertexAttribPointer(
			1,
			4,                                // size
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

		glBindBuffer(GL_ARRAY_BUFFER, VBO2);
		glBufferData(GL_ARRAY_BUFFER, sizeof(shape_2_buffer), shape_2_buffer, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO3);
		glBufferData(GL_ARRAY_BUFFER, sizeof(shape_3_buffer), shape_3_buffer, GL_DYNAMIC_DRAW);
		//player movement
		if (!stall) {
			if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
				if (cubeX == 9) { //if player reached the end and pressed L
					int i;
					for(i = 0; i < 108; i+=3) {
						shape_2_buffer[i] = shape_2_buffer[i] - 9;
						shape_2_buffer[i + 1] = shape_2_buffer[i + 1] + 5;
					}
					cubeX = 0;
					cubeY = 2;
				}
				else {
					if (mapArray[cubeY][cubeX + 1]) { //check if movement is valid
						cubeX = cubeX + 1;
						shape_2_buffer[0] = shape_2_buffer[0] + 1;
						shape_2_buffer[3] = shape_2_buffer[3] + 1;
						shape_2_buffer[6] = shape_2_buffer[6] + 1;
						shape_2_buffer[9] = shape_2_buffer[9] + 1;
						shape_2_buffer[12] = shape_2_buffer[12] + 1;
						shape_2_buffer[15] = shape_2_buffer[15] + 1;
						shape_2_buffer[18] = shape_2_buffer[18] + 1;
						shape_2_buffer[21] = shape_2_buffer[21] + 1;
						shape_2_buffer[24] = shape_2_buffer[24] + 1;
						shape_2_buffer[27] = shape_2_buffer[27] + 1;
						shape_2_buffer[30] = shape_2_buffer[30] + 1;
						shape_2_buffer[33] = shape_2_buffer[33] + 1;
						shape_2_buffer[36] = shape_2_buffer[36] + 1;
						shape_2_buffer[39] = shape_2_buffer[39] + 1;
						shape_2_buffer[42] = shape_2_buffer[42] + 1;
						shape_2_buffer[45] = shape_2_buffer[45] + 1;
						shape_2_buffer[48] = shape_2_buffer[48] + 1;
						shape_2_buffer[51] = shape_2_buffer[51] + 1;
						shape_2_buffer[54] = shape_2_buffer[54] + 1;
						shape_2_buffer[57] = shape_2_buffer[57] + 1;
						shape_2_buffer[60] = shape_2_buffer[60] + 1;
						shape_2_buffer[63] = shape_2_buffer[63] + 1;
						shape_2_buffer[66] = shape_2_buffer[66] + 1;
						shape_2_buffer[69] = shape_2_buffer[69] + 1;
						shape_2_buffer[72] = shape_2_buffer[72] + 1;
						shape_2_buffer[75] = shape_2_buffer[75] + 1;
						shape_2_buffer[78] = shape_2_buffer[78] + 1;
						shape_2_buffer[81] = shape_2_buffer[81] + 1;
						shape_2_buffer[84] = shape_2_buffer[84] + 1;
						shape_2_buffer[87] = shape_2_buffer[87] + 1;
						shape_2_buffer[90] = shape_2_buffer[90] + 1;
						shape_2_buffer[93] = shape_2_buffer[93] + 1;
						shape_2_buffer[96] = shape_2_buffer[96] + 1;
						shape_2_buffer[99] = shape_2_buffer[99] + 1;
						shape_2_buffer[102] = shape_2_buffer[102] + 1;
						shape_2_buffer[105] = shape_2_buffer[105] + 1;


					}
				}
			}
		}
		if (!stall) {
			if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
				if (cubeX == 0) {
					int i;
					for (i = 0; i < 108; i += 3) {
						shape_2_buffer[i] = shape_2_buffer[i] + 9;
						shape_2_buffer[i + 1] = shape_2_buffer[i + 1] - 5;
					}
					cubeX = 9;
					cubeY = 7;
				}
				else {
					if (mapArray[cubeY][cubeX - 1]) {
						cubeX = cubeX - 1;
						shape_2_buffer[0] = shape_2_buffer[0] - 1;
						shape_2_buffer[3] = shape_2_buffer[3] - 1;
						shape_2_buffer[6] = shape_2_buffer[6] - 1;
						shape_2_buffer[9] = shape_2_buffer[9] - 1;
						shape_2_buffer[12] = shape_2_buffer[12] - 1;
						shape_2_buffer[15] = shape_2_buffer[15] - 1;
						shape_2_buffer[18] = shape_2_buffer[18] - 1;
						shape_2_buffer[21] = shape_2_buffer[21] - 1;
						shape_2_buffer[24] = shape_2_buffer[24] - 1;
						shape_2_buffer[27] = shape_2_buffer[27] - 1;
						shape_2_buffer[30] = shape_2_buffer[30] - 1;
						shape_2_buffer[33] = shape_2_buffer[33] - 1;
						shape_2_buffer[36] = shape_2_buffer[36] - 1;
						shape_2_buffer[39] = shape_2_buffer[39] - 1;
						shape_2_buffer[42] = shape_2_buffer[42] - 1;
						shape_2_buffer[45] = shape_2_buffer[45] - 1;
						shape_2_buffer[48] = shape_2_buffer[48] - 1;
						shape_2_buffer[51] = shape_2_buffer[51] - 1;
						shape_2_buffer[54] = shape_2_buffer[54] - 1;
						shape_2_buffer[57] = shape_2_buffer[57] - 1;
						shape_2_buffer[60] = shape_2_buffer[60] - 1;
						shape_2_buffer[63] = shape_2_buffer[63] - 1;
						shape_2_buffer[66] = shape_2_buffer[66] - 1;
						shape_2_buffer[69] = shape_2_buffer[69] - 1;
						shape_2_buffer[72] = shape_2_buffer[72] - 1;
						shape_2_buffer[75] = shape_2_buffer[75] - 1;
						shape_2_buffer[78] = shape_2_buffer[78] - 1;
						shape_2_buffer[81] = shape_2_buffer[81] - 1;
						shape_2_buffer[84] = shape_2_buffer[84] - 1;
						shape_2_buffer[87] = shape_2_buffer[87] - 1;
						shape_2_buffer[90] = shape_2_buffer[90] - 1;
						shape_2_buffer[93] = shape_2_buffer[93] - 1;
						shape_2_buffer[96] = shape_2_buffer[96] - 1;
						shape_2_buffer[99] = shape_2_buffer[99] - 1;
						shape_2_buffer[102] = shape_2_buffer[102] - 1;
						shape_2_buffer[105] = shape_2_buffer[105] - 1;
					}
				}
			}
		}
		if (!stall) {
			if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
				if (mapArray[cubeY - 1][cubeX]) { 
					cubeY = cubeY - 1;
					shape_2_buffer[1] = shape_2_buffer[1] + 1;
					shape_2_buffer[4] = shape_2_buffer[4] + 1;
					shape_2_buffer[7] = shape_2_buffer[7] + 1;
					shape_2_buffer[10] = shape_2_buffer[10] + 1;
					shape_2_buffer[13] = shape_2_buffer[13] + 1;
					shape_2_buffer[16] = shape_2_buffer[16] + 1;
					shape_2_buffer[19] = shape_2_buffer[19] + 1;
					shape_2_buffer[22] = shape_2_buffer[22] + 1;
					shape_2_buffer[25] = shape_2_buffer[25] + 1;
					shape_2_buffer[28] = shape_2_buffer[28] + 1;
					shape_2_buffer[31] = shape_2_buffer[31] + 1;
					shape_2_buffer[34] = shape_2_buffer[34] + 1;
					shape_2_buffer[37] = shape_2_buffer[37] + 1;
					shape_2_buffer[40] = shape_2_buffer[40] + 1;
					shape_2_buffer[43] = shape_2_buffer[43] + 1;
					shape_2_buffer[46] = shape_2_buffer[46] + 1;
					shape_2_buffer[49] = shape_2_buffer[49] + 1;
					shape_2_buffer[52] = shape_2_buffer[52] + 1;
					shape_2_buffer[55] = shape_2_buffer[55] + 1;
					shape_2_buffer[58] = shape_2_buffer[58] + 1;
					shape_2_buffer[61] = shape_2_buffer[61] + 1;
					shape_2_buffer[64] = shape_2_buffer[64] + 1;
					shape_2_buffer[67] = shape_2_buffer[67] + 1;
					shape_2_buffer[70] = shape_2_buffer[70] + 1;
					shape_2_buffer[73] = shape_2_buffer[73] + 1;
					shape_2_buffer[76] = shape_2_buffer[76] + 1;
					shape_2_buffer[79] = shape_2_buffer[79] + 1;
					shape_2_buffer[82] = shape_2_buffer[82] + 1;
					shape_2_buffer[85] = shape_2_buffer[85] + 1;
					shape_2_buffer[88] = shape_2_buffer[88] + 1;
					shape_2_buffer[91] = shape_2_buffer[91] + 1;
					shape_2_buffer[94] = shape_2_buffer[94] + 1;
					shape_2_buffer[97] = shape_2_buffer[97] + 1;
					shape_2_buffer[100] = shape_2_buffer[100] + 1;
					shape_2_buffer[103] = shape_2_buffer[103] + 1;
					shape_2_buffer[106] = shape_2_buffer[106] + 1;
					}
			}
		}
		if (!stall) {
			if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
				if (mapArray[cubeY + 1][cubeX]) { 
					cubeY = cubeY + 1;
					shape_2_buffer[1] = shape_2_buffer[1] - 1;
					shape_2_buffer[4] = shape_2_buffer[4] - 1;
					shape_2_buffer[7] = shape_2_buffer[7] - 1;
					shape_2_buffer[10] = shape_2_buffer[10] - 1;
					shape_2_buffer[13] = shape_2_buffer[13] - 1;
					shape_2_buffer[16] = shape_2_buffer[16] - 1;
					shape_2_buffer[19] = shape_2_buffer[19] - 1;
					shape_2_buffer[22] = shape_2_buffer[22] - 1;
					shape_2_buffer[25] = shape_2_buffer[25] - 1;
					shape_2_buffer[28] = shape_2_buffer[28] - 1;
					shape_2_buffer[31] = shape_2_buffer[31] - 1;
					shape_2_buffer[34] = shape_2_buffer[34] - 1;
					shape_2_buffer[37] = shape_2_buffer[37] - 1;
					shape_2_buffer[40] = shape_2_buffer[40] - 1;
					shape_2_buffer[43] = shape_2_buffer[43] - 1;
					shape_2_buffer[46] = shape_2_buffer[46] - 1;
					shape_2_buffer[49] = shape_2_buffer[49] - 1;
					shape_2_buffer[52] = shape_2_buffer[52] - 1;
					shape_2_buffer[55] = shape_2_buffer[55] - 1;
					shape_2_buffer[58] = shape_2_buffer[58] - 1;
					shape_2_buffer[61] = shape_2_buffer[61] - 1;
					shape_2_buffer[64] = shape_2_buffer[64] - 1;
					shape_2_buffer[67] = shape_2_buffer[67] - 1;
					shape_2_buffer[70] = shape_2_buffer[70] - 1;
					shape_2_buffer[73] = shape_2_buffer[73] - 1;
					shape_2_buffer[76] = shape_2_buffer[76] - 1;
					shape_2_buffer[79] = shape_2_buffer[79] - 1;
					shape_2_buffer[82] = shape_2_buffer[82] - 1;
					shape_2_buffer[85] = shape_2_buffer[85] - 1;
					shape_2_buffer[88] = shape_2_buffer[88] - 1;
					shape_2_buffer[91] = shape_2_buffer[91] - 1;
					shape_2_buffer[94] = shape_2_buffer[94] - 1;
					shape_2_buffer[97] = shape_2_buffer[97] - 1;
					shape_2_buffer[100] = shape_2_buffer[100] - 1;
					shape_2_buffer[103] = shape_2_buffer[103] - 1;
					shape_2_buffer[106] = shape_2_buffer[106] - 1;
				}
			}
		}
		//camera movement	
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			rotationX += 1;
		}
		if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
			rotationX -= 1;
		}
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
			rotationY += 1;
		}
		if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
			rotationY -= 1;
		}
		if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) {
			zoom += 1.0f; 
			if (zoom > 90.0f) zoom = 90.0f;
		}
		if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) {
			zoom -= 1.0f;
			if (zoom < 1.0f) zoom = 1.0f;
		}
		if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
			panY += 0.1;
		}
		if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
			panX -= 0.1;
		}
		if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
			panX += 0.1;
		}
		if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
			panY -= 0.1;
		}

		// Transformation calculation
		glm::mat4 Model = glm::mat4(1.0f);
		Model = glm::rotate(Model, glm::radians(rotationX), glm::vec3(1.0f, 0.0f, 0.0f));
		Model = glm::rotate(Model, glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
		Model = glm::translate(Model, glm::vec3(panX, panY, 0.0f));  // Panning
		glm::mat4 Projection = glm::perspective(glm::radians(zoom), 4.0f / 4.0f, 0.1f, 100.0f);
		glm::mat4 View = glm::lookAt(
			glm::vec3(xPos, yPos, zPos),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);
		glm::mat4 MVP = Projection * View * Model;

		//tresure movement
		if (!lock && mapArray[newtresureY][newtresureX] && newtresureY != cubeY && newtresureX != cubeX && !lock2) {
			lock = 1; 
			lock_start_time = clock();
			int j;
			for (j = 0; j < 108; j += 3) {
				shape_3_buffer[j] = shape_3_buffer[j] - tresureX + newtresureX;
				shape_3_buffer[j + 1] = shape_3_buffer[j + 1] + tresureY - newtresureY;
			}
			tresureX = newtresureX;
			tresureY = newtresureY;
			newtresureX = rand() % 10;
			newtresureY = rand() % 10;
			int newTextureIndex = rand() % 3;
			updateTexture(newTextureIndex);
		}else {
			newtresureX = rand() % 10;
			newtresureY = rand() % 10;
		}
		
		//Player found the tresure
		if (lock2 == 0 && tresureX == cubeX && tresureY == cubeY) {
			lock2 = 1; 
			lock_start_time2 = clock(); //waits 5 seconds then sets lock=0
			halfCube(shape_3_buffer);
			PlaySound(TEXT("effect.wav"), NULL, SND_FILENAME | SND_ASYNC); //only fo windows OS
		}
		else if (lock2 == 1 && ((clock() - lock_start_time2) * 1000 / CLOCKS_PER_SEC >= lock_duration2)) {
			lock2 = 2; 
		}
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
			stall = true;
		}
		else {
			stall = false;
		}
		

		// Draw triangles 
		glDrawArrays(GL_TRIANGLES, 0, 468); 
		glDisableVertexAttribArray(0);
		glUniform1i(glGetUniformLocation(programID, "uUseTexture"), GL_TRUE);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, VBO2);
		glVertexAttribPointer(
			0,                  // attribute 0, must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffertwo);
		glVertexAttribPointer(
			1,
			4,                                // size
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glUniform1i(glGetUniformLocation(programID, "uUseTexture"), GL_FALSE);
		glDisableVertexAttribArray(0);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, VBO3);
		glVertexAttribPointer(
			0,                  // attribute 0, must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		glEnableVertexAttribArray(2);
		glUniform1i(glGetUniformLocation(programID, "uUseTexture"), GL_FALSE);


		glUniform1i(glGetUniformLocation(programID, "uUseTexture"), GL_FALSE);

		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(
			2,
			2,                                // size
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);
		if (lock2 != 2) {
			glDrawArrays(GL_TRIANGLES, 0, 36); //if lock=2 don't draw the tresure
		}

		glDisableVertexAttribArray(0);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the SPACE key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_SPACE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID);

	glBindVertexArray(0);
	glDeleteBuffers(1, &VBO2);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

