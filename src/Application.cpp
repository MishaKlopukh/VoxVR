#include "Application.h"

void VoxVRApplication::init() {
	if (!glfwInit()) {
		throw std::runtime_error("Failed to initialize GLFW!");
	}

	initVR();

	window = glfwCreateWindow(WIDTH, HEIGHT, "Hello World", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		throw std::runtime_error("Window Creation Failed!");
	}

	should_close = false;
}

void VoxVRApplication::initVR() {
	vr::EVRInitError eError = vr::VRInitError_None;
	HMD = vr::VR_Init(&eError, vr::VRApplication_Scene);

	if (eError != vr::VRInitError_None) {
		HMD = NULL;
		throw std::runtime_error(vr::VR_GetVRInitErrorAsEnglishDescription(eError));
	}


}

void VoxVRApplication::handleInputs() {
	glfwPollEvents();
	if (glfwWindowShouldClose(window)) {
		should_close = true;
	}
}

void VoxVRApplication::update() {}

void VoxVRApplication::render() {}

void VoxVRApplication::cleanup() {
	if (HMD) {
		vr::VR_Shutdown();
		HMD = NULL;
	}

	glfwDestroyWindow(window);

	glfwTerminate();
}