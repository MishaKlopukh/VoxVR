#pragma once

#include <openvr.h>
#include <glm/glm.hpp>

enum class ControllerHand {
	CONTROLLER_HAND_IGNORE,
	CONTROLLER_HAND_LEFT,
	CONTROLLER_HAND_RIGHT
};

ControllerHand getHandOfEvent(vr::IVRSystem* HMD, vr::VREvent_t* event);

glm::mat4x4 toGlmMat(vr::HmdMatrix44_t mat);
glm::mat4x4 toGlmMat(vr::HmdMatrix34_t mat);