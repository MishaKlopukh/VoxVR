#pragma once

#include <openvr.h>

enum class ControllerHand {
	CONTROLLER_HAND_IGNORE,
	CONTROLLER_HAND_LEFT,
	CONTROLLER_HAND_RIGHT
};

ControllerHand getHandOfEvent(vr::IVRSystem* HMD, vr::VREvent_t* event);