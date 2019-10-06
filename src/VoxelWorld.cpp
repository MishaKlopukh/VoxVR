#include "VoxelWorld.h"

VoxelWorld::VoxelWorld(int x, int y, int z) {
	data = (unsigned char*)malloc(sizeof(unsigned char) * x * y * z);
	maxx = x;
	maxy = y;
	maxz = z;
	resx = 1024;
	resy = 1024;
	globaltransform = glm::mat4x4(1.0f);
	cameratransform = glm::mat4x4(1.0f);
	projection = glm::mat4x4(1.0f);
	renderbuffer = nullptr;
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

void VoxelWorld::setCameraTransform(glm::mat4x4 transformation) {
	cameratransform = transformation;
}

void VoxelWorld::loadBitmapData(const char* fname, int z_index) {
	loadBitmapData(BitMap(fname), z_index);
}

void VoxelWorld::writeBitmapBuffer(BitMap& image) {
	for (int i = 0; i < sizeof(renderbuffer) / sizeof(unsigned char*); i++) {
		image.data[i] = renderbuffer[i];
	}
}

void VoxelWorld::writeBitmapBuffer(const char* fname) {
	BitMap bmp = BitMap(resx, resy, false);
	writeBitmapBuffer(bmp);
	bmp.write(fname);
}

void VoxelWorld::render(int xres, int yres, glm::mat4x4 projectionmatrix) {
	resx = xres;
	resy = yres;
	projection = projectionmatrix;
	render();
}
void VoxelWorld::render() {
	renderbuffer = (unsigned char*)malloc(sizeof(unsigned char) * resx * resy * 3);
	glm::vec4 point;
	unsigned char dv;
	unsigned char dx;
	unsigned char nv;
	int px;
	int py;
	for (int x = 0; x < maxx; x++) {
		std::cout << "rendering line " << x << std::endl;
		for (int y = 0; y < maxy; y++) {
			for (int z = maxz-1; z >= 0; z--) {
				point = glm::vec4(x, y, z, 1.0f);
				point = projection * cameratransform * globaltransform * point;
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
}

VoxelWorld::~VoxelWorld() {
	free(data);
	free(renderbuffer);
}