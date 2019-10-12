#pragma once

#include <openvr.h>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GL/gl.h>

enum class ControllerHand {
	CONTROLLER_HAND_IGNORE,
	CONTROLLER_HAND_LEFT,
	CONTROLLER_HAND_RIGHT
};

struct FramebufferDesc {
	GLuint m_nDepthBufferId;
	GLuint m_nRenderTextureId;
	GLuint m_nRenderFramebufferId;
	GLuint m_nResolveTextureId;
	GLuint m_nResolveFramebufferId;
};

ControllerHand getHandOfEvent(vr::IVRSystem* HMD, vr::VREvent_t* event);
bool CreateFrameBuffer(int nWidth, int nHeight, FramebufferDesc& framebufferDesc);

glm::mat4x4 toGlmMat(vr::HmdMatrix44_t mat);
glm::mat4x4 toGlmMat(vr::HmdMatrix34_t mat);