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
	is_gripping_left = false;
	is_gripping_right = false;
}

void VoxVRApplication::initVR() {
	vr::EVRInitError eError = vr::VRInitError_None;
	HMD = vr::VR_Init(&eError, vr::VRApplication_Scene);

	if (eError != vr::VRInitError_None) {
		HMD = NULL;
		throw std::runtime_error(vr::VR_GetVRInitErrorAsEnglishDescription(eError));
	}

	if (!vr::VRCompositor()) {
		throw std::runtime_error("VR Compositor Initialization Failed!");
	}
}

void VoxVRApplication::handleInputs() {
	glfwPollEvents();
	if (glfwWindowShouldClose(window)) {
		should_close = true;
	}
	vr::VREvent_t event;
	if (HMD->PollNextEvent(&event, sizeof(event))) {
		switch (event.eventType) {
		case vr::VREvent_Quit:
			should_close = true;
			break;
		case vr::VREvent_ButtonPress:
			switch (getHandOfEvent(HMD, &event)) {
			case ControllerHand::CONTROLLER_HAND_LEFT:
				switch (event.data.controller.button) {
				case vr::k_EButton_Grip:
					is_gripping_left = true;
					std::cout << "Left Grip Pressed!" << std::endl;
					break;
				case vr::k_EButton_SteamVR_Trigger:
					std::cout << "Left Trigger Pressed!" << std::endl;
					break;
				default:
					break;
				}
				break;
			case ControllerHand::CONTROLLER_HAND_RIGHT:
				switch (event.data.controller.button) {
				case vr::k_EButton_Grip:
					is_gripping_right = true;
					std::cout << "Right Grip Pressed!" << std::endl;
					break;
				case vr::k_EButton_SteamVR_Trigger:
					std::cout << "Right Trigger Pressed!" << std::endl;
					break;
				default:
					break;
				}
				break;
			default:
				break;
			}
			break;
		case vr::VREvent_ButtonUnpress:
			switch (getHandOfEvent(HMD, &event)) {
			case ControllerHand::CONTROLLER_HAND_LEFT:
				switch (event.data.controller.button) {
				case vr::k_EButton_Grip:
					is_gripping_left = false;
					std::cout << "Left Grip Released!" << std::endl;
					break;
				case vr::k_EButton_SteamVR_Trigger:
					std::cout << "Left Trigger Released!" << std::endl;
					break;
				default:
					break;
				}
				break;
			case ControllerHand::CONTROLLER_HAND_RIGHT:
				switch (event.data.controller.button) {
				case vr::k_EButton_Grip:
					is_gripping_right = false;
					std::cout << "Right Grip Released!" << std::endl;
					break;
				case vr::k_EButton_SteamVR_Trigger:
					std::cout << "Right Trigger Released!" << std::endl;
					break;
				default:
					break;
				}
				break;
			default:
				break;
			}
			break;
		default:
			std::cout << "OpenVR Event: " << event.eventType << std::endl;
		}
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