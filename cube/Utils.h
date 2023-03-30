#include<GL\glew.h>
#include <GLFW\glfw3.h>
#include<iostream>
#include<fstream>
#include <string>
#include <SOIL2/SOIL2.h>



#pragma once
class Utils
{

public:
	static void printShaderLog(GLuint);

	static void printProgramLog(int);

	static bool checkOpenGLError();

	static GLuint createShaderProgram(const char*, const char*);

	static std::string readShaderSource(const char*);

	static GLuint loadTexture(const char* texImagePath);
};



bool Utils::checkOpenGLError() {
	bool foundError = false;
	int glErr = glGetError();
	while (glErr != GL_NO_ERROR) {
		std::cout << "glError: " << glErr << std::endl;
		foundError = true;
		glErr = glGetError();
	}
	return foundError;
}


void Utils::printProgramLog(int prog) {
	int len = 0;
	int chWrittn = 0;
	char* log;
	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
	if (len > 0) {
		log = (char*)malloc(len);
		glGetProgramInfoLog(prog, len, &chWrittn, log);
		std::cout << "Program Info Log" << log << std::endl;
		free(log);
	}
}

void Utils::printShaderLog(GLuint shader) {
	int len = 0;
	int chWrittn = 0;
	char* log;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
	if (len > 0) {
		log = (char*)malloc(len);
		glGetShaderInfoLog(shader, len, &chWrittn, log);
		std::cout << "Shader Info Log: " << log << std::endl;
		free(log);
	}




}

GLuint Utils::createShaderProgram(const char* vp, const char* fp) {

	std::string vertshaderSource = readShaderSource(vp);

	std::string fragshaderSource = readShaderSource(fp);

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

	Utils::printShaderLog(vShader);
	Utils::printShaderLog(fShader);
	Utils::printProgramLog(vfProgram);
	//Utils::printProgramLog(vfProgram);

	return vfProgram;
}

std::string Utils::readShaderSource(const char* filePath) {

	std::string content;
	std::ifstream fileStream(filePath, std::ios::in);
	std::string line = "";
	while (!fileStream.eof()) {
		std::getline(fileStream, line);
		content.append(line + "\n");
	}
	fileStream.close();
	return content;
}


GLuint Utils::loadTexture(const char* texImagePath) {
	GLuint textureID;
	textureID = SOIL_load_OGL_texture(texImagePath,
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	if (textureID == 0) std::cout << "could not find texture file" << texImagePath << std::endl;
	return textureID;
}