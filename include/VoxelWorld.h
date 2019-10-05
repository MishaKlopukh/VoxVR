#pragma once

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <glm/glm.hpp>
#include <cstdlib>
#include <iostream>
#include "BitMap.h"

class VoxelWorld {
public:
	VoxelWorld(int x, int y, int z);
	void loadBitmapData(const char* fname, int z_index);
	void loadBitmapData(BitMap& image, int z_index);
	void transform(glm::mat4x4 transformation);
	void setCameraTransform(glm::mat4x4 transformation);
	void render(int resx, int resy, glm::mat4x4 projection, GLuint& tex);
	~VoxelWorld();

private:
	unsigned char* renderbuffer;
	glm::mat4x4 globaltransform;
	glm::mat4x4 cameratransform;
	unsigned char* data;
	int maxx;
	int maxy;
	int maxz;
};