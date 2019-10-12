#pragma once

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <cstdlib>
#include <iostream>
#include <openvr.h>
#include "vrutil.h"
#include "tinyfiledialogs.h"
#include "BitMap.h"
#include "VoxelWorld.h"

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
	void setupENV();
	void worldInit();

	bool should_close;
	GLFWwindow* window;
	vr::IVRSystem* HMD;

	vr::TrackedDevicePose_t HMPose;

	VoxelWorld* world;

	glm::mat4 projectionmatrix_left;
	glm::mat4 projectionmatrix_right;

	glm::mat4 eyeposition_left;
	glm::mat4 eyeposition_right;

	glm::mat4 pose;

	FramebufferDesc leftEyeDesc;
	FramebufferDesc rightEyeDesc;

	bool is_gripping_left;
	bool is_gripping_right;

	uint32_t HEIGHT;
	uint32_t WIDTH;
};