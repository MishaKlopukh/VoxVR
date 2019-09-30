#include "Application.h"

void VoxVRApplication::init() {
	if (!glfwInit()) {
		throw std::runtime_error("Failed to initialize GLFW!");
	}

	window = glfwCreateWindow(WIDTH, HEIGHT, "Hello World", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		throw std::runtime_error("Window Creation Failed!");
	}

	should_close = false;
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
	glfwDestroyWindow(window);

	glfwTerminate();
}