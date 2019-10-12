#include "VoxelWorld.h"

VoxelWorld::VoxelWorld(std::vector<const char*> files) {
	BitMap bmp = BitMap(files[0]);
	int x = bmp.bmp_info_header.width;
	int y = bmp.bmp_info_header.height;
	int z = files.size();
	GLubyte* pData = new GLubyte[x * y * z];
	for (int xx = 0; xx < x; xx++) {
		for (int yy = 0; yy < y; yy++) {
			pData[yy * x + xx] = bmp.getPixel(xx, yy).r;
		}
	}
	for (int ii = 1; ii < z; ii++) {
		BitMap bmp = BitMap(files[ii]);
		for (int xx = 0; xx < x; xx++) {
			for (int yy = 0; yy < y; yy++) {
				pData[(ii * y + yy) * x + xx] = bmp.getPixel(xx, yy).r;
			}
		}
	}
	maxx = x;
	maxy = y;
	maxz = z;
	resx = 1024;
	resy = 1024;

	modelTransform = glm::mat4(1.0f);

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_3D, textureID);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, 4);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, maxx, maxy, maxz, 0, GL_RED, GL_UNSIGNED_BYTE, pData);

	glGenerateMipmap(GL_TEXTURE_3D);

	delete[] pData;

	shader.LoadFromFile(GL_VERTEX_SHADER, "shaders/raycaster.vert");
	shader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/raycaster.frag");

	shader.CreateAndLinkProgram();
	shader.Use();
	shader.AddAttribute("vVertex");
	shader.AddUniform("MVP");
	shader.AddUniform("volume");
	shader.AddUniform("camPos");
	shader.AddUniform("step_size");
	glUniform3f(shader("step_size"), 1.0f / x, 1.0f / y, 1.0f / z);
	glUniform1i(shader("volume"), 0);
	shader.UnUse();

	glClearColor(bg.r, bg.g, bg.b, bg.a);

	glGenVertexArrays(1, &cubeVAOID);
	glGenBuffers(1, &cubeVBOID);
	glGenBuffers(1, &cubeIndicesID);

	glm::vec3 vertices[8] = { 
		glm::vec3(-0.5f,-0.5f,-0.5f),
		glm::vec3(0.5f,-0.5f,-0.5f),
		glm::vec3(0.5f, 0.5f,-0.5f),
		glm::vec3(-0.5f, 0.5f,-0.5f),
		glm::vec3(-0.5f,-0.5f, 0.5f),
		glm::vec3(0.5f,-0.5f, 0.5f),
		glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(-0.5f, 0.5f, 0.5f) 
	};

	GLushort cubeIndices[36] = { 
		0,5,4,
		5,0,1,
		3,7,6,
		3,6,2,
		7,4,6,
		6,4,5,
		2,1,3,
		3,1,0,
		3,0,7,
		7,0,4,
		6,5,2,
		2,5,1 
	};

	glBindVertexArray(cubeVAOID);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBOID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &(vertices[0].x), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIndicesID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), &cubeIndices[0], GL_STATIC_DRAW);
	glBindVertexArray(0);

	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void VoxelWorld::transform(glm::mat4x4 transformation) {
	modelTransform = modelTransform * transformation;
}

void VoxelWorld::setResolution(int xres, int yres) {
	resx = xres;
	resy = yres;
}

void VoxelWorld::render(glm::mat4 modelView, glm::mat4 projection) {
	glm::vec3 camPos = glm::vec3(glm::inverse(modelView * modelTransform) * glm::vec4(0, 0, 0, 1));
	glm::mat4 MVP = projection * modelView * modelTransform;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBindVertexArray(cubeVAOID);
	shader.Use();
	glUniformMatrix4fv(shader("MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
	glUniform3fv(shader("camPos"), 1, &(camPos.x));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
	shader.UnUse();
	glDisable(GL_BLEND);
}

VoxelWorld::~VoxelWorld() {
	glDeleteTextures(1, &textureID);
}