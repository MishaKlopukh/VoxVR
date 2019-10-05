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
	glm::vec4 a = glm::vec4(mat.m[0][0], mat.m[0][1], mat.m[0][2], mat.m[0][3]);
	glm::vec4 b = glm::vec4(mat.m[1][0], mat.m[1][1], mat.m[1][2], mat.m[1][3]);
	glm::vec4 c = glm::vec4(mat.m[2][0], mat.m[2][1], mat.m[2][2], mat.m[2][3]);
	glm::vec4 d = glm::vec4(mat.m[3][0], mat.m[3][1], mat.m[3][2], mat.m[3][3]);
	return glm::mat4x4(a, b, c, d);
}

glm::mat4x4 toGlmMat(vr::HmdMatrix34_t mat) {
	glm::vec4 a = glm::vec4(mat.m[0][0], mat.m[0][1], mat.m[0][2], mat.m[0][3]);
	glm::vec4 b = glm::vec4(mat.m[1][0], mat.m[1][1], mat.m[1][2], mat.m[1][3]);
	glm::vec4 c = glm::vec4(mat.m[2][0], mat.m[2][1], mat.m[2][2], mat.m[2][3]);
	glm::vec4 d = glm::vec4(1.0f);
	return glm::mat4x4(a, b, c, d);
}