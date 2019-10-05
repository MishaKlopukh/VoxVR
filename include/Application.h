#pragma once

#if defined(_WIN32)
#endif

#include <GLFW/glfw3.h>
#include <cstdlib>
#include <iostream>
#include <openvr.h>
#include <vrutil.h>

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

	bool should_close;
	GLFWwindow* window;
	vr::IVRSystem* HMD;

	bool is_gripping_left;
	bool is_gripping_right;
};