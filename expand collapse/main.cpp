#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <fstream>
#include<string>
#define numVAOs 1

using namespace std;

GLuint renderingProgram;
GLuint vao[numVAOs];

float x = 0.0f;
float inc = 0.10f;

string readShaderSource(const char* filePath) {
	string content;
	ifstream fileStream(filePath, ios::in);
	string line = "";
	while (!fileStream.eof()) {
		getline(fileStream, line);
		content.append(line + "\n");
	}
	fileStream.close();
	return content;
}

GLuint createShaderProgram() {
	string vertshaderSource = readShaderSource("vert.glsl");

	string fragshaderSource = readShaderSource("frag.glsl");

	const char* vshaderSource = vertshaderSource.c_str();
	const char* fshaderSource = fragshaderSource.c_str();

	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);



	glShaderSource(vShader, 1, &vshaderSource, NULL);
	glShaderSource(fShader, 1, &fshaderSource, NULL);

	glCompileShader(vShader);
	glCompileShader(fShader);

	GLuint vfProgram = glCreateProgram();
	glAttachShader(vfProgram, vShader);
	glAttachShader(vfProgram, fShader);
	glLinkProgram(vfProgram);

	return vfProgram;


}

void init(GLFWwindow* window) {
	renderingProgram = createShaderProgram();
	glGenVertexArrays(numVAOs, vao);
	glBindVertexArray(vao[0]);

}
void display(GLFWwindow* window, double currentTime) {
	glClear(GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(renderingProgram);
	x += inc;
	if (x > 30.0f) inc = -1.0f;

	if (x < -30.0f) inc = 1.0f;
	//GLuint offsetLoc = glGetUniformLocation(renderingProgram, "offset");
	//glProgramUniform1f(renderingProgram, offsetLoc, x);
	glPointSize(x);
	glDrawArrays(GL_POINTS, 0, 1);
}
int main(void) {
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(600, 600, "FUCK OFF", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);
	init(window);
	while (!glfwWindowShouldClose(window)) {
		display(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}