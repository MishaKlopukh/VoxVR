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

	glfwMakeContextCurrent(window);

	glutInitContextVersion(3, 3);
	glutInitContextFlags(GLUT_CORE_PROFILE);
	glewExperimental = GL_TRUE;

	GLenum err = glewInit();

	if (err != GLEW_OK) {
		std::cerr << glewGetErrorString(err) << std::endl;
		throw std::runtime_error("Glew initialization error!");
	}

	setupENV();

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

void VoxVRApplication::setupENV() {
	projectionmatrix_left = toGlmMat(HMD->GetProjectionMatrix(vr::Eye_Left, 0.1, 100));
	projectionmatrix_right = toGlmMat(HMD->GetProjectionMatrix(vr::Eye_Right, 0.1, 100));
	eyeposition_left = toGlmMat(HMD->GetEyeToHeadTransform(vr::Eye_Left));
	eyeposition_right = toGlmMat(HMD->GetEyeToHeadTransform(vr::Eye_Right));

	CreateFrameBuffer(WIDTH, HEIGHT, leftEyeDesc);
	CreateFrameBuffer(WIDTH, HEIGHT, rightEyeDesc);
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
	std::vector<const char*> filenames = std::vector<const char*>();
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
		filenames.push_back(&*filetemplate);
	}
	filenames.push_back(&*filename);
	world = new VoxelWorld(filenames);
}

void VoxVRApplication::update() {
	vr::VRCompositor()->WaitGetPoses(&HMPose, 1, NULL, 0);
	switch (HMPose.eTrackingResult) {
	case vr::TrackingResult_Running_OK:
		pose = toGlmMat(HMPose.mDeviceToAbsoluteTracking);
		break;
	default:
		break;
	}
}

void VoxVRApplication::render() {
	glEnable(GL_MULTISAMPLE);
	glBindFramebuffer(GL_FRAMEBUFFER, leftEyeDesc.m_nRenderFramebufferId);
	glViewport(0, 0, WIDTH, HEIGHT);
	world->render(eyeposition_left * pose, projectionmatrix_left);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_MULTISAMPLE);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, leftEyeDesc.m_nRenderFramebufferId);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, leftEyeDesc.m_nResolveFramebufferId);
	glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glEnable(GL_MULTISAMPLE);
	glBindFramebuffer(GL_FRAMEBUFFER, rightEyeDesc.m_nRenderFramebufferId);
	glViewport(0, 0, WIDTH, HEIGHT);
	world->render(eyeposition_right * pose, projectionmatrix_right);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_MULTISAMPLE);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, rightEyeDesc.m_nRenderFramebufferId);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, rightEyeDesc.m_nResolveFramebufferId);
	glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	vr::Texture_t leftEyeTexture = { (void*)(uintptr_t)leftEyeDesc.m_nResolveTextureId, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
	vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);

	vr::Texture_t rightEyeTexture = { (void*)(uintptr_t)rightEyeDesc.m_nResolveTextureId, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
	vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture);

	glEnable(GL_MULTISAMPLE);
	glfwMakeContextCurrent(window);
	glViewport(0, 0, WIDTH, HEIGHT);
	world->render(eyeposition_left * pose, projectionmatrix_left);
	glfwSwapBuffers(window);
	glDisable(GL_MULTISAMPLE);

	glFlush();
	glFinish();

	std::cout << "rendered frame" << std::endl;
}

void VoxVRApplication::cleanup() {
	delete world;

	glDeleteRenderbuffers(1, &leftEyeDesc.m_nDepthBufferId);
	glDeleteTextures(1, &leftEyeDesc.m_nRenderTextureId);
	glDeleteFramebuffers(1, &leftEyeDesc.m_nRenderFramebufferId);
	glDeleteTextures(1, &leftEyeDesc.m_nResolveTextureId);
	glDeleteFramebuffers(1, &leftEyeDesc.m_nResolveFramebufferId);

	glDeleteRenderbuffers(1, &rightEyeDesc.m_nDepthBufferId);
	glDeleteTextures(1, &rightEyeDesc.m_nRenderTextureId);
	glDeleteFramebuffers(1, &rightEyeDesc.m_nRenderFramebufferId);
	glDeleteTextures(1, &rightEyeDesc.m_nResolveTextureId);
	glDeleteFramebuffers(1, &rightEyeDesc.m_nResolveFramebufferId);

	if (HMD) {
		vr::VR_Shutdown();
		HMD = NULL;
	}

	glfwDestroyWindow(window);

	glfwTerminate();
}