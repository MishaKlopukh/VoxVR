#include "vrutil.h"

ControllerHand getHandOfEvent(vr::IVRSystem* HMD, vr::VREvent_t* event)  {
	vr::ETrackedDeviceClass trackedDeviceClass = HMD->GetTrackedDeviceClass(event->trackedDeviceIndex);
	if (trackedDeviceClass != vr::ETrackedDeviceClass::TrackedDeviceClass_Controller) {
		return ControllerHand::CONTROLLER_HAND_IGNORE;
	}
	vr::ETrackedControllerRole role = HMD->GetControllerRoleForTrackedDeviceIndex(event->trackedDeviceIndex);
	if (role == vr::TrackedControllerRole_Invalid) {
		return ControllerHand::CONTROLLER_HAND_IGNORE;
	}
	else if (role == vr::TrackedControllerRole_LeftHand) {
		return ControllerHand::CONTROLLER_HAND_LEFT;
	}
	else if (role == vr::TrackedControllerRole_RightHand) {
		return ControllerHand::CONTROLLER_HAND_RIGHT;
	}
}

glm::mat4x4 toGlmMat(vr::HmdMatrix44_t mat) {
	glm::vec4 a = glm::vec4(mat.m[0][0], mat.m[1][0], mat.m[2][0], mat.m[3][0]);
	glm::vec4 b = glm::vec4(mat.m[0][1], mat.m[1][1], mat.m[2][1], mat.m[3][1]);
	glm::vec4 c = glm::vec4(mat.m[0][2], mat.m[1][2], mat.m[2][2], mat.m[3][2]);
	glm::vec4 d = glm::vec4(mat.m[0][3], mat.m[1][3], mat.m[2][3], mat.m[3][3]);
	return glm::mat4x4(a, b, c, d);
}

glm::mat4x4 toGlmMat(vr::HmdMatrix34_t mat) {
	glm::vec4 a = glm::vec4(mat.m[0][0], mat.m[1][0], mat.m[2][0], 0.0f);
	glm::vec4 b = glm::vec4(mat.m[0][1], mat.m[1][1], mat.m[2][1], 0.0f);
	glm::vec4 c = glm::vec4(mat.m[0][2], mat.m[1][2], mat.m[2][2], 0.0f);
	glm::vec4 d = glm::vec4(mat.m[0][3], mat.m[1][3], mat.m[2][3], 1.0f);
	return glm::mat4x4(a, b, c, d);
}

bool CreateFrameBuffer(int nWidth, int nHeight, FramebufferDesc& framebufferDesc) {
	glGenFramebuffers(1, &framebufferDesc.m_nRenderFramebufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferDesc.m_nRenderFramebufferId);

	glGenRenderbuffers(1, &framebufferDesc.m_nDepthBufferId);
	glBindRenderbuffer(GL_RENDERBUFFER, framebufferDesc.m_nDepthBufferId);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, nWidth, nHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, framebufferDesc.m_nDepthBufferId);

	glGenTextures(1, &framebufferDesc.m_nRenderTextureId);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, framebufferDesc.m_nRenderTextureId);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, nWidth, nHeight, true);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, framebufferDesc.m_nRenderTextureId, 0);

	glGenFramebuffers(1, &framebufferDesc.m_nResolveFramebufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferDesc.m_nResolveFramebufferId);

	glGenTextures(1, &framebufferDesc.m_nResolveTextureId);
	glBindTexture(GL_TEXTURE_2D, framebufferDesc.m_nResolveTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, nWidth, nHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferDesc.m_nResolveTextureId, 0);

	// check FBO status

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}