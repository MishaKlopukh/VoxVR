#pragma once

#include <GL/gl.h>
#include <glm/glm.hpp>
#include <cstdlib>
#include <iostream>
#include "BitMap.h"

class VoxelWorld {
public:
	VoxelWorld(int x, int y, int z);
	void loadBitmapData(char* fname, int z_index);
	void loadBitmapData(BitMap& image, int z_index);
	void transform(glm::mat4x4 transformation);
	void render(glm::mat4x4 projection, GLuint& tex);
	~VoxelWorld();

private:
	GLuint worlddata;
	glm::mat4x4 globaltransform;
	char* data;
	glm::vec3* positions;
};