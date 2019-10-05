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