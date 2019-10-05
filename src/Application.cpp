#include "Application.h"

char const* lFilterPatterns[2] = { "*.bmp", "*.BMP" };

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

	worldInit();

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

	HMD->GetRecommendedRenderTargetSize(&WIDTH, &HEIGHT);
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

void VoxVRApplication::worldInit() {
	char const* filename = tinyfd_openFileDialog("Choose the last file in the stack", "", 2, lFilterPatterns, NULL, 0);
	char const* maxtxt = tinyfd_inputBox("Input the number of images",
		"Type the number f total images in the stack. this should be the number of the image you selected.", "");
	int max_idx = atoi(maxtxt);
	char* filetemplate = (char*)malloc(sizeof(filename)+1);
	int filetemplatelen = strlen(filename) - strlen(maxtxt) - 4;
	strncpy(filetemplate, filename, filetemplatelen);
	BitMap frame = BitMap(filename);
	world = new VoxelWorld(frame.bmp_info_header.width, frame.bmp_info_header.height, max_idx);
	world->loadBitmapData(frame, max_idx - 1);
	int zsize;
	for (int z = 1; z < max_idx; z++) {
		if (z > 1000) {
			zsize = 4;
		} else if (z > 100) {
			zsize = 3;
		} else if (z > 10) {
			zsize = 2;
		} else {
			zsize = 1;
		}
		itoa(z, filetemplate+filetemplatelen, 10);
		strcpy(filetemplate + filetemplatelen + zsize, ".bmp\0");
		world->loadBitmapData(filetemplate, z - 1);
	}
	projectionmatrix_left = toGlmMat(HMD->GetProjectionMatrix(vr::Eye_Left, 0.1, 100));
	projectionmatrix_right = toGlmMat(HMD->GetProjectionMatrix(vr::Eye_Right, 0.1, 100));
}

void VoxVRApplication::update() {
	HMD->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, 0.0f, &HMPose, 1);
	switch (HMPose.eTrackingResult) {
	case vr::TrackingResult_Running_OK:
		world->setCameraTransform(toGlmMat(HMPose.mDeviceToAbsoluteTracking));
		break;
	default:
		break;
	}
}

void VoxVRApplication::render() {
	GLuint leftTex;
	glGenTextures(1, &leftTex);
	world->render(WIDTH, HEIGHT, projectionmatrix_left, leftTex);
	vr::Texture_t leftEyeTexture = { (void*)leftTex, vr::TextureType_OpenGL, vr::ColorSpace_Linear };
	vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);
	GLuint rightTex;
	glGenTextures(1, &rightTex);
	world->render(WIDTH, HEIGHT, projectionmatrix_right, rightTex);
	vr::Texture_t rightEyeTexture = { (void*)rightTex, vr::TextureType_OpenGL, vr::ColorSpace_Linear };
	vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture);
}

void VoxVRApplication::cleanup() {
	free(world);

	if (HMD) {
		vr::VR_Shutdown();
		HMD = NULL;
	}

	glfwDestroyWindow(window);

	glfwTerminate();
}