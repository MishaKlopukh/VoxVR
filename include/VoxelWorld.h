#pragma once

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLSLShader.h>
#include <cstdlib>
#include <iostream>
#include "BitMap.h"

const glm::vec4 bg = glm::vec4(0.5, 0.5, 1, 1);

class VoxelWorld {
public:
	VoxelWorld(std::vector<const char*> files);
	void transform(glm::mat4x4 transformation);
	void setResolution(int xres, int yres);
	void render(glm::mat4 modelView, glm::mat4 projection);
	~VoxelWorld();

private:
	glm::mat4 modelTransform;
	GLuint cubeVBOID;
	GLuint cubeVAOID;
	GLuint cubeIndicesID;
	GLuint textureID;
	GLSLShader shader;
	int maxx;
	int maxy;
	int maxz;
	int resx;
	int resy;
};