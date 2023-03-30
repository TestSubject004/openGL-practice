#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <iostream>
#include <fstream>
#include<cmath>
#include<string>
#include<stack>
#include"Utils.h"

#define numVAOs 1
#define numVBOs 3

float cameraX, cameraY, cameraZ;
float cubeLocX, cubeLocY, cubeLocZ, pyrLocX, pyrLocY, pyrLocZ;

GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];


GLuint mvLoc, projLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mvMat, mMat, lookAt;
std::stack<glm::mat4> mvStack;

void setupVertices(void) {

	float vertexPositions[108] = {
	-1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f,
	1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f,
	1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f,
	1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f,
	-1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,
	-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f
	};

	// pyramid with 18 vertices, comprising 6 triangles (four sides, and two on the bottom)
	float pyramidPositions[54] =
	{ -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, // front face
	1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // right face
	1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // back face
	-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, // left face
	-1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, // base – left front
	1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f // base – right back
	};

	float diamondPosition[36] =
	{	0.0f, 1.0f, 0.0f, 0.0f,0.0f, 1.0f,1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,1.0f, 0.0f, 0.0f,0.0f,0.0f, -1.0f,
		0.0f, 1.0f, 0.0f, 0.0f,0.0f, -1.0f,-1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,-1.0f, 0.0f, 0.0f,0.0f,0.0f, 1.0f


	};



	glGenVertexArrays(numVAOs, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(numVBOs, vbo);


	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidPositions), pyramidPositions, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(diamondPosition), diamondPosition, GL_STATIC_DRAW);


}

void init(GLFWwindow* window) {
	renderingProgram = Utils::createShaderProgram("vpVert.glsl", "vpFrag.glsl");
	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 8.0f;
	cubeLocX = 0.0f; cubeLocY = -2.0f; cubeLocZ = 0.0f;

	pyrLocX = 2.0f; pyrLocY = 2.0f; pyrLocZ = 2.0f;
	setupVertices();
}

glm::mat4 lookAtMat(glm::vec3 eye, glm::vec3 target) {
	glm::vec3 fwd = glm::normalize(eye - target);
	glm::vec3 side = glm::normalize(glm::cross(-fwd, glm::vec3(0.0f, 1.0f, 0.0f)));
	glm::vec3 up = glm::normalize(glm::cross(side, -fwd));

	glm::mat4 val
	(glm::vec4(side, -glm::dot(side, eye)),
		glm::vec4(up, -glm::dot(up, eye)),
		glm::vec4(-fwd, -glm::dot(-fwd, eye)),
		glm::vec4(0,0,0,1));

	return val;
}


void display(GLFWwindow* window, double currentTime) {
	glClear(GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);
	//glDisable(GL_CULL_FACE);
	
	glUseProgram(renderingProgram);

	//get uniform variables for MV and projection matrices

	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");


	//build perspective matrix

	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f); //1.0472 rad = 60 degrees

	lookAt = lookAtMat(glm::vec3(cameraX, cameraY, cameraZ), glm::vec3(0.0f, 0.0f, 0.0f));

	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
	//used lookAt function to look at object and create the lookAt view matrix
	mvStack.push(glm::lookAt(glm::vec3(cameraX, cameraY, cameraZ), glm::vec3(sin((float)currentTime) * 4.0, 0.0f, cos((float)currentTime) * 4.0),glm::vec3(0.0f,1.0f,0.0f)));
	//mvStack.top() *= glm::rotate(glm::mat4(1.0f),(float)currentTime,glm::vec3(0.0f, 1.0f, 0.0f) ); //use to change camera angles
	//mvStack.top() *= lookAt;
	

	//pyramid  == sun (TESTING THE DIAMOND)

	mvStack.push(mvStack.top());
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	mvStack.push(mvStack.top());
	mvStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0f, 1.0f, 0.0f));
	

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));

	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	//associate VBO with the corresponding vertex attribute in the vertex shader
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]); //using the new figure array
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	

	//adjust openGL settings and draw a model
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLES, 0, 12); //changed from pyramid to new fig, also changed to traingle_Strip
	mvStack.pop();
	//-----cube = planet

	
	mvStack.push(mvStack.top());
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)currentTime) * 4.0, 0.0f, cos((float)currentTime) * 4.0));
	mvStack.push(mvStack.top());
	mvStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	//glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	mvStack.pop();

	// smaller cube = moon
	mvStack.push(mvStack.top());
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, sin((float)currentTime) * 2.0, cos((float)currentTime) * 2.0));
	mvStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 0.0, 1.0));
	mvStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(0.25f, 0.25f, 0.25f));

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_TRIANGLES, 0, 36); // draw the moon
	// remove moon scale/rotation/position, planet position, sun position, and view matrices from stack
	mvStack.pop(); mvStack.pop(); mvStack.pop(); mvStack.pop();


}

int main(void) {
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(1600, 900, "FUCK OFF", NULL, NULL);
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