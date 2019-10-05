#include "VoxelWorld.h"

VoxelWorld::VoxelWorld(int x, int y, int z) {
	data = (unsigned char*)malloc(sizeof(unsigned char) * x * y * z);
	maxx = x;
	maxy = y;
	maxz = z;
	globaltransform = glm::mat4x4(1.0f);
}

void VoxelWorld::loadBitmapData(BitMap& image, int z_index) {
	for (int x = 0; x < maxx; x++) {
		for (int y = 0; y < maxy; y++) {
			data[(z_index * maxy + y) * maxx + x] = image.getPixel(x, y).r;
		}
	}
}

void VoxelWorld::transform(glm::mat4x4 transformation) {
	globaltransform = globaltransform * transformation;
}

void VoxelWorld::loadBitmapData(char* fname, int z_index) {
	loadBitmapData(BitMap(fname), z_index);
}

void VoxelWorld::render(int resx, int resy, glm::mat4x4 projection, GLuint& tex) {
	renderbuffer = (unsigned char*)malloc(sizeof(unsigned char) * resx * resy * 3);
	glm::vec4 point;
	unsigned char dv;
	unsigned char dx;
	unsigned char nv;
	int px;
	int py;
	for (int x = 0; x < maxx; x++) {
		for (int y = 0; y < maxy; y++) {
			for (int z = maxz-1; z >= 0; z--) {
				point = glm::vec4(x, y, z, 1.0f);
				point = projection * globaltransform * point;
				point /= point.w;
				if (point.z >= 0 && point.z <= 1 && point.x <= 1 && point.x >= 0 && point.y <= 1 && point.y >= 0) {
					px = floor(point.x * resx);
					py = floor(point.y * resy);
					dv = data[(z * maxy + y) * maxx + x];
					if (z < maxz - 1) {
						dx = renderbuffer[((py * resx) + px) * 3];
					} else {
						dx = 0;
					}
					nv = dv + dx / dv;
					renderbuffer[((py * resx) + px) * 3] = nv;
					renderbuffer[((py * resx) + px) * 3 + 1] = nv;
					renderbuffer[((py * resx) + px) * 3 + 2] = nv;
				}
			}
		}
	}
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, resx, resy, 0, GL_RGB, GL_UNSIGNED_BYTE, renderbuffer);
	glGenerateMipmap(GL_TEXTURE_2D);
	free(renderbuffer);
}

VoxelWorld::~VoxelWorld() {
	free(data);
	free(renderbuffer);
}