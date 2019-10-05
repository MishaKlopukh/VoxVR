#pragma once

#ifdef _WIN32
#include <windows.h>
#endif

#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <cstdlib>
#include <iostream>
#include <openvr.h>
#include "vrutil.h"
#include "tinyfiledialogs.h"
#include "BitMap.h"
#include "VoxelWorld.h"

const int WIDTH = 800;
const int HEIGHT = 600;

class VoxVRApplication {
public:
	void run() {
		init();
		while (!should_close) {
			handleInputs();
			update();
			render();
		}
		cleanup();
	}
private:
	void init();
	void handleInputs();
	void update();
	void render();
	void cleanup();

	void initVR();
	void worldInit();

	bool should_close;
	GLFWwindow* window;
	vr::IVRSystem* HMD;

	VoxelWorld world;

	bool is_gripping_left;
	bool is_gripping_right;
};